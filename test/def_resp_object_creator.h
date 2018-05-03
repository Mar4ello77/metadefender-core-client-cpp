#ifndef _DEF_RESP_OBJECT_CREATOR_H_
#define _DEF_RESP_OBJECT_CREATOR_H_

#include <memory>
#include <mdrestclient/md_rest_response_objects.h>

class DefRespObjectCreator
{
public:
	static std::unique_ptr<Opswat::MDFileInfo> getFileInfoObject();
	static std::unique_ptr<Opswat::MDExtendedProcessInfo> getExtendedProcessInfoObject();
	static std::unique_ptr<Opswat::MDPostProcessInfo> getPostProcessInfoObject();
	static std::unique_ptr<Opswat::MDExtendedScanResult> getExtendedScanResultObject();
	static std::unique_ptr<Opswat::MDEngineScanResult> getEngineScanResultObject();
	static std::unique_ptr<Opswat::MDExtractedFileInfo> getExtractedFileInfoObject();
	static std::unique_ptr<Opswat::MDEngineInfo> getEngineInfoObject();
};

#endif