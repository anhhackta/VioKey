/*----------------------------------------------------------
VioKey - Vietnamese Input Application for Windows.
Released under the GPLv3 license.
-----------------------------------------------------------*/
#include "UITheme.h"
#include <objidl.h>
#include <gdiplus.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <commctrl.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "comctl32.lib")

using namespace Gdiplus;

namespace {
	struct ThemePalette {
		COLORREF crBg;
		COLORREF crCard;
		COLORREF crPanel;
		COLORREF crText;
		COLORREF crTextMuted;
		COLORREF crTextDisabled;
		COLORREF crAccent;
		COLORREF crDwmBorder;

		Color bg;
		Color card;
		Color cardBorder;
		Color cardHighlight;
		Color panel;
		Color panelHover;
		Color panelPressed;
		Color border;
		Color outline;
		Color outlineDisabled;
		Color accent;
		Color accentHover;
		Color accentPressed;
		Color accentSoft;
		Color accentDisabled;
		Color danger;
		Color dangerSoft;
		Color thumbOn;
		Color thumbOff;
		Color trackOff;
	};

	// SaaS 2026 Dark Palette (Linear / Raycast inspired obsidian + indigo-violet)
	const ThemePalette DARK_PALETTE = {
		RGB(0x0B, 0x0D, 0x13), // crBg
		RGB(0x13, 0x17, 0x22), // crCard
		RGB(0x1C, 0x21, 0x30), // crPanel
		RGB(0xF8, 0xFA, 0xFC), // crText
		RGB(0x94, 0xA3, 0xB8), // crTextMuted
		RGB(0x47, 0x55, 0x69), // crTextDisabled
		RGB(0x81, 0x8C, 0xF8), // crAccent (#818CF8)
		RGB(0x22, 0x28, 0x3A), // crDwmBorder

		Color(255, 0x0B, 0x0D, 0x13), // bg
		Color(255, 0x13, 0x17, 0x22), // card
		Color(255, 0x22, 0x28, 0x3A), // cardBorder
		Color(28,  0xFF, 0xFF, 0xFF), // cardHighlight
		Color(255, 0x1C, 0x21, 0x30), // panel
		Color(255, 0x26, 0x2D, 0x40), // panelHover
		Color(255, 0x17, 0x1B, 0x28), // panelPressed
		Color(255, 0x2E, 0x36, 0x4F), // border
		Color(255, 0x64, 0x74, 0x8B), // outline
		Color(255, 0x33, 0x41, 0x55), // outlineDisabled
		Color(255, 0x63, 0x66, 0xF1), // accent (#6366F1)
		Color(255, 0x81, 0x8C, 0xF8), // accentHover
		Color(255, 0x4F, 0x46, 0xE5), // accentPressed
		Color(42,  0x63, 0x66, 0xF1), // accentSoft
		Color(255, 0x31, 0x35, 0x6E), // accentDisabled
		Color(255, 0xF8, 0x71, 0x71), // danger
		Color(36,  0xEF, 0x44, 0x44), // dangerSoft
		Color(255, 0xFF, 0xFF, 0xFF), // thumbOn
		Color(255, 0xCB, 0xD5, 0xE1), // thumbOff
		Color(255, 0x18, 0x1D, 0x2B), // trackOff
	};

	// SaaS 2026 Light Palette (Vercel / Stripe inspired crisp Alabaster + pure white cards)
	const ThemePalette LIGHT_PALETTE = {
		RGB(0xF4, 0xF6, 0xFA), // crBg
		RGB(0xFF, 0xFF, 0xFF), // crCard
		RGB(0xF1, 0xF5, 0xF9), // crPanel
		RGB(0x0F, 0x17, 0x2A), // crText
		RGB(0x64, 0x74, 0x8B), // crTextMuted
		RGB(0x94, 0xA3, 0xB8), // crTextDisabled
		RGB(0x4F, 0x46, 0xE5), // crAccent (#4F46E5)
		RGB(0xE2, 0xE8, 0xF0), // crDwmBorder

		Color(255, 0xF4, 0xF6, 0xFA), // bg
		Color(255, 0xFF, 0xFF, 0xFF), // card
		Color(255, 0xE2, 0xE8, 0xF0), // cardBorder
		Color(180, 0xFF, 0xFF, 0xFF), // cardHighlight
		Color(255, 0xF1, 0xF5, 0xF9), // panel
		Color(255, 0xE2, 0xE8, 0xF0), // panelHover
		Color(255, 0xCB, 0xD5, 0xE1), // panelPressed
		Color(255, 0xCB, 0xD5, 0xE1), // border
		Color(255, 0x94, 0xA3, 0xB8), // outline
		Color(255, 0xCB, 0xD5, 0xE1), // outlineDisabled
		Color(255, 0x4F, 0x46, 0xE5), // accent (#4F46E5)
		Color(255, 0x63, 0x66, 0xF1), // accentHover
		Color(255, 0x43, 0x38, 0xCA), // accentPressed
		Color(28,  0x4F, 0x46, 0xE5), // accentSoft
		Color(255, 0xA5, 0xB4, 0xFC), // accentDisabled
		Color(255, 0xDC, 0x26, 0x26), // danger
		Color(28,  0xEF, 0x44, 0x44), // dangerSoft
		Color(255, 0xFF, 0xFF, 0xFF), // thumbOn
		Color(255, 0x64, 0x74, 0x8B), // thumbOff
		Color(255, 0xE2, 0xE8, 0xF0), // trackOff
	};

	// Layout metrics at 96 DPI
	const float TOGGLE_WIDTH = 34.f;
	const float TOGGLE_HEIGHT = 18.f;
	const float TOGGLE_TEXT_GAP = 8.f;
	const float BUTTON_RADIUS = 5.5f;
	const float CARD_RADIUS = 7.5f;

	const UINT_PTR SUBCLASS_ID = 1;
	const wchar_t PROP_HOVER[] = L"VioKeyHover";
	const wchar_t PROP_TAB_SELECTED[] = L"VioKeyTabSelected";

	// DWM attribute ids
	const DWORD DWMWA_DARK_MODE = 20;
	const DWORD DWMWA_DARK_MODE_LEGACY = 19;
	const DWORD DWMWA_CORNER_PREFERENCE = 33;
	const DWORD DWMWA_BORDER = 34;
	const DWORD DWMWA_CAPTION = 35;
	const int DWM_CORNER_ROUND = 2;

	// Undocumented uxtheme dark mode ordinals (Win10 1809+ / Win11)
	enum PreferredAppMode { Default, AllowDark, ForceDark, ForceLight, Max };
	using fnSetPreferredAppMode = PreferredAppMode(WINAPI*)(PreferredAppMode appMode);
	using fnAllowDarkModeForWindow = bool(WINAPI*)(HWND hWnd, bool allow);
	using fnFlushMenuThemes = void(WINAPI*)();
	fnSetPreferredAppMode pSetPreferredAppMode = nullptr;
	fnAllowDarkModeForWindow pAllowDarkModeForWindow = nullptr;
	fnFlushMenuThemes pFlushMenuThemes = nullptr;

	bool isDark = true;
	const ThemePalette* pal = &DARK_PALETTE;
	ULONG_PTR gdiplusToken = 0;
	HBRUSH brushBg = NULL;
	HBRUSH brushCard = NULL;
	HBRUSH brushPanel = NULL;
	HFONT fontTitle = NULL;
	HFONT fontSection = NULL;

	enum class ButtonKind { Toggle, Chip, ModePill, ThemeBtn, Push, Tab };

	void rebuildBrushes() {
		if (brushBg) DeleteObject(brushBg);
		if (brushCard) DeleteObject(brushCard);
		if (brushPanel) DeleteObject(brushPanel);
		brushBg = CreateSolidBrush(pal->crBg);
		brushCard = CreateSolidBrush(pal->crCard);
		brushPanel = CreateSolidBrush(pal->crPanel);
	}

	void syncUxThemeMode() {
		if (!pSetPreferredAppMode) {
			HMODULE hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
			if (hUxtheme) {
				pSetPreferredAppMode = reinterpret_cast<fnSetPreferredAppMode>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135)));
				pAllowDarkModeForWindow = reinterpret_cast<fnAllowDarkModeForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133)));
				pFlushMenuThemes = reinterpret_cast<fnFlushMenuThemes>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(136)));
			}
		}
		if (pSetPreferredAppMode) pSetPreferredAppMode(isDark ? ForceDark : ForceLight);
		if (pFlushMenuThemes) pFlushMenuThemes();
	}

	float dpiScale(HWND hWnd) {
		HDC dc = GetDC(hWnd);
		float scale = GetDeviceCaps(dc, LOGPIXELSX) / 96.f;
		ReleaseDC(hWnd, dc);
		return scale;
	}

	bool isTabPage(HWND hWnd) {
		return (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) != 0;
	}

	ButtonKind kindOf(HWND hButton) {
		int id = GetDlgCtrlID(hButton);
		if (id == IDC_BUTTON_THEME_TOGGLE)
			return ButtonKind::ThemeBtn;
		if (id >= IDC_TAB_BUTTON_GENERAL && id <= IDC_TAB_BUTTON_SYSTEM)
			return ButtonKind::Tab;
		if (id == IDC_CHECK_SWITCH_KEY_CTRL || id == IDC_CHECK_SWITCH_KEY_ALT ||
			id == IDC_CHECK_SWITCH_KEY_WIN || id == IDC_CHECK_SWITCH_KEY_SHIFT)
			return ButtonKind::Chip;
		LONG type = GetWindowLong(hButton, GWL_STYLE) & BS_TYPEMASK;
		if (type == BS_AUTORADIOBUTTON || type == BS_RADIOBUTTON)
			return ButtonKind::ModePill;
		if (type == BS_AUTOCHECKBOX || type == BS_CHECKBOX)
			return ButtonKind::Toggle;
		return ButtonKind::Push;
	}

	void roundRectPath(GraphicsPath& path, const RectF& r, float radius) {
		float d = radius * 2.f;
		if (d > r.Width) d = r.Width;
		if (d > r.Height) d = r.Height;
		path.AddArc(r.X, r.Y, d, d, 180, 90);
		path.AddArc(r.GetRight() - d, r.Y, d, d, 270, 90);
		path.AddArc(r.GetRight() - d, r.GetBottom() - d, d, d, 0, 90);
		path.AddArc(r.X, r.GetBottom() - d, d, d, 90, 90);
		path.CloseFigure();
	}

	void fillRound(Graphics& g, const RectF& r, float radius, const Color& color) {
		GraphicsPath path;
		roundRectPath(path, r, radius);
		SolidBrush brush(color);
		g.FillPath(&brush, &path);
	}

	void strokeRound(Graphics& g, const RectF& r, float radius, const Color& color, float width) {
		GraphicsPath path;
		roundRectPath(path, r, radius);
		Pen pen(color, width);
		g.DrawPath(&pen, &path);
	}

	void drawLabelText(HDC dc, HWND hWnd, const wchar_t* text, RECT textRect, COLORREF color, UINT format, HFONT overrideFont = NULL) {
		HFONT font = overrideFont ? overrideFont : (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
		HGDIOBJ oldFont = SelectObject(dc, font);
		SetBkMode(dc, TRANSPARENT);
		SetTextColor(dc, color);
		DrawText(dc, text, -1, &textRect, format | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
		SelectObject(dc, oldFont);
	}

	void drawLabel(HDC dc, HWND hWnd, RECT textRect, COLORREF color, UINT format, HFONT overrideFont = NULL) {
		wchar_t text[256];
		GetWindowText(hWnd, text, 256);
		drawLabelText(dc, hWnd, text, textRect, color, format, overrideFont);
	}

	void drawToggle(HDC dc, HWND hWnd, const RECT& rc, bool checked, bool enabled, bool hover, float scale) {
		Graphics g(dc);
		g.SetSmoothingMode(SmoothingModeAntiAlias);

		float width = TOGGLE_WIDTH * scale, height = TOGGLE_HEIGHT * scale;
		float top = rc.top + ((rc.bottom - rc.top) - height) / 2.f;
		RectF track(rc.left + 1.f, top, width - 2.f, height - 1.f);
		float inset = 3.2f * scale;
		float thumbSize = checked ? 11.6f * scale : 10.2f * scale;
		float thumbY = top + (height - 1.f - thumbSize) / 2.f;
		float thumbX = checked ? track.GetRight() - inset - thumbSize + 0.5f : track.X + inset;

		if (checked) {
			Color fill = !enabled ? pal->accentDisabled : (hover ? pal->accentHover : pal->accent);
			fillRound(g, track, track.Height / 2.f, fill);
		} else {
			fillRound(g, track, track.Height / 2.f, pal->trackOff);
			strokeRound(g, track, track.Height / 2.f,
				enabled ? (hover ? pal->accentHover : pal->outline) : pal->outlineDisabled,
				1.2f * scale);
		}
		SolidBrush thumb(checked ? pal->thumbOn : (enabled ? pal->thumbOff : pal->outlineDisabled));
		g.FillEllipse(&thumb, thumbX, thumbY, thumbSize, thumbSize);

		RECT textRect = rc;
		textRect.left = (LONG)(rc.left + width + TOGGLE_TEXT_GAP * scale);
		drawLabel(dc, hWnd, textRect, enabled ? pal->crText : pal->crTextDisabled, DT_LEFT);
	}

	void drawChip(HDC dc, HWND hWnd, const RECT& rc, bool checked, bool enabled, bool hover, float scale) {
		Graphics g(dc);
		g.SetSmoothingMode(SmoothingModeAntiAlias);

		RectF r(rc.left + 1.f, rc.top + 1.f, (float)(rc.right - rc.left) - 2.f, (float)(rc.bottom - rc.top) - 2.f);
		float radius = 4.5f * scale;
		COLORREF textColor = pal->crTextMuted;

		if (checked) {
			fillRound(g, r, radius, hover ? pal->accentHover : pal->accent);
			textColor = RGB(0xFF, 0xFF, 0xFF);
		} else {
			fillRound(g, r, radius, hover ? pal->panelHover : pal->panel);
			strokeRound(g, r, radius, hover ? pal->outline : pal->border, 1.f);
			textColor = hover ? pal->crText : pal->crTextMuted;
		}
		if (!enabled) textColor = pal->crTextDisabled;
		drawLabel(dc, hWnd, rc, textColor, DT_CENTER, fontSection);
	}

	void drawModePill(HDC dc, HWND hWnd, const RECT& rc, bool checked, bool enabled, bool hover, float scale) {
		Graphics g(dc);
		g.SetSmoothingMode(SmoothingModeAntiAlias);

		RectF r(rc.left + 1.f, rc.top + 1.f, (float)(rc.right - rc.left) - 2.f, (float)(rc.bottom - rc.top) - 2.f);
		float radius = r.Height / 2.f;
		COLORREF textColor = pal->crTextMuted;

		if (checked) {
			bool isViet = GetDlgCtrlID(hWnd) == IDC_RADIO_METHOD_VIETNAMESE;
			Color fill = isViet ? (hover ? pal->accentHover : pal->accent) : (isDark ? Color(255, 0x33, 0x41, 0x55) : Color(255, 0x47, 0x55, 0x69));
			fillRound(g, r, radius, fill);
			textColor = RGB(0xFF, 0xFF, 0xFF);
		} else {
			fillRound(g, r, radius, hover ? pal->panelHover : pal->card);
			strokeRound(g, r, radius, pal->cardBorder, 1.f);
			textColor = hover ? pal->crText : pal->crTextMuted;
		}
		if (!enabled) textColor = pal->crTextDisabled;
		drawLabel(dc, hWnd, rc, textColor, DT_CENTER, fontSection);
	}

	void drawThemeButton(HDC dc, HWND hWnd, const RECT& rc, bool hover, bool pressed, float scale) {
		Graphics g(dc);
		g.SetSmoothingMode(SmoothingModeAntiAlias);

		RectF r(rc.left + 1.f, rc.top + 1.f, (float)(rc.right - rc.left) - 2.f, (float)(rc.bottom - rc.top) - 2.f);
		float radius = r.Height / 2.f;

		fillRound(g, r, radius, pressed ? pal->panelPressed : (hover ? pal->panelHover : pal->card));
		strokeRound(g, r, radius, hover ? pal->accent : pal->cardBorder, 1.f);

		// Draw crisp Sun or Moon icon on the left side of the pill
		float cy = r.Y + r.Height / 2.f;
		float cx = r.X + 10.f * scale;
		float ir = 4.2f * scale;

		if (isDark) {
			// Crescent moon in warm amber-violet
			SolidBrush moonBrush(Color(255, 0xA5, 0xB4, 0xFC));
			g.FillEllipse(&moonBrush, cx - ir, cy - ir, ir * 2.f, ir * 2.f);
			SolidBrush cutBrush(pressed ? pal->panelPressed : (hover ? pal->panelHover : pal->card));
			g.FillEllipse(&cutBrush, cx - ir * 0.2f, cy - ir * 1.15f, ir * 1.8f, ir * 1.8f);
		} else {
			// Sun disk with soft glow ring
			SolidBrush glowBrush(Color(55, 0xF5, 0x9E, 0x0B));
			g.FillEllipse(&glowBrush, cx - ir * 1.35f, cy - ir * 1.35f, ir * 2.7f, ir * 2.7f);
			SolidBrush sunBrush(Color(255, 0xF5, 0x9E, 0x0B));
			g.FillEllipse(&sunBrush, cx - ir * 0.85f, cy - ir * 0.85f, ir * 1.7f, ir * 1.7f);
		}

		RECT textRect = rc;
		textRect.left = (LONG)(cx + 6.5f * scale);
		drawLabelText(dc, hWnd, isDark ? L"Tối" : L"Sáng", textRect, pal->crText, DT_LEFT, fontSection);
	}

	void drawPush(HDC dc, HWND hWnd, const RECT& rc, ButtonKind kind, bool enabled, bool hover, bool pressed, float scale) {
		Graphics g(dc);
		g.SetSmoothingMode(SmoothingModeAntiAlias);

		int id = GetDlgCtrlID(hWnd);
		bool isDefault = (GetWindowLong(hWnd, GWL_STYLE) & BS_TYPEMASK) == BS_DEFPUSHBUTTON;
		bool tabSelected = kind == ButtonKind::Tab && GetProp(hWnd, PROP_TAB_SELECTED) != NULL;
		bool isDanger = (id == IDBUTTON_EXIT || id == IDC_BUTTON_DELETE);

		RectF r(rc.left + 0.5f, rc.top + 0.5f, (float)(rc.right - rc.left) - 1.f, (float)(rc.bottom - rc.top) - 1.f);
		COLORREF textColor = pal->crText;

		if (kind == ButtonKind::Tab) {
			if (tabSelected) {
				fillRound(g, r, BUTTON_RADIUS * scale, pal->accentSoft);
				strokeRound(g, r, BUTTON_RADIUS * scale, pal->accent, 1.2f * scale);
				RectF bar(r.X + r.Width * 0.24f, r.GetBottom() - 2.4f * scale, r.Width * 0.52f, 2.0f * scale);
				fillRound(g, bar, 1.0f * scale, pal->accent);
				textColor = pal->crAccent;
			} else {
				if (hover || pressed)
					fillRound(g, r, BUTTON_RADIUS * scale, pressed ? pal->panelPressed : pal->card);
				textColor = hover ? pal->crText : pal->crTextMuted;
			}
		} else if (isDefault) {
			Color fill = !enabled ? pal->accentDisabled : (pressed ? pal->accentPressed : (hover ? pal->accentHover : pal->accent));
			fillRound(g, r, BUTTON_RADIUS * scale, fill);
			textColor = RGB(0xFF, 0xFF, 0xFF);
		} else if (isDanger) {
			fillRound(g, r, BUTTON_RADIUS * scale, hover ? pal->dangerSoft : pal->panel);
			strokeRound(g, r, BUTTON_RADIUS * scale, hover ? pal->danger : pal->border, 1.f);
			textColor = isDark ? RGB(0xF8, 0x71, 0x71) : RGB(0xDC, 0x26, 0x26);
		} else {
			fillRound(g, r, BUTTON_RADIUS * scale, pressed ? pal->panelPressed : (hover ? pal->panelHover : pal->panel));
			strokeRound(g, r, BUTTON_RADIUS * scale, hover ? pal->outline : pal->border, 1.f);
			if (!enabled) textColor = pal->crTextDisabled;
		}
		drawLabel(dc, hWnd, rc, textColor, DT_CENTER, (isDefault || tabSelected) ? fontSection : NULL);
	}

	HBRUSH parentBrushFor(HWND hWnd) {
		HWND parent = GetParent(hWnd);
		if (parent && isTabPage(parent))
			return brushCard;
		int id = GetDlgCtrlID(hWnd);
		if (id == IDC_CHECK_SWITCH_KEY_CTRL || id == IDC_CHECK_SWITCH_KEY_ALT ||
			id == IDC_CHECK_SWITCH_KEY_WIN || id == IDC_CHECK_SWITCH_KEY_SHIFT ||
			id == IDC_CHECK_SWITCH_KEY_BEEP)
			return brushCard;
		return brushBg;
	}

	void paintButton(HWND hWnd) {
		PAINTSTRUCT ps;
		HDC dc = BeginPaint(hWnd, &ps);
		RECT rc;
		GetClientRect(hWnd, &rc);

		HDC mem = CreateCompatibleDC(dc);
		HBITMAP bmp = CreateCompatibleBitmap(dc, rc.right, rc.bottom);
		HGDIOBJ oldBmp = SelectObject(mem, bmp);
		FillRect(mem, &rc, parentBrushFor(hWnd));

		ButtonKind kind = kindOf(hWnd);
		float scale = dpiScale(hWnd);
		bool enabled = IsWindowEnabled(hWnd) != FALSE;
		bool hover = GetProp(hWnd, PROP_HOVER) != NULL;
		LRESULT state = SendMessage(hWnd, BM_GETSTATE, 0, 0);
		bool pressed = (state & BST_PUSHED) != 0;
		bool checked = SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED;

		switch (kind) {
		case ButtonKind::Toggle:   drawToggle(mem, hWnd, rc, checked, enabled, hover, scale); break;
		case ButtonKind::Chip:     drawChip(mem, hWnd, rc, checked, enabled, hover, scale); break;
		case ButtonKind::ModePill: drawModePill(mem, hWnd, rc, checked, enabled, hover, scale); break;
		case ButtonKind::ThemeBtn: drawThemeButton(mem, hWnd, rc, hover, pressed, scale); break;
		default:                   drawPush(mem, hWnd, rc, kind, enabled, hover, pressed, scale); break;
		}

		BitBlt(dc, 0, 0, rc.right, rc.bottom, mem, 0, 0, SRCCOPY);
		SelectObject(mem, oldBmp);
		DeleteObject(bmp);
		DeleteDC(mem);
		EndPaint(hWnd, &ps);
	}

	LRESULT CALLBACK buttonProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
		switch (msg) {
		case WM_ERASEBKGND:
			return 1;
		case WM_PAINT:
		case WM_PRINTCLIENT:
			paintButton(hWnd);
			return 0;
		case WM_MOUSEMOVE:
			if (!GetProp(hWnd, PROP_HOVER)) {
				SetProp(hWnd, PROP_HOVER, (HANDLE)1);
				TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
				TrackMouseEvent(&tme);
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case WM_MOUSELEAVE:
			RemoveProp(hWnd, PROP_HOVER);
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_NCDESTROY:
			RemoveProp(hWnd, PROP_HOVER);
			RemoveProp(hWnd, PROP_TAB_SELECTED);
			RemoveWindowSubclass(hWnd, buttonProc, SUBCLASS_ID);
			break;
		}

		LRESULT result = DefSubclassProc(hWnd, msg, wParam, lParam);
		switch (msg) {
		case BM_SETCHECK:
		case BM_SETSTATE:
		case WM_ENABLE:
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_KEYUP:
		case WM_SETTEXT:
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		return result;
	}

	bool classIs(HWND hWnd, const wchar_t* className) {
		wchar_t buffer[64];
		GetClassName(hWnd, buffer, 64);
		return _wcsicmp(buffer, className) == 0;
	}

	BOOL CALLBACK styleChildProc(HWND hChild, LPARAM) {
		if (pAllowDarkModeForWindow)
			pAllowDarkModeForWindow(hChild, isDark);

		int id = GetDlgCtrlID(hChild);
		if (id == IDC_STATIC_SECTION_LEFT || id == IDC_STATIC_SECTION_RIGHT) {
			SendMessage(hChild, WM_SETFONT, (WPARAM)fontSection, TRUE);
		}

		if (classIs(hChild, L"Button")) {
			LONG type = GetWindowLong(hChild, GWL_STYLE) & BS_TYPEMASK;
			if (type != BS_GROUPBOX)
				SetWindowSubclass(hChild, buttonProc, SUBCLASS_ID, 0);
		} else if (classIs(hChild, L"ComboBox")) {
			SetWindowTheme(hChild, isDark ? L"DarkMode_CFD" : L"Explorer", NULL);
		} else if (classIs(hChild, L"Edit")) {
			SetWindowTheme(hChild, isDark ? L"DarkMode_CFD" : L"Explorer", NULL);
		} else if (classIs(hChild, WC_LISTVIEW)) {
			SetWindowTheme(hChild, isDark ? L"DarkMode_Explorer" : L"Explorer", NULL);
			ListView_SetBkColor(hChild, pal->crCard);
			ListView_SetTextBkColor(hChild, pal->crCard);
			ListView_SetTextColor(hChild, pal->crText);
			HWND hHeader = ListView_GetHeader(hChild);
			if (hHeader) {
				if (pAllowDarkModeForWindow) pAllowDarkModeForWindow(hHeader, isDark);
				SetWindowTheme(hHeader, isDark ? L"DarkMode_ItemsView" : L"ItemsView", NULL);
			}
		}
		InvalidateRect(hChild, NULL, TRUE);
		return TRUE;
	}

	void drawCardRect(Graphics& g, HWND hDlg, LONG leftDlu, LONG topDlu, LONG rightDlu, LONG bottomDlu, float scale) {
		RECT r = { leftDlu, topDlu, rightDlu, bottomDlu };
		MapDialogRect(hDlg, &r);
		RectF card((float)r.left, (float)r.top, (float)(r.right - r.left), (float)(r.bottom - r.top));
		fillRound(g, card, CARD_RADIUS * scale, pal->card);
		strokeRound(g, card, CARD_RADIUS * scale, pal->cardBorder, 1.f);
	}
}

namespace UITheme {

void init() {
	isDark = (vThemeMode == 0);
	pal = isDark ? &DARK_PALETTE : &LIGHT_PALETTE;
	syncUxThemeMode();

	INITCOMMONCONTROLSEX controls = { sizeof(controls), ICC_STANDARD_CLASSES | ICC_LISTVIEW_CLASSES | ICC_WIN95_CLASSES };
	InitCommonControlsEx(&controls);

	GdiplusStartupInput input;
	GdiplusStartup(&gdiplusToken, &input, NULL);
	rebuildBrushes();

	NONCLIENTMETRICS metrics = { sizeof(metrics) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0);
	LOGFONT titleLog = metrics.lfMessageFont;
	wcscpy_s(titleLog.lfFaceName, L"Segoe UI Semibold");
	titleLog.lfHeight = (LONG)(titleLog.lfHeight * 1.55f);
	titleLog.lfWeight = FW_SEMIBOLD;
	fontTitle = CreateFontIndirect(&titleLog);

	LOGFONT secLog = metrics.lfMessageFont;
	wcscpy_s(secLog.lfFaceName, L"Segoe UI Semibold");
	secLog.lfWeight = FW_SEMIBOLD;
	fontSection = CreateFontIndirect(&secLog);
}

void shutdown() {
	if (brushBg) DeleteObject(brushBg);
	if (brushCard) DeleteObject(brushCard);
	if (brushPanel) DeleteObject(brushPanel);
	if (fontTitle) DeleteObject(fontTitle);
	if (fontSection) DeleteObject(fontSection);
	brushBg = brushCard = brushPanel = NULL;
	fontTitle = fontSection = NULL;
	if (gdiplusToken) GdiplusShutdown(gdiplusToken);
	gdiplusToken = 0;
}

bool isDarkMode() {
	return isDark;
}

void setDarkMode(bool dark) {
	isDark = dark;
	pal = isDark ? &DARK_PALETTE : &LIGHT_PALETTE;
	APP_SET_DATA(vThemeMode, isDark ? 0 : 1);
	rebuildBrushes();
	syncUxThemeMode();
}

void toggleDarkMode() {
	setDarkMode(!isDark);
}

void applyWindow(HWND hDlg) {
	if (pAllowDarkModeForWindow)
		pAllowDarkModeForWindow(hDlg, isDark);

	BOOL dark = isDark ? TRUE : FALSE;
	if (FAILED(DwmSetWindowAttribute(hDlg, DWMWA_DARK_MODE, &dark, sizeof(dark))))
		DwmSetWindowAttribute(hDlg, DWMWA_DARK_MODE_LEGACY, &dark, sizeof(dark));

	int corner = DWM_CORNER_ROUND;
	DwmSetWindowAttribute(hDlg, DWMWA_CORNER_PREFERENCE, &corner, sizeof(corner));

	COLORREF caption = pal->crBg;
	DwmSetWindowAttribute(hDlg, DWMWA_CAPTION, &caption, sizeof(caption));
	COLORREF border = pal->crDwmBorder;
	DwmSetWindowAttribute(hDlg, DWMWA_BORDER, &border, sizeof(border));
}

void styleControls(HWND hRoot) {
	EnumChildWindows(hRoot, styleChildProc, 0);
}

void refreshWindow(HWND hDlg) {
	if (!hDlg) return;
	applyWindow(hDlg);
	styleControls(hDlg);
	RedrawWindow(hDlg, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW);
}

bool handleMessage(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam, INT_PTR& result) {
	bool tabPage = isTabPage(hDlg);
	switch (msg) {
	case WM_CTLCOLORDLG:
		result = (INT_PTR)(tabPage ? brushCard : brushBg);
		return true;
	case WM_ERASEBKGND: {
		HDC dc = (HDC)wParam;
		RECT rc;
		GetClientRect(hDlg, &rc);
		FillRect(dc, &rc, tabPage ? brushCard : brushBg);

		float scale = dpiScale(hDlg);
		Graphics g(dc);
		g.SetSmoothingMode(SmoothingModeAntiAlias);
		if (tabPage) {
			// Subtle vertical divider between the two columns
			RECT mid = { 160, 8, 161, 122 };
			MapDialogRect(hDlg, &mid);
			Pen divPen(pal->cardBorder, 1.f);
			g.DrawLine(&divPen, (INT)mid.left, (INT)mid.top, (INT)mid.left, (INT)mid.bottom);
		} else if (GetDlgItem(hDlg, IDC_COMBO_INPUT_TYPE) != NULL) {
			// Main dialog: draw Quick Config card and Tab Content card
			drawCardRect(g, hDlg, 14, 42, 342, 89, scale);
			drawCardRect(g, hDlg, 14, 119, 342, 253, scale);
		} else if (GetDlgItem(hDlg, IDC_STATIC_APP_CREDIT) != NULL) {
			// About dialog: card around credit section
			drawCardRect(g, hDlg, 14, 60, 254, 102, scale);
		}
		result = TRUE;
		return true;
	}
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN: {
		HDC dc = (HDC)wParam;
		HWND hCtrl = (HWND)lParam;
		int id = GetDlgCtrlID(hCtrl);
		bool isSection = (id == IDC_STATIC_SECTION_LEFT || id == IDC_STATIC_SECTION_RIGHT);
		bool muted = (id == IDC_STATIC_SUBTITLE || id == IDC_STATIC_APP_SUB_TITLE ||
			id == IDC_STATIC_APP_VERSION || id == IDC_STATIC_APP_CREDIT);

		SetTextColor(dc, isSection ? pal->crAccent : (muted ? pal->crTextMuted : pal->crText));

		bool onCard = tabPage;
		if (!onCard && !isTabPage(hDlg)) {
			RECT rcCtrl;
			GetWindowRect(hCtrl, &rcCtrl);
			MapWindowPoints(HWND_DESKTOP, hDlg, (LPPOINT)&rcCtrl, 2);
			RECT card1 = { 14, 42, 342, 89 };
			MapDialogRect(hDlg, &card1);
			RECT cardAbout = { 14, 60, 254, 102 };
			MapDialogRect(hDlg, &cardAbout);
			if ((GetDlgItem(hDlg, IDC_COMBO_INPUT_TYPE) && rcCtrl.top >= card1.top && rcCtrl.bottom <= card1.bottom) ||
				(!GetDlgItem(hDlg, IDC_COMBO_INPUT_TYPE) && GetDlgItem(hDlg, IDC_STATIC_APP_CREDIT) && rcCtrl.top >= cardAbout.top && rcCtrl.bottom <= cardAbout.bottom)) {
				onCard = true;
			}
		}
		SetBkColor(dc, onCard ? pal->crCard : pal->crBg);
		result = (INT_PTR)(onCard ? brushCard : brushBg);
		return true;
	}
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX: {
		HDC dc = (HDC)wParam;
		SetTextColor(dc, pal->crText);
		SetBkColor(dc, pal->crPanel);
		result = (INT_PTR)brushPanel;
		return true;
	}
	}
	return false;
}

void setTabSelected(HWND hButton, bool selected) {
	if (selected)
		SetProp(hButton, PROP_TAB_SELECTED, (HANDLE)1);
	else
		RemoveProp(hButton, PROP_TAB_SELECTED);
	InvalidateRect(hButton, NULL, FALSE);
}

HFONT titleFont() {
	return fontTitle;
}

HFONT sectionFont() {
	return fontSection;
}

}
