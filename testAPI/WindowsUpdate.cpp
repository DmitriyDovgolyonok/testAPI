#include "WindowsUpdate.h"
#include <iomanip>
#include <iostream>
#include <comutil.h>
#include <ctime>
#include <sstream>

BSTR char_to_bstr(const char* input)
{
    int wideStrLength = MultiByteToWideChar(CP_ACP, 0, input, -1, NULL, 0);
    BSTR bstr = SysAllocStringLen(NULL, wideStrLength);

    if (bstr)
    {
        MultiByteToWideChar(CP_ACP, 0, input, -1, bstr, wideStrLength);
    }

    return bstr;
}
std::string BSTRToUTF8(const BSTR bstr)
{
    const int charCount = WideCharToMultiByte(CP_UTF8, 0, bstr, -1, NULL, 0, NULL, NULL);
    char* charString = new char[charCount];
    WideCharToMultiByte(CP_UTF8, 0, bstr, -1, charString, charCount, NULL, NULL);
    std::string result(charString);
    delete[] charString;
    return result;
}

WindowsUpdate::WindowsUpdate() : updateSession(nullptr), updateHistory(nullptr), CoInitializeSucceeded(false)
{
    CoInitializeSucceeded = SUCCEEDED(CoInitialize(nullptr));
}

WindowsUpdate::~WindowsUpdate()
{
    if (updateHistory) updateHistory->Release();
    if (updateSession) updateSession->Release();
    if (CoInitializeSucceeded) CoUninitialize();
}

bool WindowsUpdate::Initialize()
{
    if (!CoInitializeSucceeded) 
    {
        std::cerr << "COM initialization failed." << std::endl;
        return false;
    }

    HRESULT const hr = CoCreateInstance(CLSID_UpdateSession, nullptr, CLSCTX_INPROC_SERVER, IID_IUpdateSession, (LPVOID*)&updateSession);
    if (FAILED(hr)) 
    {
        std::cerr << "Failed to create UpdateSession instance." << std::endl;
        return false;
    }

    return true;
}
bool WindowsUpdate::GetUpdateHistory(const char* criteria, long startIndex, long count)
{
    if (!updateSession) 
    {
        std::cerr << "UpdateSession is not initialized." << std::endl;
        return false;
    }
    BSTR const bstrCriteria = SysAllocString(char_to_bstr(criteria));
    HRESULT const hr = updateSession->QueryHistory(bstrCriteria, startIndex, count, &updateHistory);
    if (FAILED(hr)) 
    {
        std::cerr << "Failed to query update history." << std::endl;
        return false;
    }

    return true;
}

std::string WindowsUpdate::GetUpdateDescription(IUpdateHistoryEntry* historyEntry)
{
    BSTR description;
    HRESULT const hr = historyEntry->get_Title(&description);
    if (SUCCEEDED(hr)) 
    {
        std::string updateNameUTF8 = BSTRToUTF8(description);
        SysFreeString(description);
        return updateNameUTF8;
    }
    return "";
}

std::string WindowsUpdate::GetUpdateDate(IUpdateHistoryEntry* historyEntry)
{
    DATE date;
    HRESULT const hr = historyEntry->get_Date(&date);
    if (SUCCEEDED(hr)) 
    {
        SYSTEMTIME sysTime;
        VariantTimeToSystemTime(date, &sysTime);
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(4) << sysTime.wYear << "-"
            << std::setw(2) << sysTime.wMonth << "-" << std::setw(2) << sysTime.wDay;
        return ss.str();
    }
    return "";
}

void WindowsUpdate::DisplayUpdateHistory()
{
    if (!updateHistory) 
    {
        std::cerr << "UpdateHistory is not available." << std::endl;
        return;
    }
    long itemCount;
    HRESULT hr = updateHistory->get_Count(&itemCount);
    if (FAILED(hr)) 
    {   
        std::cerr << "Failed to get update history count." << std::endl;
        return;
    }

    for (long i = 0; i < itemCount; i++) 
    {
        IUpdateHistoryEntry* historyEntry;
        hr = updateHistory->get_Item(i, &historyEntry);
        if (FAILED(hr)) 
        {
            std::cerr << "Failed to get update at index " << i << std::endl;
            continue;
        }

        std::string description = GetUpdateDescription(historyEntry);
        std::string updateDate = GetUpdateDate(historyEntry);

        if (!description.empty() && !updateDate.empty()) 
        {
            std::cout << "Description: " << description << " - Date: " << updateDate << std::endl;
        }

        historyEntry->Release();
    }
}