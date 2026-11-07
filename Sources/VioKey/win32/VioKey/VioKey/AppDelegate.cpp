/*----------------------------------------------------------
VioKey - Vietnamese Input Application for Windows.

Based on OpenKey (https://github.com/tuyenvm/OpenKey)
Copyright (C) 2019 Mai Vu Tuyen - GPL license

VioKey is a simplified fork: Telex/VNI only, Unicode only.
You can fork, modify, improve this program. If you
redistribute your new version, it MUST be open source.
-----------------------------------------------------------*/
#include "AppDelegate.h"

static AppDelegate* _instance;

//see document in Engine.h
int vLanguage = 1;
int vInputType = 0;
int vCheckSpelling = 1;
int vUseModernOrthography = 1;
int vQuickTelex = 0;
#define DEFAULT_SWITCH_STATUS 0x5A00025A //default shift + z
int vSwitchKeyStatus = DEFAULT_SWITCH_STATUS;
int vRestoreIfWrongSpelling = 1;
int vFixRecommendBrowser = 0;
int vUseMacro = 1;
int vUseMacroInEnglishMode = 1;
int vAutoCapsMacro = 0;
int vSendKeyStepByStep = 1;
int vUseSmartSwitchKey = 1;
int vUpperCaseFirstChar = 0;
int vTempOffSpelling = 0;
int vAllowConsonantZFWJ = 0;
int vQuickStartConsonant = 0;
int vQuickEndConsonant = 0;
int vTempOffVioKey = 0;

int vUseGrayIcon = 0;
int vShowOnStartUp = 0;
int vRunWithWindows = 1;

int vRunAsAdmin = 0;
int vThemeMode = 0;

bool AppDelegate::isDialogMsg(MSG & msg) const {
	return (mainDialog != NULL && IsDialogMessage(mainDialog->getHwnd(), &msg)) ||
		(macroDialog != NULL && IsDialogMessage(macroDialog->getHwnd(), &msg)) ||
		(aboutDialog != NULL && IsDialogMessage(aboutDialog->getHwnd(), &msg));
}

AppDelegate::AppDelegate() {
	_instance = this;
}

AppDelegate * AppDelegate::getInstance() {
	return _instance;
}

int AppDelegate::run(HINSTANCE hInstance) {
	this->hInstance = hInstance;

	//check app has already run or not
	HWND previousInstance = FindWindow(APP_CLASS, NULL);
	if (previousInstance) {
		MessageBeep(MB_OK);
		SendMessage(previousInstance, WM_USER + 2019, 0, 0);
		PostQuitMessage(0);
		return 0;
	}

	//init VioKey Engine
	VioKeyManager::initEngine();
	UITheme::init();

	//create system tray
	SystemTrayHelper::createSystemTrayIcon(hInstance);
	SystemTrayHelper::updateData();

	//create main control
	if (vShowOnStartUp)
		createMainDialog();

	MSG msg;
	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))	{
		if (!isDialogMsg(msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	UITheme::shutdown();
	return 0;
}

void AppDelegate::createMainDialog() {
	if (mainDialog == NULL) {
		mainDialog = new MainControlDialog(hInstance, IDD_DIALOG_MAIN);
		mainDialog->show();
	} else {
		mainDialog->bringOnTop();
	}
}

void AppDelegate::closeDialog(BaseDialog * dialog) {
	dialog->closeDialog();
	if (mainDialog == dialog) {
		delete mainDialog;
		mainDialog = NULL;
	} else if (aboutDialog == dialog) {
		delete aboutDialog;
		aboutDialog = NULL;
	} else if (macroDialog == dialog) {
		delete macroDialog;
		macroDialog = NULL;
	}
}

void AppDelegate::onInputMethodChangedFromHotKey() {
	APP_SET_DATA(vLanguage, vLanguage);
	if (mainDialog) {
		mainDialog->fillData();
	}
	SystemTrayHelper::updateData();
}

void AppDelegate::onDefaultConfig() {
	APP_SET_DATA(vLanguage, 1);
	APP_SET_DATA(vInputType, 0);
	APP_SET_DATA(vCheckSpelling, 1);
	APP_SET_DATA(vUseModernOrthography, 1);
	APP_SET_DATA(vQuickTelex, 0);
	APP_SET_DATA(vSwitchKeyStatus, DEFAULT_SWITCH_STATUS);
	APP_SET_DATA(vRestoreIfWrongSpelling, 1);
	APP_SET_DATA(vFixRecommendBrowser, 0);
	APP_SET_DATA(vUseMacro, 1);
	APP_SET_DATA(vUseMacroInEnglishMode, 1);
	APP_SET_DATA(vAutoCapsMacro, 0);
	APP_SET_DATA(vSendKeyStepByStep, 1);
	APP_SET_DATA(vUseSmartSwitchKey, 1);
	APP_SET_DATA(vUpperCaseFirstChar, 0);
	APP_SET_DATA(vAllowConsonantZFWJ, 0);
	APP_SET_DATA(vTempOffSpelling, 0);
	APP_SET_DATA(vQuickStartConsonant, 0);
	APP_SET_DATA(vQuickEndConsonant, 0);
	APP_SET_DATA(vTempOffVioKey, 0);

	APP_SET_DATA(vUseGrayIcon, 0);
	APP_SET_DATA(vShowOnStartUp, 0);
	APP_SET_DATA(vRunWithWindows, 1);
	APP_SET_DATA(vRunAsAdmin, 0);
	APP_SET_DATA(vThemeMode, 0);
	UITheme::setDarkMode(true);
	applyThemeToOpenDialogs();

	if (mainDialog) {
		mainDialog->fillData();
	}
	SystemTrayHelper::updateData();
}

void AppDelegate::onToggleVietnamese() {
	APP_SET_DATA(vLanguage, vLanguage ? 0 : 1);
	if (mainDialog) {
		mainDialog->fillData();
	}

	if (vUseSmartSwitchKey) {
		string& exe = VioKeyHelper::getLastAppExecuteName();
		setAppInputMethodStatus(exe, vLanguage);
		saveSmartSwitchKeyData();
	}
}

void AppDelegate::onToggleCheckSpelling() {
	APP_SET_DATA(vCheckSpelling, vCheckSpelling ? 0 : 1);
	if (mainDialog) {
		mainDialog->fillData();
	}
	vSetCheckSpelling();
}

void AppDelegate::onToggleUseSmartSwitchKey() {
	APP_SET_DATA(vUseSmartSwitchKey, vUseSmartSwitchKey ? 0 : 1);
	if (mainDialog) {
		mainDialog->fillData();
	}
}

void AppDelegate::onToggleUseMacro() {
	APP_SET_DATA(vUseMacro, vUseMacro ? 0 : 1);
	if (mainDialog) {
		mainDialog->fillData();
	}
}

void AppDelegate::onMacroTable() {
	if (macroDialog == NULL) {
		macroDialog = new MacroDialog(hInstance, IDD_DIALOG_MACRO);
		macroDialog->show();
	} else {
		macroDialog->bringOnTop();
	}
}

void AppDelegate::onInputType(const int & type) {
	APP_SET_DATA(vInputType, type);
	if (mainDialog) {
		mainDialog->fillData();
	}
}

void AppDelegate::onControlPanel() {
	createMainDialog();
}

void AppDelegate::onVioKeyAbout() {
	if (aboutDialog == NULL) {
		aboutDialog = new AboutDialog(hInstance, IDD_ABOUTBOX);
		aboutDialog->show();
	} else {
		aboutDialog->bringOnTop();
	}
}

void AppDelegate::onVioKeyExit() {
	VioKeyManager::freeEngine();
	SystemTrayHelper::removeSystemTray();
	PostQuitMessage(0);
}

void AppDelegate::applyThemeToOpenDialogs() {
	BaseDialog* dlgs[] = { mainDialog, macroDialog, aboutDialog };
	for (BaseDialog* d : dlgs) {
		if (d && d->getHwnd()) {
			UITheme::refreshWindow(d->getHwnd());
		}
	}
}
