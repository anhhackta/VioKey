/*----------------------------------------------------------
VioKey - Vietnamese Input Application for Windows.
Released under the GPLv3 license.
-----------------------------------------------------------*/
#pragma once
#include "stdafx.h"

// SaaS 2026 UI Theme engine for Win32 dialogs with instant Dark / Light mode switching.
namespace UITheme {
	// Call once at startup / shutdown (GDI+ lifetime).
	void init();
	void shutdown();

	// Dark / Light mode control.
	bool isDarkMode();
	void setDarkMode(bool dark);
	void toggleDarkMode();

	// Title bar, border, and rounded corners (no-op where unsupported, e.g. Win10 older builds).
	void applyWindow(HWND hDlg);

	// Subclass/skin every child control of hRoot (toggles, chips, buttons, combos, edits, list views).
	void styleControls(HWND hRoot);

	// Re-apply theme colors to an existing open dialog and repaint all children.
	void refreshWindow(HWND hDlg);

	// Handles WM_CTLCOLOR* / WM_ERASEBKGND for a dialog. Returns true when handled;
	// `result` is then the value the dialog procedure must return.
	bool handleMessage(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam, INT_PTR& result);

	// Marks a push button as the selected "pill" tab.
	void setTabSelected(HWND hButton, bool selected);

	// Fonts used by title and section labels (owned by the theme).
	HFONT titleFont();
	HFONT sectionFont();
}
