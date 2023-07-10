
#include "injectorLogic.h"

void printLogo()
{
	setColor(LOGO_COLOR_BLUE);
	std::cout	<< "  ______ _     _                 _ _    _            _           _					   	\n"
				<< " |  ____(_)   | |               | | |  (_)          | |         | |					  	\n"
				<< " | |__   _ ___| |____      _____| | |   _ _ __      | | ___  ___| |_ ___  ____			\n"
				<< " |  __| | / __| '_ \\ \\ /\\ / / _ \\ | |  | | '_ \\ _   | |/ _ \\/ __| __/ _ \\|  __|	\n"
				<< " | |    | \\__ \\ | | \\ V  V /  __/ | |  | | | | | |__| |  __/ (__|  _ (_) | |			\n"
				<< " |_|    |_|___/_| |_|\\_/\\_/ \\___|_|_|  |_|_| |_|\\____/ \\___|\\___|\\__\\___/|_|	\n";
	setColor(STYLISH_COLOR_PINK);
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	setColor(DEFAULT_COLOR_WHITE);
}

// fetching the proc's list and looking for the currect module.
DWORD getProcIdViaString()
{
	// fancy output.
	char buffer[MAX_PATH];
	std::cout << "Enter a process [";
	setColor(INFO_COLOR_YELLOW);
	std::cout << "name.exe"; 
	setColor(DEFAULT_COLOR_WHITE);
	std::cout << "] to inject into : ";
	std::cin.getline(buffer, MAX_PATH);

	//// dynamic buffer to char.
	char* inputProcName = new char[strlen(buffer) + 1];
	strcpy(inputProcName, buffer);

	// fancy output.
	std::cout << '[';
	setColor(OK_COLOR_GREEN);
	std::cout << "OK";
	setColor(DEFAULT_COLOR_WHITE);
	std::cout << "]~> Trying to look for [";
	setColor(INFO_COLOR_YELLOW);
	std::cout << inputProcName;
	setColor(DEFAULT_COLOR_WHITE);
	std::cout << "]\n";
	
	// creating a handle to fetch a snap of proc list.
	DWORD curProcId = 0; // returen value.
	HANDLE procList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL); // procList.
	PROCESSENTRY32 thisProc; // [in/out] <-> lambda.
	thisProc.dwSize = sizeof(PROCESSENTRY32); // winapi req by default.

	// handle failed return 0.
	if (procList == INVALID_HANDLE_VALUE)
		return BAD_PROC_ID_VIA_SNAP;

	// handle success.
	if (Process32First(procList, &thisProc)) // return false if fails.
	{
		do
		{
			if (!_stricmp(inputProcName, thisProc.szExeFile))
			{
				curProcId = thisProc.th32ProcessID; // found the procId by string compare [non-case sense].
				// print procInfo.
				std::cout << '[';
				setColor(OK_COLOR_GREEN);
				std::cout << "OK";
				setColor(DEFAULT_COLOR_WHITE);
				std::cout << "]~> Found: [";
				setColor(INFO_COLOR_YELLOW);
				std::cout << thisProc.szExeFile;
				setColor(DEFAULT_COLOR_WHITE);
				std::cout << "], PID: [";
				setColor(INFO_COLOR_YELLOW);
				std::cout << thisProc.th32ProcessID;
				setColor(DEFAULT_COLOR_WHITE);
				std::cout << "]\n";
				break;
			}
		} while (Process32Next(procList, &thisProc));
	}
	else
	{
		// clear heap.
		CloseHandle(procList);
		delete[] inputProcName;
		return PROC_FIRST_OR_NEXT_HAS_FAILED;
	}
	
	// clear heap.
	CloseHandle(procList);
	delete[] inputProcName;
	return curProcId;
}

bool verifyProcId(DWORD procId)
{
	// handling bad snap.
	if (procId == BAD_PROC_ID_VIA_SNAP || procId == PROC_FIRST_OR_NEXT_HAS_FAILED)
	{
		// print error.
		std::cout << '[';
		setColor(ERROR_COLOR_RED);
		std::cout << "ER";
		setColor(DEFAULT_COLOR_WHITE);
		std::cout << "]~> SnapTool has Failed!\n";
		return false;
	}

	// handling name was not found.
	if (procId == GIVEN_NAME_WAS_NOT_FOUND_IN_PROC_LIST)
	{
		// print procInfo.
		std::cout << '[';
		setColor(ERROR_COLOR_RED);
		std::cout << "ER";
		setColor(DEFAULT_COLOR_WHITE);
		std::cout << "]~> Process name was unable to find a match!\n";
		return false;
	}

	// procId is good.
	return true;
}

void setColor(int color)
{
	// making a handle to output.
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, color);
}

LPWSTR getDllPathViaDialogBox()
{
	// define the file struct.
	OPENFILENAME fileStruct;
	TCHAR dllPath[MAX_PATH] = {};

	// default values to get file info.
	ZeroMemory(&fileStruct, sizeof(fileStruct));
	fileStruct.lStructSize = sizeof(OPENFILENAME);
	fileStruct.hwndOwner = NULL;
	fileStruct.lpstrFile = dllPath;
	fileStruct.nMaxFile = sizeof(dllPath);
	fileStruct.lpstrFilter = TEXT("Dynamic Link Libraries (*.dll)\0*.dll");
	fileStruct.nFilterIndex = 1;
	fileStruct.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// open file dialog.
	if (GetOpenFileName(&fileStruct) == true)
	{
		TCHAR* retDllPath = new TCHAR[sizeof(dllPath) + 1];
		_tcscpy_s(retDllPath, sizeof(dllPath) + 1, dllPath);
		okPrint("Dll path has been saved!");
		infPrint("Dll Path: [");
		setColor(LOGO_COLOR_BLUE);
		std::cout << retDllPath;
		setColor(DEFAULT_COLOR_WHITE);
		std::cout << "]\n";
		return (LPWSTR)retDllPath;
	}
	else
	{
		// handle error.
		erPrint("Failed to get the file path!");
		TCHAR* failDllPath = new TCHAR[sizeof(dllPath) + 1];
		_tcscpy_s(failDllPath, sizeof(dllPath) + 1, dllPath);
		return (LPWSTR)failDllPath;
	}
}

void okPrint(const char* dialog)
{
	// print ok.
	std::cout << '[';
	setColor(OK_COLOR_GREEN);
	std::cout << "OK";
	setColor(DEFAULT_COLOR_WHITE);
	std::cout << "]~> " << dialog << "\n";
}

void erPrint(const char* dialog)
{
	// print error.
	std::cout << '[';
	setColor(ERROR_COLOR_RED);
	std::cout << "ER";
	setColor(DEFAULT_COLOR_WHITE);
	std::cout << "]~> " << dialog << "\n";
}

void infPrint(const char* dialog)
{
	// info print.
	std::cout << '[';
	setColor(LOGO_COLOR_BLUE);
	std::cout << "!!";
	setColor(DEFAULT_COLOR_WHITE);
	std::cout << "]~> " << dialog;
}