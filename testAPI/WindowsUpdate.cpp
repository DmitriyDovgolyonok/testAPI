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

WindowsUpdate::WindowsUpdate() : updateSession(nullptr), updateSearcher(nullptr), searchResult(nullptr), CoInitializeSucceeded(false)
{
	CoInitializeSucceeded = SUCCEEDED(CoInitialize(NULL));
}

WindowsUpdate::~WindowsUpdate()
{
    if (searchResult) searchResult->Release();
    if (updateSearcher) updateSearcher->Release();
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

    HRESULT hr = CoCreateInstance(CLSID_UpdateSession, NULL, CLSCTX_INPROC_SERVER, IID_IUpdateSession, (LPVOID*)&updateSession);
    if (FAILED(hr)) 
    {
        std::cerr << "Failed to create UpdateSession instance." << std::endl;
        return false;
    }

    hr = updateSession->CreateUpdateSearcher(&updateSearcher);
    if (FAILED(hr)) 
    {
        std::cerr << "Failed to create UpdateSearcher instance." << std::endl;
        return false;
    }

    return true;
}

bool WindowsUpdate::SearchUpdates(const char* input)
{
    if (!updateSearcher) 
    {
        std::cerr << "UpdateSearcher is not initialized." << std::endl;
        return false;
    }

    BSTR const criteria = char_to_bstr(input);
    HRESULT const hr = updateSearcher->Search(criteria, &searchResult);
    SysFreeString(criteria);

    return SUCCEEDED(hr);
}

std::string WindowsUpdate::GetUpdateName(IUpdate* update)
{
    BSTR updateName;
    HRESULT const hr = update->get_Title(&updateName);
    if (SUCCEEDED(hr)) 
    {
        std::string updateNameUTF8 = BSTRToUTF8(updateName);
        SysFreeString(updateName);
        return updateNameUTF8;
    }
    return "";
}

std::string WindowsUpdate::GetUpdateDate(IUpdate* update)
{
    DATE lastDeploymentChangeTime;
    HRESULT const hr = update->get_LastDeploymentChangeTime(&lastDeploymentChangeTime);
    if (SUCCEEDED(hr)) 
    {
        SYSTEMTIME sysTime;
        VariantTimeToSystemTime(lastDeploymentChangeTime, &sysTime);
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(4) << sysTime.wYear << "-"
            << std::setw(2) << sysTime.wMonth << "-" << std::setw(2) << sysTime.wDay;
        return ss.str();
    }
    return "";
}

void WindowsUpdate::DisplayUpdates()
{
    if (!searchResult) 
    {
        std::cerr << "SearchResult is not available." << std::endl;
        return;
    }

    IUpdateCollection* updateCollection;
    HRESULT hr = searchResult->get_Updates(&updateCollection);
    if (FAILED(hr)) 
    {
        std::cerr << "Failed to get Updates from SearchResult." << std::endl;
        return;
    }

    long itemCount;
    hr = updateCollection->get_Count(&itemCount);
    if (FAILED(hr)) 
    {
        std::cerr << "Failed to get update count." << std::endl;
        return;
    }

    for (long i = 0; i < itemCount; i++) 
    {
        IUpdate* update;
        hr = updateCollection->get_Item(i, &update);
        if (FAILED(hr)) 
        {
            std::cerr << "Failed to get update at index " << i << std::endl;
            continue;
        }

        std::string updateName = GetUpdateName(update);
        std::string updateDate = GetUpdateDate(update);

        if (!updateName.empty() && !updateDate.empty()) 
        {
            std::cout << "Update Name: " << updateName << " - Date: " << updateDate << std::endl;
        }

        update->Release();
    }

    updateCollection->Release();
}