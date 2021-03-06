#ifndef _MD_CURL_HTTP_CLIENT_H
#define _MD_CURL_HTTP_CLIENT_H

#include "md_http_client.h"
#include "md_stringbuf.h"
#include "md_exception.h"
#include "md_error_messages.h"
#include "md_utils.h"
#include <cctype>
#include <memory>
#include <algorithm>
#include <curl/curl.h>

namespace Opswat
{

/// @brief Libcurl based http client
///
/// This class represents a simple http client, implemented by using libcurl.
class MDCurlHttpClient : public IMDHttpClient
{
public:
	MDCurlHttpClient(const std::string& serverAddress, int port);
	~MDCurlHttpClient();

	std::unique_ptr<MDHttpResponse> sendRequest(MDHttpRequest& request);
	std::unique_ptr<MDHttpResponse> sendRequest(MDHttpRequest& request, std::ostream& outStream);

	/// @brief Initializes libcurl with ALL flags
	///
	/// This function calls curl_global_init, and inherently thread unsafe. Libcurl has to be initialized
	/// before calling any other function in libcurl.
	static void curlHttpClientInitAll()
	{
		curl_global_init(CURL_GLOBAL_ALL);
	}

	/// @brief Initializes libcurl with the specified flags
	///
	/// This function calls curl_global_init, and inherently thread unsafe. Libcurl has to be initialized
	/// before calling any other function in libcurl.
	///
	/// @param flags Tells libcurl what features to initialize
	static void curlHttpClientInit(long flags)
	{
		curl_global_init(flags);
	}

	/// @brief Cleanup the resources owned by libcurl
	///
	/// This function calls curl_global_cleanup, therefore inherently thread unsafe. It should be called
	/// after the program done using libcurl.
	static void curlHttpClientCleanup()
	{
		curl_global_cleanup();
	}


private:

	std::unique_ptr<MDHttpResponse> send(MDHttpRequest& request, std::ostream& outStream);

private:

	std::string address_;
	CURL* curl_;
};

static
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t bytesWritten = 0;
	try
	{
		size_t realsize = size * nmemb;
		std::ostream* buffer = static_cast<std::ostream*>(userp);
		buffer->write(static_cast<char*>(contents), realsize);
		bytesWritten = *buffer ? realsize : 0;
	}
	catch(...)
	{
		bytesWritten = 0;
	}

	return bytesWritten;
}

static
size_t ReadCallback(char* ptr, size_t size, size_t nmemb, void* input)
{
	try
	{
		std::istream* inStream = static_cast<std::istream*>(input);

		inStream->read(ptr, size * nmemb);
		size_t readBytes = inStream->gcount();
		if(readBytes > 0)
		{
			return readBytes;
		}
		else
		{
			return inStream->eof() ? 0 : CURL_READFUNC_ABORT;
		}
	}
	catch(...)
	{
		return CURL_READFUNC_ABORT;
	}
}

static
size_t ReadHeadersCallback(char *buffer, size_t size, size_t nitems, void *userdata)
{
	size_t length = 0;

	std::vector<std::string> *headerLines = static_cast<std::vector<std::string>*>(userdata);
	if (headerLines != nullptr) {
		length = size * nitems;
		if (length > 0) {
			headerLines->emplace_back(buffer, size * nitems);
		}
	}

	return length;
}

static inline
void leftTrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
}

static inline
void rightTrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

static
std::string extractCookieFromHeaders(const std::vector<std::string> &headerLines)
{
	bool found = false;
	std::string cookie;
	for (auto line : headerLines) {
		if (found) {
			if (std::isspace(static_cast<unsigned char>(line[0]))) {
				leftTrim(line);
				rightTrim(line);
				cookie.append(line);
			} else {
				return cookie;
			}
		} else {
			std::string upcaseLine = line;
			std::transform(upcaseLine.begin(), upcaseLine.end(), upcaseLine.begin(), ::toupper);
			if (upcaseLine.find("SET-COOKIE:") == 0) {
				const long valuePositionInLine = 11; // length of string "SET-COOKIE"
				cookie = line.substr(valuePositionInLine);
				leftTrim(cookie);
				rightTrim(cookie);
				found = true;
			}
		}
	}

	return cookie;
}

inline
MDCurlHttpClient::MDCurlHttpClient(const std::string& serverAddress, int port)
{
	if(serverAddress.find("http://") == std::string::npos && serverAddress.find("https://") == std::string::npos)
	{
		address_ += "http://";
	}
	address_ += serverAddress;
	address_ += ":";
	address_ += std::to_string(port);
	curl_ = curl_easy_init();
}

inline
MDCurlHttpClient::~MDCurlHttpClient()
{
	curl_easy_cleanup(curl_);
}

inline
std::unique_ptr<MDHttpResponse> MDCurlHttpClient::sendRequest(MDHttpRequest& request)
{
	Opswat::StringBuf stringBuf;
	std::ostream outStream(&stringBuf);
	auto resp = send(request, outStream);
	resp->body = std::move(stringBuf.getBuffer());
	return resp;
}

inline
std::unique_ptr<MDHttpResponse> MDCurlHttpClient::sendRequest(MDHttpRequest& request, std::ostream& outStream)
{
	return send(request, outStream);
}

inline
std::unique_ptr<MDHttpResponse> MDCurlHttpClient::send(MDHttpRequest& request, std::ostream& outStream)
{
	if(!curl_)
	{
		throw Opswat::MDConnectionException(MD_CONNECTION_ERROR);
	}

	curl_easy_reset(curl_);
	curl_easy_setopt(curl_, CURLOPT_URL, (address_ + request.url).c_str());
	curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, 30L);
	if(!verifyPeer_)
	{
		curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0L);
	}

	if(request.method == HTTP_METHOD::HTTP_GET)
	{
		curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
	}
	else if(request.method == HTTP_METHOD::HTTP_POST)
	{
		if(request.inStream)
		{
			request.inStream->seekg(0, std::ios::end);
			unsigned long long contentSize = request.inStream->tellg();
			request.inStream->seekg(0, std::ios::beg);
			request.headers["X-File-Size"] = std::to_string(contentSize);

			curl_easy_setopt(curl_, CURLOPT_POST, 1L);
			curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, NULL);
			curl_easy_setopt(curl_, CURLOPT_READFUNCTION, ReadCallback);
			curl_easy_setopt(curl_, CURLOPT_READDATA, request.inStream);
			curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE_LARGE, contentSize);
		}
		else
		{
			curl_easy_setopt(curl_, CURLOPT_POST, 1L);
			curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, "");
		}
	}
	else
	{
		throw MDConnectionException(MD_CONNECTION_ERROR);
	}

	curl_slist *slist = nullptr;
	for(auto &i : request.headers)
	{
		if(!i.second.empty())
		{
			std::string header = i.first;
			header += ": ";
			if (i.first == "User-Agent" || i.first == "Cookie")
			{
				header += i.second;
			}
			else
			{
				char* urlEncoded = curl_easy_escape(curl_, i.second.c_str(), static_cast<int>(i.second.size()));
				if(!urlEncoded)
				{
					header += i.second;
				}
				else
				{
					header += std::string(urlEncoded);
					curl_free(urlEncoded);
				}
			}
			auto tempList = curl_slist_append(slist, header.c_str());
			if(tempList)
			{
				slist = tempList;
			}
			else
			{
				curl_slist_free_all(slist);
				throw MDConnectionException(MD_CONNECTION_ERROR);
			}
		}
	}
	slist = curl_slist_append(slist, "Expect: ");
	curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, slist);
	curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &outStream);

	char errbuf[CURL_ERROR_SIZE];
	curl_easy_setopt(curl_, CURLOPT_ERRORBUFFER, errbuf);
	errbuf[0] = 0;

	std::vector<std::string> headersLines;
	curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION, ReadHeadersCallback);
	curl_easy_setopt(curl_, CURLOPT_HEADERDATA, &headersLines);

	CURLcode res = curl_easy_perform(curl_);
	curl_slist_free_all(slist);
	if(res != CURLE_OK)
	{
		size_t len = strlen(errbuf);
		if(len)
		{
			throw Opswat::MDConnectionException(errbuf);
		}
		else
		{
			throw Opswat::MDConnectionException(curl_easy_strerror(res));
		}
	}

	auto resp = Utils::make_unique<MDHttpResponse>();
	curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &resp->statusCode);

	resp->cookie = extractCookieFromHeaders(headersLines);

	return resp;
}


}

#endif
