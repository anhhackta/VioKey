/*----------------------------------------------------------
VioKey - Vietnamese Input Application for Windows.

Based on OpenKey (https://github.com/tuyenvm/OpenKey)
Copyright (C) 2019 Mai Vu Tuyen - GPL license

VioKey is a simplified fork: Telex/VNI only, Unicode only.
You can fork, modify, improve this program. If you
redistribute your new version, it MUST be open source.
-----------------------------------------------------------*/
#pragma once
#include "BaseDialog.h"

class AboutDialog : public BaseDialog {
protected:
	INT_PTR eventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void initDialog();
public:
	AboutDialog(const HINSTANCE& hInstance, const int& resourceId);
	~AboutDialog();
	virtual void fillData() override;
};
