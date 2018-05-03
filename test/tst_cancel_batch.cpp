#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;
class TestCancelBatch : public ::testing::Test
{
public:
	TestCancelBatch();
	Opswat::MDRest<SessionType> client;
};

TestCancelBatch::TestCancelBatch() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008))
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestCancelBatch, NotOkStatus)
{
	MockHttpClient::expectedResp_.statusCode = 400;
	EXPECT_ANY_THROW(client.closeBatch("default_batch_id"));
}

TEST_F(TestCancelBatch, OkStatusGoodResponse)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "\"default_batch_id\": \"cancelled\"";
	client.cancelBatch("default_batch_id");
}
