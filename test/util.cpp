#include "util.h"

bool operator==(const Opswat::MDFileScanResult &a, const Opswat::MDFileScanResult &b)
{
	return a.dataId == b.dataId && 
		equal(a.extractedFiles, b.extractedFiles) &&
		equal(a.fileInfo, b.fileInfo) &&
		equal(a.processInfo, b.processInfo) &&
		equal(a.scanResult, b.scanResult);
}

bool operator==(const Opswat::MDFileInfo &a, const Opswat::MDFileInfo &b)
{
	return a.displayName == b.displayName &&
		a.fileSize == b.fileSize &&
		a.fileTypeDesc == b.fileTypeDesc &&
		a.fileType == b.fileType &&
		a.md5 == b.md5 &&
		a.sha1 == b.sha1 &&
		a.sha256 == b.sha256 &&
		a.uploadDate == b.uploadDate;
}

bool operator==(const Opswat::MDProcessInfo &a, const Opswat::MDProcessInfo &b)
{
	return a.blockedReason == b.blockedReason &&
		equal(a.postProcessInfo, b.postProcessInfo) &&
		a.processPercentage == b.processPercentage &&
		a.profile == b.profile &&
		a.result == b.result &&
		a.skippedFileType == b.skippedFileType &&
		a.userAgent == b.userAgent;
}

bool operator==(const Opswat::MDPostProcessInfo &a, const Opswat::MDPostProcessInfo &b)
{
	return a.actionsFailed == b.actionsFailed &&
		a.actionsRun == b.actionsRun &&
		a.convertedDestination == b.convertedDestination &&
		a.convertedTo == b.convertedTo &&
		a.copyMoveDestination == b.copyMoveDestination;
}

bool operator==(const Opswat::MDScanResult &a, const Opswat::MDScanResult &b)
{
	return a.numEngines == b.numEngines &&
		a.processPercentage == b.processPercentage &&
		a.scanAllResultCode == b.scanAllResultCode &&
		a.scanAllResultDesc == b.scanAllResultDesc &&
		a.scanDuration == b.scanDuration &&
		a.startDate == b.startDate &&
		equal(a.scanDetails, b.scanDetails) &&
		a.dataId == b.dataId;
}

bool operator==(const Opswat::MDExtractedFileInfo &a, const Opswat::MDExtractedFileInfo &b)
{
	return a.dataId == b.dataId &&
		a.detectedBy == b.detectedBy &&
		a.displayName == b.displayName &&
		a.fileSize == b.fileSize &&
		a.fileTypeDescription == b.fileTypeDescription &&
		a.fileType == b.fileType &&
		a.progressPercentage == b.progressPercentage &&
		a.scanAllResultCode == b.scanAllResultCode &&
		a.scannedWith == b.scannedWith;
}

bool operator==(const Opswat::MDEngineScanResult &a, const Opswat::MDEngineScanResult &b)
{
	return a.engineName == b.engineName &&
		a.scanDuration == b.scanDuration &&
		a.scanResultCode == b.scanResultCode &&
		a.threat == b.threat &&
		a.virusDefDate == b.virusDefDate &&
		a.location == b.location;
}

bool operator==(const Opswat::MDEngineInfo &a, const Opswat::MDEngineInfo &b)
{
	return a.active == b.active &&
		a.defTime == b.defTime &&
		a.downloadProgress == b.downloadProgress &&
		a.downloadTime == b.downloadTime &&
		a.engineId == b.engineId &&
		a.engineName == b.engineName &&
		a.engineTypeDescription == b.engineTypeDescription &&
		a.engineType == b.engineType &&
		a.engineVersion == b.engineVersion &&
		a.state == b.state &&
		a.type == b.type;
}

bool operator==(const Opswat::MDScanRuleInfo &a, const Opswat::MDScanRuleInfo &b)
{
	return a.name == b.name &&
		a.maxFileSize == b.maxFileSize;
}