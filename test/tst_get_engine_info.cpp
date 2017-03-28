#include "util.h"
#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"
#include "json_builder.h"
#include "def_resp_object_creator.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;
class TestGetEngineInfo : public ::testing::Test
{
public:
	TestGetEngineInfo();
	Opswat::MDRest<SessionType> client;
};

TestGetEngineInfo::TestGetEngineInfo() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008))
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestGetEngineInfo, NotOkStatus)
{
	MockHttpClient::expectedResp_.statusCode = 404;
	EXPECT_ANY_THROW(client.getEngineInfo());
}

TEST_F(TestGetEngineInfo, OkStatusWithEveryField)
{
	std::vector<std::unique_ptr<Opswat::MDEngineInfo>> engineInfos;
	auto engineInfo1 = DefRespObjectCreator::getEngineInfoObject();
	auto engineInfo2 = DefRespObjectCreator::getEngineInfoObject();
	engineInfo2->engineName = "engine2";
	engineInfos.push_back(std::move(engineInfo1));
	engineInfos.push_back(std::move(engineInfo2));

	JsonBuilder builder;
	builder.addEngineInfos(engineInfos);

	MockHttpClient::expectedResp_.body = builder.getJson();
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDEnginesList>> result = client.getEngineInfo();
	std::unique_ptr<Opswat::MDEnginesList> parsedResult = result->parse();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_TRUE(equal(parsedResult->engines, engineInfos));
}

TEST_F(TestGetEngineInfo, OkStatusWithMissingField)
{
	std::vector<std::unique_ptr<Opswat::MDEngineInfo>> engineInfos;
	auto engineInfo1 = DefRespObjectCreator::getEngineInfoObject();
	engineInfos.push_back(std::move(engineInfo1));

	JsonBuilder builder;
	builder.addEngineInfos(engineInfos);
	builder.doc_.Begin()->RemoveMember("active");

	MockHttpClient::expectedResp_.body = builder.getJson();
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDEnginesList>> result = client.getEngineInfo();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}

TEST_F(TestGetEngineInfo, OkStatusWithBadJson)
{
	MockHttpClient::expectedResp_.body = "{\"test\": \"bad\"}";
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDEnginesList>> result = client.getEngineInfo();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}
