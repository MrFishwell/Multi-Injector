#pragma once

// includes.
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <commdlg.h>
#include "TCHAR.H"
#include <cstring>

// error defines.
#define BAD_PROC_ID_VIA_SNAP 1
#define PROC_FIRST_OR_NEXT_HAS_FAILED 2
#define GIVEN_NAME_WAS_NOT_FOUND_IN_PROC_LIST 0
#define OPEN_FILE_SELECTOR_FAILED 'a'

// color defines.
#define OK_COLOR_GREEN 10
#define LOGO_COLOR_BLUE 11
#define INFO_COLOR_YELLOW 14
#define DEFAULT_COLOR_WHITE 15
#define ERROR_COLOR_RED 12
#define STYLISH_COLOR_PINK 13

// function headers.
void printLogo();
DWORD getProcIdViaString();
bool verifyProcId(DWORD procId);
void setColor(int color);
TCHAR* getDllPathViaDialogBox();
int getInjectionMethod();
void okPrint(const char* dialog);
void infPrint(const char* dialog);
void erPrint(const char* dialog);