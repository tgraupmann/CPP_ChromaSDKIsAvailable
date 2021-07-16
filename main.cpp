#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <filesystem>
namespace fs = std::filesystem;

using namespace std;

bool IsChromaSDKAvailable()
{
    string fileName;
#ifdef _WIN64
    fileName = "C:\\Program Files\\Razer Chroma SDK\\bin\\RzChromaSDK64.dll";
#else
    fileName = "C:\\Program Files (x86)\\Razer Chroma SDK\\bin\\RzChromaSDK.dll";
#endif
    fs::path p = fileName.c_str();
    if (!fs::exists(p))
    {
        return false;
    }

    wstring szVersionFile(fileName.begin(), fileName.end());

    bool result = false;

    DWORD  verHandle = 0;
    UINT   size = 0;
    LPBYTE lpBuffer = NULL;
    DWORD  verSize = GetFileVersionInfoSize(szVersionFile.c_str(), &verHandle);

    if (verSize != NULL)
    {
        LPSTR verData = new char[verSize];

        if (GetFileVersionInfo(szVersionFile.c_str(), verHandle, verSize, verData))
        {
            if (VerQueryValue(verData, L"\\", (VOID FAR * FAR*) & lpBuffer, &size))
            {
                if (size)
                {
                    VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
                    if (verInfo->dwSignature == 0xfeef04bd)
                    {
                        const int major = (verInfo->dwFileVersionMS >> 16) & 0xffff;
                        const int minor = (verInfo->dwFileVersionMS >> 0) & 0xffff;
                        const int revision = (verInfo->dwFileVersionLS >> 16) & 0xffff;
                        const int build = (verInfo->dwFileVersionLS >> 0) & 0xffff;

                        printf("File Version: %d.%d.%d.%d\n", major, minor, revision, build);

                        // Anything less than the min version returns false
                        const int minMajor = 3;
                        const int minMinor = 20;
                        const int minRevision = 2;

                        if (major < minMajor) // Less than 3.X.X
                        {
                            result = false;
                        }
                        else if (major == minMajor && minor < minMinor) // Less than 3.20
                        {
                            result = false;
                        }
                        else if (major == minMajor && minor == minMinor && revision < minRevision) // Less than 3.20.2
                        {
                            result = false;
                        }
                        else
                        {
                            result = true; // production version or better
                        }
                    }
                }
            }
        }
        delete[] verData;
    }

    return result;
}

int main()
{
    if (IsChromaSDKAvailable())
    {
        cout << "ChromaSDK is available!" << endl;
    }
    else
    {
        cerr << "ChromaSDK is not available!" << endl;
    }
    return 0;
}
