#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;
class TestLicenseInfo : public ::testing::Test
{
public:
	TestLicenseInfo();
	Opswat::MDRest<SessionType> client;
};

TestLicenseInfo::TestLicenseInfo() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008))
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestLicenseInfo, OkJson)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"days_left\": 10, \"deployment\": \"ab\", \"expiration\": \"2016\", \"licensed_engines\": \"eng1\", \
										    \"licensed_to\": \"opswat\", \"max_agent_count\" : 23, \"online_activated\": false, \"product_id\": \"ms4\", \
											\"product_name\": \"metascan\", \"is_extra\": false}";
	auto result = client.getLicenseInfo();
	auto parsedResult = result->parse();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_EQ(10, parsedResult->daysLeft);
	EXPECT_EQ("ab", parsedResult->deployment);
	EXPECT_FALSE(parsedResult->onlineActivated);
	EXPECT_TRUE(parsedResult->isValid);
	EXPECT_EQ(parsedResult->additionalLicenseInfo["expiration"], "2016");
	EXPECT_EQ(parsedResult->additionalLicenseInfo["licensed_engines"], "eng1");
	EXPECT_EQ(parsedResult->additionalLicenseInfo["licensed_to"], "opswat");
	EXPECT_EQ(parsedResult->additionalLicenseInfo["max_agent_count"], "23");
	EXPECT_EQ(parsedResult->additionalLicenseInfo["product_id"], "ms4");
	EXPECT_EQ(parsedResult->additionalLicenseInfo["product_name"], "metascan");
	EXPECT_EQ(parsedResult->additionalLicenseInfo["is_extra"], "false");
}

TEST_F(TestLicenseInfo, OkJsonNoValidLicense)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"deployment\": \"asd\", \"days_left\": 0, \"online_activated\": false}";
	auto result = client.getLicenseInfo();
	auto parsedResult = result->parse();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_EQ(0, parsedResult->daysLeft);
	EXPECT_EQ("asd", parsedResult->deployment);
	EXPECT_FALSE(parsedResult->onlineActivated);
	EXPECT_FALSE(parsedResult->isValid);
	EXPECT_EQ(parsedResult->additionalLicenseInfo.size(), 0);
}

TEST_F(TestLicenseInfo, BadJson)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"deployment\": \"ab\", \"expiration\": \"2016\", \"licensed_engines\": \"eng1\" \
											\"licensed_to\": \"opswat\", \"max_agent_count\" : \"23\", \"online_activated\": false, \"product_id\": \"ms4\" \
											\"product_name\": \"metascan\"}"; //days_left missing
	auto result = client.getLicenseInfo();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}

TEST_F(TestLicenseInfo, NotOk)
{
	MockHttpClient::expectedResp_.statusCode = 400;
	MockHttpClient::expectedResp_.body = "{\"err\": \"error\"}";
	EXPECT_ANY_THROW(client.getLicenseInfo());	
}
