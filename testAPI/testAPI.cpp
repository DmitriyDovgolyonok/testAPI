#include "WindowsUpdate.h"
#include <iostream>

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    WindowsUpdate windowsUpdate;
    if (!windowsUpdate.Initialize())
        return 1;

    const char* criteria = "";
    long startIndex = 0;
    long count = 5; // Можно изменить для желаемого количества записей 

    if (windowsUpdate.GetUpdateHistory(criteria, startIndex, count))
        windowsUpdate.DisplayUpdateHistory();

    return 0;
}

