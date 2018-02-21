#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>

#define FILE_PE_START   0x80
#define NUM_SECTIONS    1
#define FILE_TEXT       0x200
#define RVA_TEXT        0x1000

#include <pshpack1.h>
static struct _PeImage {
    IMAGE_DOS_HEADER DosHeader;
    unsigned char Padding1[FILE_PE_START - sizeof(IMAGE_DOS_HEADER)];
    IMAGE_NT_HEADERS32 NtHeaders;
    IMAGE_SECTION_HEADER Sections[NUM_SECTIONS];
    unsigned char Padding2[FILE_TEXT - FILE_PE_START - sizeof(IMAGE_NT_HEADERS32) - NUM_SECTIONS * sizeof(IMAGE_SECTION_HEADER)];
    unsigned char TextSection[FILE_TEXT];
} DllData =
{
    // DosHeader
    { IMAGE_DOS_SIGNATURE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0}, 0, 0, {0}, FILE_PE_START },
    // Padding1
    {0},

    // Nt header
    {
        IMAGE_NT_SIGNATURE,
        // FileHeader
        {
            IMAGE_FILE_MACHINE_I386, NUM_SECTIONS, 0, 0, 0, sizeof(IMAGE_OPTIONAL_HEADER32),
            IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE | IMAGE_FILE_DLL
        },
        // OptionalHeader
        {
            IMAGE_NT_OPTIONAL_HDR32_MAGIC, 4, 0, 0, 0, 0x400, RVA_TEXT, RVA_TEXT, 0, 0x11220000, 0x1000, 0x200,
            4, 0, 1, 0, 4, 0, 0, 0x2000, FILE_TEXT, 0, IMAGE_SUBSYSTEM_WINDOWS_GUI, 0,
            0x100000, 0x1000, 0x100000, 0x1000, 0, 0x10,
            // DataDirectory
            {
                {0, 0},
            }
        }
    },
    // Sections
    {
        {
            ".text", {0x18}, RVA_TEXT, FILE_TEXT, FILE_TEXT, 0, 0, 0, 0,
            IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ },
    },
    // Padding2
    {0},
    // TextSection
    {
        0x33, 0xC0,         // xor eax, eax
        0x40,               // inc eax
        0xC2, 0x0C, 0x00    // ret C
    }
};
#include <poppack.h>

std::vector<std::string> g_Files;
std::vector<HMODULE> g_Loaded;

using std::cout;
using std::endl;
using std::cin;

static void LoadEmbeddedAt(DWORD addr)
{
    char buffer[MAX_PATH];
    DllData.NtHeaders.OptionalHeader.ImageBase = addr;
    if (tmpnam_s(buffer))
    {
        cout << "Failed generating tmp name" << endl;
    }
    FILE* f;
    if (fopen_s(&f, buffer, "wb"))
    {
        cout << "Failed creating " << buffer << endl;
        return;
    }

    g_Files.push_back(buffer);
    fwrite(&DllData, sizeof(DllData), 1, f);
    fclose(f);

    HMODULE mod = LoadLibraryA(buffer);
    if (!mod)
    {
        cout << "Failed loading " << buffer << endl;
        return;
    }
    g_Loaded.push_back(mod);
    cout << buffer << " loaded at 0x" << std::hex << mod << endl;
}


int main()
{
    static_assert(sizeof(DllData) == 1024, "fail");

    char choice = 0;

    while (choice != 'q')
    {
        cout << "A: Load embedded dll at address" << endl;
        cout << "L: Load" << endl;
        cout << "U: Unload" << endl;
        cout << "Q: Quit" << endl;
        cin >> choice;

        if (tolower(choice) == 'a')
        {
            std::string addrStr;
            cout << "Specify address(hex): ";
            cin >> addrStr;
            DWORD addr = stoul(addrStr, 0, 16);
            LoadEmbeddedAt(addr);
        }
        else if (tolower(choice) == 'l')
        {
            std::string name;
            cout << "Specify dll name: ";
            cin >> name;
            HMODULE mod = LoadLibraryA(name.c_str());
            if (mod)
            {
                g_Loaded.push_back(mod);
                cout << "Loaded at 0x" << std::hex << mod << endl;
            }
            else
            {
                cout << "Unabled to load" << endl;
            }
        }
        else if (tolower(choice) == 'u')
        {
            std::string nameOrAddr;
            cout << "Specify dll name or address(hex): ";
            cin >> nameOrAddr;
            HMODULE mod = GetModuleHandleA(nameOrAddr.c_str());
            if (!mod)
            {
                cout << "Does not seem to be a module, trying address" << endl;
                mod = (HMODULE)stoul(nameOrAddr, 0, 16);
            }
            if (!mod)
            {
                cout << "Don't know what this is" << endl;
            }
            else
            {
                cout << "Unloaded" << endl;
                FreeLibrary(mod);
                auto& it = std::find(g_Loaded.begin(), g_Loaded.end(), mod);
                if (it != g_Loaded.end())
                {
                    g_Loaded.erase(it);
                }
            }
        }
    }

    for (const auto& module : g_Loaded)
    {
        FreeLibrary(module);
    }

    for (const auto& file : g_Files)
    {
        DeleteFileA(file.c_str());
    }

    return 0;
}
