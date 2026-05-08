// dllmain.cpp : Defines the entry point for the DLL application.
#include <windows.h>
#include <dbghelp.h>
#include <cstdio>
#include "MinHook.h"
#include <fstream>
#include <d3d.h>
#include <dinput.h>

#include "d3drm.h"
#include "resource.h"

FILE* pConsole = nullptr, * pConsoleErr = nullptr;

std::ofstream logFile;

/// <summary>
/// Creates a console window using the current processes outputs
/// </summary>
/// <returns>Whether the creation of the console window was successful</returns>
static bool CreateConsole(void) {
    if (AllocConsole() != TRUE) return false;
    freopen_s(&pConsole, "CONOUT$", "w", stdout);
    freopen_s(&pConsoleErr, "CONOUT$", "w", stderr);

    SetConsoleTitleA("OpenLSR");

    // enable ANSI escape codes
    DWORD conMode;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &conMode);
    conMode = conMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), conMode);

    // disable cursor
    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curInfo.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);

    return true;
}

// Hooked functions

enum LogType {
    app_log,
    _3dman_log,
    tex_log,
    fatalerror_log,
    fileaccess_log,
    data_log,
    performance_log,
    WorldObject_log,
    FilterInclude_log,
    FilterExclude_log,
    IOTimer_log,
    IO1_log,
    IO2_log,
    IO3_log,
    IO4_log,
    Workshop_log,
    StartUp_log,
    ScriptParsing_log,
    Sound_log,
    RouteNode_log,
    Cache_log,
};

char** logName = (char**)0x0052edec;

void __cdecl debugLog(char* unused, LogType logID, char* format, ...) {
    if (!format) return;

    va_list args;
    va_start(args, format);

    char buf[1024];
    std::vsnprintf(buf, sizeof(buf), format, args);

    std::printf("\x1b[93m[D] [%s] %s\n\x1b[0m", logName[logID], buf);
    logFile << buf << std::endl;
}

void __cdecl engineDebugLog(void* _CEngineMan, int param_1, char* format, ...) {
    va_list args;
    va_start(args, format);

    char buf[1024];
    std::vsnprintf(buf, sizeof(buf), format, args);

    std::printf("\x1b[94m[G] [%x] %s\n\x1b[0m", param_1, buf);
    logFile << buf << std::endl;
}

void __cdecl errorLog(char* format, ...) {
    if (!format) return;

    va_list args;
    va_start(args, format);

    char buf[1024];
    std::vsnprintf(buf, sizeof(buf), format, args);

    std::printf("\x1b[31m[E] %s\n\x1b[0m", buf);
    logFile << buf << std::endl;
}

struct TypeDescriptor
{
    void* pVFTable;
    void* spare;
    char* name;
};

void __cdecl logWithRtti(void* manager, LogType logLevel, LPVOID param_3, char* format, ...) {
    if (!format) return;

    va_list args;
    va_start(args, format);

    char buf[1024];
    std::vsnprintf(buf, sizeof(buf), format, args);

    char* rtti = (char*)param_3 + 8;

    // -1 is extremely obnoxious
    //if (logLevel < 0) return;

    // bro i don't care about the FPS
    if (strstr(rtti, "PerformanceMonitor")) return;

    std::printf("\x1b[33m[R,%d] <%s> %s\n\x1b[0m", logLevel, rtti, buf);
    logFile << "<" << rtti << "> " << buf << std::endl;
}

typedef void(WINAPI* OutputDebugStringA_t)(LPCSTR);

OutputDebugStringA_t fpOutputDebugStringA = nullptr;

void WINAPI outputDebug(LPCSTR lpOutputString) {
    if (!lpOutputString) return;

    std::printf("\x1b[96m%s\x1b[0m", lpOutputString);
    logFile << lpOutputString << std::endl;
}


struct CCommandLineParser_t {
    void** vtable;
    bool files; // try to load files from remote server
    bool debug_info; // various debug things
    bool field3_0x6;
    bool field4_0x7;
    bool field5_0x8;
    bool log_FPS_debug; // logs framerate into CEngineMan logger [NO FLAG]
    bool windowed; // windowed mode, broken
    bool field8_0xb;
    bool field9_0xc;
    bool hardware_3D_sound_disabled; // force disables hardware 3D sound support in QMDX [NO FLAG]
    bool field11_0xe;
    bool field12_0xf;
    int field13_0x10;
    bool next_node_debug; // highlight next node with sparkles [NO FLAG]
    bool freeform; // allow full car control
    bool field16_0x16; // shows lap camera and any key causes race to end??? might be setting race state [NO FLAG]
    bool field17_0x17;
    bool field18_0x18;
    bool field19_0x19; // sets some flag in the AI drivers [NO FLAG]
    bool restart; // the game was restarted from the options menu
    bool field21_0x1b; // tries to update a viewport instead of using device flip [NO FLAG]
    int field22_0x1c;
    bool field23_0x20; // something to do with a max plastic help sound??? [NO FLAG]
    bool ig_test; // more debug stuff
    bool sound_disabled; // sound disabled [NO FLAG]
    bool field26_0x23;
    bool field27_0x24;
    bool field28_0x25; // all cars get frozen in place, idk [NO FLAG]
    bool field29_0x26; // disables vehicle collisions [NO FLAG]
    bool ai_disabled; // disable AI drivers [NO FLAG]
    bool preload_mvd; // preload MVD videos [NO FLAG]
    bool field32_0x29; // cars go really fast? [NO FLAG]
    bool field33_0x2a; // does something with car object [NO FLAG]
    bool res2; // something to do with resources
    bool xaf2xbf;
    bool field36_0x2d;
    bool field37_0x2e;
    bool field38_0x2f;
    bool field39_0x30; // does something to AI logic [NO FLAG]
    bool field40_0x31;
    bool printer_disabled; // printer disabled [NO FLAG]
    bool load_text;
    bool field43_0x34;
    bool field44_0x35; // something related to the performance monitor [NO FLAG]
    bool res1; // something to do with resources
    bool no_intro_video; // skip intro videos
    bool cd_in;
    bool from_launcher;
    bool all_ai;
    bool field50_0x3b;
};

typedef void(__thiscall* Parse_t)(CCommandLineParser_t*, void*, void*);

Parse_t fpParse = nullptr;

CCommandLineParser_t* cmdArgs = nullptr;

void __fastcall parseHook(CCommandLineParser_t* parser, void* _EBX, void* param_1, void* param_2) {
    // weird string structure thing
    char* cmd = *(char**)((int)(&param_2) + 0x4);

    if ((int)cmd == 0)
        cmd == ((char*(*)(void))0x0040d4c0)();

    std::printf("using new command line parser...\ncmd: %s\n", cmd);

    // parse original command line options
    if(strstr(cmd, "/FILES") != NULL) {
        parser->files = true;
    }
    if(strstr(cmd, "/DEBUGINFO") != NULL) {
        parser->debug_info = true;
    }
    if(strstr(cmd, "/WINDOWED") != NULL) {
        parser->windowed = true;
    }
    *((bool*)0x0053b984) = true;
    if(strstr(cmd, "/FREEFORM") != NULL) {
        parser->freeform = true;
    }
    if(strstr(cmd, "RESTART") != NULL) {
        parser->restart = true;
    }
    if(strstr(cmd, "/LOAD_TEXT") != NULL) {
        parser->load_text = true;
    }
    if(strstr(cmd, "/RES_FILES") != NULL) {
        parser->res1 = true;
        parser->res2 = true;
    }
    if(strstr(cmd, "/IGTEST") != NULL) {
        parser->ig_test = true;
    }
    if(strstr(cmd, "/XAFTOXBF") != NULL) {
        parser->xaf2xbf = true;
    }
    if(strstr(cmd, "/NORES") != NULL) {
        parser->res1 = false;
        parser->res2 = false;
    }
    if(strstr(cmd, "/NOINTROVIDEO") != NULL) {
        parser->no_intro_video = true;
    }
    if(strstr(cmd, "/CDIN") != NULL) {
        parser->cd_in = false;
    }
    if(strstr(cmd, "/FROMLAUNCHER") != NULL) {
        parser->from_launcher = true;
    }
    if(strstr(cmd, "/ALLAI") != NULL) {
        parser->all_ai = true;
    }
    
    parser->field36_0x2d = true;

    // new options

    // OpenLSR always on options
    parser->from_launcher = true;
    parser->printer_disabled = true;

    if (strstr(cmd, "/NODEDEBUG") != NULL) {
        parser->next_node_debug = true;
    }
    if (strstr(cmd, "/") != NULL) {
        parser->cd_in = false;
    }

    //parser->no_intro_video = true;
    //parser->igTest = true;
    //parser->debug_info = true;

    //parser->freeform = true;
    
    // new stuff
    //parser->next_node_debug = true;
    //parser->lapCameraDebug = true;
    //parser->logFPSDebug = true;
    //parser->hardware3DSoundDisabled = true;

    // unknown
    //parser->lapCameraDebug = true;
    //parser->field28_0x25 = true;
    //parser->field29_0x26 = true;
    //parser->field44_0x35 = true;
    //parser->field5_0x8 = true;

    //parser->windowed = true;

    cmdArgs = parser;
}

typedef HWND(WINAPI* CreateWindowExA_t)(
    DWORD, LPCSTR, LPCSTR, DWORD,
    int, int, int, int,
    HWND, HMENU, HINSTANCE, LPVOID
);

CreateWindowExA_t fpCreateWindowExA = nullptr;

HWND WINAPI createWindow(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    DWORD myStyle = WS_TILEDWINDOW;

    // TODO: this is more or less Win10 specific, check other OS (Win11)
    int extraHeight = (GetSystemMetrics(SM_CYFRAME) * 2) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER);
    int extraWidth = (GetSystemMetrics(SM_CXFRAME) * 2);

    HWND win = fpCreateWindowExA(dwExStyle, lpClassName, lpWindowName, myStyle, X, Y, nWidth + extraWidth, nHeight + extraHeight, hWndParent, hMenu, hInstance, lpParam);
    
    // TODO: this doesn't work???
    HICON icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OPENLSR));
    SendMessage(win, WM_SETICON, ICON_BIG, (LPARAM)icon);
    SendMessage(win, WM_SETICON, ICON_SMALL, (LPARAM)icon);
    
    return win;
}

// redirect registry writes from HKLM to HKCU and to an OpenLSR registry key (for now)
void regPatch() {
    const char* regKey = "Software\\OpenLSR Team\\OpenLSR";

    int* ptr = reinterpret_cast<int*>(0x004155c6);

    DWORD oldProtect;

    if (VirtualProtect(ptr, sizeof(int), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        *ptr = 0x80000001;

        VirtualProtect(ptr, sizeof(int), oldProtect, &oldProtect);
    }

    ptr = reinterpret_cast<int*>(0x004157c8);

    if (VirtualProtect(ptr, sizeof(int), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        *ptr = 0x80000001;

        VirtualProtect(ptr, sizeof(int), oldProtect, &oldProtect);
    }

    char** ptr2 = reinterpret_cast<char**>(0x004155d7);

    if (VirtualProtect(ptr2, sizeof(char*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        *ptr2 = (char*)regKey;

        VirtualProtect(ptr2, sizeof(char*), oldProtect, &oldProtect);
    }

    ptr2 = reinterpret_cast<char**>(0x004157d9);

    if (VirtualProtect(ptr2, sizeof(char*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        *ptr2 = (char*)regKey;

        VirtualProtect(ptr2, sizeof(char*), oldProtect, &oldProtect);
    }

    ptr2 = reinterpret_cast<char**>(0x004158cd);

    if (VirtualProtect(ptr2, sizeof(char*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        *ptr2 = (char*)regKey;

        VirtualProtect(ptr2, sizeof(char*), oldProtect, &oldProtect);
    }
}

// replace the font used in the game with another system font
void fontPatch(const char* font) {
    char** ptr = reinterpret_cast<char**>(0x004161a9);

    DWORD oldProtect;

    if (VirtualProtect(ptr, sizeof(char*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        *ptr = (char*)font;

        VirtualProtect(ptr, sizeof(char*), oldProtect, &oldProtect);
    }

    ptr = reinterpret_cast<char**>(0x004161e3);

    if (VirtualProtect(ptr, sizeof(char*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        *ptr = (char*)font;

        VirtualProtect(ptr, sizeof(char*), oldProtect, &oldProtect);
    }

    ptr = reinterpret_cast<char**>(0x0041621d);

    if (VirtualProtect(ptr, sizeof(char*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        *ptr = (char*)font;

        VirtualProtect(ptr, sizeof(char*), oldProtect, &oldProtect);
    }

    ptr = reinterpret_cast<char**>(0x00416257);

    if (VirtualProtect(ptr, sizeof(char*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        *ptr = (char*)font;

        VirtualProtect(ptr, sizeof(char*), oldProtect, &oldProtect);
    }
}

// fix switching to region champion car in completed save runs
// this is NOT how you're supposed to be doing an assembly patch.
void carSwitchPatch() {
    byte* ptr = reinterpret_cast<byte*>(0x004270bd);
    byte newBytes[6] = {0xe9, 0xc1, 0x00, 0x00, 0x00, 0x90};

    DWORD oldProtect;

    if (VirtualProtect(ptr, 6, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        memcpy(ptr, newBytes, 6);
        VirtualProtect(ptr, 6, oldProtect, &oldProtect);
    }
}

typedef BOOL(WINAPI* ControllerCallback_t)(LPCDIDEVICEINSTANCEA, void*);

ControllerCallback_t fpControllerCallback = nullptr;

BOOL WINAPI controllerCallback(LPCDIDEVICEINSTANCEA lpddi, void* param_2) {
    // filter out "DEVICE" type devices, we don't care about those.
    if ((lpddi->dwDevType & 7) == 1) {
        //std::printf("unsupported device %s, skipping...\n", lpddi->tszInstanceName);
        return 1;
    }
    else {
        std::printf("device: %s, type: %X\n", lpddi->tszInstanceName, lpddi->dwDevType);
    }

    return fpControllerCallback(lpddi, param_2);
}

typedef int(WINAPI* CheckDirectX_t)(void);

CheckDirectX_t fpCheckDirectX = nullptr;

int WINAPI checkDirectX() {
    return 0;
}

#define AddHook(name, target, detour, original) \
if(MH_CreateHook(target, detour, original) != MH_OK) {\
    std::printf("failed to create hook for %s", name);\
    logFile << "failed to create hook for " << name << std::endl;\
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        logFile.open("debug_log.txt", std::ios::out);

        if (MH_Initialize() != MH_OK) return FALSE;

        if (!CreateConsole()) return FALSE;

        std::printf("hooking, please wait!\r");

        if (!logFile.is_open()) {
            std::printf("failed to open debug_log.txt");
            return FALSE;
        }

        AddHook("check directx", (LPVOID)0x00411d12, &checkDirectX, reinterpret_cast<LPVOID*>(&fpCheckDirectX));

        AddHook("command line parse hook", (LPVOID)0x00435a3e, &parseHook, reinterpret_cast<LPVOID*>(&fpParse));

        #pragma region Logging

        AddHook("debug log", (LPVOID)0x00453c8c, &debugLog, NULL);
        
        // very noisy
        //AddHook("engine debug log", (LPVOID)0x00451261, &engineDebugLog, NULL);
        
        /*AddHook("error log", (LPVOID)0x004d9580, &errorLog, NULL);
        AddHook("error log 2", (LPVOID)0x004d9590, &errorLog, NULL);
        AddHook("error log 3", (LPVOID)0x004d95a0, &errorLog, NULL);*/

        AddHook("error log with rtti", (LPVOID)0x00453aa5, &logWithRtti, NULL);

        AddHook("output debug string",
            &OutputDebugStringA, &outputDebug, reinterpret_cast<LPVOID*>(&fpOutputDebugStringA));

        #pragma endregion

        #pragma region Window

        AddHook("create window",
            &CreateWindowExA, &createWindow, reinterpret_cast<LPVOID*>(&fpCreateWindowExA));

        #pragma endregion

        #pragma region D3DRM Replacements

        AddHook("d3drm matrix from quat", (LPVOID)0x005108d0, &D3DRMMatrixFromQuaternion, NULL);
        AddHook("d3drm quat slerp", (LPVOID)0x005108d6, &D3DRMQuaternionSlerp, NULL);

        #pragma endregion

        #pragma region Fixes

        AddHook("ignore invalid IO devices", (LPVOID)0x00465cdb, &controllerCallback, reinterpret_cast<LPVOID*>(&fpControllerCallback));

        #pragma endregion

        #pragma region Experiments

        regPatch();
        
        // this is a reminder that i have it injected
        fontPatch("Comic Sans MS");

        carSwitchPatch();

        #pragma endregion

        MH_EnableHook(MH_ALL_HOOKS);
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        logFile.close();

        if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK) return FALSE;
        if (MH_Uninitialize() != MH_OK) return FALSE;
    }

    return TRUE;
}