/*----------------------------------------------------------
VioKey - Vietnamese Input Application for Windows.

Based on OpenKey (https://github.com/tuyenvm/OpenKey)
Copyright (C) 2019 Mai Vu Tuyen - GPL license

VioKey is a simplified fork: Telex/VNI only, Unicode only.
You can fork, modify, improve this program. If you
redistribute your new version, it MUST be open source.
-----------------------------------------------------------*/
#pragma once
#include "stdafx.h"

class VioKeyHelper {
private:
	static void openKey();
public:
	static void setRegInt(LPCTSTR key, const int& val);
	static int getRegInt(LPCTSTR key, const int& defaultValue);

	static void setRegBinary(LPCTSTR key, const BYTE* pData, const int& size);
	static BYTE* getRegBinary(LPCTSTR key, DWORD& outSize);

	static void registerRunOnStartup(const int& val);

	static LPTSTR getExecutePath();

	static string& getFrontMostAppExecuteName();
	static string& getLastAppExecuteName();

	static wstring getFullPath();

	static wstring getClipboardText(const int& type);
	static void setClipboardText(LPCTSTR data, const int& len, const int& type);

	static DWORD getVersionNumber();
	static wstring getVersionString();
};
