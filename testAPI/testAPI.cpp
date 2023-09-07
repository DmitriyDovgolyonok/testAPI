#include "WindowsUpdate.h"

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    WindowsUpdate updater;
    if(updater.Initialize())
    {
	    if(updater.SearchUpdates("IsInstalled=1"))
	    {
            updater.DisplayUpdates();
	    }
    }

    return 0;
}

