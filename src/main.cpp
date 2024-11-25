#include "injector.h"

int main(int argc, const char** argv)
{
	if (argc < 2) {
		printf("dndinjector <PathToEXE>\n");
		system("pause");
		return 1;
	}

	std::string path = get_dll_path_from_clipboard();
	
	if (path.empty()) {
		printf("fatal: Clipboard does not contain files\n");
		system("pause");
		return 1;
	}

	proc_info Proc = get_process_info(get_filename_from_path(argv[1]));

	if (!Proc.is_running) {
		printf("fatal: Process is not running\n");
		system("pause");
		return 1;
	}

	if (!inject(path, Proc.id)) {
		printf("fatal: Failed to inject dll library\n");
		system("pause");
		return 1;
	}

	return 0;
}