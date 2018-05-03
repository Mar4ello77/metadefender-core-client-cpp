#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;
class TestInitBatch : public ::testing::Test
{
public:
	TestInitBatch();
	Opswat::MDRest<SessionType> client;
};

TestInitBatch::TestInitBatch() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008))
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestInitBatch, NotOkStatusGoodResponse)
{
	MockHttpClient::expectedResp_.statusCode = 400;
	MockHttpClient::expectedResp_.body = "{\"batch_id\": \"default_batch_id\"}";
	EXPECT_ANY_THROW(client.initBatch("userData", "rule", "userAgent"));
}

TEST_F(TestInitBatch, OkStatusGoodResponse)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"batch_id\": \"default_batch_id\"}";
	auto result = client.initBatch("userData", "rule", "userAgent");
	auto parsedResult = result->parse();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_EQ("default_batch_id", parsedResult->batchId);
}

TEST_F(TestInitBatch, OkStatusBadResponse)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"err\": \"Error\"}";
	auto result = client.initBatch("userData", "rule", "userAgent");
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}
