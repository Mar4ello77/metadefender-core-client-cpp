#ifndef _MD_TEST_UTIL_H_
#define _MD_TEST_UTIL_H_

#include <memory>
#include <mdrestclient/md_rest_response_objects.h>

bool operator==(const Opswat::MDFileScanResult &a, const Opswat::MDFileScanResult &b);
bool operator==(const Opswat::MDFileInfo &a, const Opswat::MDFileInfo &b);
bool operator==(const Opswat::MDExtendedProcessInfo &a, const Opswat::MDExtendedProcessInfo &b);
bool operator==(const Opswat::MDPostProcessInfo &a, const Opswat::MDPostProcessInfo &b);
bool operator==(const Opswat::MDExtendedScanResult &a, const Opswat::MDExtendedScanResult &b);
bool operator==(const Opswat::MDExtractedFileInfo &a, const Opswat::MDExtractedFileInfo &b);
bool operator==(const Opswat::MDEngineScanResult &a, const Opswat::MDEngineScanResult &b);
bool operator==(const Opswat::MDEngineInfo &a, const Opswat::MDEngineInfo &b);
bool operator==(const Opswat::MDScanRuleInfo &a, const Opswat::MDScanRuleInfo &b);
bool operator==(const Opswat::MDBatchResult &a, const Opswat::MDBatchResult &b);
bool operator==(const Opswat::MDBaseProcessInfo &a, const Opswat::MDBaseProcessInfo &b);
bool operator==(const Opswat::MDBaseScanResult &a, const Opswat::MDBaseScanResult &b);
bool operator==(const Opswat::MDBatchFiles &a, const Opswat::MDBatchFiles &b);
bool operator==(const Opswat::MDFileInBatch &a, const Opswat::MDFileInBatch &b);
bool operator==(const Opswat::MDBatchFileProcessInfo &a, const Opswat::MDBatchFileProcessInfo &b);

template<typename T>
bool equal(const std::unique_ptr<T> &a, const std::unique_ptr<T> &b)
{
	// if both nullptr or has the same ptr then they are equal
	if (a == b)
	{
		return true;
	}
	return *a == *b;
}

template<typename T>
bool equal(const std::vector<std::unique_ptr<T>> &a, const std::vector<std::unique_ptr<T>> &b)
{
	if (a.size() != b.size())
	{
		return false;
	}
	for (size_t i = 0; i < a.size(); ++i)
	{
		if (!equal(a[i], b[i]))
		{
			return false;
		}
	}
	return true;
}

template<typename T>
bool equal(const std::map<std::string, std::unique_ptr<T>> &a, const std::map<std::string, std::unique_ptr<T>> &b)
{
	if (a.size() != b.size())
	{
		return false;
	}
	for (auto &i : a)
	{
		if (b.find(i.first) == b.end() || !equal(b.at(i.first), i.second))
		{
			return false;
		}
	}
	return true;
}


#endif