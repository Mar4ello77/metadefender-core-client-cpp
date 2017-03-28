#ifndef _MD_HTTP_CLIENT_H
#define _MD_HTTP_CLIENT_H

#include <map>
#include <vector>
#include <memory>

namespace Opswat
{

	enum class HTTP_METHOD
	{
		HTTP_GET	= 0,
		HTTP_POST	= 1,
	};

/// \brief Http response returned by Metadefender Rest
///
/// Rest calls are carried out via http. This structure represents a http response returned
/// by the Metadefender Rest server.
///
/// @var MDHttpResponse::statusCode Returned http status code
/// @var MDHttpResponse::reasonPhrase Holds the returned http reason phrase
/// @var MDHttpResponse::body Holds the returned http body
struct MDHttpResponse
{
	int statusCode;
	std::string reasonPhrase;
	std::string body;
};

/// \brief Http response returned by Metadefender Rest
///
/// Rest calls are carried out via http. This structure represents a http request sent to the
/// Metadefender Rest server.
///
/// @var MDHttpRequest::method Http request method
/// @var MDHttpRequest::url URI for the Metadefender Rest server
/// @var MDHttpRequest::headers Http headers
/// @var MDHttpRequest::inStream Stream to read the body from
struct MDHttpRequest
{
	HTTP_METHOD method;
	std::string url;
	std::map<std::string, std::string> headers;
	std::istream* inStream;
};

/// @brief Base class for Http connection
class IMDHttpClient
{
public:

	/// @brief Constructor
	IMDHttpClient() {}

	/// @brief Destructor
	virtual ~IMDHttpClient() {}

	/// @brief Sends a http request
	///
	/// @param request Http request to be sent
	/// @return Http response for the initiated request
	virtual std::unique_ptr<MDHttpResponse> sendRequest(MDHttpRequest& request) = 0;

	/// @brief Sends a http request
	///
	/// @param request Http request to be sent
	/// @param outStream Stream that the response body will be written into
	/// @return Http response for the initiated request
	virtual std::unique_ptr<MDHttpResponse> sendRequest(MDHttpRequest& request, std::ostream& outStream) = 0;
};

};

#endif
