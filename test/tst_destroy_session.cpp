#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;
class TestDestroySession : public ::testing::Test
{
public:
	TestDestroySession();
	Opswat::MDRest<SessionType> client;
};

TestDestroySession::TestDestroySession() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008))
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestDestroySession, NotOkStatus)
{
	MockHttpClient::expectedResp_.statusCode = 403;
	EXPECT_ANY_THROW(client.destroySession("a"));
}

TEST_F(TestDestroySession, OkStatusGoodJson)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"response\": \"success\"}";
	auto result = client.destroySession("a");
	auto parsedResult = result->parse();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_EQ("success", parsedResult->response);
}

TEST_F(TestDestroySession, OkStatusBadJson)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"session\": \"123\"}";
	auto result = client.destroySession("a");
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}
