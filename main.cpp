
// includes.
#include "injectorLogic.h"
#include "manualMap.h";

int main() {

	// print logo.
	printLogo();

	// getting proc id from snap.
	DWORD procId = getProcIdViaString();

	// error handling.
	if (!verifyProcId(procId))
		return -1;

	// getting dll path.
	TCHAR* dllPath = getDllPathViaDialogBox();
	
	// handle to the proc it self.
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);

	// handle falty proc.
	if (!hProc)
	{
		delete[] dllPath;
		return 0;
	}

	// selecting injection method.
	int method = getInjectionMethod();
	enum injectionMethods {manualMap = 1};
	
	switch (method)
	{
	case manualMap:
	{
		okPrint("Starting Manual Map!");
		if (!manualMapSequence(hProc, dllPath))
		{
			// clean the heap.
			CloseHandle(hProc);
			delete[] dllPath;
			return 0;
		}
		break;
	}
	default:
		break;
	}
	
	// clean the heap.
	CloseHandle(hProc);
	delete[] dllPath;
	return 0;
}