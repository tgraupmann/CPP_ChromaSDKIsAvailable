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
    wstring fileName;
#ifdef _WIN64
    fileName = L"C:\\Program Files\\Razer Chroma SDK\\bin\\RzChromaSDK64.dll";
#else
    fileName = L"C:\\Program Files (x86)\\Razer Chroma SDK\\bin\\RzChromaSDK.dll";
#endif
    fs::path p = fileName.c_str();
    if (!fs::exists(p))
    {
        return false;
    }

    bool result = false;

    DWORD  verHandle = 0;
    UINT   size = 0;
    LPBYTE lpBuffer = NULL;
    DWORD  verSize = GetFileVersionInfoSize(fileName.c_str(), &verHandle);

    if (verSize)
    {
        LPSTR verData = (LPSTR)malloc(verSize);

        if (GetFileVersionInfo(fileName.c_str(), verHandle, verSize, verData))
        {
            if (VerQueryValue(verData, L"\\", (VOID FAR * FAR*) & lpBuffer, &size))
            {
                if (size)
                {
                    VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
                    if (verInfo->dwSignature == 0xfeef04bd)
                    {
                        const int major = (verInfo->dwProductVersionMS >> 16) & 0xffff;
                        const int minor = (verInfo->dwProductVersionMS >> 0) & 0xffff;
                        const int revision = (verInfo->dwProductVersionMS >> 16) & 0xffff;
                        const int build = (verInfo->dwProductVersionMS >> 0) & 0xffff;

                        printf("Product Version: %d.%d.%d.%d\n", major, minor, revision, build);

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
        free(verData);
    }

    return result;
}

int main()
{
    if (IsChromaSDKAvailable())
    {
        printf("ChromaSDK is available!\r\n");
    }
    else
    {
        fprintf(stderr, "ChromaSDK is not available!\r\n");
    }
    return 0;
}
