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

class MainControlDialog : public BaseDialog {
private:
	static const int TAB_COUNT = 3;
	HWND hTabButtons[TAB_COUNT];
	HWND hTabPages[TAB_COUNT];
	HWND comboBoxInputType;
	HWND checkCtrl, checkAlt, checkWin, checkShift, textSwitchKey, checkBeep;
	HWND checkVietnamese, checkEnglish;
	// tab: general
	HWND checkModernOrthorgraphy, checkFixRecommendBrowser, checkSpelling, checkRestoreIfWrongSpelling,
		checkAllowZWJF, checkTempOffSpelling, checkSmartSwitchKey, checkCapsFirstChar, checkTempOffVioKey;
	// tab: macro
	HWND checkQuickTelex, checkUseMacro, checkUseMacroInEnglish, checkMacroAutoCaps,
		checkQuickStartConsonant, checkQuickEndConsonant;
	// tab: system
	HWND checkShowOnStartup, checkRunWithWindows, checkRunAsAdmin, checkModernIcon, checkUseClipboard;
private:
	void initDialog();
	void createTabs();
	void bindControls();
	void addTooltips();
	void onComboBoxSelected(const HWND& hCombobox);
	void onCheckboxClicked(const HWND& hWnd);
	void onSwitchKeyPressed(const UINT16& keyCode);
	void setSwitchKeyText(const HWND& hWnd, const UINT16 & keyCode);
	void selectTab(const int& index);
	void requestRestartAsAdmin();
	static LRESULT CALLBACK switchKeyProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR refData);
protected:
	INT_PTR eventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK tabPageEventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	MainControlDialog(const HINSTANCE& hInstance, const int& resourceId);
	virtual ~MainControlDialog();
	virtual void fillData() override;
	void setSwitchKey(const unsigned short& code);
};
