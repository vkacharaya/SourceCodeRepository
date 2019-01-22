#pragma once
/////////////////////////////////////////////////////////////////////////////
// Browse.h - Supports browsing operation where the user will be able to   //
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

#include <iostream>
#include "../Query/Query.h"

using namespace NoSqlDb;


/////////////////////////////////////////////////////////
// Namespace used for the repository
namespace LocalSVN
{
	/////////////////////////////////////////////////////////
	// Browse class to retriev metadata and showing text
	// in a new process
	class Browse
	{
	public:
		//Alias Check In Item inserted into DB 
		using CheckInItem = DbElement<PayLoad>;

		/////////////////////////////////////////////////////////
		// constructor for the browser with query object
		// from Repository Core
		Browse(Query<PayLoad>& db) :_query(db) {}

		/////////////////////////////////////////////////////////
		// provides the refernece to the dbelement by querying 
		// using the filename and namespace
		CheckInItem getDbElement(const std::string& fileName, const std::string& namepsace);

		/////////////////////////////////////////////////////////
		// Function used to run the process in which the text 
		// opens in a new notepad instance each time when called
		void runProcess(const std::string& fileKey, const std::string& namepsace, const std::string& appPath);

	private:
		/////////////////////////////////////////////////////////
		// this object is used to query the db to find the 
		// related key value for getting the metadata info
		Query<PayLoad>& _query;
	};
}
