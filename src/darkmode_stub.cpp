// darkmode_stub.cpp - Minimal stub implementation for DarkMode functionality
#include "../foobar2000_SDK/foobar2000/SDK/foobar2000.h"
#include "../foobar2000_SDK/libPPUI/DarkMode.h"
#include <windows.h>
#include <string>

namespace DarkMode {
    bool IsSupportedSystem() {
        // Check if we're on Windows 10 1809 or later
        // Use a simpler approach - just check if the registry key exists
        HKEY hKey;
        LONG result = RegOpenKeyExW(HKEY_CURRENT_USER,
                                   L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                                   0, KEY_READ, &hKey);
        if (result == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return true;
        }
        return false;
    }

    bool QueryUserOption() {
        DWORD v = 0;
        DWORD cb = sizeof(v);
        DWORD type = 0;
        if (RegGetValue(HKEY_CURRENT_USER, 
                       L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 
                       L"AppsUseLightTheme", 
                       RRF_RT_REG_DWORD, &type, &v, &cb) == 0) {
            if (type == REG_DWORD) {
                return v == 0; // 0 means dark mode
            }
        }
        return false;
    }

    void SetAppDarkMode(bool bDark) {
        // Stub implementation - does nothing
        (void)bDark;
    }

    void UpdateTitleBar(HWND wnd, bool bDark) {
        // Stub implementation - does nothing
        (void)wnd;
        (void)bDark;
    }

    void ApplyDarkThemeCtrl(HWND ctrl, bool bDark, const wchar_t* ThemeID) {
        if (!ctrl || !IsSupportedSystem()) return;
        
        if (bDark) {
            std::wstring darkTheme = L"DarkMode_";
            darkTheme += ThemeID;
            SetWindowTheme(ctrl, darkTheme.c_str(), NULL);
        } else {
            SetWindowTheme(ctrl, ThemeID, NULL);
        }
    }

    void ApplyDarkThemeCtrl2(HWND ctrl, bool bDark, const wchar_t* ThemeID_light, const wchar_t* ThemeID_dark) {
        // Stub implementation - does nothing
        (void)ctrl;
        (void)bDark;
        (void)ThemeID_light;
        (void)ThemeID_dark;
    }

    void AllowDarkModeForWindow(HWND wnd, bool bDark) {
        if (!wnd || !IsSupportedSystem()) return;
        
        // This is a simplified version - the real implementation uses undocumented APIs
        // For now, just apply the theme
        if (bDark) {
            SetWindowTheme(wnd, L"DarkMode_Explorer", NULL);
        } else {
            SetWindowTheme(wnd, L"Explorer", NULL);
        }
    }

    void DarkenEditLite(HWND ctrl) {
        // Stub implementation - does nothing
        (void)ctrl;
    }

    void DarkenComboLite(HWND ctrl) {
        // Stub implementation - does nothing
        (void)ctrl;
    }

    bool IsDialogDark(HWND dlg, UINT msgSend) {
        // Stub implementation - return false
        (void)dlg;
        (void)msgSend;
        return false;
    }

    bool IsDCDark(HDC dc) {
        // Stub implementation - return false
        (void)dc;
        return false;
    }

    bool IsThemeDark(COLORREF text, COLORREF background) {
        // Stub implementation - return false
        (void)text;
        (void)background;
        return false;
    }

    COLORREF GetSysColor(int idx, bool bDark) {
        // Just return the normal system color
        (void)bDark;
        return ::GetSysColor(idx);
    }

    LRESULT CustomDrawToolbar(NMHDR* hdr) {
        // Stub implementation
        (void)hdr;
        return CDRF_DODEFAULT;
    }

    LRESULT OnCustomDraw(int, NMHDR* hdr, BOOL& bHandled) {
        // Stub implementation
        (void)hdr;
        bHandled = FALSE;
        return 0;
    }

    void NCPaintDarkFrame(HWND ctrl, HRGN rgn) {
        // Stub implementation - does nothing
        (void)ctrl;
        (void)rgn;
    }

    bool IsHighContrast() {
        HIGHCONTRASTW highContrast = { sizeof(highContrast) };
        if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, FALSE))
            return (highContrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
        return false;
    }

    UINT msgSetDarkMode() {
        static UINT val = 0;
        if (val == 0) val = RegisterWindowMessage(L"libPPUI:msgSetDarkMode");
        return val;
    }

    // CHooks class implementation
    void CHooks::AddDialog(HWND wnd) { (void)wnd; }
    void CHooks::AddTabCtrl(HWND wnd) { (void)wnd; }
    void CHooks::AddComboBox(HWND wnd) { (void)wnd; }
    void CHooks::AddComboBoxEx(HWND wnd) { (void)wnd; }
    void CHooks::AddButton(HWND wnd) { (void)wnd; }
    void CHooks::AddEditBox(HWND wnd) { (void)wnd; }
    void CHooks::AddPopup(HWND wnd) { (void)wnd; }
    void CHooks::AddStatusBar(HWND wnd) { (void)wnd; }
    void CHooks::AddScrollBar(HWND wnd) { (void)wnd; }
    void CHooks::AddToolBar(HWND wnd, bool bExplorerTheme) { (void)wnd; (void)bExplorerTheme; }
    void CHooks::AddReBar(HWND wnd) { (void)wnd; }
    void CHooks::AddStatic(HWND wnd) { (void)wnd; }
    void CHooks::AddUpDown(HWND wnd) { (void)wnd; }
    void CHooks::AddListBox(HWND wnd) { (void)wnd; }
    void CHooks::AddListView(HWND wnd) { (void)wnd; }
    void CHooks::AddTreeView(HWND wnd) { (void)wnd; }
    void CHooks::AddPPListControl(HWND wnd) { (void)wnd; }
    void CHooks::AddGeneric(HWND wnd, const wchar_t* name) { (void)wnd; (void)name; }
    void CHooks::AddClassic(HWND wnd, const wchar_t* normalTheme) { (void)wnd; (void)normalTheme; }
    void CHooks::AddCtrlAuto(HWND wnd) { (void)wnd; }
    void CHooks::AddCtrlMsg(HWND wnd) { (void)wnd; }
    void CHooks::AddDialogWithControls(HWND wnd) {
        if (!wnd || !IsSupportedSystem()) return;
        
        // Apply dark mode to the dialog itself
        if (m_dark) {
            AllowDarkModeForWindow(wnd, true);
        }
        
        // Apply to all child controls
        AddControls(wnd);
    }
    void CHooks::AddControls(HWND wndParent) {
        if (!wndParent) return;
        
        // Enumerate all child windows and apply dark mode
        EnumChildWindows(wndParent, [](HWND hwndChild, LPARAM lParam) -> BOOL {
            CHooks* pThis = reinterpret_cast<CHooks*>(lParam);
            if (pThis && pThis->m_dark && IsSupportedSystem()) {
                wchar_t className[256];
                GetClassNameW(hwndChild, className, 256);
                
                // Apply dark theme to different control types
                if (wcscmp(className, L"Edit") == 0) {
                    ApplyDarkThemeCtrl(hwndChild, true, L"Explorer");
                } else if (wcscmp(className, L"Button") == 0) {
                    ApplyDarkThemeCtrl(hwndChild, true, L"Explorer");
                } else if (wcscmp(className, L"ComboBox") == 0) {
                    SetWindowTheme(hwndChild, L"DarkMode_CFD", NULL);
                } else if (wcscmp(className, L"Static") == 0) {
                    ApplyDarkThemeCtrl(hwndChild, true, L"Explorer");
                } else {
                    ApplyDarkThemeCtrl(hwndChild, true, L"Explorer");
                }
                
                AllowDarkModeForWindow(hwndChild, true);
            }
            return TRUE;
        }, reinterpret_cast<LPARAM>(this));
    }
    void CHooks::SetDark(bool v) { m_dark = v; }
    void CHooks::clear() {
        // Clear any stored state
        m_dark = false;
    }
    void CHooks::AddApp() { }
}