#pragma once

#include <iostream>
#include <cctype>
#include "../Query/Query.h"

using namespace NoSqlDb;

namespace LocalSVN
{
	class Versioning
	{
	public:
		Versioning(Query<PayLoad> queryObj) :_queryObj(std::move(queryObj)) {};

		// Not using DbCore because it will make dependent on two classes (DbCore & PayLoad)
		std::string getCurrentKeyVersion(const std::string& fileName);

		//Get the next version number without considering the check-in status
		std::string& getNextVersionNumber(const std::string& fileName);

		//Used to retrieve the version number from the file name
		std::string& getVersionedFN() { return _versionFileName; }

		//This function will trim the version number leaving the first filename
		std::string trimExtension(const std::string&);

		bool isVersion(const std::string&);

		void reorderKeys(std::vector<std::string>& filteredKeys);
	private:
		Query<PayLoad> _queryObj;
		std::string _versionFileName;
	};
}