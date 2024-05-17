#include "main.h"

void getTargetPoint(App* app, Point* p)
{
  if (app->config.wndTargetPoint)
  {
#ifdef __WIN32
    HWND focusedWnd = GetForegroundWindow();

    if (focusedWnd)
    {
      RECT rect;
      GetWindowRect(focusedWnd, &rect);

      p->x = rect.left + (rect.right - rect.left) / 2;
      p->y = rect.top + (rect.bottom - rect.top) / 2;
    }
#else
    Window wnd;
    int    revertTo;
    XGetInputFocus(app->display, &wnd, &revertTo);

    int          x, y;
    unsigned int w, h, bw, bh;
    Window       rootWnd;
    XGetGeometry(app->display, wnd, &rootWnd, &x, &y, &w, &h, &bw, &bh);

    p->x = x;
    p->y = y;
#endif
  }
  else
  {
#ifdef __WIN32
    POINT mPos;
    GetCursorPos(&mPos);
    p->x = mPos.x - GetSystemMetrics(SM_XVIRTUALSCREEN);
    p->y = mPos.y - GetSystemMetrics(SM_YVIRTUALSCREEN);
#else
    int x, y;
    SDL_GetGlobalMouseState(&x, &y);
    p->x = x;
    p->y = y;
#endif
  }
}
