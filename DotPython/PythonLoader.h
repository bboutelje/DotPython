// This is a C++/CLI project file. It must be compiled with the /clr option.

#include <iostream>
#include <windows.h> // For LoadLibraryW and GetProcAddress
#include <vcclr.h>   // For marshal_as to convert managed strings to native
#include <vector>
#include <msclr\marshal.h>
#include <Python.h>
#include <string>

using namespace System;
using namespace System::IO;
using namespace System::Linq;
using namespace Microsoft::Win32;

// Define the function signatures for the CPython API.
// We use Cdecl calling convention as it's the standard for C libraries.
typedef int(__cdecl* PyInitializeExFunc)(int);
typedef int(__cdecl* PyFinalizeExFunc)(void);
typedef void(__cdecl* PyRunSimpleStringFlagsFunc)(const char*, void*);

// A simple C++/CLI wrapper class to manage the CPython DLL.
public ref class PythonLoader
{
private:
    // Handle to the loaded DLL.
    HMODULE pyDllHandle;

    // Pointers to the native CPython functions.
    PyInitializeExFunc pyInitializeEx;
    PyFinalizeExFunc pyFinalizeEx;
    PyRunSimpleStringFlagsFunc pyRunSimpleStringFlags;

    

public:
    // Constructor
    PythonLoader() : pyDllHandle(NULL), pyInitializeEx(nullptr), pyFinalizeEx(nullptr), pyRunSimpleStringFlags(nullptr) {}

    // Destructor to ensure the DLL is unloaded properly.
    !PythonLoader()
    {
        // Unload the DLL if it was loaded.
        if (pyDllHandle != NULL)
        {
            FreeLibrary(pyDllHandle);
            pyDllHandle = NULL;
        }
    }


    // Helper method to find the Python DLL by checking common paths.
    System::String^ FindPythonDll()
    {
        // 1. Check for the PYTHONNET_PYDLL environment variable first,
        // which mimics the high-priority check in Python.NET.


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


    // Public method to load the Python DLL and initialize the interpreter.
    bool SetDllAndInitialize(System::String^ dllPath)
    {
        if (System::String::IsNullOrEmpty(dllPath))
        {
            System::Console::WriteLine("ERROR: DLL path is null or empty.");
            return false;
        }

        // Convert the managed System::String to a native wide-character string.
        pin_ptr<const wchar_t> wDllPath = PtrToStringChars(dllPath);

        // Load the Python DLL from the specified path.
        pyDllHandle = LoadLibraryW(wDllPath);
        if (pyDllHandle == NULL)
        {
            System::Console::WriteLine("ERROR: Could not load the Python DLL from '{0}'", dllPath);
            return false;
        }

        // Get the function pointers for the required CPython functions.
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

        // Initialize the Python interpreter.

        pin_ptr<const wchar_t> pWchar = PtrToStringChars(GetPythonHomeFromRegistry());
        Py_SetPythonHome(pWchar);
        pyInitializeEx(0); // Pass 0 to avoid installing signal handlers.
        System::Console::WriteLine("Python interpreter initialized successfully.");

        return true;
    }

    // Public method to execute a simple Python string.
    void RunString(System::String^ code)
    {
        if (pyRunSimpleStringFlags == nullptr)
        {
            System::Console::WriteLine("ERROR: Python interpreter is not initialized.");
            return;
        }

        // Convert managed string to native char* for execution.
        msclr::interop::marshal_context context;
        const char* nativeCode = context.marshal_as<const char*>(code);

        pyRunSimpleStringFlags(nativeCode, nullptr);
    }

    static String^ GetPythonHomeFromRegistry()
    {
        // Path to the Python core registry key. The version number will vary.
        String^ keyPath = "SOFTWARE\\Python\\PythonCore";
        String^ latestVersion = nullptr;

        // Open the registry key for HKEY_CURRENT_USER
        RegistryKey^ currentUserKey = Registry::CurrentUser->OpenSubKey(keyPath);
        if (currentUserKey != nullptr)
        {
            // Find the latest Python version installed by iterating through the keys
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

        // Also check HKEY_LOCAL_MACHINE for all-users installations.
        latestVersion = nullptr;
        RegistryKey^ localMachineKey = Registry::LocalMachine->OpenSubKey(keyPath);
        if (localMachineKey != nullptr)
        {
            // Find the latest Python version installed.
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
    

    // Public method to finalize the Python interpreter.
    /*void Finalize()
    {
        if (pyFinalizeEx != nullptr)
        {
            pyFinalizeEx();
            System::Console::WriteLine("Python interpreter finalized.");
        }
    }*/
};
//
//int main(array<System::String^>^ args)
//{
//    // Create an instance of the loader class.
//    PythonLoader^ loader = gcnew PythonLoader();
//    System::String^ pythonDllPath = nullptr;
//
//    try
//    {
//        // First, find the DLL.
//        pythonDllPath = loader->FindPythonDll();
//
//        // If a DLL was found, load it and initialize Python.
//        if (loader->SetDllAndInitialize(pythonDllPath))
//        {
//            // Run a simple Python script.
//            loader->RunString("import sys; print('Hello from Python. Version:', sys.version)");
//        }
//    }
//    finally
//    {
//        // Always finalize the Python interpreter.
//        loader->Finalize();
//    }
//
//    System::Console::WriteLine("Press any key to exit...");
//    System::Console::ReadKey();
//
//    return 0;
//}
