#include "pch.h"
#include "PythonLoader.h"

PythonLoader::!PythonLoader()
{
    if (pyDllHandle != NULL)
    {
        FreeLibrary(pyDllHandle);
        pyDllHandle = NULL;
    }
}

System::String^ PythonLoader::FindPythonDll()
{

    System::String^ envVarPath;
#ifdef _DEBUG
    envVarPath = System::Environment::GetEnvironmentVariable("PYTHONNET_PYDLL");
#else
    envVarPath = System::Environment::GetEnvironmentVariable("PYTHONNET_PYDLL");
#endif


    if (!System::String::IsNullOrEmpty(envVarPath))
    {
        System::Console::WriteLine("Found PYTHONNET_PYDLL environment variable: {0}", envVarPath);
        return envVarPath;
    }

    // 2. Search common installation locations.
    // This list can be expanded to include more paths.
    cli::array<System::String^>^ potentialPaths = gcnew cli::array<System::String^>
    {
        "C:\\Python310\\python310.dll",
            "C:\\Python311\\python311.dll",
            "C:\\Python312\\python312.dll",
            "C:\\Python313\\python313.dll",
            "C:\\Program Files\\Python310\\python310.dll",
            "C:\\Program Files\\Python311\\python311.dll",
            "C:\\Program Files\\Python312\\python312.dll",
            "C:\\Program Files\\Python313\\python313.dll",
    };

    for each (System::String ^ path in potentialPaths)
    {
        if (System::IO::File::Exists(path))
        {
            System::Console::WriteLine("Found Python DLL at: {0}", path);
            return path;
        }
    }

    System::Console::WriteLine("Could not find a Python DLL in common locations.");
    return nullptr;
}

bool PythonLoader::SetDllAndInitialize(System::String^ dllPath)
{
    if (System::String::IsNullOrEmpty(dllPath))
    {
        System::Console::WriteLine("ERROR: DLL path is null or empty.");
        return false;
    }

    pin_ptr<const wchar_t> wDllPath = PtrToStringChars(dllPath);

    pyDllHandle = LoadLibraryW(wDllPath);
    if (pyDllHandle == NULL)
    {
        System::Console::WriteLine("ERROR: Could not load the Python DLL from '{0}'", dllPath);
        return false;
    }

    pyInitializeEx = (PyInitializeExFunc)GetProcAddress(pyDllHandle, "Py_InitializeEx");
    pyFinalizeEx = (PyFinalizeExFunc)GetProcAddress(pyDllHandle, "Py_FinalizeEx");
    pyRunSimpleStringFlags = (PyRunSimpleStringFlagsFunc)GetProcAddress(pyDllHandle, "PyRun_SimpleStringFlags");

    if (pyInitializeEx == nullptr || pyFinalizeEx == nullptr || pyRunSimpleStringFlags == nullptr)
    {
        System::Console::WriteLine("ERROR: Could not find required functions in the Python DLL.");
        FreeLibrary(pyDllHandle);
        pyDllHandle = NULL;
        return false;
    }


    pin_ptr<const wchar_t> pWchar = PtrToStringChars(GetPythonHomeFromRegistry());
    Py_SetPythonHome(pWchar);
    pyInitializeEx(0);
    System::Console::WriteLine("Python interpreter initialized successfully.");

    return true;
}

String^ PythonLoader::GetPythonHomeFromRegistry()
{
    String^ keyPath = "SOFTWARE\\Python\\PythonCore";
    String^ latestVersion = nullptr;

    RegistryKey^ currentUserKey = Registry::CurrentUser->OpenSubKey(keyPath);
    if (currentUserKey != nullptr)
    {
        for each (String ^ version in currentUserKey->GetSubKeyNames())
        {
            if (latestVersion == nullptr || String::Compare(version, latestVersion, StringComparison::OrdinalIgnoreCase) > 0)
            {
                latestVersion = version;
                break;
            }
        }

        if (latestVersion != nullptr)
        {
            RegistryKey^ installKey = currentUserKey->OpenSubKey(latestVersion + "\\InstallPath");
            if (installKey != nullptr)
            {
                String^ installPath = (String^)installKey->GetValue(nullptr);
                installKey->Close();
                currentUserKey->Close();
                return installPath;
            }
        }
        currentUserKey->Close();
    }

    latestVersion = nullptr;
    RegistryKey^ localMachineKey = Registry::LocalMachine->OpenSubKey(keyPath);
    if (localMachineKey != nullptr)
    {
        for each (String ^ version in localMachineKey->GetSubKeyNames())
        {
            if (latestVersion == nullptr || String::Compare(version, latestVersion, StringComparison::OrdinalIgnoreCase) > 0)
            {
                latestVersion = version;
                break;
            }
        }

        if (latestVersion != nullptr)
        {
            RegistryKey^ installKey = localMachineKey->OpenSubKey(latestVersion + "\\InstallPath");
            if (installKey != nullptr)
            {
                String^ installPath = (String^)installKey->GetValue(nullptr);
                installKey->Close();
                localMachineKey->Close();
                return installPath;
            }
        }
        localMachineKey->Close();
    }

    return String::Empty;
}
