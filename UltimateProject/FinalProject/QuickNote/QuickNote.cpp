#include "stdafx.h"
#include "QuickNote.h"
#include <shellapi.h>
#include <iostream>
#include <comdef.h>
using namespace std;
#define MAX_LOADSTRING 100
#define	WM_USER_SHELLICON WM_USER + 1
#define WM_USER_HOOKING WM_USER - 1
#define MAX_BUF_LENGTH 512

// Global Variables:
void doInstallHook(HWND hWnd);
void doRemoveHook(HWND hWnd);
BOOL bDisable = FALSE;
//directory hold all note
PTSTR pszCurDir;
HMENU hPopMenu;
LRESULT CALLBACK Dialog1_WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Dialog2_WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
NOTIFYICONDATA nidApp = {

};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_QUICKNOTE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QUICKNOTE));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QUICKNOTE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_QUICKNOTE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable
	HICON hMainIcon;
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	doInstallHook(hWnd);

	if (!hWnd)
	{
		return FALSE;
	}

	hMainIcon = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_ICON1));

	nidApp.uVersion = NOTIFYICON_VERSION_4;
	nidApp.cbSize = sizeof(NOTIFYICONDATA);
	nidApp.hWnd = (HWND)hWnd;
	nidApp.uID = IDI_ICON1;
	nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nidApp.hIcon = hMainIcon;
	nidApp.uCallbackMessage = WM_USER_SHELLICON;
	LoadString(hInstance, NULL, nidApp.szTip, MAX_LOADSTRING);
	Shell_NotifyIcon(NIM_ADD, &nidApp);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmEvent;
	POINT lpClickPoint;
	switch (message)
	{
	case WM_USER_HOOKING: {
		DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Dialog2_WndProc);
	}

	case WM_USER_SHELLICON:
		//systray msg callback
		switch (LOWORD(lParam))
		{
		case WM_RBUTTONDOWN:
			UINT uFlag = MF_BYPOSITION | MF_STRING;
			GetCursorPos(&lpClickPoint);
			hPopMenu = CreatePopupMenu();
			if (bDisable == TRUE)
			{
				uFlag |= MF_GRAYED;
			}
			InsertMenu(hPopMenu, 0xFFFFFFFF, uFlag, ID_QUICKNOTEMENU_VIEWNOTE, _T("Test 2")); // Test 2
			InsertMenu(hPopMenu, 0xFFFFFFFF, uFlag, ID_QUICKNOTEMENU_VIEWSTATISTICS, _T("Test 1")); // Test 1				
			InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_EXIT, _T("Exit"));

			SetForegroundWindow(hWnd);
			TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, hWnd, NULL);
			return TRUE;

		}
		break;

	case WM_COMMAND:
	{
		wmEvent = HIWORD(wParam);
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_QUICKNOTEMENU_VIEWNOTE:
			//MessageBox(NULL, _T("This is a test for menu view note"), _T("Test 1"), MB_OK);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), 0, Dialog1_WndProc);
			break;
		case ID_QUICKNOTEMENU_VIEWSTATISTICS:
			MessageBox(NULL, _T("This is a test for menu statistics"), _T("Test 2"), MB_OK);
			break;
		case IDM_EXIT:
			Shell_NotifyIcon(NIM_DELETE, &nidApp);
			doRemoveHook(hWnd);
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		doRemoveHook(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK Dialog1_WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {

	TCHAR achBuffer[MAX_PATH];
	switch (message)
	{
	case WM_INITDIALOG:
		pszCurDir = achBuffer;
		GetCurrentDirectory(MAX_PATH, pszCurDir);
		wcscat(pszCurDir, L"NoteStored");
		CreateDirectory(pszCurDir, 0);
		wcscat(pszCurDir, L"\\");
		DlgDirList(hDlg, achBuffer, IDC_LIST1, 0, DDL_ARCHIVE | DDL_DIRECTORY);
		return FALSE;
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDC_BUTTON1: {
			EndDialog(hDlg, TRUE);
		}
						  break;
		}
	}
					 break;
	case WM_DESTROY: {
		EndDialog(hDlg, TRUE);
		return TRUE;
	}
					 break;
	}
	return FALSE;
}

void doInstallHook(HWND hWnd) {
	typedef void(*MYPROC)(HWND);
	HINSTANCE hinstLib;
	MYPROC ProcAddr;

	hinstLib = LoadLibrary(L"QuickNoteDLL.dll");
	if (hinstLib != NULL) {
		ProcAddr = (MYPROC)GetProcAddress(hinstLib, "_doInstallHook");
		if (ProcAddr != NULL)
			ProcAddr(hWnd);
	}
}

void doRemoveHook(HWND hWnd) {
	typedef void(*MYPROC)(HWND);
	HINSTANCE hinstLib;
	MYPROC ProcAddr;

	hinstLib = LoadLibrary(L"QuickNoteDLL.dll");
	if (hinstLib != NULL) {
		ProcAddr = (MYPROC)GetProcAddress(hinstLib, "_doRemoveHook");
		if (ProcAddr != NULL)
			ProcAddr(hWnd);
	}
}

void WriteFile(WCHAR* NoteBuf, WCHAR* TagBuf) {
	TCHAR achBuffer[MAX_PATH];
	pszCurDir = achBuffer;
	GetCurrentDirectory(MAX_PATH, pszCurDir);
	wcscat(pszCurDir, L"NoteStored");
	CreateDirectory(pszCurDir, 0);
	WCHAR* buf = new WCHAR[250];
	wsprintf(buf, L"\\%s.txt", TagBuf);
	wcscat(pszCurDir, buf);
	CreateFile(pszCurDir, GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	_bstr_t temp;
	char* fileName = temp;
	FILE* f;
	f = fopen(fileName, "a+");
	fwprintf(f, L"%s", NoteBuf);
	fclose(f);
}

INT_PTR CALLBACK Dialog2_WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	WCHAR* NoteBuf = new WCHAR[MAX_BUF_LENGTH];
	WCHAR* TagBuf = new WCHAR[MAX_BUF_LENGTH];
	FILE* f;
	char* fileName;

	TCHAR achBuffer[MAX_PATH];
	switch (message)
	{
	case WM_INITDIALOG: {
		return TRUE;
	}
						break;
	case WM_COMMAND: {
		switch (LOWORD(wParam))
		{
		case IDOK: {
			GetDlgItemText(hDlg, IDC_EDIT1, NoteBuf, MAX_BUF_LENGTH);
			GetDlgItemText(hDlg, IDC_EDIT2, TagBuf, MAX_BUF_LENGTH);

			//write file
			pszCurDir = achBuffer;
			GetCurrentDirectory(MAX_PATH, pszCurDir);
			wcscat(pszCurDir, L"NoteStored");
			CreateDirectory(pszCurDir, 0);

			wcscat(TagBuf, L".txt");
			wcscat(pszCurDir, TagBuf);
			CreateFile(pszCurDir, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			_bstr_t temp(pszCurDir);
			fileName = temp;

			f = fopen(fileName, "a+");
			fwprintf(f, L"%s", NoteBuf);
			fclose(f);

			EndDialog(hDlg, TRUE);
			if (NoteBuf != NULL)
				delete[] NoteBuf;
			if (TagBuf != NULL)
				delete[] TagBuf;
		}
				   break;
		case IDCANCEL:
			EndDialog(hDlg, TRUE);
			break;
		}
	}
					 break;
	default:
		return FALSE;
	}
}
