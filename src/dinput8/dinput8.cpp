#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <string>

#pragma comment(linker, "/EXPORT:DirectInput8Create=_DirectInput8Create@20")
#pragma comment(linker, "/EXPORT:DllCanUnloadNow=_DllCanUnloadNow@0")
#pragma comment(linker, "/EXPORT:DllGetClassObject=_DllGetClassObject@12")
#pragma comment(linker, "/EXPORT:DllRegisterServer=_DllRegisterServer@0")
#pragma comment(linker, "/EXPORT:DllUnregisterServer=_DllUnregisterServer@0")
#pragma comment(linker, "/EXPORT:GetdfDIJoystick=_GetdfDIJoystick@0")

HMODULE hRebind = nullptr;
const wchar_t *kRebindOriginal = L"kits\\RebindKit.dll";

static HMODULE original = nullptr;
static std::wstring GetSystemPathDll(const wchar_t *name) {
  wchar_t sysDir[MAX_PATH];
  GetSystemDirectoryW(sysDir, MAX_PATH);
  return std::wstring(sysDir) + L"\\" + name;
}

void LoadOriginal() {
  if (!original) {
    auto path = GetSystemPathDll(L"dinput8.dll");
    original = LoadLibraryW(path.c_str());
  }
}

FARPROC LoadOriginalFunc(LPCSTR name) {
  LoadOriginal();
  return GetProcAddress(original, name);
}

extern "C" {

HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion,
                                  REFIID riidltf, LPVOID *ppvOut,
                                  LPUNKNOWN punkOuter) {
  using fn = HRESULT(WINAPI *)(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);
  static fn orig = nullptr;
  if (!orig) {
    orig = (fn)GetProcAddress(original, "DirectInput8Create");
    if (!orig) {
      MessageBoxA(NULL, "Failed to find DirectInput8Create", "Error",
                  MB_OK | MB_ICONERROR);
      ExitProcess(1);
    }
  }
  return orig(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}

STDAPI DllCanUnloadNow(void) {
  using fn = HRESULT(WINAPI *)(void);
  static fn orig = (fn)LoadOriginalFunc("DllCanUnloadNow");
  return orig();
}

STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid,
                         _Outptr_ LPVOID FAR *ppv) {
  using fn = HRESULT(WINAPI *)(REFCLSID, REFIID, LPVOID *);
  static fn orig = (fn)LoadOriginalFunc("DllGetClassObject");
  return orig(rclsid, riid, ppv);
}

HRESULT WINAPI DllRegisterServer() {
  using fn = HRESULT(WINAPI *)();
  static fn orig = (fn)LoadOriginalFunc("DllRegisterServer");
  return orig();
}

HRESULT WINAPI DllUnregisterServer() {
  using fn = HRESULT(WINAPI *)();
  static fn orig = (fn)LoadOriginalFunc("DllUnregisterServer");
  return orig();
}

HRESULT WINAPI GetdfDIJoystick() {
  using fn = HRESULT(WINAPI *)();
  static fn orig = (fn)LoadOriginalFunc("GetdfDIJoystick");
  return orig();
}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(hModule);
    std::srand(static_cast<unsigned int>(time(nullptr)));

    // Load original dinput8.dll
    wchar_t systemPath[MAX_PATH];
    GetSystemDirectoryW(systemPath, MAX_PATH);
    std::wstring path = std::wstring(systemPath) + L"\\dinput8.dll";
    original = LoadLibraryW(path.c_str());
    if (!original) {
      MessageBoxA(NULL, "Failed to load dinput8.dll", "Error",
                  MB_OK | MB_ICONERROR);
      ExitProcess(1);
    }

    hRebind = LoadLibraryW(L"kits\\RebindKit.dll");
    if (!hRebind) {
      MessageBoxA(NULL, "Failed to load RebindKit.dll", "Error",
                  MB_OK | MB_ICONERROR);
    }
  }
  return TRUE;
}
