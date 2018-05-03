#ifndef _BATCH_JSON_BUILDER_H
#define _BATCH_JSON_BUILDER_H

#include <mdrestclient/md_rest_response_objects.h>
#include <mdrestclient/md_rapidjson_wrapper.h>
#include <memory>
#include <string>
#include <vector>

class BaseProcessInfoBuilder
{
public:
	BaseProcessInfoBuilder(std::string blockedReason = "default_blocked_reason",
		bool fileTypeSkippedScan = false,
		std::string profile = "default_profile",
		std::string result = "default_result",
		std::string userAgent = "default_user_agent") : blockedReason(std::move(blockedReason)),
									fileTypeSkippedScan(fileTypeSkippedScan),
									profile(std::move(profile)),
									result(std::move(result)),
									userAgent(std::move(userAgent))
	{
	}

	std::unique_ptr<Opswat::MDBaseProcessInfo> getBaseProcessInfo() const
	{
		return Opswat::Utils::make_unique<Opswat::MDBaseProcessInfo>(fileTypeSkippedScan, blockedReason, profile, result, userAgent);
	}

public:
	std::string blockedReason;
	bool fileTypeSkippedScan;
	std::string profile;
	std::string result;
	std::string userAgent;
};

class BaseScanResultsBuilder
{
public:
	BaseScanResultsBuilder(std::string id = "default_id",
		std::string scanAllResultDesc = "default_scan_all_result_desc",
		int scanAllResultCode = 2,
		std::string startTime = "1000",
		int numEngine = 4,
		int64_t totalTime = 1000) : batchId(std::move(batchId)),
								scanAllResultDesc(std::move(scanAllResultDesc)),
								scanAllResultCode(scanAllResultCode),
								startTime(std::move(startTime)),
								numEngine(numEngine),
								totalTime(totalTime)
	{
	}

	std::unique_ptr<Opswat::MDBaseScanResult> getBaseScanResults() const
	{
		return Opswat::Utils::make_unique<Opswat::MDBaseScanResult>(batchId, scanAllResultDesc, scanAllResultCode, startTime, numEngine, totalTime);
	}

public:

	std::string batchId;
	std::string scanAllResultDesc;
	int scanAllResultCode;
	std::string startTime;
	int numEngine;
	int64_t totalTime;
};

class BatchFileProcessInfoBuilder
{
public:
	BatchFileProcessInfoBuilder(std::string blockedReason = "default_blocked_reason",
		int progressPercentage = 60,
		std::string result = "default_result") : blockedReason(blockedReason),
								progressPercentage(progressPercentage),
								result(result)
	{
	}

	std::unique_ptr<Opswat::MDBatchFileProcessInfo> getBatchFileProcessInfo() const
	{
		return Opswat::Utils::make_unique<Opswat::MDBatchFileProcessInfo>(blockedReason, progressPercentage, result);
	}

public:

	std::string blockedReason;
	int progressPercentage;
	std::string result;
};

class FileInBatchBuilder
{
public:
	FileInBatchBuilder(std::string dataId = "default_data_id",
		int detectedBy = 1,
		std::string displayName = "default_display_name",
		int64_t fileSize = 1024,
		std::string fileType = "default_file_type",
		std::string fileTypeDesc = "default_file_type_desc",
		BatchFileProcessInfoBuilder processInfoBuilder = BatchFileProcessInfoBuilder(),
		int progressPercentage = 50,
		std::string scanAllResultDesc = "default_scan_all_result_desc",
		int scanAllResultCode = 2,
		int scannedWith = 4) : dataId(std::move(dataId)),
							detectedBy(detectedBy),
							displayName(std::move(displayName)),
							fileSize(fileSize),
							fileType(std::move(fileType)),
							fileTypeDesc(std::move(fileTypeDesc)),
							processInfoBuilder(std::move(processInfoBuilder)),
							progressPercentage(progressPercentage),
							scanAllResultDesc(std::move(scanAllResultDesc)),
							scanAllResultCode(scanAllResultCode),
							scannedWith(scannedWith)
	{
	}

	std::unique_ptr<Opswat::MDFileInBatch> getFileInBatch() const
	{
		return Opswat::Utils::make_unique<Opswat::MDFileInBatch>(dataId,
			detectedBy,
			displayName,
			fileSize,
			fileType,
			fileTypeDesc,
			processInfoBuilder.getBatchFileProcessInfo(),
			progressPercentage,
			scanAllResultDesc,
			scanAllResultCode,
			scannedWith
			);
	}

public:
	std::string dataId;
	int detectedBy;
	std::string displayName;
	int64_t fileSize;
	std::string fileType;
	std::string fileTypeDesc;
	BatchFileProcessInfoBuilder processInfoBuilder;
	int progressPercentage;
	std::string scanAllResultDesc;
	int scanAllResultCode;
	int scannedWith;
};

class BatchFilesBuilder
{
public:
	BatchFilesBuilder(int batchCount = 1,
		int firstIndex = 0,
		int pageSize = 50,
		std::vector<FileInBatchBuilder> fileInBatchBuilders = {FileInBatchBuilder()}) : batchCount(batchCount),
																					firstIndex(firstIndex),
																					pageSize(pageSize),
																					fileInBatchBuilders(std::move(fileInBatchBuilders))
	{
	}

	BatchFilesBuilder& addFileInBatch(FileInBatchBuilder fileInBatchBuilder)
	{
		fileInBatchBuilders.push_back(std::move(fileInBatchBuilder));
		return *this;
	}

	std::unique_ptr<Opswat::MDBatchFiles> getBatchFiles() const
	{
		std::vector<std::unique_ptr<Opswat::MDFileInBatch>> filesInBatch;
		for (const auto &i : fileInBatchBuilders)
		{
			filesInBatch.push_back(i.getFileInBatch());
		}
		return Opswat::Utils::make_unique<Opswat::MDBatchFiles>(batchCount, firstIndex, pageSize, std::move(filesInBatch));
	}

public:

	int batchCount;
	int firstIndex;
	int pageSize;
	std::vector<FileInBatchBuilder> fileInBatchBuilders;
};

class BatchResultJsonBuilder
{
public:
	BatchResultJsonBuilder(std::string batchId = "default_batch_id",
		bool isClosed = false,
		std::string userData = "default_user_data",
		BaseProcessInfoBuilder processInfoBuilder = BaseProcessInfoBuilder(),
		BaseScanResultsBuilder scanResultsBuilder = BaseScanResultsBuilder(),
		BatchFilesBuilder batchFilesBuilder = BatchFilesBuilder()) : batchId(std::move(batchId)),
																	isClosed(isClosed),
																	userData(std::move(userData)),
																	processInfoBuilder(std::move(processInfoBuilder)),
																	scanResultsBuilder(std::move(scanResultsBuilder)),
																	batchFilesBuilder(std::move(batchFilesBuilder)),
																	areFilesInBatchIncluded(true)
	{}

	std::string getBatchResultJson();

	std::unique_ptr<Opswat::MDBatchResult> getBatchResult() const;

private:

	void appendProcessInfoToJson();
	void appendScanResultsToJson();
	void appendBatchFilesToJson();

public:

	rapidjson::Document doc;
	std::string batchId;
	bool isClosed;
	std::string userData;
	BaseProcessInfoBuilder processInfoBuilder;
	BaseScanResultsBuilder scanResultsBuilder;
	BatchFilesBuilder batchFilesBuilder;
	bool areFilesInBatchIncluded;
};

#endif