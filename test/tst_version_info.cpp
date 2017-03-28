#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;

class TestVersionInfo : public ::testing::Test
{
public:
	TestVersionInfo();
	Opswat::MDRest<SessionType> client;
};

TestVersionInfo::TestVersionInfo() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008))
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestVersionInfo, OkJson)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"version\": \"4.0\", \"product_id\": \"MSCL\"}";
	auto result = client.getVersionInfo();	
	auto parsedResult = result->parse();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_EQ("4.0", parsedResult->version);
	EXPECT_EQ("MSCL", parsedResult->productId);
}

TEST_F(TestVersionInfo, BadJson)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"version\": \"4.0\", \"production_id\": \"MSCL\"}";
	auto result = client.getVersionInfo();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}
