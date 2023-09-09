#pragma once
#include <wuapi.h>
#include <string>
struct UpdateHistoryEntry
{
    std::string title;
    std::string date;
};

class WindowsUpdate
{
public:
    WindowsUpdate();
    ~WindowsUpdate();

    bool Initialize();
    bool GetUpdateHistory(const char* criteria, long startIndex, long count);
    std::string GetUpdateJSON();
    void DisplayUpdateHistory();

    std::string GetUpdateTitle(IUpdateHistoryEntry* historyEntry);
    std::string GetUpdateDate(IUpdateHistoryEntry* historyEntry);

private:
    IUpdateSession3* updateSession;
    IUpdateHistoryEntryCollection* updateHistory;
    bool CoInitializeSucceeded;
};

