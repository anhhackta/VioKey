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
#include "UITheme.h"
#define SET_DIALOG_ICON(ICON_RES) { \
	HICON hBig = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_SHARED); \
	HICON hSmall = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_SHARED); \
	if (hBig) SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hBig); \
	if (hSmall) SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hSmall); \
}
class BaseDialog {
protected:
	UINT resId;
	HINSTANCE hInstance;
	HWND hDlg;
protected:
	void createToolTip(const HWND& control, LPCTSTR toolTipString);
	void createToolTip(const HWND& control, const UINT& stringResId);
	virtual INT_PTR eventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
public:
	BaseDialog(const HINSTANCE& hInstance, const int& resourceId);
	void show();
	void bringOnTop();
	virtual void fillData() = 0;

	HWND getHwnd();
	void closeDialog();
	friend INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
