/*----------------------------------------------------------
VioKey - Vietnamese Input Application for Windows.
Released under the GPLv3 license.
-----------------------------------------------------------*/
#pragma once
#include "BaseDialog.h"

class MainControlDialog : public BaseDialog {
private:
	static const int TAB_COUNT = 3;
	HWND hTabButtons[TAB_COUNT];
	HWND hTabPages[TAB_COUNT];
	HWND radioTelex, radioVNI;
	HWND checkCtrl, checkAlt, checkWin, checkShift, textSwitchKey, checkBeep;
	HWND checkVietnamese, checkEnglish;
	// Page 1: Typing & Spelling
	HWND checkModernOrthorgraphy, checkFixRecommendBrowser, checkSpelling, checkRestoreIfWrongSpelling,
		checkAllowZWJF, checkTempOffSpelling, checkSmartSwitchKey, checkCapsFirstChar, checkTempOffVioKey,
		checkQuickTelex, checkQuickStartConsonant, checkQuickEndConsonant;
	// Page 2: Integrated Macro Workspace
	HWND checkUseMacro, checkUseMacroInEnglish, checkMacroAutoCaps;
	HWND listMacro, hMacroName, hMacroContent, hAddButton;
	vector<vector<Uint32>> keys;
	vector<string> macroText;
	vector<string> macroContent;
	// Page 3: System & Info
	HWND checkShowOnStartup, checkRunWithWindows, checkRunAsAdmin, checkModernIcon, checkUseClipboard;
private:
	void initDialog();
	void createTabs();
	void bindControls();
	void initMacroList();
	void addTooltips();
	void onCheckboxClicked(const HWND& hWnd);
	void onSwitchKeyPressed(const UINT16& keyCode);
	void setSwitchKeyText(const HWND& hWnd, const UINT16 & keyCode);
	void requestRestartAsAdmin();
	// Macro management
	void reloadMacroList();
	void saveAndReloadMacros();
	void insertMacroRow(const int& index, LPTSTR macroName, LPTSTR macroContent);
	void onSelectMacroItem(const int& index);
	void onAddMacroButton();
	void onDeleteMacroButton();
	void onImportMacroButton();
	void onExportMacroButton();
	static LRESULT CALLBACK switchKeyProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR refData);
protected:
	INT_PTR eventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK tabPageEventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	MainControlDialog(const HINSTANCE& hInstance, const int& resourceId);
	virtual ~MainControlDialog();
	virtual void fillData() override;
	void selectTab(const int& index);
	void setSwitchKey(const unsigned short& code);
};
