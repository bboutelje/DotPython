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

typedef int(__cdecl* PyInitializeExFunc)(int);
typedef int(__cdecl* PyFinalizeExFunc)(void);
typedef void(__cdecl* PyRunSimpleStringFlagsFunc)(const char*, void*);

public ref class PythonLoader
{
private:
    HMODULE pyDllHandle;

    PyInitializeExFunc pyInitializeEx;
    PyFinalizeExFunc pyFinalizeEx;
    PyRunSimpleStringFlagsFunc pyRunSimpleStringFlags;

public:
    PythonLoader() : pyDllHandle(NULL), pyInitializeEx(nullptr), pyFinalizeEx(nullptr), pyRunSimpleStringFlags(nullptr) {}

    !PythonLoader();
    

    System::String^ FindPythonDll();
    
    bool SetDllAndInitialize(System::String^ dllPath);
    

    static String^ GetPythonHomeFromRegistry();
    
    

    
};
