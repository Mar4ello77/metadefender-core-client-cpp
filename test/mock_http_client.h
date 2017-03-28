#ifndef _MOCK_HTTP_CLIENT_H_
#define _MOCK_HTTP_CLIENT_H_

#include <mdrestclient/md_http_client.h>

class MockHttpClient : public Opswat::IMDHttpClient
{
public:
	MockHttpClient(const std::string &serverAddress, short port) : IMDHttpClient() {}
	~MockHttpClient() {}
	std::unique_ptr<Opswat::MDHttpResponse> sendRequest(Opswat::MDHttpRequest& request);
	std::unique_ptr<Opswat::MDHttpResponse> sendRequest(Opswat::MDHttpRequest& request, std::ostream& outStream);

	static void resetExpectedResp();
	static Opswat::MDHttpResponse expectedResp_;
};

#endif