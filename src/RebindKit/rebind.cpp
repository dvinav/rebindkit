#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason, LPVOID lpReserved) {
  if (ul_reason == DLL_PROCESS_ATTACH) {
    MessageBoxA(0, "RebindKit loaded!", "Debug", MB_OK);
  }
  return TRUE;
}
