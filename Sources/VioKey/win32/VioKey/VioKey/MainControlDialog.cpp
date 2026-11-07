/*----------------------------------------------------------
VioKey - Vietnamese Input Application for Windows.

Based on OpenKey (https://github.com/tuyenvm/OpenKey)
Copyright (C) 2019 Mai Vu Tuyen - GPL license

VioKey is a simplified fork: Telex/VNI only, Unicode only.
You can fork, modify, improve this program. If you
redistribute your new version, it MUST be open source.
-----------------------------------------------------------*/
#include "MainControlDialog.h"
#include "AppDelegate.h"
#include <Shlobj.h>

#define EMPTY_SWITCH_KEY 0xFE

// Design-time (dialog unit) rectangle where the tab pages are placed (inside the lower card).
static const RECT PAGE_AREA_DLU = { 16, 121, 340, 251 };

MainControlDialog::MainControlDialog(const HINSTANCE& hInstance, const int& resourceId)
    : BaseDialog(hInstance, resourceId) {
}

MainControlDialog::~MainControlDialog() {
}

void MainControlDialog::initDialog() {
    SET_DIALOG_ICON(IDI_APP_ICON);

    //set title version
    TCHAR title[256];
    TCHAR titleBuffer[256];
    LoadString(hInstance, IDS_MAIN_DIALOG_TITLE, title, 256);
    wsprintfW(titleBuffer, title, VioKeyHelper::getVersionString().c_str());
    SetWindowText(hDlg, titleBuffer);

    SendDlgItemMessage(hDlg, IDC_STATIC_TITLE, WM_SETFONT, (WPARAM)UITheme::titleFont(), TRUE);

    //input type
    comboBoxInputType = GetDlgItem(hDlg, IDC_COMBO_INPUT_TYPE);
    vector<LPCTSTR>& inputType = VioKeyManager::getInputType();
    for (size_t i = 0; i < inputType.size(); i++) {
        SendMessage(comboBoxInputType, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(inputType[i]));
    }

    createTabs();
    bindControls();
    addTooltips();

    //capture the switch key ourselves instead of letting the edit box show raw text
    SetWindowSubclass(textSwitchKey, switchKeyProc, 1, (DWORD_PTR)this);

    fillData();
}

void MainControlDialog::createTabs() {
    const int tabButtonIds[TAB_COUNT] = { IDC_TAB_BUTTON_GENERAL, IDC_TAB_BUTTON_MACRO, IDC_TAB_BUTTON_SYSTEM };
    const int pageIds[TAB_COUNT] = { IDD_DIALOG_TAB_GENERAL, IDD_DIALOG_TAB_MACRO, IDD_DIALOG_TAB_SYSTEM };

    RECT area = PAGE_AREA_DLU;
    MapDialogRect(hDlg, &area);

    for (int i = 0; i < TAB_COUNT; i++) {
        hTabButtons[i] = GetDlgItem(hDlg, tabButtonIds[i]);
        hTabPages[i] = CreateDialogParam(hInstance, MAKEINTRESOURCE(pageIds[i]), hDlg, tabPageEventProc, (LPARAM)this);
        SetWindowPos(hTabPages[i], NULL, area.left, area.top, area.right - area.left, area.bottom - area.top,
            SWP_NOZORDER | SWP_HIDEWINDOW);
    }
    selectTab(0);
}

void MainControlDialog::bindControls() {
    checkCtrl = GetDlgItem(hDlg, IDC_CHECK_SWITCH_KEY_CTRL);
    checkAlt = GetDlgItem(hDlg, IDC_CHECK_SWITCH_KEY_ALT);
    checkWin = GetDlgItem(hDlg, IDC_CHECK_SWITCH_KEY_WIN);
    checkShift = GetDlgItem(hDlg, IDC_CHECK_SWITCH_KEY_SHIFT);
    textSwitchKey = GetDlgItem(hDlg, IDC_SWITCH_KEY_KEY);
    checkBeep = GetDlgItem(hDlg, IDC_CHECK_SWITCH_KEY_BEEP);
    checkVietnamese = GetDlgItem(hDlg, IDC_RADIO_METHOD_VIETNAMESE);
    checkEnglish = GetDlgItem(hDlg, IDC_RADIO_METHOD_ENGLISH);

    HWND general = hTabPages[0];
    checkModernOrthorgraphy = GetDlgItem(general, IDC_CHECK_MODERN_ORTHORGRAPHY);
    checkFixRecommendBrowser = GetDlgItem(general, IDC_CHECK_FIX_RECOMMEND_BROWSER);
    checkSpelling = GetDlgItem(general, IDC_CHECK_SPELLING);
    checkRestoreIfWrongSpelling = GetDlgItem(general, IDC_CHECK_RESTORE_IF_WRONG_SPELLING);
    checkAllowZWJF = GetDlgItem(general, IDC_CHECK_ALLOW_ZJWF);
    checkTempOffSpelling = GetDlgItem(general, IDC_CHECK_TEMP_OFF_SPELLING);
    checkSmartSwitchKey = GetDlgItem(general, IDC_CHECK_SMART_SWITCH_KEY);
    checkCapsFirstChar = GetDlgItem(general, IDC_CHECK_CAPS_FIRST_CHAR);
    checkTempOffVioKey = GetDlgItem(general, IDC_CHECK_TEMP_OFF_VIOKEY);

    HWND macro = hTabPages[1];
    checkQuickTelex = GetDlgItem(macro, IDC_CHECK_QUICK_TELEX);
    checkUseMacro = GetDlgItem(macro, IDC_CHECK_USE_MACRO);
    checkUseMacroInEnglish = GetDlgItem(macro, IDC_CHECK_USE_MACRO_IN_ENGLISH);
    checkMacroAutoCaps = GetDlgItem(macro, IDC_CHECK_AUTO_CAPS);
    checkQuickStartConsonant = GetDlgItem(macro, IDC_CHECK_QUICK_START_CONSONANT);
    checkQuickEndConsonant = GetDlgItem(macro, IDC_CHECK_QUICK_END_CONSONANT);

    HWND system = hTabPages[2];
    checkShowOnStartup = GetDlgItem(system, IDC_CHECK_SHOW_ON_STARTUP);
    checkRunWithWindows = GetDlgItem(system, IDC_CHECK_RUN_WITH_WINDOWS);
    checkRunAsAdmin = GetDlgItem(system, IDC_CHECK_RUN_AS_ADMIN);
    checkModernIcon = GetDlgItem(system, IDC_CHECK_MODERN_ICON);
    checkUseClipboard = GetDlgItem(system, IDC_CHECK_USE_CLIPBOARD);
}

void MainControlDialog::addTooltips() {
    createToolTip(comboBoxInputType, IDS_STRING_INPUT);
    createToolTip(checkCtrl, IDS_STRING_CTRL);
    createToolTip(checkAlt, IDS_STRING_ALT);
    createToolTip(checkWin, IDS_STRING_WIN);
    createToolTip(checkShift, IDS_STRING_SHIFT);
    createToolTip(textSwitchKey, IDS_STRING_SWITCH_KEY);
    createToolTip(checkBeep, IDS_STRING_BEEP);
    createToolTip(checkVietnamese, IDS_STRING_VIET);
    createToolTip(checkEnglish, IDS_STRING_ENG);
    createToolTip(GetDlgItem(hDlg, IDC_BUTTON_THEME_TOGGLE), IDS_STRING_THEME_TOGGLE);

    createToolTip(checkModernOrthorgraphy, IDS_STRING_MODERN_ORTHORGRAPHY);
    createToolTip(checkFixRecommendBrowser, IDS_STRING_FIX_BROWSER);
    createToolTip(checkSpelling, IDS_STRING_SPELLING_CHECK);
    createToolTip(checkRestoreIfWrongSpelling, IDS_STRING_RESTORE_IF_WRONG_SPELLING);
    createToolTip(checkAllowZWJF, IDS_STRING_ALLOW_ZWFJ);
    createToolTip(checkTempOffSpelling, IDS_STRING_TEMP_OFF_SPELLING);
    createToolTip(checkSmartSwitchKey, IDS_STRING_SMART_SWITCH_KEY);
    createToolTip(checkCapsFirstChar, IDS_STRING_CAPS_FIRST_CHAR);
    createToolTip(checkTempOffVioKey, IDS_STRING_TEMP_OFF_VIOKEY);

    createToolTip(checkQuickStartConsonant, IDS_STRING_START_CONSONANT);
    createToolTip(checkQuickEndConsonant, IDS_STRING_END_CONSONANT);
    createToolTip(checkQuickTelex, IDS_STRING_QUICK_TELEX);
    createToolTip(checkUseMacro, IDS_STRING_MACRO);
    createToolTip(checkUseMacroInEnglish, IDS_STRING_MACRO_IN_ENG);
    createToolTip(checkMacroAutoCaps, IDS_STRING_MACRO_AUTO_CAP);

    createToolTip(checkModernIcon, IDS_STRING_MODERN_ICON);
    createToolTip(checkShowOnStartup, IDS_STRING_SHOW_ON_STARTUP);
    createToolTip(checkRunWithWindows, IDS_STRING_RUN_ON_STARTUP);
    createToolTip(checkRunAsAdmin, IDS_STRING_RUN_AS_ADMIN);
    createToolTip(checkUseClipboard, IDS_STRING_USE_CLIPBOARD);
}

INT_PTR MainControlDialog::eventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        this->hDlg = hDlg;
        initDialog();
        return TRUE;
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDBUTTON_OK:
            AppDelegate::getInstance()->closeDialog(this);
            break;
        case IDBUTTON_EXIT:
            AppDelegate::getInstance()->onVioKeyExit();
            break;
        case ID_BTN_DEFAULT: {
            int msgboxID = MessageBox(
                hDlg,
                _T("Bạn có chắc chắn muốn thiết lập lại cài đặt gốc?"),
                _T("VioKey"),
                MB_ICONEXCLAMATION | MB_YESNO
            );
            if (msgboxID == IDYES) {
                AppDelegate::getInstance()->onDefaultConfig();
            }
            break;
        }
        case IDC_BUTTON_MACRO_TABLE:
            AppDelegate::getInstance()->onMacroTable();
            break;
        case IDC_TAB_BUTTON_GENERAL:
            selectTab(0);
            break;
        case IDC_TAB_BUTTON_MACRO:
            selectTab(1);
            break;
        case IDC_TAB_BUTTON_SYSTEM:
            selectTab(2);
            break;
        case IDC_BUTTON_THEME_TOGGLE:
            UITheme::toggleDarkMode();
            AppDelegate::getInstance()->applyThemeToOpenDialogs();
            break;
        default:
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                this->onComboBoxSelected((HWND)lParam);
            }
            else if (HIWORD(wParam) == BN_CLICKED) {
                this->onCheckboxClicked((HWND)lParam);
            }
            break;
        }
        break;
    }
    }

    return FALSE;
}

INT_PTR MainControlDialog::tabPageEventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INITDIALOG) {
        SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
        return TRUE;
    }

    INT_PTR themed = FALSE;
    if (UITheme::handleMessage(hDlg, uMsg, wParam, lParam, themed)) {
        return themed;
    }

    LONG_PTR attr = GetWindowLongPtr(hDlg, GWLP_USERDATA);
    if (attr != 0) {
        return ((MainControlDialog*)attr)->eventProc(hDlg, uMsg, wParam, lParam);
    }
    return FALSE;
}

LRESULT CALLBACK MainControlDialog::switchKeyProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR refData) {
    switch (msg) {
    case WM_SETFOCUS:
        gHookPaused = true;
        break;
    case WM_KILLFOCUS:
        gHookPaused = false;
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        ((MainControlDialog*)refData)->onSwitchKeyPressed((UINT16)wParam);
        return 0;
    case WM_CHAR:
    case WM_SYSCHAR:
    case WM_PASTE:
        return 0;
    case WM_NCDESTROY:
        gHookPaused = false;
        RemoveWindowSubclass(hWnd, switchKeyProc, id);
        break;
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

void MainControlDialog::fillData() {
    SendMessage(comboBoxInputType, CB_SETCURSEL, vInputType, 0);

    SendMessage(checkCtrl, BM_SETCHECK, HAS_CONTROL(vSwitchKeyStatus) ? 1 : 0, 0);
    SendMessage(checkAlt, BM_SETCHECK, HAS_OPTION(vSwitchKeyStatus) ? 1 : 0, 0);
    SendMessage(checkWin, BM_SETCHECK, HAS_COMMAND(vSwitchKeyStatus) ? 1 : 0, 0);
    SendMessage(checkShift, BM_SETCHECK, HAS_SHIFT(vSwitchKeyStatus) ? 1 : 0, 0);
    setSwitchKeyText(textSwitchKey, (vSwitchKeyStatus >> 24) & 0xFF);
    SendMessage(checkBeep, BM_SETCHECK, HAS_BEEP(vSwitchKeyStatus) ? 1 : 0, 0);

    SendMessage(checkVietnamese, BM_SETCHECK, vLanguage ? 1 : 0, 0);
    SendMessage(checkEnglish, BM_SETCHECK, vLanguage ? 0 : 1, 0);
    InvalidateRect(checkVietnamese, NULL, FALSE);
    InvalidateRect(checkEnglish, NULL, FALSE);

    SendMessage(checkModernOrthorgraphy, BM_SETCHECK, vUseModernOrthography ? 1 : 0, 0);
    SendMessage(checkFixRecommendBrowser, BM_SETCHECK, vFixRecommendBrowser ? 1 : 0, 0);
    SendMessage(checkSpelling, BM_SETCHECK, vCheckSpelling ? 1 : 0, 0);
    SendMessage(checkRestoreIfWrongSpelling, BM_SETCHECK, vRestoreIfWrongSpelling ? 1 : 0, 0);
    SendMessage(checkAllowZWJF, BM_SETCHECK, vAllowConsonantZFWJ ? 1 : 0, 0);
    SendMessage(checkTempOffSpelling, BM_SETCHECK, vTempOffSpelling ? 1 : 0, 0);
    SendMessage(checkSmartSwitchKey, BM_SETCHECK, vUseSmartSwitchKey ? 1 : 0, 0);
    SendMessage(checkCapsFirstChar, BM_SETCHECK, vUpperCaseFirstChar ? 1 : 0, 0);
    SendMessage(checkTempOffVioKey, BM_SETCHECK, vTempOffVioKey ? 1 : 0, 0);

    SendMessage(checkQuickTelex, BM_SETCHECK, vQuickTelex ? 1 : 0, 0);
    SendMessage(checkUseMacro, BM_SETCHECK, vUseMacro ? 1 : 0, 0);
    SendMessage(checkUseMacroInEnglish, BM_SETCHECK, vUseMacroInEnglishMode ? 1 : 0, 0);
    SendMessage(checkMacroAutoCaps, BM_SETCHECK, vAutoCapsMacro ? 1 : 0, 0);
    SendMessage(checkQuickStartConsonant, BM_SETCHECK, vQuickStartConsonant ? 1 : 0, 0);
    SendMessage(checkQuickEndConsonant, BM_SETCHECK, vQuickEndConsonant ? 1 : 0, 0);

    SendMessage(checkShowOnStartup, BM_SETCHECK, vShowOnStartUp ? 1 : 0, 0);
    SendMessage(checkRunWithWindows, BM_SETCHECK, vRunWithWindows ? 1 : 0, 0);
    SendMessage(checkRunAsAdmin, BM_SETCHECK, vRunAsAdmin ? 1 : 0, 0);
    SendMessage(checkModernIcon, BM_SETCHECK, vUseGrayIcon ? 1 : 0, 0);
    SendMessage(checkUseClipboard, BM_SETCHECK, vSendKeyStepByStep ? 0 : 1, 0);

    EnableWindow(checkRestoreIfWrongSpelling, vCheckSpelling);
    EnableWindow(checkAllowZWJF, vCheckSpelling);
    EnableWindow(checkTempOffSpelling, vCheckSpelling);
}

void MainControlDialog::setSwitchKey(const unsigned short& code) {
    vSwitchKeyStatus &= 0xFFFFFF00;
    vSwitchKeyStatus |= code;
    vSwitchKeyStatus &= 0x00FFFFFF;
    vSwitchKeyStatus |= ((unsigned int)code << 24);
    APP_SET_DATA(vSwitchKeyStatus, vSwitchKeyStatus);
}

void MainControlDialog::selectTab(const int& index) {
    for (int i = 0; i < TAB_COUNT; i++) {
        ShowWindow(hTabPages[i], (i == index) ? SW_SHOW : SW_HIDE);
        UITheme::setTabSelected(hTabButtons[i], i == index);
    }
}

void MainControlDialog::onComboBoxSelected(const HWND& hCombobox) {
    if (hCombobox == comboBoxInputType) {
        APP_SET_DATA(vInputType, (int)SendMessage(hCombobox, CB_GETCURSEL, 0, 0));
    }
    SystemTrayHelper::updateData();
}

// Updates one modifier bit of the language switch hotkey.
static void setSwitchKeyModifier(HWND hCheck, const int& mask) {
    int checked = (int)SendMessage(hCheck, BM_GETCHECK, 0, 0) ? 1 : 0;
    vSwitchKeyStatus &= ~mask;
    if (checked)
        vSwitchKeyStatus |= mask;
    APP_SET_DATA(vSwitchKeyStatus, vSwitchKeyStatus);
}

// Reads a checkbox into an engine/app setting.
#define BIND_CHECK(HANDLE, SETTING) \
    else if (hWnd == HANDLE) { \
        APP_SET_DATA(SETTING, (int)SendMessage(hWnd, BM_GETCHECK, 0, 0) ? 1 : 0); \
    }

void MainControlDialog::onCheckboxClicked(const HWND& hWnd) {
    if (hWnd == checkCtrl) {
        setSwitchKeyModifier(hWnd, 0x100);
    }
    else if (hWnd == checkAlt) {
        setSwitchKeyModifier(hWnd, 0x200);
    }
    else if (hWnd == checkWin) {
        setSwitchKeyModifier(hWnd, 0x400);
    }
    else if (hWnd == checkShift) {
        setSwitchKeyModifier(hWnd, 0x800);
    }
    else if (hWnd == checkBeep) {
        setSwitchKeyModifier(hWnd, 0x8000);
    }
    else if (hWnd == checkVietnamese || hWnd == checkEnglish) {
        APP_SET_DATA(vLanguage, hWnd == checkVietnamese ? 1 : 0);
        SendMessage(checkVietnamese, BM_SETCHECK, vLanguage ? 1 : 0, 0);
        SendMessage(checkEnglish, BM_SETCHECK, vLanguage ? 0 : 1, 0);
        InvalidateRect(checkVietnamese, NULL, FALSE);
        InvalidateRect(checkEnglish, NULL, FALSE);
        if (vUseSmartSwitchKey) {
            setAppInputMethodStatus(VioKeyHelper::getFrontMostAppExecuteName(), vLanguage);
            saveSmartSwitchKeyData();
        }
    }
    else if (hWnd == checkRunWithWindows) {
        APP_SET_DATA(vRunWithWindows, (int)SendMessage(hWnd, BM_GETCHECK, 0, 0) ? 1 : 0);
        VioKeyHelper::registerRunOnStartup(vRunWithWindows);
    }
    else if (hWnd == checkSpelling) {
        APP_SET_DATA(vCheckSpelling, (int)SendMessage(hWnd, BM_GETCHECK, 0, 0) ? 1 : 0);
        vSetCheckSpelling();
        EnableWindow(checkRestoreIfWrongSpelling, vCheckSpelling);
        EnableWindow(checkAllowZWJF, vCheckSpelling);
        EnableWindow(checkTempOffSpelling, vCheckSpelling);
    }
    else if (hWnd == checkRunAsAdmin) {
        APP_SET_DATA(vRunAsAdmin, (int)SendMessage(hWnd, BM_GETCHECK, 0, 0) ? 1 : 0);
        requestRestartAsAdmin();
    }
    else if (hWnd == checkUseClipboard) {
        // checked = paste through clipboard, i.e. NOT step by step
        APP_SET_DATA(vSendKeyStepByStep, (int)SendMessage(hWnd, BM_GETCHECK, 0, 0) ? 0 : 1);
    }
    BIND_CHECK(checkModernOrthorgraphy, vUseModernOrthography)
    BIND_CHECK(checkFixRecommendBrowser, vFixRecommendBrowser)
    BIND_CHECK(checkRestoreIfWrongSpelling, vRestoreIfWrongSpelling)
    BIND_CHECK(checkSmartSwitchKey, vUseSmartSwitchKey)
    BIND_CHECK(checkCapsFirstChar, vUpperCaseFirstChar)
    BIND_CHECK(checkQuickTelex, vQuickTelex)
    BIND_CHECK(checkUseMacro, vUseMacro)
    BIND_CHECK(checkUseMacroInEnglish, vUseMacroInEnglishMode)
    BIND_CHECK(checkMacroAutoCaps, vAutoCapsMacro)
    BIND_CHECK(checkQuickStartConsonant, vQuickStartConsonant)
    BIND_CHECK(checkQuickEndConsonant, vQuickEndConsonant)
    BIND_CHECK(checkAllowZWJF, vAllowConsonantZFWJ)
    BIND_CHECK(checkTempOffSpelling, vTempOffSpelling)
    BIND_CHECK(checkTempOffVioKey, vTempOffVioKey)
    BIND_CHECK(checkShowOnStartup, vShowOnStartUp)
    BIND_CHECK(checkModernIcon, vUseGrayIcon)
    SystemTrayHelper::updateData();
}

void MainControlDialog::onSwitchKeyPressed(const UINT16& keyCode) {
    UINT16 code = GET_SWITCH_KEY(vSwitchKeyStatus);
    if (keyCode == VK_DELETE || keyCode == VK_BACK) {
        code = EMPTY_SWITCH_KEY;
    }
    else if (keyCodeToCharacter(keyCode) != 0) {
        code = keyCode;
    }
    else {
        return;
    }
    setSwitchKey(code);
    setSwitchKeyText(textSwitchKey, code);
}

void MainControlDialog::setSwitchKeyText(const HWND& hWnd, const UINT16& keyCode) {
    if (keyCode == KEY_SPACE) {
        SetWindowText(hWnd, _T("Space"));
    }
    else if (keyCode == EMPTY_SWITCH_KEY) {
        SetWindowText(hWnd, _T(""));
    }
    else {
        wchar_t key[] = { (wchar_t)keyCode, 0 };
        SetWindowText(hWnd, key);
    }
}

void MainControlDialog::requestRestartAsAdmin() {
    VioKeyHelper::registerRunOnStartup(false);
    if (vRunAsAdmin && !IsUserAnAdmin()) {
        int msgboxID = MessageBox(
            hDlg,
            _T("Bạn cần phải khởi động lại VioKey để kích hoạt chế độ Admin!\nBạn có muốn khởi động lại VioKey không?"),
            _T("VioKey"),
            MB_ICONEXCLAMATION | MB_YESNO
        );
        if (msgboxID == IDYES) {
            PostQuitMessage(0);
            ShellExecute(0, L"runas", VioKeyHelper::getFullPath().c_str(), 0, 0, SW_SHOWNORMAL);
        }
    }
    else {
        VioKeyHelper::registerRunOnStartup(vRunWithWindows);
    }
}
