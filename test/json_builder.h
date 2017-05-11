#ifndef _JSON_BUILDER_H
#define _JSON_BUILDER_H

#include <memory>

#include <mdrestclient/md_rapidjson_wrapper.h>

#include <mdrestclient/md_rest_response_objects.h>

class JsonBuilder
{
public:
	JsonBuilder();

	void addFileScanResult(std::unique_ptr<Opswat::MDFileScanResult> &fileScanResult);
	void createFileScanResult();
	void addDataId(const std::string &dataId);
	void addExtractedFiles(std::vector<std::unique_ptr<Opswat::MDExtractedFileInfo>> &extractedFileInfo);
	void addFileInfo(const std::unique_ptr<Opswat::MDFileInfo> &fileInfo);
	void addScanResults(const std::unique_ptr<Opswat::MDScanResult> &scanResult, bool lowVisibility = false);
	void addProcessInfo(const std::unique_ptr<Opswat::MDProcessInfo> &processInfo);
	void addEngineInfos(std::vector<std::unique_ptr<Opswat::MDEngineInfo>> &engineInfos);
	void addScanRules(const std::vector<std::unique_ptr<Opswat::MDScanRuleInfo>> &scanRules);

	std::string getJson();
	rapidjson::Document doc_;
};

#endif