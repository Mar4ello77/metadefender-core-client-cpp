#include <ostream>
#include <mdrestclient/md_utils.h>
#include "mock_http_client.h"

Opswat::MDHttpResponse MockHttpClient::expectedResp_ = Opswat::MDHttpResponse();

std::unique_ptr<Opswat::MDHttpResponse> MockHttpClient::sendRequest(Opswat::MDHttpRequest& request)
{
	return Opswat::Utils::make_unique<Opswat::MDHttpResponse>(expectedResp_);
}

std::unique_ptr<Opswat::MDHttpResponse> MockHttpClient::sendRequest(Opswat::MDHttpRequest& request, std::ostream& outStream)
{
	auto res = expectedResp_;
	outStream.write(res.body.c_str(), res.body.size());
	res.body.clear();
	return Opswat::Utils::make_unique<Opswat::MDHttpResponse>(res);
}

void MockHttpClient::resetExpectedResp()
{
	expectedResp_ = Opswat::MDHttpResponse();
}
