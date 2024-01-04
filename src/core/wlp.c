#include "main.h"

#ifdef __WIN32
#include <windows.h>
#include <tchar.h>
#endif

#ifdef __WIN32
static HANDLE hWlpProcess = NULL;
#else
static FILE *wlpProcess = NULL;
static int   wlpPid     = 0;
#endif

void killWlp()
{
#ifdef __WIN32
    TerminateProcess(hWlpProcess, 0);
#else
  if (wlpPid != 0) kill(wlpPid, SIGINT);
  if (wlpProcess != NULL) pclose(wlpProcess);

  wlpPid     = 0;
  wlpProcess = NULL;
#endif
}

void runWlp()
{
#ifdef __WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    path[_tcslen(path)-4] = '\0';
    _tcscat(path, TEXT("wlp.exe"));
    printf("%s\n", path);

    if(CreateProcess(
        NULL,
        path,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi 
    ))
        hWlpProcess = pi.hProcess;
    else
        printf("Failed to start a wallpaper subprocess");

    
#else
    wlpProcess = popen("/home/cziken/Projects/lwp/build/src/wlp/lwpwlp", "r");
    char buff[10];
    fgets(buff, sizeof(buff) - 1, wlpProcess);
    wlpPid = atoi(buff);
#endif
}