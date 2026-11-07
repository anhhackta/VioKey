/*----------------------------------------------------------
VioKey - Vietnamese Input Application for Windows.

Based on OpenKey (https://github.com/tuyenvm/OpenKey)
Copyright (C) 2019 Mai Vu Tuyen - GPL license

VioKey is a simplified fork: Telex/VNI only, Unicode only.
You can fork, modify, improve this program. If you
redistribute your new version, it MUST be open source.
-----------------------------------------------------------*/
#include "VioKeyManager.h"

static vector<LPCTSTR> _inputType = {
	_T("Telex"),
	_T("VNI"),
};

extern void VioKeyInit();
extern void VioKeyFree();

vector<LPCTSTR>& VioKeyManager::getInputType() {
	return _inputType;
}

void VioKeyManager::initEngine() {
	VioKeyInit();
}

void VioKeyManager::freeEngine() {
	VioKeyFree();
}
