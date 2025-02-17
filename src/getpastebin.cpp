#ifdef _WIN32

#include "getpastebin.h"

std::string GetPastebin(HINTERNET& hInternet, HINTERNET& hConnect, const LPCWSTR& raw_url)
{
    char buffer[1024];
    DWORD bytesRead;
    std::string response;

    // open request
    HINTERNET hRequest = ::HttpOpenRequestW(hConnect, L"GET", raw_url/*TEXT("raw/0PW9gmS7")*/, NULL, NULL, 0, INTERNET_FLAG_SECURE, 1);

    if (hRequest == NULL) {
        printf("HttpOpenRequest failed: %d\n", GetLastError());
        return "";
    }

    // send request
    if (!HttpSendRequest(hRequest, NULL, 0, NULL, 0))
    {
        printf("HttpSendRequest failed: %d\n", GetLastError());
        InternetCloseHandle(hRequest);
        return "";
    }

    while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = 0;
        response += buffer;
    }

    // close request
    ::InternetCloseHandle(hRequest);

    return response;
}

#endif