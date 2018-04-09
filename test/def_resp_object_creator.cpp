#include <mdrestclient/md_utils.h>
#include "def_resp_object_creator.h"

std::unique_ptr<Opswat::MDFileInfo> DefRespObjectCreator::getFileInfoObject()
{
	return Opswat::Utils::make_unique<Opswat::MDFileInfo>(1337, "test.txt", "text/plain", "ASCII text", "md5val", "sha1val", "sha256", "2016-09-08");
}

std::unique_ptr<Opswat::MDExtendedProcessInfo> DefRespObjectCreator::getExtendedProcessInfoObject()
{
	auto processInfo = Opswat::Utils::make_unique<Opswat::MDExtendedProcessInfo>(false, "", "File scan", 100, "Allowed", "webscan");
	processInfo->postProcessInfo = getPostProcessInfoObject();
	return processInfo;
}

std::unique_ptr<Opswat::MDPostProcessInfo> DefRespObjectCreator::getPostProcessInfoObject()
{
	return Opswat::Utils::make_unique<Opswat::MDPostProcessInfo>("", "", "", "", "");
}

std::unique_ptr<Opswat::MDExtendedScanResult> DefRespObjectCreator::getExtendedScanResultObject()
{
	auto scanResult = Opswat::Utils::make_unique<Opswat::MDExtendedScanResult>("456", 100, "No threat detected", 0, "2016-09-08", 2, 567);
	scanResult->scanDetails["Engine1"] = getEngineScanResultObject();
	scanResult->scanDetails["Engine2"] = getEngineScanResultObject();
	scanResult->scanDetails["Engine2"]->engineName = "Engine2";
	return scanResult;
}

std::unique_ptr<Opswat::MDEngineScanResult> DefRespObjectCreator::getEngineScanResultObject()
{
	return Opswat::Utils::make_unique<Opswat::MDEngineScanResult>("Engine1", 0, "", 120, "2010-09-08", "local");
}

std::unique_ptr<Opswat::MDExtractedFileInfo> DefRespObjectCreator::getExtractedFileInfoObject()
{
	return Opswat::Utils::make_unique<Opswat::MDExtractedFileInfo>("a", 0, "sample.zip/a", 89, "image/jpeg", "JPEG image data", 100, 0, 4);
}

std::unique_ptr<Opswat::MDEngineInfo> DefRespObjectCreator::getEngineInfoObject()
{
	return Opswat::Utils::make_unique<Opswat::MDEngineInfo>(true, "2016", 100, "2016", "id1", "name1", "antivirusengine", "2.3", "av", "production", "engine");
}
