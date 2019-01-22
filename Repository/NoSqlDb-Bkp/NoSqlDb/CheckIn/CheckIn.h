#pragma once
/////////////////////////////////////////////////////////////////////////////
// CheckIn.h - Supports check-in of the files. It is also responsible for  //
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

#include <iostream>
#include "../Versioning/Versioning.h"
#include "../FileSystemDemo/FileSystemDemo.h"
#include "../FileNameParser/FileNameParser.h"
#include "GraphUtils.h"

using namespace FileSystem;

namespace LocalSVN
{
	/////////////////////////////////////////////////////////
	// Responsible for doing the checkin, close and 
	// editing metadata
	class Checkin
	{
	public:
		/////////////////////////////////////////////////////////
		// Alias names for Check in items and the db collections
		using CheckInItem = DbElement<PayLoad>;
		using ItemCollection = DbCore<PayLoad>;

		/////////////////////////////////////////////////////////
		// constructor for the Check-in class which initiates value 
		// for versioning, latest file names
		Checkin(const std::string& fileName, const std::string& nameSp, ItemCollection& map)
			: _filePath(fileName), _map(map), _vs{ map }, _namespace{ nameSp }
		{
			//Retrieves the latest file name
			_latestVersionFileName = _vs.getCurrentKeyVersion((nameSp == "" ? "" : (nameSp + ".")) + Path::getName(fileName));
			//This checks for new entries in the db elemnt
			if (map.contains(_latestVersionFileName))
				_elem = map[_latestVersionFileName];
		};

		/////////////////////////////////////////////////////////
		// function used for adding dependency to the 
		// check-in items
		void addDependency(const std::vector<std::string>&);

		/////////////////////////////////////////////////////////
		// Initiates the check-in
		// whihc does file copying to source repository
		void intializeCheckIn();

		/////////////////////////////////////////////////////////
		// Initiates the check-in
		// whihc does file copying to source repository
		void setTempFolder(const std::string& tmpFilePath);

		/////////////////////////////////////////////////////////
		// set the value for the check-in item
		void setCheckInItem(const CheckInItem& item) { _elem = item; }

		/////////////////////////////////////////////////////////
		// allows editing the metadata information of checkin item
		DbElement<PayLoad>& editMetaData();

		/////////////////////////////////////////////////////////
		// getter for the checkin item
		DbElement<PayLoad>& getCheckItem() { return _elem; };

		/////////////////////////////////////////////////////////
		// returns the check in status based on the file name 
		// passed ot the function
		ApplicationConstants::CheckinStatus getCheckInStatus(const std::string&);

		/////////////////////////////////////////////////////////
		// updates the closed status to the check-in item
		std::string updateClosedStatus();

		/////////////////////////////////////////////////////////
		// Retrieves the latest version name by querying 
		// using versioning
		std::string& getLatestVersionFN() { return _latestVersionFileName; };

		/////////////////////////////////////////////////////////
		// checks for the closed dependencies list using SCC
		ApplicationConstants::CheckinStatus checkClosedDependences();
	private:
		/////////////////////////////////////////////////////////
		// Hold the chekc-in details
		CheckInItem _elem;
		/////////////////////////////////////////////////////////
		// Collection of check-in item details
		ItemCollection& _map;
		/////////////////////////////////////////////////////////
		// versioning obejct
		Versioning _vs;
		/////////////////////////////////////////////////////////
		// Filepath for the filename passed to this class
		std::string _filePath;
		/////////////////////////////////////////////////////////
		// Latest file name retrieved on check-in 
		std::string _latestVersionFileName;
		/////////////////////////////////////////////////////////
		// Namespace usedc for mapping in the DB
		std::string _namespace;

		/////////////////////////////////////////////////////////
		// Namespace usedc for mapping in the DB
		std::string _tempPath;

		/////////////////////////////////////////////////////////
		// This private method is used for returning th
		// corresponding vertex name using the id passed
		std::vector<std::string> getNodeValue(std::unordered_map<int, std::vector<int>>& scc, std::unordered_map<std::string, std::string>& idMap);
	};
}