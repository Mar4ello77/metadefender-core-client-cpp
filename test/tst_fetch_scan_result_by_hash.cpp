#include "util.h"
#include "gtest/gtest.h"

#include <mdrestclient/md_rest.h>
#include <mdrestclient/md_utils.h>

#include "mock_http_client.h"
#include "json_builder.h"
#include "def_resp_object_creator.h"

using SessionType = Opswat::MDSimpleHttpSession<MockHttpClient>;
class TestFetchScanResultByHash : public ::testing::Test
{
public:
	TestFetchScanResultByHash();
	Opswat::MDRest<SessionType> client;
	std::string fakeHash;
};

TestFetchScanResultByHash::TestFetchScanResultByHash() : client(Opswat::Utils::make_unique<SessionType>(std::string("172.16.201.23"), 8008)), fakeHash("123")
{
	MockHttpClient::resetExpectedResp();
}

TEST_F(TestFetchScanResultByHash, NotOkStatus)
{
	MockHttpClient::expectedResp_.statusCode = 404;
	EXPECT_ANY_THROW(client.fetchScanResultByHash(fakeHash));
}

TEST_F(TestFetchScanResultByHash, OkStatusWithNotFoundId)
{
	std::string notFoundJson = "{ \"123\" : \"Not found\" }";

	MockHttpClient::expectedResp_.body = notFoundJson;
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDFileScanResult>> result = client.fetchScanResultByHash(fakeHash);
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}

TEST_F(TestFetchScanResultByHash, OkStatusWithEveryField)
{
	std::string dataId = "123";
	auto fileInfoObj = DefRespObjectCreator::getFileInfoObject();
	auto processInfoObj = DefRespObjectCreator::getProcessInfoObject();
	auto scanResultsObj = DefRespObjectCreator::getScanResultObject();
	std::vector<std::unique_ptr<Opswat::MDExtractedFileInfo>> extractedFileInfos;
	auto extracted1 = DefRespObjectCreator::getExtractedFileInfoObject();
	auto extracted2 = DefRespObjectCreator::getExtractedFileInfoObject();
	extracted2->displayName = "turtle";
	extractedFileInfos.push_back(std::move(extracted1));
	extractedFileInfos.push_back(std::move(extracted2));

	JsonBuilder builder;
	builder.addDataId(dataId);
	builder.addExtractedFiles(extractedFileInfos);
	builder.addFileInfo(fileInfoObj);
	builder.addProcessInfo(processInfoObj);
	builder.addScanResults(scanResultsObj);

	MockHttpClient::expectedResp_.body = builder.getJson();
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDFileScanResult>> result = client.fetchScanResultByHash(fakeHash);
	std::unique_ptr<Opswat::MDFileScanResult> parsedResult = result->parse();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_EQ(parsedResult->dataId, dataId);
	EXPECT_TRUE(equal(parsedResult->extractedFiles, extractedFileInfos));
	EXPECT_TRUE(equal(parsedResult->fileInfo, fileInfoObj));
	EXPECT_TRUE(equal(parsedResult->processInfo, processInfoObj));
	EXPECT_TRUE(equal(parsedResult->scanResult, scanResultsObj));
}

TEST_F(TestFetchScanResultByHash, OkStatusWithMissingFileInfo)
{
	std::string dataId = "123";
	auto processInfoObj = DefRespObjectCreator::getProcessInfoObject();
	auto scanResultsObj = DefRespObjectCreator::getScanResultObject();
	std::vector<std::unique_ptr<Opswat::MDExtractedFileInfo>> extractedFileInfos;
	auto extracted1 = DefRespObjectCreator::getExtractedFileInfoObject();
	auto extracted2 = DefRespObjectCreator::getExtractedFileInfoObject();
	extracted2->displayName = "turtle";
	extractedFileInfos.push_back(std::move(extracted1));
	extractedFileInfos.push_back(std::move(extracted2));

	JsonBuilder builder;
	builder.addDataId(dataId);
	builder.addExtractedFiles(extractedFileInfos);
	builder.addProcessInfo(processInfoObj);
	builder.addScanResults(scanResultsObj);

	MockHttpClient::expectedResp_.body = builder.getJson();
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDFileScanResult>> result = client.fetchScanResultByHash(fakeHash);
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}

TEST_F(TestFetchScanResultByHash, OkStatusWithMissingExtractedFiles)
{
	std::string dataId = "123";
	auto fileInfoObj = DefRespObjectCreator::getFileInfoObject();
	auto processInfoObj = DefRespObjectCreator::getProcessInfoObject();
	auto scanResultsObj = DefRespObjectCreator::getScanResultObject();

	JsonBuilder builder;
	builder.addDataId(dataId);
	builder.addFileInfo(fileInfoObj);
	builder.addProcessInfo(processInfoObj);
	builder.addScanResults(scanResultsObj);

	MockHttpClient::expectedResp_.body = builder.getJson();
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDFileScanResult>> result = client.fetchScanResultByHash(fakeHash);
	std::unique_ptr<Opswat::MDFileScanResult> parsedResult = result->parse();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_EQ(parsedResult->dataId, dataId);
	EXPECT_TRUE(equal(parsedResult->fileInfo, fileInfoObj));
	EXPECT_TRUE(equal(parsedResult->processInfo, processInfoObj));
	EXPECT_TRUE(equal(parsedResult->scanResult, scanResultsObj));
}

TEST_F(TestFetchScanResultByHash, OkStatusWithMissingProcessInfo)
{
	std::string dataId = "123";
	auto fileInfoObj = DefRespObjectCreator::getFileInfoObject();
	auto scanResultsObj = DefRespObjectCreator::getScanResultObject();

	JsonBuilder builder;
	builder.addDataId(dataId);
	builder.addFileInfo(fileInfoObj);
	builder.addScanResults(scanResultsObj);

	MockHttpClient::expectedResp_.body = builder.getJson();
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDFileScanResult>> result = client.fetchScanResultByHash(fakeHash);
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}

TEST_F(TestFetchScanResultByHash, OkStatusWithMissingPostProcessing)
{
	std::string dataId = "123";
	auto fileInfoObj = DefRespObjectCreator::getFileInfoObject();
	auto processInfoObj = DefRespObjectCreator::getProcessInfoObject();
	processInfoObj->postProcessInfo.release();
	auto scanResultsObj = DefRespObjectCreator::getScanResultObject();

	JsonBuilder builder;
	builder.addDataId(dataId);
	builder.addFileInfo(fileInfoObj);
	builder.addProcessInfo(processInfoObj);
	builder.addScanResults(scanResultsObj);

	MockHttpClient::expectedResp_.body = builder.getJson();
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDFileScanResult>> result = client.fetchScanResultByHash(fakeHash);
	std::unique_ptr<Opswat::MDFileScanResult> parsedResult = result->parse();
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_EQ(parsedResult->dataId, dataId);
	EXPECT_TRUE(equal(parsedResult->fileInfo, fileInfoObj));
	EXPECT_TRUE(equal(parsedResult->processInfo, processInfoObj));
	EXPECT_TRUE(equal(parsedResult->scanResult, scanResultsObj));
}

TEST_F(TestFetchScanResultByHash, OkStatusWithMissingScanResult)
{
	std::string dataId = "123";
	auto fileInfoObj = DefRespObjectCreator::getFileInfoObject();
	auto processInfoObj = DefRespObjectCreator::getProcessInfoObject();

	JsonBuilder builder;
	builder.addDataId(dataId);
	builder.addFileInfo(fileInfoObj);
	builder.addProcessInfo(processInfoObj);

	MockHttpClient::expectedResp_.body = builder.getJson();
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDFileScanResult>> result = client.fetchScanResultByHash(fakeHash);
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}

TEST_F(TestFetchScanResultByHash, OkStatusWithMissingScanResultDetails)
{
	std::string dataId = "123";
	auto fileInfoObj = DefRespObjectCreator::getFileInfoObject();
	auto processInfoObj = DefRespObjectCreator::getProcessInfoObject();
	auto scanResultsObj = DefRespObjectCreator::getScanResultObject();
	scanResultsObj->numEngines = 0;
	scanResultsObj->scanDetails.clear();

	JsonBuilder builder;
	builder.addDataId(dataId);
	builder.addFileInfo(fileInfoObj);
	builder.addProcessInfo(processInfoObj);
	builder.addScanResults(scanResultsObj);

	MockHttpClient::expectedResp_.body = builder.getJson();
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDFileScanResult>> result = client.fetchScanResultByHash(fakeHash);
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}

TEST_F(TestFetchScanResultByHash, OkStatusWithMissingDataId)
{
	auto fileInfoObj = DefRespObjectCreator::getFileInfoObject();
	auto processInfoObj = DefRespObjectCreator::getProcessInfoObject();
	auto scanResultsObj = DefRespObjectCreator::getScanResultObject();

	JsonBuilder builder;
	builder.addFileInfo(fileInfoObj);
	builder.addProcessInfo(processInfoObj);
	builder.addScanResults(scanResultsObj);

	MockHttpClient::expectedResp_.body = builder.getJson();
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDFileScanResult>> result = client.fetchScanResultByHash(fakeHash);
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}

TEST_F(TestFetchScanResultByHash, OkStatusWithMissingChildField)
{
	std::string dataId = "123";
	auto fileInfoObj = DefRespObjectCreator::getFileInfoObject();
	auto processInfoObj = DefRespObjectCreator::getProcessInfoObject();
	auto scanResultsObj = DefRespObjectCreator::getScanResultObject();

	JsonBuilder builder;
	builder.addDataId(dataId);
	builder.addFileInfo(fileInfoObj);
	builder.addProcessInfo(processInfoObj);
	builder.addScanResults(scanResultsObj);
	builder.doc_["process_info"].RemoveMember("result");

	MockHttpClient::expectedResp_.body = builder.getJson();
	MockHttpClient::expectedResp_.statusCode = 200;
	MockHttpClient::expectedResp_.reasonPhrase = "OK";

	std::unique_ptr<Opswat::MDResponse<Opswat::MDFileScanResult>> result = client.fetchScanResultByHash(fakeHash);
	EXPECT_EQ(MockHttpClient::expectedResp_.body, result->getJSON());
	EXPECT_ANY_THROW(result->parse());
}
