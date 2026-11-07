/*----------------------------------------------------------
VioKey - Vietnamese Input Application for Windows.

Based on OpenKey (https://github.com/tuyenvm/OpenKey)
Copyright (C) 2019 Mai Vu Tuyen - GPL license

VioKey is a simplified fork: Telex/VNI only, Unicode only.
You can fork, modify, improve this program. If you
redistribute your new version, it MUST be open source.
-----------------------------------------------------------*/
#include "VioKeyHelper.h"
#include <stdarg.h>
#include <fstream>

#pragma comment(lib, "version.lib")

static BYTE* _regData = 0;

static LPCTSTR sk = TEXT("SOFTWARE\\VioKey");
static HKEY hKey;
static LPCTSTR _runOnStartupKeyPath = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
static TCHAR _executePath[MAX_PATH];
static bool _hasGetPath = false;

static DWORD _cacheProcessId = 0, _tempProcessId = 0;
static HWND _tempWnd;
static TCHAR _exePath[1024] = { 0 };
static LPCTSTR _exeName = _exePath;
static HANDLE _proc;
static string _exeNameUtf8 = "TheVioKeyProject";
static string _unknownProgram = "UnknownProgram";

void VioKeyHelper::openKey() {
	LONG nError = RegOpenKeyEx(HKEY_CURRENT_USER, sk, NULL, KEY_ALL_ACCESS, &hKey);
	if (nError == ERROR_FILE_NOT_FOUND) 	{
		nError = RegCreateKeyEx(HKEY_CURRENT_USER, sk, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY, NULL, &hKey, NULL);
	}
	if (nError) {
		LOG(L"result %d\n", nError);
	}
}

void VioKeyHelper::setRegInt(LPCTSTR key, const int & val) {
	openKey();
	RegSetValueEx(hKey, key, 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
	RegCloseKey(hKey);
}

int VioKeyHelper::getRegInt(LPCTSTR key, const int & defaultValue) {
	openKey();
	int val = defaultValue;
	DWORD size = sizeof(val);
	if (ERROR_SUCCESS != RegQueryValueEx(hKey, key, 0, 0, (LPBYTE)&val, &size)) {
		val = defaultValue;
	}
	RegCloseKey(hKey);
	return val;
}

void VioKeyHelper::setRegBinary(LPCTSTR key, const BYTE * pData, const int & size) {
	openKey();
	RegSetValueEx(hKey, key, 0, REG_BINARY, pData, size);
	RegCloseKey(hKey);
}

BYTE * VioKeyHelper::getRegBinary(LPCTSTR key, DWORD& outSize) {
	openKey();
	if (_regData) {
		delete[] _regData;
		_regData = NULL;
	}
	DWORD size = 0;
	RegQueryValueEx(hKey, key, 0, 0, 0, &size);
	_regData = new (std::nothrow) BYTE[size];
	if (_regData != NULL && ERROR_SUCCESS != RegQueryValueEx(hKey, key, 0, 0, _regData, &size)) {
		delete[] _regData;
		_regData = NULL;
	}
	outSize = (_regData == NULL) ? 0 : size;
	RegCloseKey(hKey);
	return _regData;
}

void VioKeyHelper::registerRunOnStartup(const int& val) {
	if (val) {
		if (vRunAsAdmin) {
			string path = wideStringToUtf8(getFullPath());
			char buff[MAX_PATH];
			sprintf_s(buff, "schtasks /create /sc onlogon /tn VioKey /rl highest /tr \"%s\" /f", path.c_str());
			WinExec(buff, SW_HIDE);
		} else {
			RegOpenKeyEx(HKEY_CURRENT_USER, _runOnStartupKeyPath, NULL, KEY_ALL_ACCESS, &hKey);
			wstring path = getFullPath();
			RegSetValueEx(hKey, _T("VioKey"), 0, REG_SZ, (BYTE*)path.c_str(), ((DWORD)path.size() + 1) * sizeof(TCHAR));
			RegCloseKey(hKey);
		}
	} else {
		RegOpenKeyEx(HKEY_CURRENT_USER, _runOnStartupKeyPath, NULL, KEY_ALL_ACCESS, &hKey);
		RegDeleteValue(hKey, _T("VioKey"));
		RegCloseKey(hKey);
		WinExec("schtasks /delete  /tn VioKey /f", SW_HIDE);
	}
}

LPTSTR VioKeyHelper::getExecutePath() {
	if (!_hasGetPath) {
		HMODULE hModule = GetModuleHandleW(NULL);
		GetModuleFileNameW(hModule, _executePath, MAX_PATH);
		_hasGetPath = true;
	}
	return _executePath;
}

string& VioKeyHelper::getFrontMostAppExecuteName() {
	_tempWnd = GetForegroundWindow();
	GetWindowThreadProcessId(_tempWnd, &_tempProcessId);
	if (_tempProcessId == _cacheProcessId) {
		return _exeNameUtf8;
	}
	_cacheProcessId = _tempProcessId;
	_proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, _tempProcessId);
	GetProcessImageFileName((HMODULE)_proc, _exePath, 1024);
	CloseHandle(_proc);

	if (wcscmp(_exePath, _T("")) == 0) {
		return _unknownProgram;
	}
	_exeName = _tcsrchr(_exePath, '\\') + 1;
	if (wcscmp(_exeName, _T("VioKey.exe")) == 0 ||
		wcscmp(_exeName, _T("VioKey64.exe")) == 0 ||
		wcscmp(_exeName, _T("VioKey32.exe")) == 0 ||
		wcscmp(_exeName, _T("explorer.exe")) == 0) {
		return _exeNameUtf8;
	}
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, _exeName, (int)lstrlen(_exeName), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, _exeName, (int)lstrlen(_exeName), &strTo[0], size_needed, NULL, NULL);
	_exeNameUtf8 = strTo;
	//LOG(L"%s\n", utf8ToWideString(_exeNameUtf8).c_str());
	return _exeNameUtf8;
}

string & VioKeyHelper::getLastAppExecuteName() {
	if (!vUseSmartSwitchKey)
		return getFrontMostAppExecuteName();
	return _exeNameUtf8;
}

wstring VioKeyHelper::getFullPath() {
	HMODULE hModule = GetModuleHandle(NULL);
	TCHAR path[MAX_PATH];
	GetModuleFileName(hModule, path, MAX_PATH);
	wstring rs(path);
	return rs;
}

wstring VioKeyHelper::getClipboardText(const int& type) {
	// Try opening the clipboard
	if (!OpenClipboard(nullptr)) {
		return _T("");
	}

	// Get handle of clipboard object for ANSI text
	HANDLE hData = GetClipboardData(type);
	if (hData == nullptr) {
		CloseClipboard();
		return _T("");
	}

	// Lock the handle to get the actual text pointer
	wchar_t * pszText = static_cast<wchar_t*>(GlobalLock(hData));
	if (pszText == nullptr) {
		CloseClipboard();
		return _T("");
	}

	// Save text in a string class instance
	wstring text(pszText);

	// Release the lock
	GlobalUnlock(hData);

	// Release the clipboard
	CloseClipboard();

	return text;
}

void VioKeyHelper::setClipboardText(LPCTSTR data, const int & len, const int& type) {
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(WCHAR));
	memcpy(GlobalLock(hMem), data, len * sizeof(WCHAR));
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(type, hMem);
	CloseClipboard();
}

DWORD VioKeyHelper::getVersionNumber() {
	// get the filename of the executable containing the version resource
	TCHAR szFilename[MAX_PATH + 1] = { 0 };
	if (GetModuleFileName(NULL, szFilename, MAX_PATH) == 0) {
		return 0;
	}

	// allocate a block of memory for the version info
	DWORD dummy;
	UINT dwSize = GetFileVersionInfoSize(szFilename, &dummy);
	if (dwSize == 0) {
		return 0;
	}
	std::vector<BYTE> data(dwSize);

	// load the version info
	if (!GetFileVersionInfo(szFilename, NULL, dwSize, &data[0])) {
		return 0;
	}

	LPBYTE lpBuffer = NULL;

	if (VerQueryValue(&data[0], _T("\\"), (VOID FAR * FAR*) & lpBuffer, &dwSize)) {
		if (dwSize) {
			VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
			if (verInfo->dwSignature == 0xfeef04bd) {
				return ((verInfo->dwFileVersionMS >> 16) & 0xffff) |
					(((verInfo->dwFileVersionMS >> 0) & 0xffff) << 8) |
					(((verInfo->dwFileVersionLS >> 16) & 0xffff) << 16);
			}
		}
	}

	return 0;
}

wstring VioKeyHelper::getVersionString() {
	TCHAR versionBuffer[MAX_PATH];
	DWORD ver = getVersionNumber();
	wsprintfW(versionBuffer, _T("%d.%d.%d"), ver & 0xFF, (ver>>8) & 0xFF, (ver >> 16) & 0xFF);
	return wstring(versionBuffer);
}
