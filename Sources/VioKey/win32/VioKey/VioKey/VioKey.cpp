/*----------------------------------------------------------
VioKey - Vietnamese Input Application for Windows.

Based on OpenKey (https://github.com/tuyenvm/OpenKey)
Copyright (C) 2019 Mai Vu Tuyen - GPL license

VioKey is a simplified fork: Telex/VNI only, Unicode only.
You can fork, modify, improve this program. If you
redistribute your new version, it MUST be open source.
-----------------------------------------------------------*/
#include "stdafx.h"
#include "AppDelegate.h"

#pragma comment(lib, "imm32")
#define IMC_GETOPENSTATUS 0x0005

#define MASK_SHIFT				0x01
#define MASK_CONTROL			0x02
#define MASK_ALT				0x04
#define MASK_CAPITAL			0x08
#define MASK_NUMLOCK			0x10
#define MASK_WIN				0x20
#define MASK_SCROLL				0x40

#define OTHER_CONTROL_KEY (_flag & MASK_ALT) || (_flag & MASK_CONTROL)
#define DYNA_DATA(macro, pos) (macro ? pData->macroData[pos] : pData->charData[pos])
#define EMPTY_HOTKEY 0xFE0000FE

extern int vSendKeyStepByStep;

static HHOOK hKeyboardHook;
static HHOOK hMouseHook;
static HWINEVENTHOOK hSystemEvent;
static KBDLLHOOKSTRUCT* keyboardData;
static MSLLHOOKSTRUCT* mouseData;
static vKeyHookState* pData;
static Uint32 _flag = 0, _lastFlag = 0;
static bool _flagChanged = false, _isFlagKey;
static Uint16 _keycode;
static Uint16 _newChar;

static vector<Uint16> _newCharString;
static Uint16 _newCharSize;
static bool _willSendControlKey = false;

static int _i, _j, _k;
static Uint32 _tempChar;

static int _languageTemp = 0; //use for smart switch key
static vector<Byte> savedSmartSwitchKeyData; //use for smart switch key

static bool _hasJustUsedHotKey = false;

// Set while the settings window records a new hotkey, so keys reach it untouched.
bool gHookPaused = false;

static INPUT backspaceEvent[2];
static INPUT keyEvent[2];

LRESULT CALLBACK keyboardHookProcess(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK mouseHookProcess(int nCode, WPARAM wParam, LPARAM lParam);
VOID CALLBACK winEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);

void VioKeyFree() {
	UnhookWindowsHookEx(hMouseHook);
	UnhookWindowsHookEx(hKeyboardHook);
	UnhookWinEvent(hSystemEvent);
}

void VioKeyInit() {
	APP_GET_DATA(vLanguage, 1);
	APP_GET_DATA(vInputType, 0);
	APP_GET_DATA(vCheckSpelling, 1);
	APP_GET_DATA(vUseModernOrthography, 1);
	APP_GET_DATA(vQuickTelex, 0);
	APP_GET_DATA(vSwitchKeyStatus, 0x5A00025A);
	APP_GET_DATA(vRestoreIfWrongSpelling, 1);
	APP_GET_DATA(vFixRecommendBrowser, 0);
	APP_GET_DATA(vUseMacro, 1);
	APP_GET_DATA(vUseMacroInEnglishMode, 1);
	APP_GET_DATA(vAutoCapsMacro, 0);
	APP_GET_DATA(vSendKeyStepByStep, 1);
	APP_GET_DATA(vUseSmartSwitchKey, 1);
	APP_GET_DATA(vUpperCaseFirstChar, 0);
	APP_GET_DATA(vTempOffSpelling, 0);
	APP_GET_DATA(vAllowConsonantZFWJ, 0);
	APP_GET_DATA(vQuickStartConsonant, 0);
	APP_GET_DATA(vQuickEndConsonant, 0);
	APP_GET_DATA(vTempOffVioKey, 0);
	APP_GET_DATA(vUseGrayIcon, 0);
	APP_GET_DATA(vShowOnStartUp, 0);
	APP_GET_DATA(vRunWithWindows, 1);
	APP_GET_DATA(vRunAsAdmin, 0);
	APP_GET_DATA(vThemeMode, 0);
	VioKeyHelper::registerRunOnStartup(vRunWithWindows);

	pData = (vKeyHookState*)vKeyInit();

	//pre-create back key
	backspaceEvent[0].type = INPUT_KEYBOARD;
	backspaceEvent[0].ki.dwFlags = 0;
	backspaceEvent[0].ki.wVk = VK_BACK;
	backspaceEvent[0].ki.wScan = 0;
	backspaceEvent[0].ki.time = 0;
	backspaceEvent[0].ki.dwExtraInfo = 1;

	backspaceEvent[1].type = INPUT_KEYBOARD;
	backspaceEvent[1].ki.dwFlags = KEYEVENTF_KEYUP;
	backspaceEvent[1].ki.wVk = VK_BACK;
	backspaceEvent[1].ki.wScan = 0;
	backspaceEvent[1].ki.time = 0;
	backspaceEvent[1].ki.dwExtraInfo = 1;

	//get key state
	_flag = 0;
	if (GetKeyState(VK_LSHIFT) < 0 || GetKeyState(VK_RSHIFT) < 0) _flag |= MASK_SHIFT;
	if (GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0) _flag |= MASK_CONTROL;
	if (GetKeyState(VK_LMENU) < 0 || GetKeyState(VK_RMENU) < 0) _flag |= MASK_ALT;
	if (GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0) _flag |= MASK_WIN;
	if (GetKeyState(VK_NUMLOCK) < 0) _flag |= MASK_NUMLOCK;
	if (GetKeyState(VK_CAPITAL) == 1) _flag |= MASK_CAPITAL;
	if (GetKeyState(VK_SCROLL) < 0) _flag |= MASK_SCROLL;

	//init and load macro data
	DWORD macroDataSize;
	BYTE* macroData = VioKeyHelper::getRegBinary(_T("macroData"), macroDataSize);
	initMacroMap((Byte*)macroData, (int)macroDataSize);

	//init and load smart switch key data
	DWORD smartSwitchKeySize;
	BYTE* data = VioKeyHelper::getRegBinary(_T("smartSwitchKey"), smartSwitchKeySize);
	initSmartSwitchKey((Byte*)data, (int)smartSwitchKeySize);

	//init hook
	HINSTANCE hInstance = GetModuleHandle(NULL);
	hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookProcess, hInstance, 0);
	hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHookProcess, hInstance, 0);
	hSystemEvent = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, winEventProcCallback, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
}

void saveSmartSwitchKeyData() {
	getSmartSwitchKeySaveData(savedSmartSwitchKeyData);
	VioKeyHelper::setRegBinary(_T("smartSwitchKey"), savedSmartSwitchKeyData.data(), (int)savedSmartSwitchKeyData.size());
}

static inline void prepareKeyEvent(INPUT& input, const Uint16& keycode, const bool& isPress, const DWORD& flag=0) {
	input.type = INPUT_KEYBOARD;
	input.ki.dwFlags = isPress ? flag : flag|KEYEVENTF_KEYUP;
	input.ki.wVk = keycode;
	input.ki.wScan = 0;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 1;
}

static inline void prepareUnicodeEvent(INPUT& input, const Uint16& unicode, const bool& isPress) {
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = 0;
	input.ki.wScan = unicode;
	input.ki.time = 0;
	input.ki.dwFlags = (isPress ? 0 : KEYEVENTF_KEYUP) | KEYEVENTF_UNICODE;
	input.ki.dwExtraInfo = 1;
}

static void SendCombineKey(const Uint16& key1, const Uint16& key2, const DWORD& flagKey1=0, const DWORD& flagKey2 = 0) {
	prepareKeyEvent(keyEvent[0], key1, true, flagKey1);
	SendInput(1, keyEvent, sizeof(INPUT));

	prepareKeyEvent(keyEvent[0], key2, true, flagKey2);
	prepareKeyEvent(keyEvent[1], key2, false, flagKey2);
	SendInput(2, keyEvent, sizeof(INPUT));

	prepareKeyEvent(keyEvent[0], key1, false, flagKey1);
	SendInput(1, keyEvent, sizeof(INPUT));
}

//Unicode precomposed: every character is exactly one UTF-16 unit
static void SendKeyCode(Uint32 data) {
	_newChar = (Uint16)data;
	if (!(data & CHAR_CODE_MASK)) {
		_newChar = keyCodeToCharacter(data);
		if (_newChar == 0) {
			_newChar = (Uint16)data;
			prepareKeyEvent(keyEvent[0], _newChar, true);
			prepareKeyEvent(keyEvent[1], _newChar, false);
			SendInput(2, keyEvent, sizeof(INPUT));
		} else {
			prepareUnicodeEvent(keyEvent[0], _newChar, true);
			prepareUnicodeEvent(keyEvent[1], _newChar, false);
			SendInput(2, keyEvent, sizeof(INPUT));
		}
	} else {
		prepareUnicodeEvent(keyEvent[0], _newChar, true);
		prepareUnicodeEvent(keyEvent[1], _newChar, false);
		SendInput(2, keyEvent, sizeof(INPUT));
	}
}

static void SendBackspace() {
	SendInput(2, backspaceEvent, sizeof(INPUT));
}

static void SendEmptyCharacter() {
	_newChar = 0x202F; //empty char

	prepareUnicodeEvent(keyEvent[0], _newChar, true);
	prepareUnicodeEvent(keyEvent[1], _newChar, false);
	SendInput(2, keyEvent, sizeof(INPUT));
}

static void SendNewCharString(const bool& dataFromMacro = false) {
	_j = 0;
	_newCharSize = dataFromMacro ? (Uint16)pData->macroData.size() : pData->newCharCount;
	if (_newCharString.size() < _newCharSize + 1) {
		_newCharString.resize(_newCharSize + 1);
	}
	_willSendControlKey = false;

	if (_newCharSize > 0) {
		for (_k = dataFromMacro ? 0 : pData->newCharCount - 1;
			dataFromMacro ? _k < pData->macroData.size() : _k >= 0;
			dataFromMacro ? _k++ : _k--) {

			_tempChar = DYNA_DATA(dataFromMacro, _k);
			if (_tempChar & PURE_CHARACTER_MASK) {
				_newCharString[_j++] = _tempChar;
			} else if (!(_tempChar & CHAR_CODE_MASK)) {
				_newCharString[_j++] = keyCodeToCharacter(_tempChar);
			} else {
				_newCharString[_j++] = _tempChar;
			}
		}//end for
	}

	if (pData->code == vRestore || pData->code == vRestoreAndStartNewSession) { //if is restore
		if (keyCodeToCharacter(_keycode) != 0) {
			_newCharSize++;
			_newCharString[_j++] = keyCodeToCharacter(_keycode | ((_flag & MASK_SHIFT) || (_flag & MASK_CAPITAL) ? CAPS_MASK : 0));
		} else {
			_willSendControlKey = true;
		}
	}
	if (pData->code == vRestoreAndStartNewSession) {
		startNewSession();
	}

	VioKeyHelper::setClipboardText((LPCTSTR)_newCharString.data(), _newCharSize + 1, CF_UNICODETEXT);

	//Send shift + insert
	SendCombineKey(KEY_LEFT_SHIFT, VK_INSERT, 0, KEYEVENTF_EXTENDEDKEY);

	//the case when hCode is vRestore or vRestoreAndStartNewSession,
	//the word is invalid and last key is control key such as TAB, LEFT ARROW, RIGHT ARROW,...
	if (_willSendControlKey) {
		SendKeyCode(_keycode);
	}
}

bool checkHotKey(int hotKeyData, bool checkKeyCode = true) {
	if ((hotKeyData & (~0x8000)) == EMPTY_HOTKEY)
		return false;
	if (HAS_CONTROL(hotKeyData) ^ GET_BOOL(_lastFlag & MASK_CONTROL))
		return false;
	if (HAS_OPTION(hotKeyData) ^ GET_BOOL(_lastFlag & MASK_ALT))
		return false;
	if (HAS_COMMAND(hotKeyData) ^ GET_BOOL(_lastFlag & MASK_WIN))
		return false;
	if (HAS_SHIFT(hotKeyData) ^ GET_BOOL(_lastFlag & MASK_SHIFT))
		return false;
	if (checkKeyCode) {
		if (GET_SWITCH_KEY(hotKeyData) != _keycode)
			return false;
	}
	return true;
}

void switchLanguage() {
	if (vLanguage == 0)
		vLanguage = 1;
	else
		vLanguage = 0;
	if (HAS_BEEP(vSwitchKeyStatus))
		MessageBeep(MB_OK);
	AppDelegate::getInstance()->onInputMethodChangedFromHotKey();
	if (vUseSmartSwitchKey) {
		setAppInputMethodStatus(VioKeyHelper::getFrontMostAppExecuteName(), vLanguage);
		saveSmartSwitchKeyData();
	}
	startNewSession();
}

static void SendPureCharacter(const Uint16& ch) {
	if (ch < 128)
		SendKeyCode(ch);
	else {
		prepareUnicodeEvent(keyEvent[0], ch, true);
		prepareUnicodeEvent(keyEvent[1], ch, false);
		SendInput(2, keyEvent, sizeof(INPUT));
	}
}

static void handleMacro() {
	//fix autocomplete
	if (vFixRecommendBrowser) {
		SendEmptyCharacter();
		pData->backspaceCount++;
	}

	//send backspace
	if (pData->backspaceCount > 0) {
		for (int i = 0; i < pData->backspaceCount; i++) {
			SendBackspace();
		}
	}
	//send real data
	if (!vSendKeyStepByStep) {
		SendNewCharString(true);
	} else {
		for (int i = 0; i < pData->macroData.size(); i++) {
			if (pData->macroData[i] & PURE_CHARACTER_MASK) {
				SendPureCharacter(pData->macroData[i]);
			} else {
				SendKeyCode(pData->macroData[i]);
			}
		}
	}
	SendKeyCode(_keycode | (_flag & MASK_SHIFT ? CAPS_MASK : 0));
}

static bool SetModifierMask(const Uint16& vkCode) {
	// For caps lock case, toggling the flag isn't enough. We need to check the actual state, which should be done before each key press.
	// Example: the caps lock state can be changed without the key being pressed, or the key toggle is made with admin privilege, making the app not able to detect the change.
	if (GetKeyState(VK_CAPITAL) == 1) _flag |= MASK_CAPITAL;
	else _flag &= ~MASK_CAPITAL;

	if (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT) _flag |= MASK_SHIFT;
	else if (vkCode == VK_LCONTROL || vkCode == VK_RCONTROL) _flag |= MASK_CONTROL;
	else if (vkCode == VK_LMENU || vkCode == VK_RMENU) _flag |= MASK_ALT;
	else if (vkCode == VK_LWIN || vkCode == VK_RWIN) _flag |= MASK_WIN;
	else if (vkCode == VK_NUMLOCK) _flag |= MASK_NUMLOCK;
	else if (vkCode == VK_SCROLL) _flag |= MASK_SCROLL;
	else {
		_isFlagKey = false;
		return false;
	}
	_isFlagKey = true;
	return true;
}

static bool UnsetModifierMask(const Uint16& vkCode) {
	if (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT) _flag &= ~MASK_SHIFT;
	else if (vkCode == VK_LCONTROL || vkCode == VK_RCONTROL) _flag &= ~MASK_CONTROL;
	else if (vkCode == VK_LMENU || vkCode == VK_RMENU) _flag &= ~MASK_ALT;
	else if (vkCode == VK_LWIN || vkCode == VK_RWIN) _flag &= ~MASK_WIN;
	else if (vkCode == VK_NUMLOCK) _flag &= ~MASK_NUMLOCK;
	else if (vkCode == VK_SCROLL) _flag &= ~MASK_SCROLL;
	else {
		_isFlagKey = false;
		return false;
	}
	_isFlagKey = true;
	return true;
}

LRESULT CALLBACK keyboardHookProcess(int nCode, WPARAM wParam, LPARAM lParam) {
	keyboardData = (KBDLLHOOKSTRUCT *)lParam;
	//ignore my event
	if (keyboardData->dwExtraInfo != 0) {
		return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
	}

	//ignore if IME pad is open when typing Japanese/Chinese...
	HWND hWnd = GetForegroundWindow();
	HWND hIME = ImmGetDefaultIMEWnd(hWnd);
	LRESULT isImeON = SendMessage(hIME, WM_IME_CONTROL, IMC_GETOPENSTATUS, 0);
	if (isImeON) {
		return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
	}

	//check modifier key
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		SetModifierMask((Uint16)keyboardData->vkCode);
	} else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
		UnsetModifierMask((Uint16)keyboardData->vkCode);
	}
	//keep modifier state in sync, but let the settings window receive the raw hotkey
	if (gHookPaused) {
		_keycode = 0;
		_lastFlag = 0;
		return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
	}
	if (!_isFlagKey && wParam != WM_KEYUP && wParam != WM_SYSKEYUP)
		_keycode = (Uint16)keyboardData->vkCode;

	//switch language shortcut
	if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && !_isFlagKey && _keycode != 0) {
		if (GET_SWITCH_KEY(vSwitchKeyStatus) != _keycode) {
			_lastFlag = 0;
		} else {
			if (checkHotKey(vSwitchKeyStatus, GET_SWITCH_KEY(vSwitchKeyStatus) != 0xFE)) {
				switchLanguage();
				_hasJustUsedHotKey = true;
				_keycode = 0;
				return -1;
			}
		}
		_hasJustUsedHotKey = _lastFlag != 0;
	} else if (_isFlagKey) {
		if (_lastFlag == 0 || _lastFlag < _flag)
			_lastFlag = _flag;
		else if (_lastFlag > _flag) {
			//check switch
			if (checkHotKey(vSwitchKeyStatus, GET_SWITCH_KEY(vSwitchKeyStatus) != 0xFE)) {
				switchLanguage();
				_hasJustUsedHotKey = true;
			}
			//check temporarily turn off spell checking
			if (vTempOffSpelling && !_hasJustUsedHotKey && _lastFlag & MASK_CONTROL) {
				vTempOffSpellChecking();
			}
			if (vTempOffVioKey && !_hasJustUsedHotKey && _lastFlag & MASK_ALT) {
				vTempOffEngine();
			}
			_lastFlag = _flag;
			_hasJustUsedHotKey = false;
		}
		_keycode = 0;
		return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
	}

	//if is in english mode
	if (vLanguage == 0) {
		if (vUseMacro && vUseMacroInEnglishMode && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
			vEnglishMode(((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) ? vKeyEventState::KeyDown : vKeyEventState::MouseDown),
				_keycode,
				(_flag & MASK_SHIFT) || (_flag & MASK_CAPITAL),
				OTHER_CONTROL_KEY);

			if (pData->code == vReplaceMaro) { //handle macro in english mode
				handleMacro();
				return NULL;
			}
		}
		return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
	}

	//handle keyboard
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		//send event signal to Engine
		vKeyHandleEvent(vKeyEvent::Keyboard,
						vKeyEventState::KeyDown,
						_keycode,
						(_flag & MASK_SHIFT && _flag & MASK_CAPITAL) ? 0 : (_flag & MASK_SHIFT ? 1 : (_flag & MASK_CAPITAL ? 2 : 0)),
						OTHER_CONTROL_KEY);
		if (pData->code == vDoNothing) { //do nothing
			return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
		} else if (pData->code == vWillProcess || pData->code == vRestore || pData->code == vRestoreAndStartNewSession) { //handle result signal
			//fix autocomplete
			if (vFixRecommendBrowser && pData->extCode != 4) {
				SendEmptyCharacter();
				pData->backspaceCount++;
			}

			//send backspace
			if (pData->backspaceCount > 0 && pData->backspaceCount < MAX_BUFF) {
				for (_i = 0; _i < pData->backspaceCount; _i++) {
					SendBackspace();
				}
			}

			//send new character
			if (!vSendKeyStepByStep) {
				SendNewCharString();
			} else {
				if (pData->newCharCount > 0 && pData->newCharCount <= MAX_BUFF) {
					for (int i = pData->newCharCount - 1; i >= 0; i--) {
						SendKeyCode(pData->charData[i]);
					}
				}
				if (pData->code == vRestore || pData->code == vRestoreAndStartNewSession) {
					SendKeyCode(_keycode | ((_flag & MASK_CAPITAL) || (_flag & MASK_SHIFT) ? CAPS_MASK : 0));
				}
				if (pData->code == vRestoreAndStartNewSession) {
					startNewSession();
				}
			}
		} else if (pData->code == vReplaceMaro) { //MACRO
			handleMacro();
		}
		return -1; //consume event
	}
	return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK mouseHookProcess(int nCode, WPARAM wParam, LPARAM lParam) {
	mouseData = (MSLLHOOKSTRUCT *)lParam;
	switch (wParam) {
	case WM_LBUTTONDOWN:

	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_NCXBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	case WM_NCXBUTTONUP:
		//send event signal to Engine
		vKeyHandleEvent(vKeyEvent::Mouse, vKeyEventState::MouseDown, 0);
		break;
	}
	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

VOID CALLBACK winEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
	//smart switch key
	if (vUseSmartSwitchKey) {
		string& exe = VioKeyHelper::getFrontMostAppExecuteName();
		if (exe.compare("explorer.exe") == 0) //dont apply with windows explorer
			return;
		_languageTemp = getAppInputMethodStatus(exe, vLanguage);
		vTempOffEngine(false);
		if ((_languageTemp & 0x01) != vLanguage) {
			if (_languageTemp != -1) {
				vLanguage = _languageTemp & 0x01;
				AppDelegate::getInstance()->onInputMethodChangedFromHotKey();
			} else {
				saveSmartSwitchKeyData();
			}
		}
		startNewSession();
	}
}
