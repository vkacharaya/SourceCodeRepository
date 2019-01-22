#pragma once
/////////////////////////////////////////////////////////////////////////////
// ApplicationConstants.h - This class contains the constants needed for   //
//							extracting the file from local source folder   //
//							and the repository path                        //
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
* This module provides class, ApplicationConstants
*
* Required Files:
* ===============
* ApplicationConstants.h,ApplicationConstants.cpp
*
* Maintenance History:
* ====================
* ver 1.0 : 10 Mar 2018
*			Initial Implementation
*/
#include <iostream>
#include "../FileSystemDemo/FileSystemDemo.h"

using namespace FileSystem;

namespace LocalSVN
{
	/////////////////////////////////////////////////////////
	// This class is responsible for holding the application 
	// constants needed in this project
	class ApplicationConstants
	{
	public:

		/////////////////////////////////////////////////////////
		// Holds the checkin status
		enum class CheckinStatus :char
		{
			Closed = 'C',
			ClosePending = 'P',
			Open = 'O',
			New = 'N'
		};

		/////////////////////////////////////////////////////////
		// Setter for repository path
		static void setRepositoryPath(const std::string& folderPath) { _destinationFolder = const_cast<std::string&>(folderPath); }

		/////////////////////////////////////////////////////////
		// setter for local path
		static void setLocalPath(const std::string& folderPath) { _localSourceFolder = const_cast<std::string&>(folderPath); }

		/////////////////////////////////////////////////////////
		// setter for xml repository path
		static void setXmlReposPath(const std::string& reposPath) { _xmlPath = const_cast<std::string&>(reposPath); }

		/////////////////////////////////////////////////////////
		// setter for temp local server path
		static void setTmpFilePath(const std::string& tmpLocalFP) { _tmpLocalFilePath = const_cast<std::string&>(tmpLocalFP); }

		/////////////////////////////////////////////////////////
		// Getter for repository path
		static std::string getRepositoryPath() { return Path::getFullFileSpec(_destinationFolder); }

		/////////////////////////////////////////////////////////
		// Getter for absolute path
		static std::string getDestAbsolutePath() { return _destinationFolder; }

		/////////////////////////////////////////////////////////
		// Getter for local path
		static std::string getLocalPath() { return Path::getFullFileSpec(_localSourceFolder); }

		/////////////////////////////////////////////////////////
		// Getter for xml path
		static std::string getXmlReposPath() { return Path::getFullFileSpec(_xmlPath); }

		/////////////////////////////////////////////////////////
		// Getter for xml path
		static std::string getTmpFilePath() { return Path::getFullFileSpec(_tmpLocalFilePath); }

	private:
		/////////////////////////////////////////////////////////
		// Destination repository folder
		static std::string _destinationFolder;
		/////////////////////////////////////////////////////////
		// local source code folder path
		static std::string _localSourceFolder;
		/////////////////////////////////////////////////////////
		//XML repository path
		static std::string _xmlPath;
		/////////////////////////////////////////////////////////
		//XML repository path
		static std::string _tmpLocalFilePath;
	};
}