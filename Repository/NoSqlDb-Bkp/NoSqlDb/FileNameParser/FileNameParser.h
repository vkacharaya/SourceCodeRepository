#pragma once
/////////////////////////////////////////////////////////////////////////////
// FileNameParser.h - This package is responsible for doing file path related 
//                    operations. It also does the job of copying the file //
//                    file from one source to other using file system.     //
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
* This module provides class, FileNameParser
*
* The FileNameParser class is responsible for doing file path related 
* operations. It also does the job of copying the file 
* file from one source to other using file system. 

* Public Interface:
* =================
* std::string constructRelFilePath(const std::string&, const std::string& = "", const bool& = true);
* void fileSave(const std::string&, const std::string&);
* std::string trimNameSpace(const std::string& fileName);
* std::string getReposAbsolutePath(const std::string& filePath);
*
* Required Files:
* ===============
* FileHandler.h, FileHandler.cpp, ApplicationConstants.h
*
* Maintenance History:
* ====================
* ver 1.0 : 10 Mar 2018
*			Initial Implementation
*/
#include <iostream>
#include "../Utils/ApplicationConstants.h"

namespace LocalSVN
{
	//////////////////////////////////////////////////////////////////////////
	// File name and file path manipulator
	class FileHandler
	{
	public:

		//////////////////////////////////////////////////////////////////////////
		// Retrieves the relative path based on the file path
		// and the isRepository flag
		std::string constructRelFilePath(const std::string&, const std::string& = "", const bool& = true);

		//////////////////////////////////////////////////////////////////////////
		// This function saves the file in the respective folders
		void fileSave(const std::string&, const std::string&);

		//////////////////////////////////////////////////////////////////////////
		// Trims the namespace from DB elemnts key
		std::string trimNameSpace(const std::string& fileName);

		//////////////////////////////////////////////////////////////////////////
		//Get repository absolute path for file passed
		std::string getReposAbsolutePath(const std::string& filePath);

	private:

		//////////////////////////////////////////////////////////////////////////
		// This function returns the application root folder name
		std::string getApplicationRootFolder();
	};
}