/////////////////////////////////////////////////////////////////////////////
// CheckIn.cpp - Supports check-in of the files. It is also responsible for//
//	    	   copying the file from loacl to the source repository        //
//             The close opertaion of  the check -in is done in this package. 
//		       This package also interacts with the graphUtils to find     //
//			   out strong component which is necessary when closing the file.
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
* This module provides class, CheckIn
*
* The CheckIn class supports adding dependency, closing the check-in item,
* editing metadata, and also its main responsiblity of initiating the oprtaion
* to store files in repository
*
*
* Required Files:
* ===============
* CheckIn.h, CheckIn.cpp, Versioning.h, Versioning.cpp, FileSystemDemo.h
* FileNameParser.h
*
* Maintenance History:
* ====================
* ver 1.0 : 10 Mar 2018
*			Initial Implementation
*/
#include "CheckIn.h"

namespace LocalSVN
{
	//This function is mainly used for adding multiple dependency at the same times
	//It also creates a new check-in if the item is closed
	void Checkin::addDependency(const std::vector<std::string>& dependents)
	{
		//Calls the metadata to intiate internal opening of the checkin item
		editMetaData();
		for (auto dp : dependents)
			_elem.addChild(dp);
		//Updates the element
		_map[_latestVersionFileName] = _elem;
	}

	//////////////////////////////////////////////////////////////////////////
	// This function is used to edit the metadata
	Checkin::CheckInItem& Checkin::editMetaData()
	{
		if (getCheckInStatus(_latestVersionFileName) == ApplicationConstants::CheckinStatus::Closed
			|| getCheckInStatus(_latestVersionFileName) == ApplicationConstants::CheckinStatus::ClosePending)
			intializeCheckIn();
		return _elem;
	}

	//////////////////////////////////////////////////////////////////////////
	// Updates the closed status for the check-in item
	std::string Checkin::updateClosedStatus()
	{
		//Updates the closed status when the function returns closed and closed pending
		if (checkClosedDependences() == ApplicationConstants::CheckinStatus::Closed
			|| checkClosedDependences() == ApplicationConstants::CheckinStatus::ClosePending)
		{
			_elem.payLoad().setCheckInStatus(checkClosedDependences());
			_map[_latestVersionFileName].payLoad().setCheckInStatus(checkClosedDependences());
			//Updates dependecny files
			for (auto elem : _elem.children())
			{
				_latestVersionFileName = _vs.getCurrentKeyVersion(_vs.trimExtension(elem));
				_elem = _map[_latestVersionFileName];
				if (checkClosedDependences() == ApplicationConstants::CheckinStatus::Closed || checkClosedDependences() == ApplicationConstants::CheckinStatus::ClosePending)
				{
					_map[_latestVersionFileName].payLoad().setCheckInStatus(checkClosedDependences());
				}
			}
			if (checkClosedDependences() == ApplicationConstants::CheckinStatus::ClosePending)
			{
				return "Files Closed successfully with Closed Pending Status.";
			}
			return "Files Closed successfully.";
		}
		return "Some open dependencies available. Please close the files one by one.";
	}

	//////////////////////////////////////////////////////////////////////////
	// This function gets the current check-in status of the file
	void Checkin::setTempFolder(const std::string& tmpFilePath)
	{
		_tempPath = tmpFilePath;
	}

	//////////////////////////////////////////////////////////////////////////
	// This function gets the current check-in status of the file
	ApplicationConstants::CheckinStatus Checkin::getCheckInStatus(const std::string& fileName)
	{
		if (fileName == "")
			return ApplicationConstants::CheckinStatus::New; // This initialization is for new check-in 
		return _map[fileName].payLoad().getCheckInStatus();
	}

	/////////////////////////////////////////////////////////
	// Initiates the check-in
	// whihc does file copying to source repository
	void Checkin::intializeCheckIn()
	{
		ApplicationConstants::CheckinStatus checkInStatus;
		FileHandler fh;

		checkInStatus = getCheckInStatus(_latestVersionFileName);

		if (checkInStatus == ApplicationConstants::CheckinStatus::Closed || checkInStatus == ApplicationConstants::CheckinStatus::New)
			_latestVersionFileName = _vs.getNextVersionNumber(_namespace + "." + Path::getName(_filePath));
		std::string localFilePath;
		if (ApplicationConstants::getTmpFilePath() != "")
			localFilePath = ApplicationConstants::getTmpFilePath() + "/" + Path::getName(_filePath);
		else
			localFilePath = fh.constructRelFilePath(_filePath, "", false);

		const std::string& sourceCodeFilePath = fh.constructRelFilePath(_filePath, fh.trimNameSpace(_latestVersionFileName));

		//This uopdates the elemnt with suitable values
		_elem.payLoad().setFilePath(fh.getReposAbsolutePath(sourceCodeFilePath));
		_elem.payLoad().setCheckInStatus(ApplicationConstants::CheckinStatus::Open);
		_elem.name(_vs.trimExtension(_latestVersionFileName));
		_elem.dateTime(DateTime());

		//Copies the file to the source repository
		fh.fileSave(localFilePath, sourceCodeFilePath);
		_map[_latestVersionFileName] = _elem;
	}

	//////////////////////////////////////////////////////////////////////////
	// Check whether all the dependencies are closed or not
	ApplicationConstants::CheckinStatus Checkin::checkClosedDependences()
	{
		std::vector<std::string>& dependents = _elem.children();
		GraphUtils gh;
		int nodeID = 0;
		std::unordered_map<std::string, std::string > idMap;

		for (auto item : _map)
		{
			idMap[item.first] = std::to_string(nodeID);
			nodeID++;
		}

		//Checks for the closed components in the graph and later used for finding the 
		//packages which are open and then moved closed penidng
		gh.createGraph(_map, idMap);
		std::unordered_map<int, std::vector<int>> scc = gh.SCC();
		std::vector<std::string> cyclicNodes = getNodeValue(scc, idMap);
		ApplicationConstants::CheckinStatus dependentStatus = ApplicationConstants::CheckinStatus::Closed;
		for (auto fn : dependents)
			if ((getCheckInStatus(fn) == ApplicationConstants::CheckinStatus::Open || getCheckInStatus(fn) == ApplicationConstants::CheckinStatus::New)
				&& !(std::find(cyclicNodes.begin(), cyclicNodes.end(), fn) != cyclicNodes.end()))
			{
				//std::cout << "\n  Cannot close file as there are dependencies with open status\n";
				return ApplicationConstants::CheckinStatus::Open;
			}
		for (auto child : dependents)
			if (std::find(cyclicNodes.begin(), cyclicNodes.end(), child) != cyclicNodes.end() &&
				(getCheckInStatus(child) == ApplicationConstants::CheckinStatus::New || getCheckInStatus(child) == ApplicationConstants::CheckinStatus::Open))
				dependentStatus = ApplicationConstants::CheckinStatus::ClosePending;
		return dependentStatus;
	}

	/////////////////////////////////////////////////////////
	// This private method is used for returning th
	// corresponding vertex name using the id passed
	std::vector<std::string> Checkin::getNodeValue(std::unordered_map<int, std::vector<int>>& scc, std::unordered_map<std::string, std::string>& idMap)
	{
		int nodeID = std::atoi(idMap[_latestVersionFileName].c_str());
		std::vector<std::string> nodeValues;
		for (auto component : scc)
		{
			auto ir = std::find(component.second.begin(), component.second.end(), nodeID);
			if (ir != component.second.end())
			{
				for (auto value : component.second)
					for (auto nodeName : idMap)
						if (nodeName.second == std::to_string(value))
							nodeValues.push_back(nodeName.first);
				break;
			}
		}
		return nodeValues;
	}
}

#ifdef TEST_CHECKIN

int main()
{
	DbCore<PayLoad> db;
	DbElement<PayLoad> elemdb;

	elemdb.name("DbCore.h.0");
	db.insert("DbCore.h.0", elemdb);

	Query<PayLoad> query(db);
	Checkin ch("..//DbCore//DbCore.h.0", "NoSqlDb", db);

	std::cout << "Editing Check In Item";
	DbElement<PayLoad> elem = ch.editMetaData().descrip("Hello");
	db["DbCore.h.0"] = elem;

	std::cout << "Initialize check in";
	ch.intializeCheckIn();

	ch.updateClosedStatus();
	std::cout << "Updates the closed status";

	std::cout << "Adding dependencies to the checkin item";
	ch.addDependency({ "Test1" });
}

#endif TEST_CHECKIN