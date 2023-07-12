#include "manualMap.h"

bool manualMapSequence(HANDLE hProc, TCHAR* dllPath)
{
	// Open the DLL file
	HANDLE hDll = CreateFile(dllPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDll == INVALID_HANDLE_VALUE)
	{
		erPrint("Failed to open DLL file!");
		return false;
	}

	// Get the size of the DLL file
	DWORD fileSize = GetFileSize(hDll, NULL);
	if (fileSize == INVALID_FILE_SIZE)
	{
		erPrint("Failed to get DLL file size!");
		CloseHandle(hDll);
		return false;
	}

	// alloc a page inside the running proc.
	void* memPage = VirtualAllocEx(hProc, NULL, fileSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	
	// clean up if page failed.
	if (!memPage)
	{
		erPrint("Failed to alloc a new memory page inside the process!");
		return false;
	}

	// Read the DLL content into the allocated memory
	DWORD bytesRead = 0;
	if (!ReadFile(hDll, memPage, fileSize, &bytesRead, NULL) || bytesRead != fileSize)
	{
		erPrint("Failed to read DLL content!");
		VirtualFreeEx(hProc, memPage, 0, MEM_RELEASE);
		CloseHandle(hDll);
		return false;
	}

	CloseHandle(hDll);

	// write to proc mem.
	if (!WriteProcessMemory(hProc, memPage, dllPath, lstrlen(dllPath) + 1, NULL))
	{
		erPrint("Failed to write to the process memory!");
		VirtualFreeEx(hProc, memPage, 0, MEM_RELEASE);
		return false;
	}

	// get the kernal32.dll via the running proc.
	HMODULE ker32Module = GetModuleHandleA("kernel32.dll");

	if (!ker32Module)
	{
		erPrint("Failed to get Kernel32.dll!");
		VirtualFreeEx(hProc, memPage, NULL, MEM_RELEASE);
		return false;
	}

	// get loadLib from kernal32.dll.
	FARPROC inLoader = GetProcAddress(ker32Module, "LoadLibraryA");

	// no module was found.
	if (!inLoader)
	{
		erPrint("Unable to fetch LoadLibrary!");
		FreeLibrary(ker32Module);
		VirtualFreeEx(hProc, memPage, 0, MEM_RELEASE);
		return false;
	}

	// create a thread.
	HANDLE hTread = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)inLoader, memPage, NULL, NULL);

	// wait for theard.
	if (hTread)
		WaitForSingleObject(hTread, INFINITE);

	// thread has failed.
	if (!hTread)
	{
		erPrint("Unable to get a thread!");
		FreeLibrary(ker32Module);
		VirtualFreeEx(hProc, memPage, 0, MEM_RELEASE);
		return false;
	}
	
	okPrint("Manual mapping has been successfully executed!");
	infPrint("Mapped at: ");
	std::cout << &memPage << '\n';
	FreeLibrary(ker32Module);
	CloseHandle(hTread);
	return true;
}