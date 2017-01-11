// QuickNoteDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <windowsx.h>
#include "resource.h"
#define EXPORT __declspec(dllexport)

HHOOK hHook = NULL;
HINSTANCE hinstLib;
//INT_PTR CALLBACK Dialog1Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0)
		return CallNextHookEx(hHook, nCode, wParam, lParam);
	if (GetKeyState(VK_LWIN) && GetKeyState(VK_SPACE))
	{
			SendMessage(HWND_BROADCAST, WM_USER - 1, 0, 0);
			return TRUE;			
	}
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}


EXPORT void _doInstallHook(HWND hWnd) {
	if (hHook != NULL) return;
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, hinstLib, 0);
}

EXPORT void _doRemoveHook(HWND hWnd) {
	if (hHook == NULL) return;
	UnhookWindowsHookEx(hHook);
	hHook = NULL;
}