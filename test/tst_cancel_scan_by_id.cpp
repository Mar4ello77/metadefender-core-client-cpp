#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>

#include "mock_http_client.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;
class TestCancelScanById : public ::testing::Test
{
	public:
		TestCancelScanById();
		Opswat::MDRest<SessionType> client;
		std::string fakeId;
};

TestCancelScanById::TestCancelScanById()
	: client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008)),
	  fakeId("123")
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestCancelScanById, NotOkStatus)
{
	MockHttpClient::expectedResp_.statusCode = 404;
	EXPECT_ANY_THROW(client.cancelScanById(fakeId));
}

TEST_F(TestCancelScanById, CancelOk)
{
	MockHttpClient::expectedResp_.body = "{\"123\": \"cancelled\"}";
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::string result = client.cancelScanById(fakeId);
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result);
}

