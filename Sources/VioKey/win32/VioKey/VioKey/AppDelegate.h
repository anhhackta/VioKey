/*----------------------------------------------------------
VioKey - Vietnamese Input Application for Windows.
Released under the GPLv3 license.
-----------------------------------------------------------*/
#pragma once
#include "stdafx.h"
#include "MainControlDialog.h"
#include "AboutDialog.h"

class BaseDialog;
class AppDelegate {
private:
	HINSTANCE hInstance;
	MainControlDialog* mainDialog = NULL;
	BaseDialog* aboutDialog = NULL;
private:
	bool isDialogMsg(MSG &msg) const;
public:
	AppDelegate();
	static AppDelegate* getInstance();
	int run(HINSTANCE hInstance);
	void createMainDialog();
	void closeDialog(BaseDialog* dialog);
public: //event
	void onInputMethodChangedFromHotKey();
	void onDefaultConfig();

	void onToggleVietnamese();
	void onToggleCheckSpelling();
	void onToggleUseSmartSwitchKey();
	void onToggleUseMacro();

	void onMacroTable();

	void onInputType(const int& type);

	void onControlPanel();
	void onVioKeyAbout();
	void onVioKeyExit();
	void applyThemeToOpenDialogs();
};
