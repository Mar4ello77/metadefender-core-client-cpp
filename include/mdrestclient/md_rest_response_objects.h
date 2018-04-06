#ifndef _MD_REST_RESPONSE_OBJECTS_H
#define _MD_REST_RESPONSE_OBJECTS_H

#include "md_rest_response.h"
#include "md_rapidjson_wrapper.h"
#include "md_utils.h"
#include <string>
#include <map>
#include <memory>
#include <vector>

namespace Opswat
{

struct MDFileScanId
{
	MDFileScanId(const std::string& fileScanIdJSON)
	{
		try
		{
			rapidjson::Document document;
			document.Parse(fileScanIdJSON.c_str());
			dataId = document["data_id"].GetString();
		}
		catch(MDException& e)
		{
			throw MDParsingException(e.what(), fileScanIdJSON);
		}
	}

public:
	std::string dataId;
};

struct MDFileInfo
{
	MDFileInfo(int64_t fileSize,
		const std::string& displayName,
		const std::string& fileType,
		const std::string& fileTypeDesc,
		const std::string& md5,
		const std::string& sha1,
		const std::string& sha256,
		const std::string& uploadDate) : fileSize(fileSize),
										displayName(displayName),
										fileType(fileType),
										fileTypeDesc(fileTypeDesc),
										md5(md5),
										sha1(sha1),
										sha256(sha256),
										uploadDate(uploadDate)
	{
	}

	int64_t fileSize;
	std::string displayName;
	std::string fileType;
	std::string fileTypeDesc;
	std::string md5;
	std::string sha1;
	std::string sha256;
	std::string uploadDate;
};

struct MDEngineScanResult
{
	MDEngineScanResult(const std::string& engineName,
		int scanResultCode,
		const std::string& threat,
		int64_t duration,
		const std::string& virusDefDate,
		const std::string& location) : engineName(engineName),
										scanResultCode(scanResultCode),
										threat(threat),
										scanDuration(duration),
										virusDefDate(virusDefDate),
										location(location)
	{
	}

	std::string engineName;
	int scanResultCode;
	std::string threat;
	int64_t scanDuration;
	std::string virusDefDate;
	std::string location;
};

struct MDBaseScanResult
{
	MDBaseScanResult(const std::string& id,
		const std::string& scanAllResultDesc,
		int scanAllResultCode,
		const std::string& startDate,
		int numEngines,
		int64_t scanDuration) : id(id),
								scanAllResultDesc(scanAllResultDesc),
								scanAllResultCode(scanAllResultCode),
								startDate(startDate),
								numEngines(numEngines),
								scanDuration(scanDuration)
	{
	}

	std::string id;
	std::string scanAllResultDesc;
	int scanAllResultCode;
	std::string startDate;
	int numEngines;
	int64_t scanDuration;
};

struct MDExtendedScanResult : public MDBaseScanResult
{
	MDExtendedScanResult(const std::string& id,
		int processPercentage,
		const std::string& scanAllResultDesc,
		int scanAllResultCode,
		const std::string& startDate,
		int numEngines,
		int64_t scanDuration) : MDBaseScanResult(id, scanAllResultDesc, scanAllResultCode, startDate, numEngines, scanDuration),
								processPercentage(processPercentage)
	{
	}

	int processPercentage;
	std::map<std::string, std::unique_ptr<MDEngineScanResult>> scanDetails;
};

struct MDPostProcessInfo
{
	MDPostProcessInfo(const std::string& actionRun,
					const std::string& actionFailed,
					const std::string& convertedTo,
					const std::string& copyMoveDestination,
					const std::string& convertedDeestination) : actionsRun(actionRun),
																actionsFailed(actionFailed),
																convertedTo(convertedTo),
																copyMoveDestination(copyMoveDestination),
																convertedDestination(convertedDeestination)
	{
	}

	std::string actionsRun;
	std::string actionsFailed;
	std::string convertedTo;
	std::string copyMoveDestination;
	std::string convertedDestination;
};

struct MDBaseProcessInfo
{
	MDBaseProcessInfo(bool skipped,
		const std::string& blockedReason,
		const std::string& profile,
		const std::string& result,
		const std::string& userAgent) : skippedFileType(skipped),
										blockedReason(blockedReason),
										profile(profile),
										result(result),
										userAgent(userAgent)
	{
	}

	bool skippedFileType;
	std::string blockedReason;
	std::string profile;
	std::string result;
	std::string userAgent;
};

struct MDExtendedProcessInfo : public MDBaseProcessInfo
{
	MDExtendedProcessInfo(bool skipped,
		const std::string& blockedReason,
		const std::string& profile,
		int processPercentage,
		const std::string& result,
		const std::string& userAgent) : MDBaseProcessInfo(skipped, blockedReason, profile, result, userAgent),
										processPercentage(processPercentage)
	{
	}

	int processPercentage;
	std::unique_ptr<MDPostProcessInfo> postProcessInfo;
};

struct MDExtractedFileInfo
{
	MDExtractedFileInfo(const std::string& dataId,
		int detectedBy,
		const std::string& displayName,
		int64_t fileSize,
		const std::string& fileType,
		const std::string& fileTypeDescription,
		int progressPercentage,
		int scanAllResultCode,
		int scannedWith) : dataId(dataId),
									detectedBy(detectedBy),
									displayName(displayName),
									fileSize(fileSize),
									fileType(fileType),
									fileTypeDescription(fileTypeDescription),
									progressPercentage(progressPercentage),
									scanAllResultCode(scanAllResultCode),
									scannedWith(scannedWith)
	{
	}

	std::string dataId;
	int detectedBy;
	std::string displayName;
	int64_t fileSize;
	std::string fileType;
	std::string fileTypeDescription;
	int progressPercentage;
	int scanAllResultCode;
	int scannedWith;
};

struct MDFileScanResult
{
	MDFileScanResult(const std::string& fileScanResultJSON)
	{
		try
		{
			rapidjson::Document document;
			document.Parse(fileScanResultJSON.c_str());

			if(document.GetObject().MemberCount() == 1 && document.MemberBegin()->value.GetString() == std::string("Not Found"))
			{
				std::string errorMessage = document.MemberBegin()->value.GetString();
				errorMessage += " : ";
				errorMessage += document.MemberBegin()->name.GetString();
				throw MDException(errorMessage);
			}

			dataId = document["data_id"].GetString();

			auto fileInfoObject = document["file_info"].GetObject();
			fileInfo = Utils::make_unique<MDFileInfo>(fileInfoObject["file_size"].GetInt64(),
				fileInfoObject["display_name"].GetString(),
				fileInfoObject["file_type"].GetString(),
				fileInfoObject["file_type_description"].GetString(),
				fileInfoObject["md5"].GetString(),
				fileInfoObject["sha1"].GetString(),
				fileInfoObject["sha256"].GetString(),
				fileInfoObject["upload_timestamp"].GetString()
				);

			if(document.HasMember("scan_results"))
			{
				auto scanResultsObject = document["scan_results"].GetObject();
				scanResult = Utils::make_unique<MDExtendedScanResult>(scanResultsObject["data_id"].GetString(),
					scanResultsObject["progress_percentage"].GetInt(),
					scanResultsObject["scan_all_result_a"].GetString(),
					scanResultsObject["scan_all_result_i"].GetInt(),
					scanResultsObject["start_time"].GetString(),
					scanResultsObject["total_avs"].GetInt(),
					scanResultsObject["total_time"].GetInt64()
					);

				auto scanDetailsObject = scanResultsObject["scan_details"].GetObject();
				for(const auto& elem : scanDetailsObject)
				{
					std::string engineName = elem.name.GetString();
					scanResult->scanDetails[engineName] = Utils::make_unique<MDEngineScanResult>(engineName,
						elem.value["scan_result_i"].GetInt(),
						elem.value["threat_found"].GetString(),
						elem.value.HasMember("scan_time") ? elem.value["scan_time"].GetInt64() : 0,
						elem.value.HasMember("def_time")  ? elem.value["def_time"].GetString() : std::string(),
						elem.value.HasMember("location")  ? elem.value["location"].GetString() : std::string()
						);
				}
			}

			auto procInfoObject = document["process_info"].GetObject();
			processInfo = Utils::make_unique<MDExtendedProcessInfo>(
				procInfoObject["file_type_skipped_scan"].GetBool(),
				procInfoObject["blocked_reason"].GetString(),
				procInfoObject["profile"].GetString(),
				procInfoObject["progress_percentage"].GetInt(),
				procInfoObject["result"].GetString(),
				procInfoObject["user_agent"].GetString()
				);

			if(procInfoObject.HasMember("post_processing"))
			{
				auto postProcInfoObject = procInfoObject["post_processing"].GetObject();
				processInfo->postProcessInfo = Utils::make_unique<MDPostProcessInfo>(
					postProcInfoObject["actions_ran"].GetString(),
					postProcInfoObject["actions_failed"].GetString(),
					postProcInfoObject["converted_to"].GetString(),
					postProcInfoObject["copy_move_destination"].GetString(),
					postProcInfoObject["converted_destination"].GetString()
					);
			}

			if(document.HasMember("extracted_files"))
			{
				auto extractedFilesObject = document["extracted_files"].GetObject();
				if(extractedFilesObject.HasMember("files_in_archive"))
				{
					auto filesInArchiveArray = extractedFilesObject["files_in_archive"].GetArray();
					for(auto& elem : filesInArchiveArray)
					{
						extractedFiles.push_back(Utils::make_unique<MDExtractedFileInfo>(elem["data_id"].GetString(),
							elem["detected_by"].GetInt(),
							elem["display_name"].GetString(),
							elem["file_size"].GetInt64(),
							elem["file_type"].GetString(),
							elem["file_type_description"].GetString(),
							elem["progress_percentage"].GetInt(),
							elem["scan_all_result_i"].GetInt(),
							elem["scanned_with"].GetInt()
							));
					}
				}
			}
		}
		catch(MDException& e)
		{
			throw MDParsingException(e.what(), fileScanResultJSON);
		}
	}

	std::string dataId;
	std::unique_ptr<MDFileInfo> fileInfo;
	std::unique_ptr<MDExtendedScanResult> scanResult;
	std::unique_ptr<MDExtendedProcessInfo> processInfo;
	std::vector<std::unique_ptr<MDExtractedFileInfo>> extractedFiles;
};

struct MDVersionInfo
{
	MDVersionInfo(const std::string& versionInfoJSON)
	{
		try
		{
			rapidjson::Document document;
			document.Parse(versionInfoJSON.c_str());
			version = document["version"].GetString();
			productId = document["product_id"].GetString();
		}
		catch(MDException& e)
		{
			throw MDParsingException(e.what(), versionInfoJSON);
		}
	}

	std::string version;
	std::string productId;
};

struct MDLicenseInfo
{
	MDLicenseInfo(const std::string& licenseInfoJSON)
	{
		try
		{
			rapidjson::Document document;
			document.Parse(licenseInfoJSON.c_str());
			deployment = document["deployment"].GetString();
			document.RemoveMember("deployment");
			daysLeft = document["days_left"].GetInt();
			document.RemoveMember("days_left");
			onlineActivated = document["online_activated"].GetBool();
			document.RemoveMember("online_activated");
			if(daysLeft <= 0)
			{
				isValid = false;
			}
			else
			{
				isValid = true;
				for(auto i = document.MemberBegin(); i != document.MemberEnd(); ++i)
				{
					if(i->value.IsString())
					{
						additionalLicenseInfo[i->name.GetString()] = i->value.GetString();
					}
					else
					{
						rapidjson::StringBuffer buffer;
						rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
						i->value.Accept(writer);
						additionalLicenseInfo[i->name.GetString()] = buffer.GetString();
					}
				}
			}
		}
		catch(MDException& e)
		{
			throw MDParsingException(e.what(), licenseInfoJSON);
		}
	}

	std::string deployment;
	int daysLeft;
	bool isValid;
	bool onlineActivated;

	std::map<std::string, std::string> additionalLicenseInfo;
};

struct MDLoginInfo
{
	MDLoginInfo(const std::string& loginInfoJSON)
	{
		try
		{
			rapidjson::Document document;
			document.Parse(loginInfoJSON.c_str());
			session_id = document["session_id"].GetString();
		}
		catch(MDException& e)
		{
			throw MDParsingException(e.what(), loginInfoJSON);
		}
	}

	std::string session_id;
};

struct MDLogoutInfo
{
	MDLogoutInfo(const std::string& logoutInfoJSON)
	{
		try
		{
			rapidjson::Document document;
			document.Parse(logoutInfoJSON.c_str());
			response = document["response"].GetString();
		}
		catch(MDException& e)
		{
			throw MDParsingException(e.what(), logoutInfoJSON);
		}
	}

	std::string response;
};

struct MDEngineInfo
{
	MDEngineInfo(bool active,
		const std::string& defTime,
		int downloadProgress,
		const std::string& downloadTime,
		const std::string& engineId,
		const std::string engineName,
		const std::string engineTpeDescription,
		const std::string engineVersion,
		const std::string engineType,
		const std::string state,
		const std::string type) : active(active),
								defTime(defTime),
								downloadProgress(downloadProgress),
								downloadTime(downloadTime),
								engineId(engineId),
								engineName(engineName),
								engineTypeDescription(engineTpeDescription),
								engineVersion(engineVersion),
								engineType(engineType),
								state(state),
								type(type)
	{
	}

	bool active;
	std::string defTime;
	int downloadProgress;
	std::string downloadTime;
	std::string engineId;
	std::string engineName;
	std::string engineTypeDescription;
	std::string engineVersion;
	std::string engineType;
	std::string state;
	std::string type;
};

struct MDEnginesList
{
	MDEnginesList(const std::string& engineInfoJSON)
	{
		try
		{
			rapidjson::Document document;
			document.Parse(engineInfoJSON.c_str());
			for(auto& e : document.GetArray())
			{
				engines.push_back(Utils::make_unique<MDEngineInfo>(e["active"].GetBool(),
					e["def_time"].GetString(),
					e["download_progress"].GetInt(),
					e["download_time"].GetString(),
					e["eng_id"].GetString(),
					e["eng_name"].GetString(),
					e["eng_type"].GetString(),
					e["eng_ver"].GetString(),
					e["engine_type"].GetString(),
					e["state"].GetString(),
					e["type"].GetString()));
			}
		}
		catch(MDException& e)
		{
			throw MDParsingException(e.what(), engineInfoJSON);
		}
	}

	std::vector<std::unique_ptr<MDEngineInfo>> engines;
};

struct MDErrorInfo
{
	MDErrorInfo(const std::string& errorInfoJSON)
	{
		try
		{
			rapidjson::Document document;
			document.Parse(errorInfoJSON.c_str());
			errorMessage = document["err"].GetString();
		}
		catch(MDException& e)
		{
			throw MDParsingException(e.what(), errorInfoJSON);
		}
	}

	std::string errorMessage;
};

struct MDScanRuleInfo
{
	MDScanRuleInfo(const std::string& name, int64_t maxFileSize) : name(name),
		maxFileSize(maxFileSize)
	{}

	std::string name;
    int64_t maxFileSize;
};

struct MDAvailableScanRules
{
	MDAvailableScanRules(const std::string &scanRulesJSON)
	{
		try {
			rapidjson::Document document;
			document.Parse(scanRulesJSON.c_str());
			for (auto &rule : document.GetArray())
			{
				scanRules.push_back(Utils::make_unique<MDScanRuleInfo>(
					rule["name"].GetString(),
					rule["max_file_size"].GetInt64()));
			}
		}
		catch (MDException& e)
		{
			throw MDParsingException(e.what(), scanRulesJSON);
		}
	}

	std::vector<std::unique_ptr<MDScanRuleInfo>> scanRules;
};

struct MDInitBatch
{
	MDInitBatch(const std::string& batchIdJSON)
	{
		try {
			rapidjson::Document document;
			document.Parse(batchIdJSON.c_str());
			batchId = document["batch_id"].GetString();
		}
		catch(MDException &e)
		{
			throw MDParsingException(e.what(), batchIdJSON);
		}
	}

	public:
		std::string batchId;
};

struct MDBatchFileProcessInfo
{
	MDBatchFileProcessInfo(const std::string& blockedReason,
		int progressPercentage,
		const std::string& result) : blockedReason(blockedReason),
										progressPercentage(progressPercentage),
										result(result)
	{
	}

	std::string blockedReason;
	int progressPercentage;
	std::string result;
};

struct MDFileInBatch
{
	MDFileInBatch(const std::string dataId,
		int detectedBy,
		const std::string& displayName,
		int64_t fileSize,
		const std::string& fileType,
		const std::string& fileTypeDesc,
		std::unique_ptr<MDBatchFileProcessInfo> processInfo,
		int progressPercentage,
		const std::string& scanAllResultDesc,
		int scanAllResultCode,
		int scannedWith) : dataId(dataId),
							detectedBy(detectedBy),
							displayName(displayName),
							fileSize(fileSize),
							fileType(fileType),
							fileTypeDesc(fileTypeDesc),
							processInfo(std::move(processInfo)),
							progressPercentage(progressPercentage),
							scanAllResultDesc(scanAllResultDesc),
							scanAllResultCode(scanAllResultCode),
							scannedWith(scannedWith)
	{
	}

	std::string dataId;
	int detectedBy;
	std::string displayName;
	int64_t fileSize;
	std::string fileType;
	std::string fileTypeDesc;
	std::unique_ptr<MDBatchFileProcessInfo> processInfo;
	int progressPercentage;
	std::string scanAllResultDesc;
	int scanAllResultCode;
	int scannedWith;
};

struct MDBatchFiles
{
	MDBatchFiles(int batchCount,
		int firstIndex,
		int pageSize,
		std::vector<std::unique_ptr<MDFileInBatch>> filesInBatch) : batchCount(batchCount),
																	firstIndex(firstIndex),
																	pageSize(pageSize),
																	filesInBatch(std::move(filesInBatch))
	{
	}

	int batchCount;
	int firstIndex;
	int pageSize;
	std::vector<std::unique_ptr<MDFileInBatch>> filesInBatch;
};

struct MDBatchResult
{
	MDBatchResult(const std::string& batchResultJSON)
	{
		try
		{
			rapidjson::Document document;
			document.Parse(batchResultJSON.c_str());

			batchId = document["batch_id"].GetString();
			isClosed = document["is_closed"].GetBool();
			userData = document["user_data"].GetString();

			auto batchFilesObject = document["batch_files"].GetObject();

			std::vector<std::unique_ptr<MDFileInBatch>> filesInBatch;
			if (batchFilesObject.HasMember("files_in_batch"))
			{
				auto filesInBatchArray = batchFilesObject["files_in_batch"].GetArray();
				for(auto& fileObject : filesInBatchArray)
				{
					auto fileProcessInfoObject = fileObject["process_info"].GetObject();

					filesInBatch.push_back(Utils::make_unique<MDFileInBatch>(fileObject["data_id"].GetString(),
						fileObject["detected_by"].GetInt(),
						fileObject["display_name"].GetString(),
						fileObject["file_size"].GetInt64(),
						fileObject["file_type"].GetString(),
						fileObject["file_type_description"].GetString(),
						Utils::make_unique<MDBatchFileProcessInfo>(fileProcessInfoObject["blocked_reason"].GetString(),
							fileProcessInfoObject["progress_percentage"].GetInt(),
							fileProcessInfoObject["result"].GetString()),
						fileObject["progress_percentage"].GetInt(),
						fileObject["scan_all_result_a"].GetString(),
						fileObject["scan_all_result_i"].GetInt(),
						fileObject["scanned_with"].GetInt()));
				}

				batchFiles = Utils::make_unique<MDBatchFiles>(batchFilesObject["batch_count"].GetInt(),
				batchFilesObject["first_index"].GetInt(),
				batchFilesObject["page_size"].GetInt(),
				std::move(filesInBatch));
			}
			else
			{
				batchFiles = Utils::make_unique<MDBatchFiles>(batchFilesObject["batch_count"].GetInt(), 0, 0, std::move(filesInBatch));
			}

			auto scanResultsObject = document["scan_results"].GetObject();
			scanResult = Utils::make_unique<MDBaseScanResult>(scanResultsObject["batch_id"].GetString(),
				scanResultsObject["scan_all_result_a"].GetString(),
				scanResultsObject["scan_all_result_i"].GetInt(),
				scanResultsObject["start_time"].GetString(),
				scanResultsObject["total_avs"].GetInt(),
				scanResultsObject["total_time"].GetInt64());

			auto procInfoObject = document["process_info"].GetObject();
			processInfo = Utils::make_unique<MDBaseProcessInfo>(procInfoObject["file_type_skipped_scan"].GetBool(),
				procInfoObject["blocked_reason"].GetString(),
				procInfoObject["profile"].GetString(),
				procInfoObject["result"].GetString(),
				procInfoObject["user_agent"].GetString());
		}
		catch(MDException& e)
		{
			throw MDParsingException(e.what(), batchResultJSON);
		}
	}

	MDBatchResult() {}

	std::string batchId;
	bool isClosed;
	std::string userData;
	std::unique_ptr<MDBaseScanResult> scanResult;
	std::unique_ptr<MDBaseProcessInfo> processInfo;
	std::unique_ptr<MDBatchFiles> batchFiles;
};

};

#endif
