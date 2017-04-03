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

struct MDScanResult
{
	MDScanResult(const std::string& dataId,
		int processPercentage,
		const std::string& scanAllResultDesc,
		int scanAllResultCode,
		const std::string& startDate,
		int numEngines,
		int64_t scanDuration) : dataId(dataId),
								processPercentage(processPercentage),
								scanAllResultDesc(scanAllResultDesc),
								scanAllResultCode(scanAllResultCode),
								startDate(startDate),
								numEngines(numEngines),
								scanDuration(scanDuration)
	{
	}

	std::string dataId;
	int processPercentage;
	std::string scanAllResultDesc;
	int scanAllResultCode;
	std::map<std::string, std::unique_ptr<MDEngineScanResult>> scanDetails;
	std::string startDate;
	int numEngines;
	int64_t scanDuration;
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

struct MDProcessInfo
{
	MDProcessInfo(bool skipped,
		const std::string& blockedReason,
		const std::string& profile,
		int processPercentage,
		const std::string& result,
		const std::string& userAgent) : skippedFileType(skipped),
										blockedReason(blockedReason),
										profile(profile),
										processPercentage(processPercentage),
										result(result),
										userAgent(userAgent)
	{
	}

	bool skippedFileType;
	std::string blockedReason;
	std::string profile;
	int processPercentage;
	std::string result;
	std::string userAgent;
	std::unique_ptr<MDPostProcessInfo> postProcessInfo;
};

struct MDExtractedFileInfo
{
	MDExtractedFileInfo(const std::string& dataId, 
		int detecetedBy, 
		const std::string& displayName, 
		int64_t fileSize, 
		const std::string& fileType, 
		const std::string& fileTypeDescription, 
		int progressPercentage, 
		int scanAllResultCode,
		int scannedWith) : dataId(dataId),
									detectedBy(detecetedBy),
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

			auto scanResultsObject = document["scan_results"].GetObject();
			scanResult = Utils::make_unique<MDScanResult>(scanResultsObject["data_id"].GetString(),
				scanResultsObject["progress_percentage"].GetInt(),
				scanResultsObject["scan_all_result_a"].GetString(),
				scanResultsObject["scan_all_result_i"].GetInt(),
				scanResultsObject["start_time"].GetString(),
				scanResultsObject["total_avs"].GetInt(),
				scanResultsObject["total_time"].GetInt64()
				);

			auto scanDetailsObject = scanResultsObject["scan_details"].GetObject();
			for(auto& elem : scanDetailsObject)
			{
				std::string engineName = elem.name.GetString();
				scanResult->scanDetails[engineName] = Utils::make_unique<MDEngineScanResult>(engineName,
					elem.value["scan_result_i"].GetInt(),
					elem.value["threat_found"].GetString(),
					elem.value["scan_time"].GetInt64(),
					elem.value["def_time"].GetString(),
					elem.value["location"].GetString()
					);
			}

			auto procInfoObject = document["process_info"].GetObject();
			processInfo = Utils::make_unique<MDProcessInfo>(
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
	std::unique_ptr<MDScanResult> scanResult;
	std::unique_ptr<MDProcessInfo> processInfo;
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
	MDScanRuleInfo(const std::string& name, unsigned long long maxFileSize) : name(name),
		maxFileSize(maxFileSize)
	{}

	std::string name;
	unsigned long long maxFileSize;
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

};

#endif
