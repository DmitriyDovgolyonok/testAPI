#pragma once

#include <wuapi.h>
#include <string>

class WindowsUpdate
{
public:
    WindowsUpdate();
    ~WindowsUpdate();

    bool Initialize();
    bool GetUpdateHistory(const char* criteria, long startIndex, long count);
    void DisplayUpdateHistory();

    std::string GetUpdateDescription(IUpdateHistoryEntry* historyEntry);
    std::string GetUpdateDate(IUpdateHistoryEntry* historyEntry);

private:
    IUpdateSession3* updateSession;
    IUpdateHistoryEntryCollection* updateHistory;
    bool CoInitializeSucceeded;
};

