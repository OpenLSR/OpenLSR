// injector.cpp - adapted from https://github.com/trigger-segfault/OpenLRR/ injector

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <windows.h>
#include <string>

#define EXE_NAME _T("_msr.exe")

#define PROCESS_EIP 0x4FFCE8

#define WAIT_TIME_MS 500

#define nameof_(symbol) #symbol
#define nameof(symbol) nameof_(symbol)

#define tstrsizeof(s) ((_tcslen((s)) + 1) * sizeof(TCHAR))

using tstring = std::basic_string<TCHAR>;

/*++
Source:   https://blogs.msdn.microsoft.com/twistylittlepassagesallalike/2011/04/23/everyone-quotes-command-line-arguments-the-wrong-way/
Archived: https://web.archive.org/web/20180811234026/https://blogs.msdn.microsoft.com/twistylittlepassagesallalike/2011/04/23/everyone-quotes-command-line-arguments-the-wrong-way/
Routine Description:
	This routine appends the given argument to a command line such
	that CommandLineToArgvW will return the argument string unchanged.
	Arguments in a command line should be separated by spaces; this
	function does not add these spaces.
Arguments:
	Argument - Supplies the argument to encode.
	CommandLine - Supplies the command line to which we append the encoded argument string.
	Force - Supplies an indication of whether we should quote
			the argument even if it does not contain any characters that would
			ordinarily require quoting.
--*/
static void ArgvQuote(const tstring& Argument, IN OUT tstring& CommandLine, bool Force = false) {
	// Unless we're told otherwise, don't quote unless we actually
	// need to do so --- hopefully avoid problems if programs won't
	// parse quotes properly

	// Separate arguments with a space.
	if (!CommandLine.empty())
		CommandLine.append(_T(" "));

	if (!Force && !Argument.empty() && Argument.find_first_of(_T(" \t\n\v\"")) == Argument.npos) {
		CommandLine.append(Argument);
	}
	else {
		CommandLine.push_back(_T('"'));

		for (auto It = Argument.begin(); ; ++It) {
			unsigned NumberBackslashes = 0;

			while (It != Argument.end() && *It == _T('\\')) {
				++It;
				++NumberBackslashes;
			}

			if (It == Argument.end()) {
				// Escape all backslashes, but let the terminating
				// double quotation mark we add below be interpreted
				// as a metacharacter.

				CommandLine.append(NumberBackslashes * 2, _T('\\'));
				break;
			}
			else if (*It == _T('"')) {
				// Escape all backslashes and the following
				// double quotation mark.

				CommandLine.append(NumberBackslashes * 2 + 1, _T('\\'));
				CommandLine.push_back(*It);
			}
			else {
				// Backslashes aren't special here.

				CommandLine.append(NumberBackslashes, _T('\\'));
				CommandLine.push_back(*It);
			}
		}

		CommandLine.push_back(_T('"'));
	}
}

int BuildFullPaths(OUT tstring& FullPath, OUT tstring& WorkingDir)
{
    TCHAR cwdBuff[1024] = { 0 };

    //_tcscpy(currentDir, LRR_DIR);
    if (!_tgetcwd(cwdBuff, _countof(cwdBuff))) {
        _tprintf(_T("getcwd failed\n"));
        return -1;
    }

    // Strip unexpected trailing slashes.
    size_t cwdlen = _tcslen(cwdBuff);
    if (cwdlen > 0 && (cwdBuff[cwdlen - 1] == _T('\\') || cwdBuff[cwdlen - 1] == _T('/'))) {
        cwdBuff[cwdlen - 1] = _T('\0');
    }

    WorkingDir.assign(cwdBuff);
    FullPath.assign(WorkingDir).append(_T("\\")).append(EXE_NAME);

    return 0;
}

int BuildCommandLine(int argc, TCHAR* argv[], const tstring& FullPath, OUT tstring& CommandLine)
{
    CommandLine.clear();

    // Add all command line arguments, and escape/quote them if needed.
    ArgvQuote(FullPath, CommandLine, false);
    for (int i = 1; i < argc; i++) {
        ArgvQuote(argv[i], CommandLine, false);
    }

    return 0;
}

int ConfirmEntryPoint(const tstring& FullPath, DWORD EntryPoint)
{
	IMAGE_DOS_HEADER dos;
	IMAGE_NT_HEADERS32 nt32;
	FILE* file;

	if (!(file = _tfopen(FullPath.c_str(), _T("rb")))) {
		_tprintf(_T("EXE fopen failed\n"));
		return -1; // return here, we don't need to close the file
	}

	int result = 0;
	if (!fread(&dos, sizeof(dos), 1, file)) {
		_tprintf(_T("IMAGE_DOS_HEADER fread failed\n"));
		result = -1;
	}
	else if (fseek(file, dos.e_lfanew, 0) != 0 || ftell(file) != dos.e_lfanew) {
		_tprintf(_T("IMAGE_DOS_HEADER.e_lfanew fseek failed\n"));
		result = -1;
	}
	else if (!fread(&nt32, sizeof(nt32), 1, file)) {
		_tprintf(_T("IMAGE_NT_HEADERS32 fread failed\n"));
		result = -1;
	}
	else {
		DWORD nt32EntryPoint = nt32.OptionalHeader.AddressOfEntryPoint + nt32.OptionalHeader.ImageBase;
		if (nt32EntryPoint != EntryPoint) {
			_tprintf(_T("nt32EntryPoint != EntryPoint\n"));
			_tprintf(_T("Found 0x%08x, but expected 0x%08x\n"), nt32EntryPoint, EntryPoint);
			result = -1;
		}
	}

	fclose(file);
	return result;
}

int _tmain(int argc, TCHAR* argv[])
{
    PROCESS_INFORMATION procInfo = { 0 };
    STARTUPINFO startInfo = { 0 };
    int r;

    tstring fullPath = _T("");
    tstring commandLine = _T("");
    tstring workingDir = _T("");

    // Get absolute paths for our exe, and the target workingDir (which is our current workingDir).
	if ((r = BuildFullPaths(fullPath, workingDir)) != 0) {
        return r;
    }
    _tprintf(_T("fullPath=%s\n"), fullPath.c_str());
    _tprintf(_T("workingDir=%s\n"), workingDir.c_str());

	// Check just in-case the user isn't running the right version of LSR
	if ((r = ConfirmEntryPoint(fullPath, PROCESS_EIP)) != 0) {
		MessageBoxW(NULL, L"EXE check failed!\nOpenLSR requires _msr.exe version 0.3.5.1!", L"OpenLSR Injector", MB_ICONERROR);
		return r;
	}

    // We need to pass all arguments through OpenLSR.exe into _msr.exe, handle escaping here.
    // fullPath is added as the first argument.
    if ((r = BuildCommandLine(argc, argv, fullPath, commandLine)) != 0) {
        return r;
    }
    _tprintf(_T("commandLine=%s\n"), commandLine.c_str());

    LPTSTR cmdLine = new TCHAR[commandLine.length() + 1];
    _tcscpy(cmdLine, commandLine.c_str());

	// Allocate this because CreateProcess expects a non-const lpCommandLine argument,
	// Let's play it safe and assume this isn't just a joke.
    if (!CreateProcessW(EXE_NAME, cmdLine, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, workingDir.c_str(), &startInfo, &procInfo))
    {
        MessageBoxW(NULL, L"Could not launch game process!", L"OpenLSR Injector", MB_ICONERROR);
        return -1;
    }
	delete[] cmdLine;

	_tprintf(_T("created suspended process!\n"));

	BYTE eipBackup[2] = { 0 };

	if (!ReadProcessMemory(procInfo.hProcess, (LPVOID)PROCESS_EIP, &eipBackup, sizeof(eipBackup), NULL)) {
		_tprintf(_T("EIP Backup failed\n"));
		return (-1);
	}

	_tprintf(_T("EIP Backup OK\n"));

	static constexpr const BYTE eipPatch[2] = { 0xEB, 0xFE };    // infinite jmp to itself

	if (!WriteProcessMemory(procInfo.hProcess, (LPVOID)PROCESS_EIP, &eipPatch, sizeof(eipPatch), NULL)) {
		_tprintf(_T("EIP Patch failed\n"));
		return (-1);
	}

	_tprintf(_T("EIP Patch OK\n"));

	if (ResumeThread(procInfo.hThread) == (DWORD)-1) {
		_tprintf(_T("ResumeThread failed\n"));
		return (-1);
	}

	_tprintf(_T("ResumeThread OK\n"));

	const auto addrLoadLibrary = (LPTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("kernel32.dll")), nameof(LoadLibrary));
	if (!addrLoadLibrary) {
		_tprintf(_T("GetProcAddress(kernel32.dll, %s) failed\n"), _T(nameof(LoadLibrary)));
		return (-1);
	}

	_tprintf(_T("kernel32.dll!%s @ 0x%p\n"), _T(nameof(LoadLibrary)), addrLoadLibrary);

	static constexpr LPCTSTR dllPathBuf = _T("OpenLSR.dll");

	const LPVOID vAllocMem = ::VirtualAllocEx(procInfo.hProcess, NULL, tstrsizeof(dllPathBuf), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!vAllocMem) {
		_tprintf(_T("VirtualAllocEx failed\n"));
		return (-1);
	}

	_tprintf(_T("VirtualAllocEx OK\n"));

	if (!::WriteProcessMemory(procInfo.hProcess, vAllocMem, dllPathBuf, tstrsizeof(dllPathBuf), NULL)) {
		_tprintf(_T("WriteProcessMemory failed\n"));
		return (-1);
	}

	_tprintf(_T("WriteProcessMemory OK\n"));

	const HANDLE hInjectedThread = ::CreateRemoteThread(procInfo.hProcess, NULL, 0, addrLoadLibrary, vAllocMem, NULL, NULL);
	if (!hInjectedThread) {
		_tprintf(_T("CreateRemoteThread failed\n"));
		return -1;
	}

	_tprintf(_T("CreateRemoteThread OK\n"));

	_tprintf(_T("Waiting %dms for program... "), WAIT_TIME_MS);
	::Sleep(WAIT_TIME_MS); // wait for hooks to initialize
	_tprintf(_T("OK\n"));

	_tprintf(_T("Restoring EIP... "));
	if (!::WriteProcessMemory(procInfo.hProcess, (LPVOID)PROCESS_EIP, &eipBackup, sizeof(eipBackup), NULL)) {
		_tprintf(_T("Failed\n"));
	}
	else {
		_tprintf(_T("OK\n"));
	}

	return 0;
}