#ifdef __WIN32

#include "main.h"

#include <shellapi.h>
#include <tchar.h>
#include <windows.h>


#define WM_TRAY_ICON (WM_USER + 1)

static NOTIFYICONDATA nid;

void removeTrayIcon() { Shell_NotifyIcon(NIM_DELETE, &nid); }

int updateTrayIcon() {
  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 1;
}

static LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                LPARAM lParam) {
  switch (uMsg) {
  case WM_TRAY_ICON:
    if (lParam == WM_RBUTTONDOWN || lParam == WM_LBUTTONDOWN) {
      gtk_widget_set_visible(mainWnd, 1);
    }
    break;
  }
}

void initTrayIcon() {
  // Create an invisible window to process tray icon events

  HINSTANCE hInstance = GetModuleHandle(NULL);
  const TCHAR CLASS_NAME[] = TEXT("Hidden Window");
  WNDCLASS wc;
  memset(&wc, 0, sizeof(WNDCLASS));
  wc.lpfnWndProc = wndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;
  RegisterClass(&wc);
  HWND hWnd = CreateWindowEx(0, CLASS_NAME, TEXT(""), WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

  // Create tray icon

  nid.cbSize = sizeof(NOTIFYICONDATA);
  nid.hWnd = hWnd;
  nid.uCallbackMessage = WM_TRAY_ICON;
  nid.hIcon = LoadIcon(hInstance, "ID");
  nid.uFlags = NIF_ICON | NIF_MESSAGE;

  Shell_NotifyIcon(NIM_ADD, &nid);
}

#endif