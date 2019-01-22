#pragma once
/////////////////////////////////////////////////////////////////////////////
// CheckOut.h - retrieves package files, removing version information from // 
//				their filenames. Retrieved files will be copied to a       //
//				specified directory.                                       //
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
* This module provides class, CheckOut
*
* The CheckOut class supports retrieves package files, removing version information 
* from their filenames. Retrieved files will be copied to a specified directory. 
*
* Required Files:
* ===============
* CheckOut.h, CheckOut.cpp, Versioning.h, Versioning.cpp, FileSystemDemo.h
* FileNameParser.h,Graph.h
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
#include "../GraphWalkDemo/Graph.h"
#include "../CheckIn/GraphUtils.h"

namespace LocalSVN
{
	/////////////////////////////////////////////////////////
	// Responsible for doing checkout operation
	class CheckOut
	{
	public:
		using ItemCollection = DbCore<PayLoad>;
		using Sptr = std::shared_ptr < Node<std::string> >;
		using Graph = Graph<std::string>;

		CheckOut(ItemCollection& map) :_vs{ map }, _map{ map } {}

		/////////////////////////////////////////////////////////
		// Function used to get the latest version files
		void getLatestVersionFile(const std::string& fileKey, const std::string& namespaceKey, const bool& dependencyNeeded);

		/////////////////////////////////////////////////////////
		// Function used to get the particular version files
		void getParticularFile(const std::string& fileKey, const std::string& namespaceKey, const bool& dependencyNeeded);

	private:

		/////////////////////////////////////////////////////////
		// versioning obejct
		Versioning _vs;
		/////////////////////////////////////////////////////////
		// Collection of check-in item details
		ItemCollection& _map;
		////////////////////////////////////////////////////////
		// Dependency file checkout
		void checkOutDependencyOp(const std::string & latestVersion, const bool & dependencyNeeded);
	};
}
