#include "injector.h"

std::string get_dll_path_from_clipboard()
{
	std::string ret;

	if (!OpenClipboard(nullptr))
		return ret;

	if (IsClipboardFormatAvailable(CF_HDROP)) {
		HDROP h = (HDROP)GetClipboardData(CF_HDROP);
		if (h) {
			UINT count = DragQueryFileA(h, 0xFFFFFFFF, nullptr, 0);
			for (UINT i = 0; i < count; i++) {
				char path[MAX_PATH];
				if (DragQueryFileA(h, i, path, MAX_PATH)) {
					ret = path;
					break;
				}
			}
		}
	}

	CloseClipboard();

	return ret;
}

proc_info get_process_info(const std::string& name)
{
	proc_info info;
	info.is_running = false;
	
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (h == INVALID_HANDLE_VALUE) {
		return info;
	}

	PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };

	if (!Process32First(h, &pe32)) {
		CloseHandle(h);
		return info;
	}

	do {
		if (std::string{ pe32.szExeFile } == name) {
			info.name = name;
			info.id = pe32.th32ProcessID;
			info.is_running = true;
			break;
		}
	} while (Process32Next(h, &pe32));

	CloseHandle(h);

	return info;
}

bool inject(const std::string& dll_path, const DWORD process_id)
{
	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);

	if (!h)
		return false;

	LPVOID remote_string = VirtualAllocEx(h, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(h, remote_string, (LPVOID)dll_path.c_str(), dll_path.length() + 1, 0);

	HANDLE thread = CreateRemoteThread(h, nullptr, 0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"),
		remote_string, 0, nullptr);

	WaitForSingleObject(thread, INFINITE);
	VirtualFreeEx(h, remote_string, 0, MEM_RELEASE);

	CloseHandle(thread);
	CloseHandle(h);

	return true;
}