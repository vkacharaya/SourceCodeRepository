/////////////////////////////////////////////////////////////////////////////
// Browse.cpp - Supports browsing operation where the user will be able to //
//			  get the latest version package information and also the text // 
//			  contents in the new process - notepad                        //
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
* This module provides class, Browse
*
* The Browse class supports opening the file input by the user in a new
* process. The input is also the file name and namespace given by the
* user. It also supports getting the description information from the elements
* in the Database.
*
* Public Interface:
* =================
* DbElement<PayLoad> getDbElement(const std::string& fileName, const std::string& namepsace);
* void runProcess(const std::string& fileKey, const std::string& namepsace);
*
* Required Files:
* ===============
* Browse.h, Browse.cpp, Query.h, Process.h
*
* Maintenance History:
* ====================
* ver 1.0 : 10 Mar 2018
*			Initial Implementation
*/

#include "Browse.h"
#include "../Process/Process.h"

namespace LocalSVN
{
	/////////////////////////////////////////////////////////
	// provides the refernece to the dbelement by querying 
	// using the filename and namespace
	Browse::CheckInItem Browse::getDbElement(const std::string& fileName, const std::string& namepsace)
	{
		return _query.valueOf(namepsace + "." + fileName);
	}

	/////////////////////////////////////////////////////////
	// Function used to run the process in which the text 
	// opens in a new notepad instance each time when called
	void Browse::runProcess(const std::string& fileName, const std::string& namepsace, const std::string& appPath)
	{
		try
		{
			//This code runs the notepad instance
			Process p;
			p.application(appPath);

			//Command line for opening the file in notepad instance
			std::string cmdLine = "/A " + getDbElement(fileName, namepsace).payLoad().getFilePath();
			p.commandLine(cmdLine);
			p.create();
			p.registerCallback();
			std::cout.flush();
		}
		catch (std::exception ex)
		{
			//Exception when the path is worng
			std::cout << ex.what();
		}
	}
}

#ifdef TEST_BROWSE

int main()
{
	DbCore<PayLoad> db;
	DbElement<PayLoad> elemdb;

	elemdb.name("DbCore.h.0");
	db.insert("DbCore.h.0", elemdb);

	Query<PayLoad> query(db);
	Browse br(query);

	DbElement<PayLoad> elem = br.getDbElement("Dbcore.h.0", "NoSqlDb");
	elem.name("DbCore.h.0").descrip("Test Description");

	std::cout << "Runs the process";
	br.runProcess("Dbcore.h.0", "NoSqlDb", "c:/windows/system32/notepad.exe");
}

#endif TEST_BROWSE
