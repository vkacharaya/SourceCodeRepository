#pragma once
/////////////////////////////////////////////////////////////////////////////
// RepositoryCore.h -  This package interacts with the repsotory for     //
//						 load and save. It also holds the repsonsibility to// 
//						 check-in, browse, checkout                        //
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
* This module provides class, RepositoryCore
*
* The RepositoryCore class supports interacting with the repsotory for
* load and save. It also holds the repsonsibility to check-in, browse, checkout
*
* Required Files:
* ===============
* Browse.h, CheckOut.h, Persistence.h, RepositoryCore.h, CheckIn.h
*
* Maintenance History:
* ====================
* ver 1.0 : 10 Mar 2018
*			Initial Implementation
*/
#include "../CheckIn/CheckIn.h"
#include "../Browse/Browse.h"
#include "../CheckOut/CheckOut.h"

namespace LocalSVN
{
	//////////////////////////////////////////////////////////////////////////
	// This class does the basic operations like Check-in, Check-out, Browse
	class RepositoryCore
	{
	public:

		using SourceCodeMap = DbCore<PayLoad>;
		using Collection = std::vector<DbElement<PayLoad>>;

		//////////////////////////////////////////////////////////////////////////
		// Function used to re-initialize the repository paths
		// used in this application
		RepositoryCore(const std::string& user, const std::string& xmlPath = ApplicationConstants::getXmlReposPath()) :_user(user)
		{
			//Loads the NoSql db contents needed for repository operations

			_map.registerSearchFn(&PayLoad::checkPatternInCategory).registerSearchFn(&PayLoad::checkPatternInCategory);
			loadXMLRepository(xmlPath);
		}

		//////////////////////////////////////////////////////////////////////////
		//This function gets the filtered queried keys with no par
		std::vector<std::string> GetFilteredFilesWIthNoP(std::vector<std::string> filteredKeys);

		//////////////////////////////////////////////////////////////////////////
		//This function gets the queried keys
		std::vector<std::string> GetQUeriedKeys(Conditions condn);

		//////////////////////////////////////////////////////////////////////////
		//This function gets the filtered versioned keys
		std::vector<std::string> GetVersionedKeys(std::vector<std::string> version, std::regex  versionKey);
		//////////////////////////////////////////////////////////////////////////
		// Function used to re-initialize the repository paths
		// used in this application
		void reinitiateAppPaths(const std::string&, const std::string&, const std::string& = "", const std::string& = "");

		//////////////////////////////////////////////////////////////////////////
		// Function used to perform single file check-in
		RepositoryCore& initiateSingleCheckin(const std::string&, const std::string&, const std::string& = "Default Checkin");

		//////////////////////////////////////////////////////////////////////////
		// Function used to perform multiple file check-in
		// which in turn calls the single file check-in in loops
		std::string closeCheckIn(const std::string&, const std::string&);

		DbElement<PayLoad>& GetMetaData(std::string fileName, std::string nameSP, Conditions condn, bool latestVersionValue, bool isSelected = false);

		//////////////////////////////////////////////////////////////////////////
		//This function is used ofr editing the metadata for the check-in item
		DbElement<PayLoad>& editMetadata(const std::string&, const std::string&);

		//This function is mainly used for adding multiple dependency at the same times
		//It also creates a new check-in if the item is closed
		void addDependency(const std::vector<std::string>&, const std::string& filePath, const std::string& nameSpace);

		/////////////////////////////////////////////////////////
		// Function used to run the process in which the text 
		// opens in a new notepad instance each time when called
		void browseFile(const std::string& fileName, const std::string& namepsace,
			const std::string& appPath = "c:/windows/system32/notepad.exe", const bool& separateProcNeeded = true);

		/////////////////////////////////////////////////////////
		// Function used to get the latest version files
		bool getLatestVersionFile(const std::string & fileKey, const std::string & namespaceKey, const bool & dependencyNeeded);

		/////////////////////////////////////////////////////////
		// Function used to get the particular version files
		bool getParticularFile(const std::string & fileKey, const std::string & namespaceKey, const bool & dependencyNeeded);

		//////////////////////////////////////////////////////////////////////////
		//This function updates the check-in item into the database
		void updateDB(const std::string& xmlReposPath = ApplicationConstants::getXmlReposPath());

		//////////////////////////////////////////////////////////////////////////
		//This function loads the xml repository depending on the path value
		//passed to this function
		SourceCodeMap& loadXMLRepository(const std::string& filePath = ApplicationConstants::getXmlReposPath());

		SourceCodeMap getDB() { return _map; };

	private:
		//This property contains the elements from the NoSqlDB
		//This Db value cannot be retrieved outside the class
		SourceCodeMap _map;

		//////////////////////////////////////////////////////////////////////////
		//Author Name
		std::string _user;

		/////////////////////////////////////////////////////////
		// Function used to show db browse values
		void showBrowseProp(DbElement<PayLoad> element);
	};
}
