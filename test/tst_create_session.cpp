#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;
class TestCreateSession : public ::testing::Test
{
public:
	TestCreateSession();
	Opswat::MDRest<SessionType> client;
};

TestCreateSession::TestCreateSession() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008))
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestCreateSession, NotOkStatus)
{
	MockHttpClient::expectedResp_.statusCode = 403;
	EXPECT_ANY_THROW(client.createSession("admin", "admin"));
}

TEST_F(TestCreateSession, OkStatusGoodJson)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"session_id\": \"123\"}";
	auto result = client.createSession("admin", "admin");
	auto parsedResult = result->parse();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_EQ("123", parsedResult->session_id);
}

TEST_F(TestCreateSession, OkStatusBadJson)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"session\": \"123\"}";
	auto result = client.createSession("admin", "admin");
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}
