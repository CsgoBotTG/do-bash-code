#ifdef _WIN32

#include "runbashcode.h"

int StartCommand(const char* command) 
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // Запускаем процесс.
  if (!CreateProcess(
      NULL,          // Имя исполняемого файла (NULL означает, что он из cmd_line)
      (LPSTR)command,      // Командная строка (преобразуем const char* в LPSTR)
      NULL,          // Атрибуты процесса (NULL для использования по умолчанию)
      NULL,          // Атрибуты потока (NULL для использования по умолчанию)
      FALSE,         // Наследовать дескрипторы (FALSE)
      CREATE_NO_WINDOW, // Флаги создания (CREATE_NO_WINDOW скрывает окно консоли)
      NULL,          // Блок окружения (NULL для использования текущего окружения)
      NULL,          // Текущий каталог (NULL для использования текущего каталога)
      &si,           // STARTUPINFO
      &pi            // PROCESS_INFORMATION
  )) {
    printf("CreateProcess failed: %d\n", GetLastError());
    return 1;
  }

  // Ждем завершения процесса.
//  WaitForSingleObject(pi.hProcess, INFINITE);

  // Получаем код завершения.
//  DWORD exitCode;
//  if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
//    cerr << "GetExitCodeProcess failed (" << GetLastError() << ")" << endl;
//  } else {
//    cout << "Process exited with code: " << exitCode << endl;
//  }

  // Закрываем дескрипторы.
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return 0;
}

#endif