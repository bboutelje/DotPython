#include "pch.h"
#include "PythonInitializer.h"


void DotPython::Python::Initialize()
{
    try
    {
        auto loader = gcnew PythonLoader();

        auto pythonDllPath = loader->FindPythonDllInPath();
        loader->SetDllAndInitialize(pythonDllPath);
    }
    catch (Exception^ ex)
    {
        int i = 0;
    }
}

void DotPython::Python::Initialize(String^ pythonDllPath)
{
    try
    {
        auto loader = gcnew PythonLoader();
        loader->SetDllAndInitialize(pythonDllPath);
    }
    catch (Exception^ ex)
    {
        int i = 0;
    }
}
