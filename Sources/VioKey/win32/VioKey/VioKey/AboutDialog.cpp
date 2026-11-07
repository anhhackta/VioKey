/*----------------------------------------------------------
VioKey - Vietnamese Input Application for Windows.

Based on OpenKey (https://github.com/tuyenvm/OpenKey)
Copyright (C) 2019 Mai Vu Tuyen - GPL license

VioKey is a simplified fork: Telex/VNI only, Unicode only.
You can fork, modify, improve this program. If you
redistribute your new version, it MUST be open source.
-----------------------------------------------------------*/
#include "AboutDialog.h"
#include "AppDelegate.h"

static const wchar_t ORIGINAL_PROJECT_URL[] = L"https://github.com/tuyenvm/OpenKey";

AboutDialog::AboutDialog(const HINSTANCE& hInstance, const int& resourceId)
	: BaseDialog(hInstance, resourceId) {
}

AboutDialog::~AboutDialog() {
}

void AboutDialog::initDialog() {
	SET_DIALOG_ICON(IDI_APP_ICON);
	SendDlgItemMessage(hDlg, IDC_STATIC_APP_TITLE, WM_SETFONT, (WPARAM)UITheme::titleFont(), TRUE);
	fillData();
}

void AboutDialog::fillData() {
	wchar_t buffer[256];
	wsprintfW(buffer, _T("Phiên bản %s - Ngày build: %s"), VioKeyHelper::getVersionString().c_str(), _T(__DATE__));
	SetDlgItemText(hDlg, IDC_STATIC_APP_VERSION, buffer);
}

INT_PTR AboutDialog::eventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		this->hDlg = hDlg;
		initDialog();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDBUTTON_OK:
			AppDelegate::getInstance()->closeDialog(this);
			break;
		case IDC_BUTTON_GO_SOURCE_CODE:
			ShellExecute(NULL, _T("open"), ORIGINAL_PROJECT_URL, NULL, NULL, SW_SHOWNORMAL);
			break;
		}
		break;
	}
	return FALSE;
}
