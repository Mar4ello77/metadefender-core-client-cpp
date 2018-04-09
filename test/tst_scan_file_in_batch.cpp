#include "util.h"
#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"
#include "batch_json_builder.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;
class TestscanFileInBatch : public ::testing::Test
{
public:
	TestscanFileInBatch();
	Opswat::MDRest<SessionType> client;
	std::istringstream dataStream;
};

TestscanFileInBatch::TestscanFileInBatch() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008))
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestscanFileInBatch, NotOkStatusGoodResponse)
{
	const std::string expectedBatchResultJson = "{\"data_id\": \"default_data_id\"}";

	MockHttpClient::expectedResp_.statusCode = 400;
	MockHttpClient::expectedResp_.body = expectedBatchResultJson;
	EXPECT_ANY_THROW(client.scanFileInBatch(dataStream, "default_batch_id"));
}

TEST_F(TestscanFileInBatch, OkStatusGoodResponse)
{
	const std::string expectedBatchResultJson = "{\"data_id\": \"default_data_id\"}";

	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = expectedBatchResultJson;

	auto response = client.scanFileInBatch(dataStream, "default_data_id");
	auto result = response->parse();

	EXPECT_EQ(expectedBatchResultJson, response->getJSON());
	EXPECT_EQ(std::string("default_data_id"), result->dataId);
}

TEST_F(TestscanFileInBatch, OkStatusBadResponse)
{
	const std::string expectedBatchResultJson = "{\"err\": \"Error\"}";

	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = expectedBatchResultJson;

	auto response = client.scanFileInBatch(dataStream, "default_data_id");
	EXPECT_EQ(expectedBatchResultJson, response->getJSON());
	EXPECT_ANY_THROW(response->parse());
}