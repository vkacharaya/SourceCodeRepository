
/////////////////////////////////////////////////////////////////////
// TestExecutiveRepos.cpp - Provides the functions to be executed by Executive class//
// ver 1.0																     //
// Author: Vishnu Karthik.R, CSE687 - Object Oriented Design, Spring 2018
/////////////////////////////////////////////////////////////////////
/*

* Package Operations:
* -------------------
* This package contains functions to be executed by Executive clas

*
* Maintenance History:
* --------------------

* ver 1.0 : 16 Jan 2018
* - Initial Implementation

*/

#include "TestExecutiveRepos.h"
#include "../Utilities/StringUtilities/StringUtilities.h"
#include "../Utilities/TestUtilities/TestUtilities.h"

namespace LocalSVN
{

	auto putLine = [](size_t n = 1, std::ostream& out = std::cout)
	{
		Utilities::putline(n, out);
	};

	//----< demo requirement #1 >------------------------------------------
	bool testR1()
	{
		Utilities::title("Demonstrating Requirement #1 - C++ 11 version");
		std::cout << "\n  " << typeid(std::function<bool()>).name()
			<< ", declared in this function, "
			<< "\n  is only valid for C++11 and later versions.";
		putLine();
		return true; // would not compile unless C++11
	}

	//----< demo requirement #2 >------------------------------------------
	bool testR2()
	{
		Utilities::title("Demonstrating Requirement #2 - NEW DELETE SYNTAX");
		std::cout << "\n  A visual examination of all the submitted code "
			<< "will show only\n  use of streams and operators new and delete.";
		putLine();
		return true;
	}

	//----< demo requirement #3 >------------------------------------------
	bool testR3()
	{
		Utilities::title("Demonstrating Requirement #3 - PACKAGE CREATION");
		std::cout << "\n  A visual examination of all the submitted code "
			<< "will show the creation of packages in the solution";
		putLine();
		return true;
	}

	//----< demo requirement #3-a >------------------------------------------
	bool test3a()
	{
		Utilities::title("Demonstrating Requirement #3a - CHECK OUT OPERATION");
		putLine();
		File::copy(Path::getFullFileSpec("../DbCore/xml/NoSqlDbFile.xml"), Path::getFullFileSpec("../DbCore/NoSqlDbFile.xml"));
		RepositoryCore r("User1");

		std::cout << "  Showing DbCore.cpp latest version contents from database.\n";
		showHeader();
		showElem(r.editMetadata("DbCore.cpp", "NoSqlDb"));
		putLine();
		std::cout << "\n  User1 is trying to check out latest DBCore.cpp file.....\n\n";
		std::cout << "  DbCore.cpp latest file is checked out into the local source folder.\n\n";
		r.getLatestVersionFile("DbCore.cpp", "NoSqlDb", true);
		std::cout << "  Checked-out file/pkg with all its dependencies(../DbCore/DbCore.cpp) can be found in the \"../DbCore/DbCore.h\" by removing version numbers\n\n";
		std::cout << "  PLEASE BE CAREFUL WHILE CHECKOUT AS THE FILE IN THE MAIN PROJECT WILL BE OVERWRITTEN\n";
		putLine();
		return true;
	}

	//----< demo requirement #4 >------------------------------------------
	bool test4()
	{
		Utilities::title("Demonstrating Requirement #4 - CHECKIN OPERATION");
		RepositoryCore r("User1");
		std::cout << "\n  Checked - in the DBCore.h with status as Open by User1\n\n";
		r.initiateSingleCheckin("../DbCore/DbCore.h", "NoSqlDb");
		///////////////////////////////Resret DB///////////////////////////
		r.editMetadata("DbCore.cpp", "NoSqlDb").payLoad().setCheckInStatus(ApplicationConstants::CheckinStatus::Open);
		r.editMetadata("DbCore.cpp", "NoSqlDb").removeChild("NoSqlDb.DbCore.h.0");
		r.editMetadata("DbCore.h", "NoSqlDb").payLoad().setCheckInStatus(ApplicationConstants::CheckinStatus::Open);
		r.editMetadata("DbCore.h", "NoSqlDb").removeChild("NoSqlDb.DbCore.cpp.0");
		r.updateDB();
		std::cout << "  Can find the file in \"" + ApplicationConstants::getRepositoryPath() + "\\DbCore\" with version number\n\n";
		std::cout << "  Showing DbCore.h latest version inserted into database.\n";
		showHeader();
		showElem(r.editMetadata("DbCore.h", "NoSqlDb"));
		Utilities::title("\n  Demonstrating Requirement #4a - Adding dependency to the DbCore.cpp file");

		r.addDependency({ "NoSqlDb.DbCore.h" }, "DbCore.cpp", "NoSqlDb");

		showHeader();
		showElem(r.editMetadata("DbCore.cpp", "NoSqlDb"));

		std::cout << "\n\n  Added DbCore.h.0 as dependent to DbCore.cpp.0.\n\n";

		return true;
	}

	//----< demo requirement #4 >------------------------------------------
	bool test4a()
	{
		RepositoryCore r("User1");
		Utilities::title("Demonstrating Requirement #4b - Editing the New Check in item");
		//////////////////////// Initialize check in //////////////////////
		std::cout << "\n  Performing a new check-in for showing editing demo. File can be found in \"Source Code Repository/Query\"\n";
		r.initiateSingleCheckin("../Query/Query.h", "NoSqlDb");
		showHeader();
		showElem(r.editMetadata("Query.h", "NoSqlDb"));
		r.editMetadata("Query.h", "NoSqlDb").descrip("Edited Description");
		r.updateDB();
		//////////////////////Editing description//////////////////////////
		std::cout << "\n\n  Edited description found in the NoSQL DB for Query.h.\n";
		showHeader();
		showElem(r.editMetadata("Query.h", "NoSqlDb"));
		////////////////////Closed the check-in//////////////////////////
		std::cout << "\n\n  --------------Unable to close with dependencts open in the Query.h file--------------\n";
		r.closeCheckIn("Query.h", "NoSqlDb");
		showHeader();
		///////////////////Shows DB Content////////////////////////////
		DbCore<PayLoad> db = r.loadXMLRepository();
		Query<PayLoad> q(db);
		Conditions cond; cond.setNameRegex("Query.h");
		auto keys = q.filterOn(cond).getQueriedKeys();
		for (auto key : keys)
			showElem(q.valueOf(key));
		std::cout << "\n\n  -------------Closing Query.cpp file-------------\n";
		showHeader();
		r.closeCheckIn("Query.cpp", "NoSqlDb");
		db = r.loadXMLRepository();
		Query<PayLoad> q4(db);
		Conditions cond1; cond1.setNameRegex("Query.cpp");
		auto keys_ = q4.filterOn(cond1).getQueriedKeys();
		for (auto key : keys_)
			showElem(q.valueOf(key));
		std::cout << "\n\n  --------------New version created on checkedin when file closed-------------\n";
		showHeader();
		r.initiateSingleCheckin("../Query/Query.cpp", "NoSqlDb");
		db = r.loadXMLRepository();
		Query<PayLoad> q2(db);
		Conditions cond2; cond2.setNameRegex("Query.cpp");
		auto keysValue = q2.filterOn(cond2).getQueriedKeys();
		for (auto key : keysValue)
			showElem(q.valueOf(key));
		putLine();
		return true;
	}

	//----< demo requirement #4 >------------------------------------------
	bool test4b()
	{
		RepositoryCore r("User1");
		Utilities::title("\n  Demonstrating Requirement #4c - Closed Pending case");
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

		std::cout << "\n\n  Status of DbCore.h moved to closed pending\n";
		Utilities::title("\n  Demonstrating Requirement #4c - Auto closing Dbcore.h when its dependent Dbcore.cpp closed");
		r.closeCheckIn("DbCore.cpp", "NoSqlDb");
		showHeader();
		DbCore<PayLoad> db1 = r.loadXMLRepository();
		Query<PayLoad> q1(db1);
		Conditions cond2; cond2.setNameRegex("DbCore.h");
		Conditions cond1; cond1.setNameRegex("DbCore.cpp");
		auto keys2 = q1.filterOn(cond2).unionOn(cond1).getQueriedKeys();
		for (auto key : keys2)
			showElem(q1.valueOf(key));
		putLine();

		Utilities::title("  Demonstrating Requirement #4d - Ownership cases");
		RepositoryCore r2("User2");
		std::cout << "\n  User2 trying to checkin the Query.h. The author is User1. \n  ";

		r2.initiateSingleCheckin("../Query/Query.h", "NoSqlDb");
		std::cout << "\n  The error message will be shown for close, check-in and editing element";
		putLine();
		return true;
	}

	//----< demo requirement #5 >------------------------------------------
	bool test5()
	{
		Utilities::title("  Demonstrating Requirement #5 - Browse Operation");
		RepositoryCore rc1("User1");
		std::cout << "\n  Browsing DbCore.h.0 and the notepad opened on demand\n";
		rc1.browseFile("DbCore.h.0", "NoSqlDb", "c:/windows/system32/notepad.exe", true);
		std::cout << "\n\n  Browsing Query.h.0 and the notepad opened on demand\n";
		rc1.browseFile("Query.h.0", "NoSqlDb", "c:/windows/system32/notepad.exe", true);
		putLine();
		return true;
	}

	//----< demo requirement #6 >------------------------------------------
	bool test6()
	{
		Utilities::title("Demonstrating Requirement #6 - File with open and closed check-ins");
		std::cout << "\n  File with open and closed check-in can be found in ../DbCore/NoSqlDb.xml \n\n";
		return true;
	}
}

using namespace LocalSVN;

int main()
{
	Utilities::Title("SOURCE CODE REPOSITORY - Local repository to store the files");
	Utilities::putline();
	TestExecutive ex;
	TestExecutive::TestStr ts1{ testR1, "Use C++11" };
	TestExecutive::TestStr ts2{ testR2, "Use streams and new and delete" };
	TestExecutive::TestStr ts3{ testR3, "PACKAGE CREATION" };
	TestExecutive::TestStr ts3a{ test3a, "CHECK OUT OPERATION" };
	TestExecutive::TestStr ts4{ test4, "CHECKIN OPERATION" };
	TestExecutive::TestStr ts4a{ test4a, "EDIT OPERATION" };
	TestExecutive::TestStr ts4b{ test4b, "CLOSED PENDING OPERATION" };
	TestExecutive::TestStr ts5{ test5, "Browse Operation" };
	TestExecutive::TestStr ts6{ test6, "File with open and closed check-ins" };
	ex.registerTest(ts1);
	ex.registerTest(ts2);
	ex.registerTest(ts3);
	ex.registerTest(ts3a);
	ex.registerTest(ts4);
	ex.registerTest(ts4a);
	ex.registerTest(ts4b);
	ex.registerTest(ts5);
	ex.registerTest(ts6);

	// define test structures with test function and message
	if (ex.doTests())
		std::cout << "\n  all tests passed";
	else
		std::cout << "\n  at least one test failed";
	int c;
	std::cin >> c;

	return 0;
}