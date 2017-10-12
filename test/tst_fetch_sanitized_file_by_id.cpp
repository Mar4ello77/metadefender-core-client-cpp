#include <sstream>
#include "util.h"

#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"
#include "json_builder.h"

#include "def_resp_object_creator.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;
class TestFetchSanitizedFileById : public ::testing::Test
{
public:
	TestFetchSanitizedFileById();
	Opswat::MDRest<SessionType> client;
	std::string fakeId;
};

TestFetchSanitizedFileById::TestFetchSanitizedFileById() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008)), fakeId("123")
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestFetchSanitizedFileById, OkStatus)
{
	MockHttpClient::expectedResp_.body = "This is a sanitized file";
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::string result = client.fetchSanitizedFileById(fakeId);
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result);
}

TEST_F(TestFetchSanitizedFileById, OkStatusWriteIntoStream)
{
	MockHttpClient::expectedResp_.body = "This is a sanitized file";
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::stringstream outStream;
	client.fetchSanitizedFileById(fakeId, outStream);
	EXPECT_EQ(MockHttpClient::expectedResp_.body, outStream.str());
}

TEST_F(TestFetchSanitizedFileById, NotOkStatus)
{
	MockHttpClient::expectedResp_.body = "err: File could not be found";
	MockHttpClient::expectedResp_.statusCode = 404;
	MockHttpClient::expectedResp_.reasonPhrase = "Not Found";

	EXPECT_ANY_THROW(client.fetchSanitizedFileById(fakeId));
}

TEST_F(TestFetchSanitizedFileById, NotOkStatusWriteIntoStream)
{
	MockHttpClient::expectedResp_.body = "err: File could not be found";
	MockHttpClient::expectedResp_.statusCode = 404;
	MockHttpClient::expectedResp_.reasonPhrase = "Not Found";

	std::stringstream outStream;
	EXPECT_ANY_THROW(client.fetchSanitizedFileById(fakeId, outStream));
}
