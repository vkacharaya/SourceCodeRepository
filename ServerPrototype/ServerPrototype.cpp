/////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 1.0                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018           //
// Author: Vishnu Karthik Ravindran, Spring 2018, OOD, Project 3       //
/////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* ---------------------
*  Package contains one class, Server, that contains a Message-Passing Communication
*  facility. It processes each message by invoking an installed callable object
*  defined by the message's command key.
*
*  Message handling runs on a child thread, so the Server main thread is free to do
*  any necessary background processing (none, so far).
*
*  Required Files:
* -----------------
*  ServerPrototype.h, ServerPrototype.cpp
*  Comm.h, Comm.cpp, IComm.h
*  Message.h, Message.cpp
*  FileSystem.h, FileSystem.cpp
*  Utilities.h
*
*  Maintenance History:
* ----------------------
*  ver 1.0 : 3/27/2018
*  - first release
*  ver 2.0 : 4/10/2018
*  - Added check-in, check-out, browsing, connect lambda expressions
*    to be invoked using the command received from Client
*/

#include "ServerPrototype.h"
#include <Regex>
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include <chrono>
#include "../Repository/NoSqlDb-Bkp/NoSqlDb/RepositoryCore/RepositoryCore.h"


using namespace NoSqlDb;
using namespace LocalSVN;
std::string saveFilePath;
std::string sendFilePath;
namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository;
using namespace FileSystem;

using Msg = MsgPassingCommunication::Message;

/////////////////////////////////////////////////////////////////////////
// Used to get the files in the server based on the path
// send to the server from client
Files Server::getFiles(const Repository::SearchPath& path)
{
	return Directory::getFiles(path);
}

/////////////////////////////////////////////////////////////////////////
// Used to get the directories in the server based on the path
// send to the server from client
Dirs Server::getDirs(const Repository::SearchPath& path)
{
	return Directory::getDirectories(path);
}

/////////////////////////////////////////////////////////////////////////
// Used to include the file system copy method.
// Functionalities are same as the file system copy
void Server::fileCopy(const std::string& sourceFilePath, const std::string& destinationFilePath)
{
	Directory::createDirectory(sourceFilePath);
	Directory::createDirectory(destinationFilePath);

	FileSystem::File::copy(sourceFilePath, destinationFilePath);
}

/////////////////////////////////////////////////////////////////////////
// Method used to show the message received
// in a human readable format
template<typename T>
void show(const T& t, const std::string& msg)
{
	std::cout << "\n  " << msg.c_str();
	for (auto item : t)
	{
		std::cout << "\n    " << item.c_str();
	}
}

/////////////////////////////////////////////////////////////////////////
// Method used to show the message received
// in a human readable format
std::function<Msg(Msg)> echo = [](Msg msg) {
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	return reply;
};

/////////////////////////////////////////////////////////////////////////
// Method used to show the message received
// in a human readable format
std::function<Msg(Msg)> getFiles = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("Browsing Pkg: getFiles");
	reply.filter(msg.filter());
	std::string path = msg.value("path");
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != "\\")
			searchPath = searchPath + path;
		reply.path(searchPath);
		Files files = Server::getFiles(searchPath);
		size_t count = 0;
		for (auto item : files)
		{
			if (std::regex_match(item, std::regex(msg.filter() + "(.*)")) || msg.filter() == "")
			{
				std::string countStr = Utilities::Converter<size_t>::toString(++count);
				reply.attribute("file" + countStr, item);
			}
		}
	}
	else
	{
		std::cout << "\n  getFiles message did not define a path attribute";
	}
	return reply;
};

/////////////////////////////////////////////////////////////////////////
// Lambda expression for checkIn command. This constructs a
// success message and copies the file into server folder from server 
// sendfiles folder
std::function<Msg(Msg)> checkIn = [](Msg msg) {
	//This copies the file recieved in servers saveFile path to the server storage path
	RepositoryCore r(msg.user(), Path::getFullFileSpec(repXMLFilePath));
	r.reinitiateAppPaths(msg.path() + "/" + Path::getRootFolder(msg.path()),
		Path::getFullFileSpec(storageRoot) + "/" + Path::getRootFolder(msg.path()), "",
		Path::getFullFileSpec(saveFilePath));
	r.initiateSingleCheckin(msg.path(), msg.nameSP(), msg.value("desc"));
	r.editMetadata(msg.path(), msg.nameSP()).payLoad().clearCategory();
	for (auto item : msg.attributes())
	{
		if (std::regex_match(item.first, std::regex("category(.*)")))
		{
			r.editMetadata(msg.path(), msg.nameSP()).payLoad().pushFileCategory(item.second);
			r.updateDB();
		}
	}
	r.editMetadata(msg.path(), msg.nameSP()).children().clear();
	for (auto item : msg.attributes())
	{
		if (std::regex_match(item.first, std::regex("dependents(.*)")))
		{
			r.editMetadata(msg.path(), msg.nameSP()).addChild(msg.nameSP() + "." + item.second);
			r.updateDB();
		}
	}
	r.updateDB();
	Conditions cond;
	DbElement<PayLoad> element = r.GetMetaData(msg.value("recieveFile"), msg.nameSP(), cond, false);
	Msg reply;
	reply.to(msg.from());
	reply.attribute("status", element.payLoad().getPayLoadStatusStr(element.payLoad().getCheckInStatus()));
	reply.attribute("user", element.payLoad().getUser());
	reply.attribute("dateTime", (std::string)element.dateTime());
	reply.from(msg.to());
	reply.command("checkIn");
	reply.path(storageRoot + "\\" + msg.path());
	reply.attribute("message", "Checked-In successfully. It is available in \"" + storageRoot + "\"");
	showDb(r.getDB());
	return reply;
};

/////////////////////////////////////////////////////////////////////////
// Lambda expression for close command. This constructs a
// success message and inserts value into the DB
std::function<Msg(Msg)> closeCheckInDisp = [](Msg msg) {
	RepositoryCore r(msg.user(), Path::getFullFileSpec(repXMLFilePath));
	r.reinitiateAppPaths(msg.path() + "/" + Path::getRootFolder(msg.path()),
		Path::getFullFileSpec(storageRoot) + "/" + Path::getRootFolder(msg.path()), "",
		Path::getFullFileSpec(saveFilePath));

	std::string returnMsg = r.closeCheckIn(msg.path(), msg.nameSP());
	Conditions cond;
	DbElement<PayLoad> element = r.GetMetaData(msg.filter(), msg.nameSP(), cond, false);
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.attribute("status", element.payLoad().getPayLoadStatusStr(element.payLoad().getCheckInStatus()));
	reply.attribute("user", element.payLoad().getUser());
	reply.attribute("dateTime", (std::string)element.dateTime());
	reply.command(msg.command());
	reply.attribute("message", returnMsg);
	showDb(r.getDB());
	return reply;
};

/////////////////////////////////////////////////////////////////////////
// Lambda expression for getMetaData command. This constructs a
// success message and retieves the value from DB to show to the user
std::function<Msg(Msg)> getMetaDataFromDb = [](Msg msg) {
	RepositoryCore r(msg.user(), Path::getFullFileSpec(repXMLFilePath));

	Conditions cond;
	DbElement<PayLoad> element;
	if (msg.value("executedBy") == "metaDataCheckin")
	{
		element = r.GetMetaData(msg.filter(), msg.nameSP(), cond, false);
	}
	else
	{
		element = r.GetMetaData(msg.filter(), msg.nameSP(), cond, true, true);
	}

	Msg reply;
	int count = 0;
	FileHandler fh;
	for (auto item : element.children())
	{
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("dependents" + countStr, fh.trimNameSpace(item));
	}
	count = 0;
	for (auto item : element.payLoad().getCategories())
	{
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("categories" + countStr, item);
	}

	reply.attribute("status", element.payLoad().getPayLoadStatusStr(element.payLoad().getCheckInStatus()));
	reply.attribute("user", element.payLoad().getUser());
	reply.attribute("dateTime", (std::string)element.dateTime());
	reply.attribute("description", element.descrip());

	reply.filter(msg.filter());
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	reply.attribute("message", "Retrieved the results needed.");
	return reply;
};

/////////////////////////////////////////////////////////////////////////
// Lambda expression for query command. This constructs a
// keys with the filters sent from the client
std::function<Msg(Msg)> queryFiles = [](Msg msg) {
	RepositoryCore r(msg.user(), Path::getFullFileSpec(repXMLFilePath));

	Conditions cond;
	std::string version;
	for (auto item : msg.attributes())
	{
		if (std::regex_match(item.first, std::regex("filter(.*)")))
		{
			int index = item.second.find_first_of('-');
			std::string firstPart = item.second.substr(0, index);
			std::string secondpart = item.second.substr(index + 1, item.second.length());
			if (firstPart == "Category")
				cond.setCategory(secondpart);
			if (firstPart == "File Names")
				cond.setNameRegex(secondpart);
			if (firstPart == "Children")
				cond.setCrenRegex(secondpart);
			if (firstPart == "Versions")
				cond.setVersion(secondpart);
		}
	}
	std::vector<std::string> keys = r.GetQUeriedKeys(cond);
	if (msg.value("IsOrphan") == "1")
		keys = r.GetFilteredFilesWIthNoP(keys);
	FileHandler fh;

	Msg reply; int count = 0;
	for (auto files : keys)
	{
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("file" + countStr, fh.trimNameSpace(files));
	}
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	reply.attribute("message", "Files queried successfully with the results.");
	return reply;
};

/////////////////////////////////////////////////////////////////////////
// Lambda expression for browse command. This constructs a
// message containing file metadata and sends file content to client 
std::function<Msg(Msg)> browse = [](Msg msg) {

	Server::fileCopy(Path::getFullFileSpec(msg.path()), Path::getFullFileSpec((sendFilePath + "/" + msg.filter())));
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.file(msg.filter());
	reply.path(msg.path());
	reply.attribute("message", "Browsed to get file contents and File Meta Data. File can be found in " + saveFilePath);
	RepositoryCore r(msg.user(), Path::getFullFileSpec(repXMLFilePath));
	Conditions cond;
	DbElement<PayLoad> element = r.GetMetaData(msg.filter(), msg.nameSP(), cond, true, true);
	int count = 0;
	FileHandler fh;
	for (auto item : element.children())
	{
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("dependents" + countStr, fh.trimNameSpace(item));
	}
	count = 0;
	for (auto item : element.payLoad().getCategories())
	{
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("categ" + countStr, item);
	}

	reply.attribute("status", element.payLoad().getPayLoadStatusStr(element.payLoad().getCheckInStatus()));
	reply.attribute("description", element.descrip());
	reply.command("Browse & viewMetaData");
	return reply;
};

/////////////////////////////////////////////////////////////////////////
// Lambda expression for getStatus command. This constructs a
// message containing file status and sends it to the client
std::function<Msg(Msg)> getStatus = [](Msg msg) {
	RepositoryCore r(msg.user(), Path::getFullFileSpec(repXMLFilePath));
	Conditions cond;
	Msg reply;
	int count = 0, counter = 0;
	for (auto item : msg.attributes())
	{
		if (std::regex_match(item.first, std::regex("file(.*)")))
		{
			DbElement<PayLoad> element = r.GetMetaData(item.second, msg.nameSP(), cond, false);
			if (element.payLoad().getCheckInStatus() == ApplicationConstants::CheckinStatus::New)
			{
				std::string countStr = Utilities::Converter<size_t>::toString(++count);
				reply.attribute("new" + countStr, item.second);
				count++;
			}
			if (element.payLoad().getCheckInStatus() == ApplicationConstants::CheckinStatus::Open)
			{
				std::string countStr = Utilities::Converter<size_t>::toString(++counter);
				reply.attribute("open" + countStr, item.second);
				counter++;
			}
		}
	}
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	reply.attribute("message", "File status retrieved successfully.");
	return reply;
};

/////////////////////////////////////////////////////////////////////////
// Lambda expression for editMetaData command. This constructs a
// message containing file metadata and sends file content to client 
std::function<Msg(Msg)> editMetaDataFn = [](Msg msg) {
	Msg reply = checkIn(msg);
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	reply.attribute("message", "Updated Successfully.");
	return reply;
};

/////////////////////////////////////////////////////////////////////////
// Lambda expression for checkOut command. This constructs a
// success message and returns the file to the client
std::function<Msg(Msg)> checkOut = [](Msg msg) {

	RepositoryCore r(msg.user(), Path::getFullFileSpec(repXMLFilePath));
	r.reinitiateAppPaths(Path::getFullFileSpec(sendFilePath), "");
	bool isDependencyNeeded = msg.value("Dependents") == "1";
	Conditions cond; DbElement<PayLoad> element;
	if (msg.value("IsLatest") == "0")
	{
		element = r.GetMetaData(msg.filter(), msg.nameSP(), cond, false, true);
		r.getParticularFile(msg.filter(), msg.nameSP(), isDependencyNeeded);
		msg.path(msg.path().substr(0, msg.path().find_last_of('.')));
		msg.filter(msg.filter().substr(0, msg.filter().find_last_of('.')));
	}
	else
	{
		element = r.GetMetaData(msg.filter(), msg.nameSP(), cond, false, false);
		r.getLatestVersionFile(msg.filter(), msg.nameSP(), isDependencyNeeded);
		msg.path(msg.path().substr(0, msg.path().find_last_of('.')));
	}
	Msg reply; int count = 0, counter = 0;
	if (msg.containsKey("verboseBytes"))
		reply.attribute("verboseBytes", "");
	reply.to(msg.from()); reply.from(msg.to());
	reply.attribute("sendingFile", msg.filter());
	if (isDependencyNeeded)
	{
		size_t count = 0;
		size_t startindex = Path::getFullFileSpec(msg.path()).rfind(Path::getFullFileSpec(storageRoot));
		size_t lengthString = Path::getFullFileSpec(storageRoot).length();
		std::string rootPath = Path::getFullFileSpec(msg.path()).substr(startindex + lengthString,
			Path::getFullFileSpec(msg.path()).length());
		FileHandler fh;
		for (auto elem : element.children())
		{
			std::string rootFolder = Path::getRootFolder(rootPath);
			DbElement<PayLoad> childElement = r.GetMetaData(fh.trimNameSpace(elem), msg.nameSP(), cond, false, true);
			size_t startindex = Path::getFullFileSpec(childElement.payLoad().getFilePath()).rfind("\\" + rootFolder);
			std::string childPath = Path::getFullFileSpec(childElement.payLoad().getFilePath()).substr(startindex,
				Path::getFullFileSpec(childElement.payLoad().getFilePath()).length());
			std::string revivedNS = childPath.substr(0, childPath.find_last_of('.'));
			std::string countStr = Utilities::Converter<size_t>::toString(++count);
			reply.attribute("sendingFile" + countStr, fh.trimNameSpace(elem.substr(0, elem.find_last_of('.'))));
			reply.attribute("path" + countStr, storageRoot + "\\" + revivedNS);
			count++;
		}
	}
	reply.path(msg.path());
	reply.attribute("message", "Checked-Out successfully.");
	reply.command("checkOut");
	return reply;
};

/////////////////////////////////////////////////////////////////////////
// Lambda expression for connect command. This constructs a
// success message and returns the client
std::function<Msg(Msg)> connectIP = [](Msg msg) {
	RepositoryCore r(msg.user(), Path::getFullFileSpec(repXMLFilePath));
	showDb(r.getDB());
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.attribute("message", "Connection established successfully from \"" + msg.from().toString()
		+ "\" to \"" + msg.to().toString() + "\"");
	reply.command("connect");
	return reply;
};

/////////////////////////////////////////////////////////////////////////
// Used to get the directories in the server based on the path
// send to the server from client
std::function<Msg(Msg)> getDirs = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("Browsing Pkg: getDirs");
	std::string path = msg.value("path");
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != "\\")
			searchPath = searchPath + "\\" + path;
		Files dirs = Server::getDirs(searchPath);
		size_t count = 0;
		for (auto item : dirs)
		{
			if (item != ".." && item != ".")
			{
				std::string countStr = Utilities::Converter<size_t>::toString(++count);
				reply.attribute("dir" + countStr, item);
			}
		}
	}
	else
	{
		std::cout << "\n  getDirs message did not define a path attribute";
	}
	return reply;
};

/////////////////////////////////////////////////////////////////////////
// Main method for server. Communication channel for server is created in 
// this main thread.
int main()
{
	//The below line can be commented when the test cases not needed
	FileSystem::File::copy("../ServerStorage/NoSqlDbFile-TestCase.xml", repXMLFilePath);
	SetConsoleTitleA("Server Console");
	std::cout << "\n  **********************************************************************************************";
	std::cout << "\n  * PROJECT 4 - DEMONSTRATION";
	std::cout << "\n  **********************************************************************************************";

	Server server(serverEndPoint, "ServerPrototype");

	MsgPassingCommunication::Context* pCtx = server.getContext();
	pCtx->saveFilePath = "../CppCommWithFileXfer/SaveFiles";
	pCtx->sendFilePath = "../CppCommWithFileXfer/SendFiles";
	sendFilePath = pCtx->sendFilePath;
	saveFilePath = pCtx->saveFilePath;
	server.start();
	server.addMsgProc("echo", echo);
	server.addMsgProc("Browsing Pkg: getFiles", getFiles);
	server.addMsgProc("Browsing Pkg: getDirs", getDirs);
	server.addMsgProc("serverQuit", echo);
	server.addMsgProc("checkIn", checkIn);
	server.addMsgProc("checkOut", checkOut);
	server.addMsgProc("Browse & viewMetaData", browse);
	server.addMsgProc("connect", connectIP);
	server.addMsgProc("closeCheckIn", closeCheckInDisp);
	server.addMsgProc("GetMetaData", getMetaDataFromDb);
	server.addMsgProc("getStatus", getStatus);
	server.addMsgProc("editMetaData", editMetaDataFn);
	server.addMsgProc("queryFiles", queryFiles);


	server.processMessages();
	Msg msg(serverEndPoint, serverEndPoint);
	std::cout << "\n  press enter to exit";
	std::cout << "\n";
	std::cin.get();
	std::cout << "\n";

	msg.command("serverQuit");
	server.postMessage(msg);
	server.stop();
	return 0;
}

