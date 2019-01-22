/////////////////////////////////////////////////////////////////////////////
// CheckOut.cpp - retrieves package files, removing version information from // 
//				their filenames. Retrieved files will be copied to a       //
//				specified directory.                                       //
// ver 1.0                                                                 //
// ----------------------------------------------------------------------- //
// copyright © Vishnu Karthik Ravindran, 2018                              //
// All rights granted provided that this notice is retained                //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2010                             //
// Platform:    HP Laptop , Core i5, Windows 10                            //
// Application: Course Projects, 2018                                      //
// Author:      Vishnu Karthik Ravindran, Syracuse University              //
//													                       //
/////////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
* This module provides class, CheckOut
*
* The CheckOut class supports retrieves package files, removing version information
* from their filenames. Retrieved files will be copied to a specified directory.
*
* Required Files:
* ===============
* CheckOut.h, CheckOut.cpp, Versioning.h, Versioning.cpp, FileSystemDemo.h
* FileNameParser.h,Graph.h
*
* Maintenance History:
* ====================
* ver 1.0 : 10 Mar 2018
*			Initial Implementation
*/

#include "CheckOut.h"

using namespace GraphProcessing;

namespace LocalSVN
{

	/////////////////////////////////////////////////////////
	// Function used to get the latest version files
	void CheckOut::getLatestVersionFile(const std::string & fileKey, const std::string & namespaceKey, const bool & dependencyNeeded)
	{
		std::string latestVersion = _vs.getCurrentKeyVersion(namespaceKey + "." + fileKey);
		checkOutDependencyOp(latestVersion, dependencyNeeded);
	}

	/////////////////////////////////////////////////////////
	// Function used to get the particular version files
	void CheckOut::getParticularFile(const std::string & fileKey, const std::string & namespaceKey, const bool & dependencyNeeded)
	{
		std::string version = namespaceKey + "." + fileKey;
		checkOutDependencyOp(version, dependencyNeeded);
	}

	////////////////////////////////////////////////////////
	// Dependency file checkout
	void CheckOut::checkOutDependencyOp(const std::string& latestVersion, const bool & dependencyNeeded)
	{
		std::vector<std::string> filePaths;
		GraphUtils gh;
		std::unordered_map<std::string, std::string > idMap;
		if (dependencyNeeded)
		{
			gh.createGraph(_map, idMap);
			auto proc = [&filePaths, this](Sptr pNode) {
				filePaths.push_back(pNode->name());
			};

			using P = decltype(proc);
			using V = std::string;

			Sptr pNode = gh.getGraphInstance().find(latestVersion);
			gh.getGraphInstance().walker(DFS_r_ltr<V, P>, pNode, proc);
		}
		else filePaths.push_back(latestVersion);

		FileHandler fh;
		for (auto fn : filePaths)
		{
			const std::string& sourceCodeFilePath = Path::getFullFileSpec(_map[fn].payLoad().getFilePath());
			const std::string& localFilePath = ApplicationConstants::getLocalPath() + "/" + _vs.trimExtension(fh.trimNameSpace(fn));

			fh.fileSave(sourceCodeFilePath, localFilePath);
		}
	}
}

#ifdef TEST_CHECKOUT

int main()
{
	DbCore<PayLoad> db;
	DbElement<PayLoad> elemdb;

	elemdb.name("DbCore.h.0").payLoad().setFilePath("../DbCore/DbCore.h");
	db.insert("DbCore.h.0", elemdb);

	Query<PayLoad> query(db);
	CheckOut ch(db);

	ch.getLatestVersionFile("DbCore.h", "NoSqlDb", true);
	std::cout << "Copies all dependencies";


	ch.getLatestVersionFile("DbCore.h.0", "NoSqlDb", true);
	std::cout << "Copies specific file";
}

#endif TEST_CHECKOUT
