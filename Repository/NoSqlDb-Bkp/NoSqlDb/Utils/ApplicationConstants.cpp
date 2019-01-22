/////////////////////////////////////////////////////////////////////////////
// ApplicationConstants.cpp - This package contains constants related to   //
//                            the file repository configuartion			   //
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

#include "ApplicationConstants.h"

using namespace LocalSVN;
/////////////////////////////////////////////////////////
// Destination repository folder
std::string ApplicationConstants::_destinationFolder = "../../SourceCodeRepository/NoSqlDb";
/////////////////////////////////////////////////////////
// local source code folder path
std::string ApplicationConstants::_localSourceFolder = "../../NoSqlDb";
/////////////////////////////////////////////////////////
//XML repository path
std::string ApplicationConstants::_xmlPath = "../DbCore/NoSqlDbFile.xml";
/////////////////////////////////////////////////////////
//XML repository path
std::string ApplicationConstants::_tmpLocalFilePath = "";