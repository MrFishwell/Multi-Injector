
// includes.
#include "injectorLogic.h"

int main() {

	// print logo.
	printLogo();

	// getting proc id from snap.
	DWORD procId = getProcIdViaString();

	// error handling.
	if (!verifyProcId(procId))
		return -1;

	// getting dll path.
	LPWSTR dllPath = getDllPathViaDialogBox();

	// selecting injection method.

	// print final data.
	
	// clean the heap.
	delete[] dllPath;
	return 0;
}