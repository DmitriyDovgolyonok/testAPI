#pragma once

#include <wuapi.h>
#include <string>

class WindowsUpdate
{
public:
    WindowsUpdate();
    ~WindowsUpdate();

    bool Initialize();
    bool SearchUpdates(const char* input);
    void DisplayUpdates();
    std::string GetUpdateName(IUpdate* update);
    std::string GetUpdateDate(IUpdate* update);

private:
    IUpdateSession* updateSession;
    IUpdateSearcher* updateSearcher;
    ISearchResult* searchResult;
    bool CoInitializeSucceeded;
};

