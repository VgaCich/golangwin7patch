#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

typedef BOOL (WINAPI *ProcessPrngFn)(PBYTE, SIZE_T);

int main(void)
{
#if defined(_WIN64) || defined(__x86_64__)
    const char *dll_name = "acryptprimitives64.dll";
#else
    const char *dll_name = "acryptprimitives32.dll";
#endif

    HMODULE dll = LoadLibraryA(dll_name);
    if (!dll) {
        fprintf(stderr, "LoadLibraryA(%s) failed: %lu\n",
                dll_name, (unsigned long)GetLastError());
        return 1;
    }

    ProcessPrngFn process_prng =
        (ProcessPrngFn)(void *)GetProcAddress(dll, "ProcessPrng");

    if (!process_prng) {
        fprintf(stderr, "GetProcAddress(ProcessPrng) failed: %lu\n",
                (unsigned long)GetLastError());
        FreeLibrary(dll);
        return 1;
    }
    
    printf("Testing: %s\n", dll_name);
    
    for(int i = 0; i < 3; i++) {
        unsigned char random_data[16];
        BOOL result = process_prng(random_data, sizeof(random_data));
        printf("Try %i\n", i + 1);
        printf("ProcessPrng returned: %s (%d)\n", result ? "TRUE" : "FALSE", (int)result);
        printf("Random data:");
            for (int i = 0; i < (int)sizeof(random_data); ++i)
                printf(" %02X", (unsigned)random_data[i]);
        putchar('\n');
    }
    
    FreeLibrary(dll);
    return 0;
}
