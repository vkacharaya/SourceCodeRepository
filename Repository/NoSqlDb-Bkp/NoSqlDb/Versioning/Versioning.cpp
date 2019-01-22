#include "Versioning.h"

namespace LocalSVN
{
	//////////////////////////////////////////////////////////////////////////
	// This function reorders the keys to get the latest order number
	std::string Versioning::getCurrentKeyVersion(const std::string& fileName)
	{
		//Filter the db object using the fileName regex & query object
		Conditions fileCondn;
		fileCondn.setNameRegex(fileName);
		std::vector<std::string>& filteredKeys = _queryObj.unionOn(fileCondn).filterOn(fileCondn).getQueriedKeys();

		//Sort the keys to get the latest key
		reorderKeys(filteredKeys);

		//Assigns the latest file name to version number data member
		if (filteredKeys.size() != 0)
			_versionFileName = _queryObj.getQueriedKeys().at(0);

		return _versionFileName;
	}

	//////////////////////////////////////////////////////////////////////////
	// This function reorders the keys to get the latest order number
	std::string& Versioning::getNextVersionNumber(const std::string& fileName)
	{
		getCurrentKeyVersion(fileName);
		_versionFileName = _versionFileName == "" ? fileName : _versionFileName;
		std::string extension = _versionFileName.substr(_versionFileName.find_last_of('.') + 1);
		//std::string namespaceKey = isVersion(extension) ? "" : nsKey + ".";
		extension = isVersion(extension) ? std::to_string(std::stoi(extension) + 1) : "0";
		return _versionFileName = trimExtension(_versionFileName) + "." + extension;
	}

	//////////////////////////////////////////////////////////////////////////
	// This function reorders the keys to get the latest order number
	bool Versioning::isVersion(const std::string& extension)
	{
		return extension.length() == std::count_if(extension.begin(), extension.end(),
			[](unsigned char c) { return std::isdigit(c); }
		);
	}

	//////////////////////////////////////////////////////////////////////////
	// This function reorders the keys to get the latest order number
	std::string Versioning::trimExtension(const std::string& fileName)
	{
		std::string extension = fileName.substr(fileName.find_last_of('.') + 1);

		if (isVersion(extension))
			return fileName.substr(0, fileName.find_last_of('.'));

		return fileName;
	}

	//////////////////////////////////////////////////////////////////////////
	// This function reorders the keys to get the latest order number
	void Versioning::reorderKeys(std::vector<std::string>& filteredKeys)
	{
		//Lambda expression for string comparison

		auto strCmptmp = [this](const std::string& string1, const std::string& string2)
		{

			std::string extension = string1.substr(string1.find_last_of('.') + 1);
			//std::string namespaceKey = isVersion(extension) ? "" : nsKey + ".";
			extension = isVersion(extension) ? std::to_string(std::stoi(extension)) : "0";

			std::string extension1 = string2.substr(string2.find_last_of('.') + 1);
			//std::string namespaceKey = isVersion(extension) ? "" : nsKey + ".";
			extension1 = isVersion(extension1) ? std::to_string(std::stoi(extension1)) : "0";

			return std::stoi(extension) > std::stoi(extension1);
		};

		//Sort the filtered keys and get the latest version by reordering
		std::sort(filteredKeys.begin(), filteredKeys.end(), strCmptmp);
	}
}
