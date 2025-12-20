#include "pch.h"
#include "PythonLoader.h"
#include <codecvt>
#include <locale>

PythonLoader::!PythonLoader()
{
    if (pyDllHandle != NULL)
    {
        FreeLibrary(pyDllHandle);
        pyDllHandle = NULL;
    }
}

std::wstring s2ws(const std::string& str)
{
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.from_bytes(str);
}

String^ PythonLoader::FindPythonDllInPath()
{
    const int MIN_VERSION = 12;
    const int MAX_VERSION = 14;

    const int PATH_BUFFER_SIZE = 1024;
    WCHAR pathBuffer[PATH_BUFFER_SIZE] = { 0 };

    for (int x = MAX_VERSION; x >= MIN_VERSION; --x)
    {
        std::string dllNameNarrow = "python3" + std::to_string(x) + ".dll";
        std::wstring dllNameWide = s2ws(dllNameNarrow);
        DWORD result = SearchPath(
            NULL,  
            dllNameWide.c_str(),
            NULL,     
            PATH_BUFFER_SIZE,
            pathBuffer,
            NULL
        );

        if (result > 0 && result < PATH_BUFFER_SIZE)
        {
            return gcnew String(pathBuffer);
        }
    }

    return nullptr;
}


bool PythonLoader::SetDllAndInitialize(System::String^ dllPath)
{
    if (String::IsNullOrEmpty(dllPath))
    {
        return false;
    }

    pin_ptr<const wchar_t> pinnedPath = PtrToStringChars(dllPath);
    std::wstring nativePath(pinnedPath);

    HMODULE hPython = LoadLibraryW(nativePath.c_str());
    if (!hPython)
    {
        return false;
    }

    if (Py_IsInitialized())
    {
        return true;
    }

    PyConfig config;
    PyConfig_InitPythonConfig(&config);

    PyStatus status = Py_InitializeFromConfig(&config);
    PyConfig_Clear(&config);

    if (PyStatus_Exception(status))
    {
        return false;
    }

    return Py_IsInitialized();
}
