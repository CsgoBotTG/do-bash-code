#pragma once

#ifdef _WIN32

#include <string>
#include <Windows.h>
#include <wininet.h>

#pragma comment(lib, "wininet")

std::string GetPastebin(HINTERNET& hInternet, HINTERNET& hConnect, const LPCWSTR& raw_url);

#endif