#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;

class TestScanFileByPath : public ::testing::Test
{
public:
	TestScanFileByPath();
	Opswat::MDRest<SessionType> client;
	std::string fakePath;
};

TestScanFileByPath::TestScanFileByPath() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008)), fakePath("/path/test.txt")
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestScanFileByPath, NotOkStatus)
{
	MockHttpClient::expectedResp_.statusCode = 404;
	EXPECT_ANY_THROW(client.scanLocalFile(fakePath));
}

TEST_F(TestScanFileByPath, OkStatusGoodResponse)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"data_id\": \"123\"}";
	auto result = client.scanLocalFile(fakePath);
	auto parsedResult = result->parse();
	EXPECT_EQ(result->getJSON(), MockHttpClient::expectedResp_.body);
	EXPECT_EQ(parsedResult->dataId, "123");
}

TEST_F(TestScanFileByPath, OkStatusBadResponse)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"err\": \"Error\"}";
	auto result = client.scanLocalFile(fakePath);
	EXPECT_EQ(result->getJSON(), MockHttpClient::expectedResp_.body);
	EXPECT_ANY_THROW(result->parse());
}
