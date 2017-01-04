// QuickNoteDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <windowsx.h>
#include "resource.h"
#define EXPORT __declspec(dllexport)

HHOOK hHook = NULL;
HINSTANCE hinstLib;
INT_PTR CALLBACK Dialog1Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0)
		return CallNextHookEx(hHook, nCode, wParam, lParam);
	if (wParam == WM_KEYDOWN)
	{
		KBDLLHOOKSTRUCT *mHookData = (KBDLLHOOKSTRUCT *)lParam;
		if (mHookData->vkCode == VK_SPACE && GetAsyncKeyState(VK_LWIN)) {
			//MessageBox(0, L"hello", 0, 0);
			DialogBox(hinstLib, MAKEINTRESOURCE(IDD_DIALOG1), 0, Dialog1Proc);
			return TRUE;			
		}
	}
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

INT_PTR CALLBACK Dialog1Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND: {
		
	}
					 break;
	default:
		return FALSE;

	}
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