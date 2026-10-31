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

class VioKeyManager {
public:
	static vector<LPCTSTR>& getInputType();

	static void initEngine();
	static void freeEngine();
};
