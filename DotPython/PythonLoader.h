#pragma once
#include <iostream>
#include <windows.h>
#include <vcclr.h>
#include <vector>
#include <msclr\marshal.h>
#include <Python.h>
#include <string>
#include "PythonInitializer.h"

using namespace System;
using namespace System::IO;
using namespace System::Linq;
using namespace Microsoft::Win32;

typedef int(__cdecl* PyInitializeExFunc)(int);
typedef int(__cdecl* PyFinalizeExFunc)(void);
typedef void(__cdecl* PyRunSimpleStringFlagsFunc)(const char*, void*);

[System::ComponentModel::EditorBrowsable(System::ComponentModel::EditorBrowsableState::Never)]
public ref class PythonLoader
{
private:
    HMODULE pyDllHandle;

    PyInitializeExFunc pyInitializeEx;
    PyFinalizeExFunc pyFinalizeEx;
    PyRunSimpleStringFlagsFunc pyRunSimpleStringFlags;
    

internal:
    PythonLoader() : pyDllHandle(NULL), pyInitializeEx(nullptr), pyFinalizeEx(nullptr), pyRunSimpleStringFlags(nullptr) {}

    !PythonLoader();

    System::String^ FindPythonDllInPath();
    
    bool SetDllAndInitialize(System::String^ dllPath);

};
