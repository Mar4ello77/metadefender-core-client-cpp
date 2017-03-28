#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;

class MockUselessHttpClient : public Opswat::IMDHttpClient
{
public:
	MockUselessHttpClient(const std::string &serverAddress, short port) : IMDHttpClient() {}
	~MockUselessHttpClient() {}
	std::unique_ptr<Opswat::MDHttpResponse> sendRequest(Opswat::MDHttpRequest& request) { throw Opswat::MDConnectionException("connection error"); }
	std::unique_ptr<Opswat::MDHttpResponse> sendRequest(Opswat::MDHttpRequest& request, std::ostream& outStream) { throw Opswat::MDConnectionException("connection error"); }
};

class TestExceptions : public ::testing::Test
{
public:
	TestExceptions();
	Opswat::MDRest<SessionType> client;
};

TestExceptions::TestExceptions() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008))
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestExceptions, NotOkStatusWithError)
{
	MockHttpClient::expectedResp_.statusCode = 404;
	MockHttpClient::expectedResp_.body = "{\"err\": \"test error\"}";
	try
	{
		auto result = client.getVersionInfo();
		FAIL();
	}
	catch (Opswat::MDErrorResponseException& e)
	{
		EXPECT_EQ(e.getResponseStatusCode(), MockHttpClient::expectedResp_.statusCode);
		EXPECT_EQ(e.getResponseBody(), MockHttpClient::expectedResp_.body);
		EXPECT_EQ(std::string(e.what()), "test error");
	}
	catch (...)
	{
		FAIL();
	}
}

TEST_F(TestExceptions, NotOkStatusWithoutError)
{
	MockHttpClient::expectedResp_.statusCode = 404;
	MockHttpClient::expectedResp_.body = "{\"test\": \"test error\"}";
	try
	{
		auto result = client.getVersionInfo();
		FAIL();
	}
	catch (Opswat::MDErrorResponseException& e)
	{
		EXPECT_EQ(e.getResponseStatusCode(), MockHttpClient::expectedResp_.statusCode);
		EXPECT_EQ(e.getResponseBody(), MockHttpClient::expectedResp_.body);
		EXPECT_EQ(std::string(e.what()), Opswat::MD_METASCAN_ERROR);
	}
	catch (...)
	{
		FAIL();
	}
}

TEST_F(TestExceptions, OkStatusWithBadJson)
{
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.body = "{\"bad\": \"test error\"}";
	try
	{
		auto result = client.getVersionInfo();
		result->parse();
		FAIL();
	}
	catch (Opswat::MDParsingException& e)
	{
		EXPECT_EQ(e.getJson(), MockHttpClient::expectedResp_.body);
		EXPECT_EQ(std::string(e.what()), Opswat::MD_JSON_PARSING_ERROR);
	}
	catch (...)
	{
		FAIL();
	}
}

TEST_F(TestExceptions, ConnectionError)
{
	using UselessSessionType = Opswat::MDSimpleHttpSession<MockUselessHttpClient>;
	Opswat::MDRest<UselessSessionType> uselessClient(Opswat::Utils::make_unique<UselessSessionType>(std::string("172.16.201.23"), 8008));
	try
	{
		auto result = uselessClient.getVersionInfo();
		FAIL();
	}
	catch (Opswat::MDConnectionException& e)
	{
		EXPECT_EQ(std::string(e.what()), "connection error");
	}
	catch (...)
	{
		FAIL();
	}
}
