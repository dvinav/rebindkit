#include "minhook.h"
#include <cwchar>
#include <string>
#include <windows.h>

HMODULE hRenderKit;

void Hook(int target, HMODULE *hModule, const char *funcName) {
  void *detour = GetProcAddress(*hModule, funcName);
  if (MH_CreateHook(reinterpret_cast<void *>(target), detour, nullptr) !=
      MH_OK) {
    MessageBoxA(NULL, "Failed to create hook", "Error", MB_OK | MB_ICONERROR);
    return;
  }
}

std::wstring GetExePath() {
  wchar_t path[MAX_PATH];
  GetModuleFileNameW(NULL, path, MAX_PATH);
  std::wstring fullPath(path);
  size_t pos = fullPath.find_last_of(L"\\/");
  if (pos != std::wstring::npos)
    return fullPath.substr(0, pos);
  return L"";
}

HMODULE LoadLib(const wchar_t *dllName) {
  std::wstring fullPath = GetExePath() + L"\\kits\\" + dllName + L".dll";
  HMODULE hModule = LoadLibraryW(fullPath.c_str());
  if (!hModule) {
    MessageBoxW(NULL, (L"Failed to load " + fullPath).c_str(), L"Error",
                MB_OK | MB_ICONERROR);
  }
  return hModule;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {

    MH_Initialize();

    hRenderKit = LoadLib(L"RenderKit");
    Hook(0x53E170, &hRenderKit, "RenderEffects");
    Hook(0x714650, &hRenderKit, "RenderSky");
    Hook(0x53DF40, &hRenderKit, "RenderScene");

    MH_EnableHook(MH_ALL_HOOKS);
  }
  return TRUE;
}
