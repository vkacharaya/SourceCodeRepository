#pragma once
////////////////////////////////////////////////////////////////////////////
// PayLoad.h - Provides an overall utilities package related to payload   //
//                                                                        //
// ver 1.0																  //
// Author: Vishnu Karthik.R, CSE687 - Object Oriented Design, Spring 2018 //
////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package contains class to manipulate payload object which
* will be passed on to the DBElement object.

* Required Files:
* ---------------
* Condition.h, Condition.cpp
*
* Maintenance History:
* --------------------
* ver 1.0 : 16 Jan 2018
* - Initial Implementation

*/

#include <vector>
#include <regex>
#include <iostream>
#include "../Query/Condition.h"
#include "../Utils/ApplicationConstants.h"

using namespace LocalSVN;

namespace NoSqlDb
{
	//////////////////////////////////////////////////////////////////////////
	// PayLoad
	// Used to pass to the DbCore object in place of the templated data
	//////////////////////////////////////////////////////////////////////////
	class PayLoad
	{
	public:

		PayLoad() {}

		PayLoad(const std::string& path, const std::vector<std::string>& categories)
			:path_(path), categories_(categories) {}

		/////////////////////////////////////////////////////////////////////
		// Function sets the path passed to the local member function
		void setFilePath(const std::string& path) { path_ = path; }

		/////////////////////////////////////////////////////////////////////
		// Set the check in status
		void setCheckInStatus(const ApplicationConstants::CheckinStatus& status) { status_ = status; }

		/////////////////////////////////////////////////////////////////////
		// Sets the category member in this class with the values passed
		void setCategory(const std::vector<std::string>& categories) { categories_ = categories; }

		/////////////////////////////////////////////////////////////////////
		// Inserts single category to the categor list
		void pushFileCategory(const std::string& category);

		/////////////////////////////////////////////////////////////////////
		// Inserts single category to the categor list
		std::string getPayLoadStatusStr(ApplicationConstants::CheckinStatus status);

		/////////////////////////////////////////////////////////////////////
		// Gets the file path stored in this object
		std::string& getFilePath() { return path_; }

		/////////////////////////////////////////////////////////////////////
		// Gets all the categories saved in the object
		std::vector<std::string>& getCategories() { return categories_; }

		/////////////////////////////////////////////////////////////////////
		// Sets the category member in this class with the values passed
		void setUser(const std::string& author) { checkInAuthor_ = author; }

		/////////////////////////////////////////////////////////////////////
		// Sets the category member in this class with the values passed
		std::string& getUser() { return checkInAuthor_; }

		/////////////////////////////////////////////////////////////////////
		// Sets the category member in this class with the values passed
		void clearCategory();

		/////////////////////////////////////////////////////////////////////
		// Get the check in status
		ApplicationConstants::CheckinStatus getCheckInStatus() { return status_; }

		/////////////////////////////////////////////////////////////////////
		// Compares the pattern in the category
		bool checkPatternInCategory(Conditions& cond);

		/////////////////////////////////////////////////////////////////////
		// Check the pattern in the file path
		bool checkFilePath(Conditions& cond);

		/////////////////////////////////////////////////////////////////////
		// Checks whether the passed value is present in the Category List
		bool containsCateg(const std::string& category);

		/////////////////////////////////////////////////////////////////////
		// - Operator overloaded for equality operator 
		//   to compare two payloads
		bool operator==(const PayLoad& rOperand);

		/////////////////////////////////////////////////////////////////////
		// - Operator overloaded for inequality
		bool operator!=(const PayLoad& rOperand);

		/////////////////////////////////////////////////////////////////////
		// - Operator overloaded for output stream
		friend std::ostream& operator<<(std::ostream& out, PayLoad payloadObj);

	private:
		std::string path_;
		std::string checkInAuthor_ = "";
		std::vector<std::string> categories_;
		ApplicationConstants::CheckinStatus status_;
	};
}
