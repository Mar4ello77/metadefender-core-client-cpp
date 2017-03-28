#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;
class TestScanFile : public ::testing::Test
{
public:
	TestScanFile();
	Opswat::MDRest<SessionType> client;
	std::istringstream testData;
};

TestScanFile::TestScanFile() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008))
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestScanFile, NotOkStatus)
{
	MockHttpClient::expectedResp_.statusCode = 404;
	EXPECT_ANY_THROW(client.scanFile(testData));
}

TEST_F(TestScanFile, OkStatusGoodResponse)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"data_id\": \"123\"}";
	auto result = client.scanFile(testData);
	auto parsedResult = result->parse();
	EXPECT_EQ(result->getJSON(), MockHttpClient::expectedResp_.body);
	EXPECT_EQ(parsedResult->dataId, "123");
}

TEST_F(TestScanFile, OkStatusBadResponse)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"err\": \"Error\"}";
	auto result = client.scanFile(testData);
	EXPECT_EQ(result->getJSON(), MockHttpClient::expectedResp_.body);
	EXPECT_ANY_THROW(result->parse());
}
