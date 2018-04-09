#include "util.h"
#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"
#include "batch_json_builder.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;
class TestCloseBatch : public ::testing::Test
{
public:
	TestCloseBatch();
	Opswat::MDRest<SessionType> client;
};

TestCloseBatch::TestCloseBatch() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008))
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestCloseBatch, NotOkStatusGoodResponse)
{
	BatchResultJsonBuilder builder;
	auto expectedBatchResultJson = builder.getBatchResultJson();

	MockHttpClient::expectedResp_.statusCode = 400;
	MockHttpClient::expectedResp_.body = expectedBatchResultJson;

	EXPECT_ANY_THROW(client.closeBatch("default_batch_id"));
}

TEST_F(TestCloseBatch, OkStatusGoodResponse)
{
	BatchResultJsonBuilder builder;
	auto expectedBatchResultJson = builder.getBatchResultJson();
	auto expectedBatchResult = builder.getBatchResult();

	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = expectedBatchResultJson;

	auto response = client.closeBatch("default_batch_id");
	auto result = response->parse();

	EXPECT_EQ(expectedBatchResultJson, response->getJSON());
	EXPECT_TRUE(equal(expectedBatchResult, result));
}

TEST_F(TestCloseBatch, OkStatusFilesInBatchExcluded)
{
	BatchResultJsonBuilder builder;
	builder.areFilesInBatchIncluded = false;
	auto expectedBatchResultJson = builder.getBatchResultJson();
	auto expectedBatchResult = builder.getBatchResult();
	expectedBatchResult->batchFiles->firstIndex = 0;
	expectedBatchResult->batchFiles->pageSize = 0;
	expectedBatchResult->batchFiles->filesInBatch.clear();

	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = expectedBatchResultJson;

	auto response = client.closeBatch("default_batch_id");
	auto result = response->parse();

	EXPECT_EQ(expectedBatchResultJson, response->getJSON());
	EXPECT_TRUE(equal(expectedBatchResult, result));
}

TEST_F(TestCloseBatch, OkStatusBadResponse)
{
	auto expectedBatchResultJson = "{\"err\": \"Error\"}";

	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = expectedBatchResultJson;

	auto response = client.closeBatch("default_batch_id");
	EXPECT_EQ(expectedBatchResultJson, response->getJSON());
	EXPECT_ANY_THROW(response->parse());
}
