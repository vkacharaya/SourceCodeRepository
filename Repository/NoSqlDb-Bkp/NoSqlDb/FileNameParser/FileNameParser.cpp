/////////////////////////////////////////////////////////////////////////////
//FileNameParser.cpp - This package is responsible for doing file path related 
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

#include "FileNameParser.h"

namespace LocalSVN
{
	//////////////////////////////////////////////////////////////////////////
	// This function saves the file in the respective folders
	void FileHandler::fileSave(const std::string& sourceFilePath, const std::string& destinationFilePath)
	{
		Directory::createDirectory(sourceFilePath);
		Directory::createDirectory(destinationFilePath);

		File::copy(sourceFilePath, destinationFilePath);
	}

	//////////////////////////////////////////////////////////////////////////
	// Retrieves the relative path based on the file path
	// and the isRepository flag
	std::string FileHandler::constructRelFilePath(const std::string& filePath, const std::string& versionFN, const bool& isRepPathNeeded)
	{
		//This path holds the repository path dependending on the isRepPathNeeded parameter
		std::string repositoryPath = isRepPathNeeded ? ApplicationConstants::getRepositoryPath() : ApplicationConstants::getLocalPath();

		//This code standardizes the path if different slashes are used
		std::string applicationRootFolder = "\\" + getApplicationRootFolder() + "\\";

		//This code standardizes the path if different slashes are used
		std::string standardizeFilePath = Path::getFullFileSpec(filePath);

		std::size_t foundFPIndex = standardizeFilePath.rfind(applicationRootFolder);
		if (versionFN != "")
			standardizeFilePath = Path::getFullFileSpec(Path::getPath(filePath));
		return repositoryPath + "\\" +
			standardizeFilePath.substr(foundFPIndex + applicationRootFolder.length()) + versionFN;
	}

	std::string FileHandler::getReposAbsolutePath(const std::string& filePath)
	{
		std::string applicationRootFolder = "\\" + getApplicationRootFolder() + "\\";

		//This code standardizes the path if different slashes are used
		std::string absoluteDestPath = ApplicationConstants::getDestAbsolutePath();
		std::size_t foundFPIndex = filePath.rfind(applicationRootFolder);

		return absoluteDestPath + "\\" +
			filePath.substr(foundFPIndex + applicationRootFolder.length());
	}

	//////////////////////////////////////////////////////////////////////////
	// Trims the namespace from DB elemnts key
	std::string FileHandler::trimNameSpace(const std::string& fileName) {
		return fileName.substr(fileName.find_first_of('.') + 1);
	}

	//////////////////////////////////////////////////////////////////////////
	// This function returns the application root folder name
	inline std::string FileHandler::getApplicationRootFolder() {
		std::string localPath = ApplicationConstants::getLocalPath();
		return localPath.substr(localPath.find_last_of("\\/") + 1);
	}
}



#ifdef TEST_FILENAME

using namespace LocalSVN;

int main()
{
	FileHandler fhl;
	std::string path = fhl.getReposAbsolutePath("DbCore.h.0");
	std::cout << "Absolute path of reposiory file";

	std::cout << "Local path of absolute file";
	std::string localPath = fhl.constructRelFilePath("../../DbCore.h.0", "", false);

	std::cout << "File Saved";
	fhl.fileSave(path, localPath);
}

#endif TEST_FILENAME
