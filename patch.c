#include "windows.h"
HANDLE hProcess;
int base;
void (*init)(int);

/* make a function dummy */
void dummy_func(int addr)
{
	char retn = '\xC3';
	WriteProcessMemory(hProcess, (void *)addr, &retn, 1, NULL);
}

/* addr: JMP target */
void hook_jmp(int addr, void *target)
{
	int t = (int)target - (int)addr - 5;
	char buf[5];
	buf[0] = '\xE9';
	memcpy(buf + 1, &t, sizeof(t));
	WriteProcessMemory(hProcess, (void *)addr, buf, 5, NULL);
}

void init_rmalloc()
{
    int old_pos = 0x38bf41;
    int new_pos = 0x4189ea;
    char buf[7];
    int len = sizeof(buf);

    /* save old realloc header */
	memcpy(buf, (void *)(base + old_pos), len);
	
	/* move old header to a new place, then glue them */
	WriteProcessMemory(hProcess, (void *)(new_pos + base), buf, len, NULL);
	hook_jmp(new_pos + len + base, (void *)(base + old_pos + len));

    /* init rmalloc with native remalloc address*/
	init(new_pos + base);
}

__declspec(dllexport)
void patch()
{
	HANDLE rmalloc = LoadLibraryA("rmalloc.dll");
	void *my_malloc = GetProcAddress(rmalloc, "rmalloc");
	void *my_calloc = GetProcAddress(rmalloc, "rcalloc");
	void *my_realloc = GetProcAddress(rmalloc, "rrealloc");
	void *my_free = GetProcAddress(rmalloc, "rfree");
	init = (void *)GetProcAddress(rmalloc, "init"); 
	hProcess = GetCurrentProcess();
	base = (int)GetModuleHandle(NULL) + 0xC00;
	
	MessageBoxA(NULL, "TH145 patcher loaded successfully.", "Hello World!", 0);

	init_rmalloc();

	hook_jmp(0x38d34e + base, my_malloc); /* patch malloc functions */
	hook_jmp(0x3961c7 + base, my_calloc);
	hook_jmp(0x38bf41 + base, my_realloc);
	hook_jmp(0x38a804 + base, my_free);

	dummy_func(0x2579a0 + base); /* disable th145 antidebugger */
	dummy_func(0x258a20 + base);
}
