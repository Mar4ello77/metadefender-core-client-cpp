#define RAPIDJSON_HAS_STDSTRING 1

#include "def_resp_object_creator.h"

#include "json_builder.h"

JsonBuilder::JsonBuilder()
{
	doc_.SetObject();
}

void JsonBuilder::addFileScanResult(std::unique_ptr<Opswat::MDFileScanResult> &fileScanResult)
{
	addDataId(fileScanResult->dataId);
	addExtractedFiles(fileScanResult->extractedFiles);
	addFileInfo(fileScanResult->fileInfo);
	addScanResults(fileScanResult->scanResult);
	addProcessInfo(fileScanResult->processInfo);
}

void JsonBuilder::createFileScanResult()
{
	addDataId("123");
	std::vector<std::unique_ptr<Opswat::MDExtractedFileInfo>> extractedFileInfos;
	extractedFileInfos.push_back(DefRespObjectCreator::getExtractedFileInfoObject());
	extractedFileInfos.push_back(DefRespObjectCreator::getExtractedFileInfoObject());
	extractedFileInfos.back()->displayName = "turtle";
	addExtractedFiles(extractedFileInfos);
	addFileInfo(DefRespObjectCreator::getFileInfoObject());
	addScanResults(DefRespObjectCreator::getExtendedScanResultObject());
	addProcessInfo(DefRespObjectCreator::getExtendedProcessInfoObject());
}

void JsonBuilder::addDataId(const std::string& dataId)
{
	doc_.AddMember("data_id", dataId, doc_.GetAllocator());
}

void JsonBuilder::addExtractedFiles(std::vector<std::unique_ptr<Opswat::MDExtractedFileInfo>> &extractedFileInfo)
{
	rapidjson::Value extractedFiles(rapidjson::kObjectType);
	rapidjson::Value filesInArchive(rapidjson::kArrayType);
	for (auto &i : extractedFileInfo)
	{
		rapidjson::Value file(rapidjson::kObjectType);
		file.AddMember("data_id", i->dataId, doc_.GetAllocator());
		file.AddMember("detected_by", i->detectedBy, doc_.GetAllocator());
		file.AddMember("display_name", i->displayName, doc_.GetAllocator());
		file.AddMember("file_size", i->fileSize, doc_.GetAllocator());
		file.AddMember("file_type", i->fileType, doc_.GetAllocator());
		file.AddMember("file_type_description", i->fileTypeDescription, doc_.GetAllocator());
		file.AddMember("progress_percentage", i->progressPercentage, doc_.GetAllocator());
		file.AddMember("scan_all_result_i", i->scanAllResultCode, doc_.GetAllocator());
		file.AddMember("scanned_with", i->scannedWith, doc_.GetAllocator());
		filesInArchive.PushBack(file, doc_.GetAllocator());
	}
	extractedFiles.AddMember("files_in_archive", filesInArchive, doc_.GetAllocator());
	doc_.AddMember("extracted_files", extractedFiles, doc_.GetAllocator());
}

void JsonBuilder::addFileInfo(const std::unique_ptr<Opswat::MDFileInfo> &fileInfo)
{
	rapidjson::Value fileInfoValue(rapidjson::kObjectType);
	fileInfoValue.AddMember("display_name", fileInfo->displayName, doc_.GetAllocator());
	fileInfoValue.AddMember("file_size", fileInfo->fileSize, doc_.GetAllocator());
	fileInfoValue.AddMember("file_type", fileInfo->fileType, doc_.GetAllocator());
	fileInfoValue.AddMember("file_type_description", fileInfo->fileTypeDesc, doc_.GetAllocator());
	fileInfoValue.AddMember("md5", fileInfo->md5, doc_.GetAllocator());
	fileInfoValue.AddMember("sha1", fileInfo->sha1, doc_.GetAllocator());
	fileInfoValue.AddMember("sha256", fileInfo->sha256, doc_.GetAllocator());
	fileInfoValue.AddMember("upload_timestamp", fileInfo->uploadDate, doc_.GetAllocator());
	doc_.AddMember("file_info", fileInfoValue, doc_.GetAllocator());
}

void JsonBuilder::addScanResults(const std::unique_ptr<Opswat::MDExtendedScanResult> &scanResult, bool lowVisibility)
{
	rapidjson::Value scanResultValue(rapidjson::kObjectType);
	scanResultValue.AddMember("data_id", scanResult->id, doc_.GetAllocator());
	scanResultValue.AddMember("progress_percentage", scanResult->processPercentage, doc_.GetAllocator());
	scanResultValue.AddMember("scan_all_result_a", scanResult->scanAllResultDesc, doc_.GetAllocator());
	scanResultValue.AddMember("scan_all_result_i", scanResult->scanAllResultCode, doc_.GetAllocator());
	scanResultValue.AddMember("start_time", scanResult->startDate, doc_.GetAllocator());
	scanResultValue.AddMember("total_avs", scanResult->numEngines, doc_.GetAllocator());
	scanResultValue.AddMember("total_time", scanResult->scanDuration, doc_.GetAllocator());
	if (scanResult->scanDetails.size() > 0)
	{
		rapidjson::Value scanDetails(rapidjson::kObjectType);
		for (auto &i : scanResult->scanDetails)
		{
			rapidjson::Value engineDetail(rapidjson::kObjectType);
			if (!lowVisibility) {
				engineDetail.AddMember("def_time", i.second->virusDefDate, doc_.GetAllocator());
				engineDetail.AddMember("location", i.second->location, doc_.GetAllocator());
				engineDetail.AddMember("scan_time", i.second->scanDuration, doc_.GetAllocator());
			}
			engineDetail.AddMember("scan_result_i", i.second->scanResultCode, doc_.GetAllocator());
			engineDetail.AddMember("threat_found", i.second->threat, doc_.GetAllocator());
			rapidjson::Value key(i.second->engineName, doc_.GetAllocator());
			scanDetails.AddMember(key, engineDetail, doc_.GetAllocator());
		}
		scanResultValue.AddMember("scan_details", scanDetails, doc_.GetAllocator());
	}
	doc_.AddMember("scan_results", scanResultValue, doc_.GetAllocator());
}

void JsonBuilder::addProcessInfo(const std::unique_ptr<Opswat::MDExtendedProcessInfo> &processInfo)
{
	rapidjson::Value processInfoValue(rapidjson::kObjectType);
	if (processInfo->postProcessInfo)
	{
		rapidjson::Value postProcessInfo(rapidjson::kObjectType);
		postProcessInfo.AddMember("actions_ran", processInfo->postProcessInfo->actionsRun, doc_.GetAllocator());
		postProcessInfo.AddMember("actions_failed", processInfo->postProcessInfo->actionsFailed, doc_.GetAllocator());
		postProcessInfo.AddMember("converted_to", processInfo->postProcessInfo->convertedTo, doc_.GetAllocator());
		postProcessInfo.AddMember("copy_move_destination", processInfo->postProcessInfo->copyMoveDestination, doc_.GetAllocator());
		postProcessInfo.AddMember("converted_destination", processInfo->postProcessInfo->convertedDestination, doc_.GetAllocator());
		processInfoValue.AddMember("post_processing", postProcessInfo, doc_.GetAllocator());
	}
	processInfoValue.AddMember("progress_percentage", processInfo->processPercentage, doc_.GetAllocator());
	processInfoValue.AddMember("user_agent", processInfo->userAgent, doc_.GetAllocator());
	processInfoValue.AddMember("profile", processInfo->profile, doc_.GetAllocator());
	processInfoValue.AddMember("result", processInfo->result, doc_.GetAllocator());
	processInfoValue.AddMember("blocked_reason", processInfo->blockedReason, doc_.GetAllocator());
	processInfoValue.AddMember("file_type_skipped_scan", processInfo->skippedFileType, doc_.GetAllocator());
	doc_.AddMember("process_info", processInfoValue, doc_.GetAllocator());
}

std::string JsonBuilder::getJson()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc_.Accept(writer);

	return buffer.GetString();
}

void JsonBuilder::addEngineInfos(std::vector<std::unique_ptr<Opswat::MDEngineInfo>> &engineInfos)
{
	doc_.SetArray();
	for (auto &i : engineInfos)
	{
		rapidjson::Value engineInfo(rapidjson::kObjectType);
		engineInfo.AddMember("active", i->active, doc_.GetAllocator());
		engineInfo.AddMember("def_time", i->defTime, doc_.GetAllocator());
		engineInfo.AddMember("download_progress", i->downloadProgress, doc_.GetAllocator());
		engineInfo.AddMember("download_time", i->downloadTime, doc_.GetAllocator());
		engineInfo.AddMember("eng_id", i->engineId, doc_.GetAllocator());
		engineInfo.AddMember("eng_name", i->engineName, doc_.GetAllocator());
		engineInfo.AddMember("eng_type", i->engineTypeDescription, doc_.GetAllocator());
		engineInfo.AddMember("engine_type", i->engineType, doc_.GetAllocator());
		engineInfo.AddMember("eng_ver", i->engineVersion, doc_.GetAllocator());
		engineInfo.AddMember("state", i->state, doc_.GetAllocator());
		engineInfo.AddMember("type", i->type, doc_.GetAllocator());
		doc_.PushBack(engineInfo, doc_.GetAllocator());
	}
}

void JsonBuilder::addScanRules(const std::vector<std::unique_ptr<Opswat::MDScanRuleInfo>> &scanRules)
{
	doc_.SetArray();
	for (const auto& i : scanRules)
	{
		rapidjson::Value scanRuleInfo(rapidjson::kObjectType);
		scanRuleInfo.AddMember("max_file_size", i->maxFileSize, doc_.GetAllocator());
		scanRuleInfo.AddMember("name", i->name, doc_.GetAllocator());
		doc_.PushBack(scanRuleInfo, doc_.GetAllocator());
	}
}