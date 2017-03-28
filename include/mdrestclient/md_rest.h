#ifndef _MD_REST_CONNECTOR_H
#define _MD_REST_CONNECTOR_H

#include "md_rest_response_objects.h"
#include "md_error_messages.h"
#include "md_exception.h"
#include "md_http_client.h"
#include "md_http_session.h"
#include "md_utils.h"
#include <memory>
#include <sstream>

namespace Opswat
{

/// @brief Metadefender Rest connection
///
/// It represents a connection to Metadefender Rest server. The connection's traits
///	(e.g.: security, persistency, thread-safety) determined by the HttpSession 
///	template parameter.
///
/// @tparam HttpSession Session type
template<typename HttpSession>
class MDRest
{
public:

	/// @brief Constructor
	/// 
	/// Takes ownership over session passed in, and uses it to initiate rest requests.
	/// 
	/// @param session The session MDRest can use for connection to Metadefender Rest
	MDRest(std::unique_ptr<HttpSession> session);

	/// @bried Destructor
	~MDRest();

	/// @brief Scan local file
	/// 
	/// Creates and sends rest request to scan a local file. Local file scanning has
	/// to be enabled on Metadefender side.
	///
	/// @param filePath Path of the local file to be scanned.
	/// @param fileName Display name
	/// @param userAgent Client's identification
	/// @param rule Name of the selected rule
	/// @param archivePwd Password for archive file
	/// @return Structure holding the returned Id for the initiated scan
	std::unique_ptr<MDResponse<MDFileScanId>> scanLocalFile(const std::string& filePath,
		const std::string& fileName = std::string(),
		const std::string& userAgent = std::string(),
		const std::string& rule = std::string(),
		const std::string& archivePwd = std::string());

	/// @brief Scan file
	/// 
	/// Creates and sends rest request to scan a file. 
	///
	/// @param inStream Stream to read the content from
	/// @param fileName Display name
	/// @param userAgent Client's identification
	/// @param rule Name of the selected rule
	/// @param archivePwd Password for archive file
	/// @return Structure holding the returned Id for the initiated scan
	std::unique_ptr<MDResponse<MDFileScanId>> scanFile(std::istream& inStream,
		const std::string& fileName = std::string(),
		const std::string& userAgent = std::string(),
		const std::string& rule = std::string(),
		const std::string& archivePwd = std::string());
	
	/// @brief Fetch scan result by Id
	/// 
	/// Fetches the status of a previously initiated scan based on the scan's Id.
	///
	/// @param dataId DataId for a previously initiated scan
	/// @return Structure holding the returned status
	std::unique_ptr<MDResponse<MDFileScanResult>> fetchScanResultById(const std::string& dataId);

	/// @brief Fetch scan result by hash
	/// 
	/// Fetches the status of a previously initiated scan based on the scanned 
	/// content's hash. Currently md5/sha1/sha256 hashes are supported.
	///
	/// @param dataId DataId for a previously initiated scan
	/// @return Structure holding the returned status
	std::unique_ptr<MDResponse<MDFileScanResult>> fetchScanResultByHash(const std::string& hash);

	/// @brief Fetch a sanitized content
	/// 
	/// If sanitization occured during scanning, the sanitized content can be acquired 
	/// by calling this function.
	///
	/// @param dataId DataId for a previously initiated scan
	/// @return String holding the sanitized content
	std::string fetchSanitizedFileById(const std::string& dataId);

	/// @brief Fetch a sanitized content
	/// 
	/// If sanitization occured during scanning, the sanitized content can be acquired 
	/// by calling this function. The content is written into the stream specified by the
	/// outStream parameter. The stream has to be opened for writing before passing it 
	/// into this function.
	///
	/// @param dataId DataId for a previously initiated scan
	/// @param outStream Stream where the sanitized content will be written
	/// @return String holding the sanitized content
	void fetchSanitizedFileById(const std::string& dataId, std::ostream& outStream);

	/// @brief Fetch the Metadefender server's version
	/// 
	/// Fetches the version information of the Metadefender server. It requires a valid
	/// api key, that can be acquired by calling createSession first.
	///
	/// @param apiKey Valid api key
	/// @return Structure holding the returned version information
	std::unique_ptr<MDResponse<MDVersionInfo>> getVersionInfo();

	/// @brief Fetch the Metadefender server's license information
	/// 
	/// Fetches the license information of the Metadefender server. It requires a valid
	/// api key, that can be acquired by calling createSession first.
	///
	/// @param apiKey Valid api key
	/// @return Structure holding the returned license information
	std::unique_ptr<MDResponse<MDLicenseInfo>> getLicenseInfo();

	/// @brief Create login session
	/// 
	/// Creates a login session and acquire an api key that is valid for the duration of the login session.
	/// Take note, that login session is not equivalent to rest session.
	///
	/// @param userName User's login name
	/// @param password User's login password
	/// @return Structure holding the returned login session information
	std::unique_ptr<MDResponse<MDLoginInfo>> createSession(const std::string& userName, const std::string& password);

	/// @brief Sets apikey
	///
	/// Sets an apikey, that will be sent along with every requests made by this MDRest object. The user is responsible
	/// for invalidating the api key -by calling destroySession- after it is no longer needed.
	/// Take note, that apiKey can be invalidated by Metadefender after some time of inactivity. In this case, a new
	/// api key has to be acquired by calling createSession.
	/// @pname apiKey Api key to use
	void useSession(std::string apiKey);

	/// @brief Destroys login session
	/// 
	/// Destroys a previously created login session and invalidates the api key associated with it.
	///	
	/// @param apiKey Api key acquired by a previous call to createSession
	/// @return Structure holding the returned logout information
	std::unique_ptr<MDResponse<MDLogoutInfo>> destroySession(const std::string& apiKey);

	/// @brief Fetch engine information from Metadefender server
	/// 
	/// Fetches the detailed information about the engines used by the Metadefender server.
	///		
	/// @return Structure holding the returned engine informations
	std::unique_ptr<MDResponse<MDEnginesList>> getEngineInfo();

private:
	void checkResponse(const MDHttpResponse& response) const;

private:
	
	std::unique_ptr<HttpSession> session_;
	std::string apiKey_;
};

template<typename HttpSession>
MDRest<HttpSession>::MDRest(std::unique_ptr<HttpSession> session) : session_(std::move(session))
{
}

template<typename HttpSession>
MDRest<HttpSession>::~MDRest()
{
}

template<typename HttpSession>
std::unique_ptr<MDResponse<MDFileScanId>> MDRest<HttpSession>::scanLocalFile(const std::string& filePath,
																			const std::string& fileName,
																			const std::string& userAgent,
																			const std::string& rule,
																			const std::string& archivePwd)
{
	MDHttpRequest request;
	request.method = HTTP_METHOD::HTTP_POST;
	request.url = "/file";
	request.headers = std::map<std::string, std::string>{{"filepath", filePath},
	{"filename", fileName},
	{"user_agent", userAgent},
	{"rule", rule},
	{"archivepwd", archivePwd},
	{"apikey", apiKey_}};
	request.inStream = nullptr;
	
	auto response = session_->sendRequest(request);

	checkResponse(*response);

	return Utils::make_unique<MDResponse<MDFileScanId>>(response->body);
}

template<typename HttpSession>
std::unique_ptr<MDResponse<MDFileScanId>> MDRest<HttpSession>::scanFile(std::istream& inStream,
																		const std::string& fileName,
																		const std::string& userAgent,
																		const std::string& rule,
																		const std::string& archivePwd)
{		
	MDHttpRequest request;
	request.method = HTTP_METHOD::HTTP_POST;
	request.url = "/file";
	request.headers = std::map<std::string, std::string>{{"filename", fileName},
	{ "user_agent", userAgent },
	{ "rule", rule },
	{ "archivepwd", archivePwd },
	{ "apikey", apiKey_}};
	request.inStream = &inStream;
	
	auto response = session_->sendRequest(request);

	checkResponse(*response);

	return Utils::make_unique<MDResponse<MDFileScanId>>(response->body);
}

template<typename HttpSession>
std::unique_ptr<MDResponse<MDFileScanResult>> MDRest<HttpSession>::fetchScanResultById(const std::string& dataId)
{
	MDHttpRequest request;
	request.method = HTTP_METHOD::HTTP_GET;
	request.url = "/file/" + dataId;
	request.headers = std::map<std::string, std::string>{{"apikey", apiKey_}};
	request.inStream = nullptr;
	
	auto response = session_->sendRequest(request);

	checkResponse(*response);

	return Utils::make_unique<MDResponse<MDFileScanResult>>(response->body);
}

template<typename HttpSession>
std::unique_ptr<MDResponse<MDFileScanResult>> MDRest<HttpSession>::fetchScanResultByHash(const std::string& hash)
{
	MDHttpRequest request;
	request.method = HTTP_METHOD::HTTP_GET;
	request.url = "/hash/" + hash;
	request.headers = std::map<std::string, std::string>{{"apikey", apiKey_}};
	request.inStream = nullptr;
	
	auto response = session_->sendRequest(request);

	checkResponse(*response);

	return Utils::make_unique<MDResponse<MDFileScanResult>>(response->body);
}

template<typename HttpSession>
std::string MDRest<HttpSession>::fetchSanitizedFileById(const std::string& dataId)
{
	MDHttpRequest request;
	request.method = HTTP_METHOD::HTTP_GET;
	request.url = "/file/converted/" + dataId;
	request.headers = std::map<std::string, std::string>{{"apikey", apiKey_}};
	request.inStream = nullptr;

	auto response = session_->sendRequest(request);

	checkResponse(*response);

	return std::move(response->body);
}

template<typename HttpSession>
void MDRest<HttpSession>::fetchSanitizedFileById(const std::string& dataId, std::ostream& outStream)
{
	MDHttpRequest request;
	request.method = HTTP_METHOD::HTTP_GET;
	request.url = "/file/converted/" + dataId;
	request.headers = std::map<std::string, std::string>{{"apikey", apiKey_}};
	request.inStream = nullptr;

	auto response = session_->sendRequest(request, outStream);

	checkResponse(*response);	
}

template<typename HttpSession>
std::unique_ptr<MDResponse<MDVersionInfo>> MDRest<HttpSession>::getVersionInfo()
{
	MDHttpRequest request;
	request.method = HTTP_METHOD::HTTP_GET;
	request.url = "/version";
	request.headers = std::map<std::string, std::string>{{"apikey", apiKey_}};
	request.inStream = nullptr;
	
	auto response = session_->sendRequest(request);

	checkResponse(*response);

	return Utils::make_unique<MDResponse<MDVersionInfo>>(response->body);
}

template<typename HttpSession>
std::unique_ptr<MDResponse<MDLicenseInfo>> MDRest<HttpSession>::getLicenseInfo()
{
	MDHttpRequest request;
	request.method = HTTP_METHOD::HTTP_GET;
	request.url = "/admin/license";
	request.headers = std::map<std::string, std::string>{{"apikey", apiKey_}};
	request.inStream = nullptr;
	
	auto response = session_->sendRequest(request);

	checkResponse(*response);

	return Utils::make_unique<MDResponse<MDLicenseInfo>>(response->body);
}

template<typename HttpSession>
std::unique_ptr <MDResponse<MDLoginInfo>> MDRest<HttpSession>::createSession(const std::string& userName, const std::string& password)
{
	MDHttpRequest request;
	request.method = HTTP_METHOD::HTTP_POST;
	request.url = "/login";
	std::string body = "{\"user\":\"" + userName + "\",\"password\":\"" + password + "\"}";	
	std::istringstream iss(body);
	request.inStream = &iss;
	
	auto response = session_->sendRequest(request);

	checkResponse(*response);

	return Utils::make_unique<MDResponse<MDLoginInfo>>(response->body);
}

template<typename HttpSession>
std::unique_ptr<MDResponse<MDLogoutInfo>> MDRest<HttpSession>::destroySession(const std::string& apiKey)
{
	MDHttpRequest request;
	request.method = HTTP_METHOD::HTTP_POST;
	request.url = "/logout";
	request.headers = std::map<std::string, std::string>{{"apikey", apiKey}};
	request.inStream = nullptr;
	
	auto response = session_->sendRequest(request);

	checkResponse(*response);

	return Utils::make_unique<MDResponse<MDLogoutInfo>>(response->body);
}

template<typename HttpSession>
std::unique_ptr<MDResponse<MDEnginesList>> MDRest<HttpSession>::getEngineInfo()
{
	MDHttpRequest request;
	request.method = HTTP_METHOD::HTTP_GET;
	request.url = "/stat/engines";
	request.headers = std::map<std::string, std::string>{{"apikey", apiKey_}};
	request.inStream = nullptr;
	
	auto response = session_->sendRequest(request);

	checkResponse(*response);

	return Utils::make_unique<MDResponse<MDEnginesList>>(response->body);
}

template<typename HttpSession>
void MDRest<HttpSession>::useSession(std::string apiKey)
{
	apiKey_ = std::move(apiKey);
}

template<typename HttpSession>
void MDRest<HttpSession>::checkResponse(const MDHttpResponse& response) const
{
	const int HTTP_STATUS_OK = 200;

	if (response.statusCode != HTTP_STATUS_OK)
	{
		try
		{
			auto errorInfo = MDResponse<MDErrorInfo>(response.body).parse();
			
			throw MDErrorResponseException(errorInfo->errorMessage, response.statusCode, response.body);
		}
		catch(MDParsingException&)
		{
			throw MDErrorResponseException(MD_METASCAN_ERROR, response.statusCode, response.body);
		}
	}
}

};

#endif
