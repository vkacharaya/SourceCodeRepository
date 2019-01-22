#include "PayLoadUtils.h"

void Test() {}

using namespace NoSqlDb;
#ifdef TEST_PROCESS2

int main()
{
	std::cout << "\n  Demonstrating code pop-up";
	std::cout << "\n ===========================";

	Process p;
	p.title("test application");
	std::string appPath = "c:/su/temp/Project4Sample/Debug/CodeAnalyzer.exe";
	p.application(appPath);

	std::string cmdLine = "c:/su/temp/Project4Sample/Debug/CodeAnalyzer.exe c:/su/temp/Project4Sample  *.h *.cpp /m /r /f";
	p.commandLine(cmdLine);

	std::cout << "\n  starting process: \"" << appPath << "\"";
	std::cout << "\n  with this cmdlne: \"" << cmdLine << "\"";
	if (!p.create())
	{
		std::cout << "\n  could not create process " << appPath;
	}

	CBP callback = []() { std::cout << "\n  --- child process exited ---"; };
	p.setCallBackProcessing(callback);
	p.registerCallback();

	//std::cout << "\n  after OnExit";
	//std::cout.flush();
	//char ch;
	//std::cin.read(&ch,1);
	return 0;
}

#endif TEST_PROCESS2

//Adding a simple function will eliminate the linker error