/////////////////////////////////////////////////////////////////////////////
// RepositoryCore.cpp -  This package interacts with the repsotory for     //
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

#include "../Persistence/Persistence.h"
#include "RepositoryCore.h"

using namespace NoSqlDb;
using namespace LocalSVN;

//////////////////////////////////////////////////////////////////////////
//This function loads the xml repository depending on the path value
//passed to this function
DbCore<PayLoad>& RepositoryCore::loadXMLRepository(const std::string& xmlReposPath)
{
	//This piece of code is used to load the xml values
	//into the map element
	Persistence<PayLoad> ps(_map);
	ps.loadXML(true, xmlReposPath.c_str());
	ApplicationConstants::setXmlReposPath(xmlReposPath);
	return _map;
}

//////////////////////////////////////////////////////////////////////////
//This function gets the metadata
DbElement<PayLoad>& RepositoryCore::GetMetaData(std::string fileName, std::string nameSP, Conditions condn, bool latestVersionValue, bool isSelected)
{
	Query<PayLoad> q{ _map };
	if (isSelected)
		return _map[nameSP + "." + fileName];
	Checkin ch(fileName, nameSP, _map);
	std::string latestVersion = ch.getLatestVersionFN();
	return _map[latestVersion];
}

//////////////////////////////////////////////////////////////////////////
//This function gets the queried keys
std::vector<std::string> RepositoryCore::GetQUeriedKeys(Conditions condn)
{
	Query<PayLoad> q{ _map };
	return GetVersionedKeys(q.filterOn(condn).getQueriedKeys(), condn.getVersionNo());
}

//////////////////////////////////////////////////////////////////////////
//This function gets the versioned keys
std::vector<std::string> RepositoryCore::GetVersionedKeys(std::vector<std::string> version, std::regex versionKey)
{
	std::vector<std::string> stringVector;
	if (versionKey._Empty())
		return version;
	for (std::string key : version)
	{
		if (regex_search(key.substr(key.find_last_of('.') + 1, key.length()), versionKey))
		{
			stringVector.push_back(key);
		}
	}
	return stringVector;
}

//////////////////////////////////////////////////////////////////////////
//This function gets the keys for the filtered content
std::vector<std::string> RepositoryCore::GetFilteredFilesWIthNoP(std::vector<std::string> filteredKeys)
{
	bool flag = true;
	std::vector<std::string> keys;
	for (auto child : filteredKeys)
	{
		for (auto keyElem : _map)
		{
			if (std::find(keyElem.second.children().begin(), keyElem.second.children().end(), child) != keyElem.second.children().end())
			{
				flag = false;
				break;
			}
		}
		if (flag)
			keys.push_back(child);
		flag = true;
	}
	return keys;
}

//////////////////////////////////////////////////////////////////////////
//This function is used ofr editing the metadata for the check-in item
DbElement<PayLoad>& RepositoryCore::editMetadata(const std::string& filePath, const std::string& nameSpace)
{
	Checkin ch(filePath, nameSpace, _map);
	if (_user == ch.getCheckItem().payLoad().getUser() || ch.getCheckItem().payLoad().getUser() == "")
	{
		DbElement<PayLoad>& elem = ch.editMetaData();
		return _map[ch.getLatestVersionFN()] = ch.getCheckItem();
	}
	else
	{
		std::cout << "\n  User Mismatch\n";
		return _map[ch.getLatestVersionFN()] = ch.getCheckItem();
	}
}

//This function is mainly used for adding multiple dependency at the same times
//It also creates a new check-in if the item is closed
void RepositoryCore::addDependency(const std::vector<std::string>& dependents, const std::string& filePath, const std::string& nameSpace)
{
	Checkin ch1(filePath, nameSpace, _map);
	if (_user == ch1.getCheckItem().payLoad().getUser() || ch1.getCheckItem().payLoad().getUser() == "")
	{
		for (auto dep : dependents)
		{
			Checkin ch(dep, "", _map);
			std::vector<std::string>& children = ch1.getCheckItem().children();
			if (!(std::find(children.begin(), children.end(), ch.getLatestVersionFN()) != children.end()) || children.size() == 0)
			{
				ch1.addDependency({ ch.getLatestVersionFN() });
			}
		}
	}
	else
	{
		std::cout << "\n  User Mismatch\n";
	}
	updateDB();
}

/////////////////////////////////////////////////////////
// Function used to run the process in which the text 
// opens in a new notepad instance each time when called
void RepositoryCore::browseFile(const std::string& filename, const std::string& namepsace, const std::string& appPath, const bool & separateProcNeeded)
{
	Query<PayLoad> query(_map);
	Browse br(query);
	showBrowseProp(br.getDbElement(filename, namepsace));
	if (separateProcNeeded)
		br.runProcess(filename, namepsace, appPath);
}

/////////////////////////////////////////////////////////
// Function used to get the latest version files
bool RepositoryCore::getLatestVersionFile(const std::string & fileKey, const std::string & namespaceKey, const bool & dependencyNeeded)
{
	try
	{
		CheckOut ch(_map);
		ch.getLatestVersionFile(fileKey, namespaceKey, dependencyNeeded);
		return true;
	}
	catch (std::exception ex)
	{
		return false;
	}
}

/////////////////////////////////////////////////////////
// Function used to get the particular version files
bool RepositoryCore::getParticularFile(const std::string & fileKey, const std::string & namespaceKey, const bool & dependencyNeeded)
{
	try
	{
		CheckOut ch(_map);
		ch.getParticularFile(fileKey, namespaceKey, dependencyNeeded);
		return true;
	}
	catch (std::exception ex)
	{
		return false;
	}
}
//////////////////////////////////////////////////////////////////////////
// Function used to re-initialize the repository paths
// used in this application
void RepositoryCore::reinitiateAppPaths(const std::string& localPath, const std::string& repositoryPath, const std::string& xmlRepositoryPath, const std::string& tmpFilePath)
{
	//Sets the Local path
	ApplicationConstants::setLocalPath(localPath);

	//Sets the source code repository path
	ApplicationConstants::setRepositoryPath(repositoryPath);

	if (tmpFilePath != "")
		ApplicationConstants::setTmpFilePath(tmpFilePath);

	//This is used to reload the db object
	if (xmlRepositoryPath != "")
	{
		ApplicationConstants::setXmlReposPath(xmlRepositoryPath);
		loadXMLRepository(ApplicationConstants::getXmlReposPath());
	}
}

//////////////////////////////////////////////////////////////////////////
// Initiates single file check-in
RepositoryCore& RepositoryCore::initiateSingleCheckin(const std::string& filePath, const std::string& nameSpce, const std::string& desc)
{
	//The entire map object is not passed to the check in class 
	//and this is done so to provide encapsulation
	Checkin ch(filePath, nameSpce, _map);
	if (_user == ch.getCheckItem().payLoad().getUser() || ch.getCheckItem().payLoad().getUser() == "")
	{
		ch.editMetaData().descrip(desc);
		ch.editMetaData().payLoad().setUser(_user);

		//This code gets the latest versioned key for the file name
		//and starts the check-in process
		ch.intializeCheckIn();

		//This updates the Db instance
		updateDB();
	}
	else
	{
		std::cout << "\n  User Mismatch\n";
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
// Initiates single file check-in
std::string RepositoryCore::closeCheckIn(const std::string& filePath, const std::string& nameSpace)
{
	std::string status;
	//The entire map object is not passed to the check in class 
	//and this is done so to provide encapsulation
	Checkin ch(filePath, nameSpace, _map);
	if (_user == ch.getCheckItem().payLoad().getUser() || ch.getCheckItem().payLoad().getUser() == "")
	{
		status = ch.updateClosedStatus();
		updateDB();
	}
	else
	{
		status =  "User Mismatch";
	}
	return status;
}

//////////////////////////////////////////////////////////////////////////
// Updates XML with the DB Object
void RepositoryCore::updateDB(const std::string& xmlReposPath)
{
	//This code updates the mappings in the xml path specified
	Persistence<PayLoad> ps(_map);
	ps.save(xmlReposPath.c_str());
}

/////////////////////////////////////////////////////////
// Function used to show db browse values
void RepositoryCore::showBrowseProp(DbElement<PayLoad> element)
{
	std::cout << "\n  Browse package description: " + element.descrip() + "\n";
	std::cout << "\n  The child keys:";
	for (auto elem : element.children())
	{
		std::cout << elem + "  ";
	}
}


#ifdef TEST_RESPOSITORYCORE

int main()
{
	RepositoryCore r("User1");
	//////////////////////// Initialize check in //////////////////////
	std::cout << "\n  Closing a file with cyclic dependency\n";

	r.addDependency({ "NoSqlDb.DbCore.cpp" }, "DbCore.h", "NoSqlDb");

	r.closeCheckIn("DbCore.h", "NoSqlDb");
	showHeader();
	DbCore<PayLoad> db = r.loadXMLRepository();
	Query<PayLoad> q(db);
	Conditions cond; cond.setNameRegex("DbCore.h");
	auto keys = q.filterOn(cond).getQueriedKeys();
	for (auto key : keys)
		showElem(q.valueOf(key));

	std::cout << "\n  Status of DbCore.h moved to closed pending\n";

	r.closeCheckIn("DbCore.cpp", "NoSqlDb");
	showHeader();
	DbCore<PayLoad> db1 = r.loadXMLRepository();
	Query<PayLoad> q1(db1);
	Conditions cond2; cond2.setNameRegex("DbCore.h");
	Conditions cond1; cond1.setNameRegex("DbCore.cpp");
	auto keys2 = q1.filterOn(cond2).unionOn(cond1).getQueriedKeys();
	for (auto key : keys2)
		showElem(q1.valueOf(key));

	RepositoryCore r2("User2");
	std::cout << "\n  User2 trying to checkin the Query.h. The author is User1. \n  ";

	r2.initiateSingleCheckin("../Query/Query.h", "NoSqlDb");
	std::cout << "\n  The error message will be shown for close, check-in and editing element\n";
	return 0;
}

#endif TEST_RESPOSITORYCORE
