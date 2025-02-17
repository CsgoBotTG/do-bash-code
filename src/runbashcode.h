#ifdef _WIN32

#include <sys\stat.h>
#include <fstream>

#include <string>
#include <vector>
#include <Windows.h>

std::string FindPythonPath();
int         StartCommand(const char* command);

#endif