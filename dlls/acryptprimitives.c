#include <windows.h>

typedef BOOLEAN (WINAPI *rng_fn)(PVOID, ULONG);

enum {
    RNG_NOT_TRIED = 0,
    RNG_LOADING   = 1,
    RNG_DONE      = 2,
    LOAD_LIBRARY_SEARCH_SYSTEM32 = 0x800
};

static HMODULE g_module;
static rng_fn  g_rng;
static volatile LONG g_state;

static int load_rng(void)
{
    LONG state;

    for (;;) {
        state = InterlockedCompareExchange(&g_state, RNG_LOADING, RNG_NOT_TRIED);

        if (state == RNG_NOT_TRIED) {
            HMODULE module;
            rng_fn fn;

            module = LoadLibraryExW(L"advapi32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
            fn = module ? (rng_fn)GetProcAddress(module, "SystemFunction036") : 0;

            if (!fn && module) {
                FreeLibrary(module);
                module = 0;
            }

            g_rng = fn;
            g_module = module;

            /* ѕосле этого остальные потоки увид€т оба указател€. */
            InterlockedExchange(&g_state, RNG_DONE);
            return module != 0;
        }

        if (state == RNG_LOADING) {
            Sleep(0);
            continue;
        }

        /* RNG_DONE означает и успешную, и неуспешную попытку. */
        return g_module != 0;
    }
}


__declspec(dllexport) int WINAPI ProcessPrng(PVOID buffer, ULONG size)
{
    if (!load_rng())
        return 0;

    return (int)g_rng(buffer, size);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    (void)instance;
    (void)reserved;

    if (reason == DLL_PROCESS_ATTACH) {
        g_module = 0;
        g_rng = 0;
        g_state = RNG_NOT_TRIED;
        DisableThreadLibraryCalls(instance);
    }
    else if (reason == DLL_PROCESS_DETACH) {
        if (g_module) {
            FreeLibrary(g_module);
            g_module = 0;
            g_rng = 0;
        }
    }

    return TRUE;
}
