#pragma once

#include <iostream>
#include <windows.h>
#include <string>
#include <tlhelp32.h>

struct proc_info {
	DWORD id;
	std::string name;
	bool is_running;
};

std::string get_dll_path_from_clipboard();
proc_info get_process_info(const std::string& name);
std::string get_filename_from_path(const std::string& path);
bool inject(const std::string& dll_path, const DWORD process_id);