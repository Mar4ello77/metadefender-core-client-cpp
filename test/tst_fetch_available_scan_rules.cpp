#include "util.h"
#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"
#include "json_builder.h"
#include "def_resp_object_creator.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;
class TestFetchAvailableScanRules : public ::testing::Test
{
public:
	TestFetchAvailableScanRules();
	Opswat::MDRest<SessionType> client;
};

TestFetchAvailableScanRules::TestFetchAvailableScanRules() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008))
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestFetchAvailableScanRules, NotOkStatus)
{
	MockHttpClient::expectedResp_.statusCode = 404;	
	EXPECT_ANY_THROW(client.fetchAvailableScanRules());
}

TEST_F(TestFetchAvailableScanRules, OkStatusWithEveryField)
{
	std::vector<std::unique_ptr<Opswat::MDScanRuleInfo>> scanRules;
	scanRules.push_back(Opswat::Utils::make_unique<Opswat::MDScanRuleInfo>("rule1", 100));
	scanRules.push_back(Opswat::Utils::make_unique<Opswat::MDScanRuleInfo>("rule2", 1000000));

	JsonBuilder builder;
	builder.addScanRules(scanRules);

	MockHttpClient::expectedResp_.body = builder.getJson();
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDAvailableScanRules>> result = client.fetchAvailableScanRules();
	std::unique_ptr<Opswat::MDAvailableScanRules> parsedResult = result->parse();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_TRUE(equal(parsedResult->scanRules, scanRules));
}
