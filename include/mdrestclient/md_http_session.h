#ifndef _MD_HTTP_SESSION_H
#define _MD_HTTP_SESSION_H

#include "md_http_client.h"
#include "md_utils.h"
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace Opswat
{

/// @brief Base class for Http session
///
/// @tparam HttpClient Http client type instanced by this session
template<typename HttpClient>
class IMDHttpSession
{
public:

	/// @brief Constructor
	///
	/// @param serverAddress Remote server's IP address. Format: X.X.X.X or http://X.X.X.X
	/// @param port Remote server's port
	IMDHttpSession(const std::string& serverAddress, int port);

	/// @bried Destructor
	virtual ~IMDHttpSession();

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

	/// @brief Returns the IP address of the server this session is connected to
	///
	/// @return Remote server's IP address
	std::string getServerAddress() const;

	/// @brief Returns the port of the server this session is connected to
	///
	/// @return Remote server's port
	int getPort() const;

private:

	std::string serverAddress_;
	int port_;
};

/// @brief This class represents a persistent Http session
///
/// The utilized Http client is instanced only once.
template<typename HttpClient>
class MDPersistentHttpSession : public IMDHttpSession<HttpClient>
{
public:

	MDPersistentHttpSession(const std::string& serverAddress, int port);

	std::unique_ptr<MDHttpResponse> sendRequest(MDHttpRequest& request);
	std::unique_ptr<MDHttpResponse> sendRequest(MDHttpRequest& request, std::ostream& outStream);

private:

	std::unique_ptr<HttpClient> client_;
};

/// @brief This class represents a simple Http session
///
/// One Http client is instanced for every requests.
template<typename HttpClient>
class MDSimpleHttpSession : public IMDHttpSession<HttpClient>
{
public:

	MDSimpleHttpSession(const std::string& serverAddress, int port);

	std::unique_ptr<MDHttpResponse> sendRequest(MDHttpRequest& request);
	std::unique_ptr<MDHttpResponse> sendRequest(MDHttpRequest& request, std::ostream& outStream);
};

template<typename HttpClient>
IMDHttpSession<HttpClient>::IMDHttpSession(const std::string& serverAddress, int port) : serverAddress_(serverAddress), port_(port)
{
}

template<typename HttpClient>
IMDHttpSession<HttpClient>::~IMDHttpSession()
{
}

template<typename HttpClient>
std::string IMDHttpSession<HttpClient>::getServerAddress() const
{
	return serverAddress_;
}

template<typename HttpClient>
int IMDHttpSession<HttpClient>::getPort() const
{
	return port_;
}

template<typename HttpClient>
MDPersistentHttpSession<HttpClient>::MDPersistentHttpSession(const std::string& serverAddress, int port) : IMDHttpSession<HttpClient>(serverAddress, port), client_(Utils::make_unique<HttpClient>(serverAddress, port))
{
}

template<typename HttpClient>
std::unique_ptr<MDHttpResponse> MDPersistentHttpSession<HttpClient>::sendRequest(MDHttpRequest& request)
{
	return client_->sendRequest(request);
}

template<typename HttpClient>
std::unique_ptr<MDHttpResponse> MDPersistentHttpSession<HttpClient>::sendRequest(MDHttpRequest& request, std::ostream& outStream)
{
	return client_->sendRequest(request, outStream);
}

template<typename HttpClient>
MDSimpleHttpSession<HttpClient>::MDSimpleHttpSession(const std::string& serverAddress, int port) : IMDHttpSession<HttpClient>(serverAddress, port)
{
}

template<typename HttpClient>
std::unique_ptr<MDHttpResponse> MDSimpleHttpSession<HttpClient>::sendRequest(MDHttpRequest& request)
{
	HttpClient client(this->getServerAddress(), this->getPort());	
	return client.sendRequest(request);
}

template<typename HttpClient>
std::unique_ptr<MDHttpResponse> MDSimpleHttpSession<HttpClient>::sendRequest(MDHttpRequest& request, std::ostream& outStream)
{
	HttpClient client(this->getServerAddress(), this->getPort());
	return client.sendRequest(request, outStream);
}

};

#endif
