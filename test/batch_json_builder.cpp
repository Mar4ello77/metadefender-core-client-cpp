#define RAPIDJSON_HAS_STDSTRING 1
#include "batch_json_builder.h"

std::string BatchResultJsonBuilder::getBatchResultJson()
{
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.SetObject();
	doc.AddMember("batch_id", batchId, allocator);
	doc.AddMember("is_closed", isClosed, allocator);
	doc.AddMember("user_data", userData, allocator);

	appendProcessInfoToJson();
	appendScanResultsToJson();
	appendBatchFilesToJson();

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);

	return buffer.GetString();
}

void BatchResultJsonBuilder::appendProcessInfoToJson()
{
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	auto processInfo = processInfoBuilder.getBaseProcessInfo();
	rapidjson::Value processInfoJson(rapidjson::kObjectType);
	processInfoJson.AddMember("blocked_reason", processInfo->blockedReason, allocator);
	processInfoJson.AddMember("file_type_skipped_scan", processInfo->skippedFileType, allocator);
	processInfoJson.AddMember("profile", processInfo->profile, allocator);
	processInfoJson.AddMember("result", processInfo->result, allocator);
	processInfoJson.AddMember("user_agent", processInfo->userAgent, allocator);
	doc.AddMember("process_info", processInfoJson, allocator);
}

void BatchResultJsonBuilder::appendScanResultsToJson()
{
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	auto scanResult = scanResultsBuilder.getBaseScanResults();
	rapidjson::Value scanResultJson(rapidjson::kObjectType);
	scanResultJson.AddMember("batch_id", scanResult->id, allocator);
	scanResultJson.AddMember("scan_all_result_a", scanResult->scanAllResultDesc, allocator);
	scanResultJson.AddMember("scan_all_result_i", scanResult->scanAllResultCode, allocator);
	scanResultJson.AddMember("start_time", scanResult->startDate, allocator);
	scanResultJson.AddMember("total_avs", scanResult->numEngines, allocator);
	scanResultJson.AddMember("total_time", scanResult->scanDuration, allocator);
	doc.AddMember("scan_results", scanResultJson, allocator);
}

void BatchResultJsonBuilder::appendBatchFilesToJson()
{
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	auto batchFiles = batchFilesBuilder.getBatchFiles();
	rapidjson::Value batchFilesJson(rapidjson::kObjectType);
	batchFilesJson.AddMember("batch_count", batchFiles->batchCount, allocator);
	if (areFilesInBatchIncluded)
	{
		batchFilesJson.AddMember("first_index", batchFiles->firstIndex, allocator);
		batchFilesJson.AddMember("page_size", batchFiles->pageSize, allocator);
		rapidjson::Value filesInBatchJson(rapidjson::kArrayType);
		for (auto &i : batchFiles->filesInBatch)
		{
			rapidjson::Value fileJson(rapidjson::kObjectType);
			fileJson.AddMember("data_id", i->dataId, allocator);
			fileJson.AddMember("detected_by", i->detectedBy, allocator);
			fileJson.AddMember("display_name", i->displayName, allocator);
			fileJson.AddMember("file_size", i->fileSize, allocator);
			fileJson.AddMember("file_type", i->fileType, allocator);
			fileJson.AddMember("file_type_description", i->fileTypeDesc, allocator);
			fileJson.AddMember("progress_percentage", i->progressPercentage, allocator);
			fileJson.AddMember("scan_all_result_a", i->scanAllResultDesc, allocator);
			fileJson.AddMember("scan_all_result_i", i->scanAllResultCode, allocator);
			fileJson.AddMember("scanned_with", i->scannedWith, allocator);

			rapidjson::Value processInfoJson(rapidjson::kObjectType);
			processInfoJson.AddMember("blocked_reason", i->processInfo->blockedReason, allocator);
			processInfoJson.AddMember("progress_percentage", i->processInfo->progressPercentage, allocator);
			processInfoJson.AddMember("result", i->processInfo->result, allocator);

			fileJson.AddMember("process_info", processInfoJson, allocator);

			filesInBatchJson.PushBack(fileJson, allocator);
		}
		batchFilesJson.AddMember("files_in_batch", filesInBatchJson, allocator);
	}
	doc.AddMember("batch_files", batchFilesJson, allocator);
}

std::unique_ptr<Opswat::MDBatchResult> BatchResultJsonBuilder::getBatchResult() const
{
	auto batchResult = Opswat::Utils::make_unique<Opswat::MDBatchResult>();
	batchResult->batchId = batchId;
	batchResult->isClosed = isClosed;
	batchResult->userData = userData;
	batchResult->batchFiles = batchFilesBuilder.getBatchFiles();
	batchResult->processInfo = processInfoBuilder.getBaseProcessInfo();
	batchResult->scanResult = scanResultsBuilder.getBaseScanResults();

	return std::move(batchResult);
}