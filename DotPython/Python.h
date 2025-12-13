#pragma once
#include <memory>
#include <Python.h>
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>
#include "DynamicPyObject.h"
#include "DynamicPyObjectCollection.h"
#include "PythonLoader.h"
#include "utils.h"
#include "StringMarshal.h"

using namespace System;
using namespace System::Collections;
using namespace System::Runtime::InteropServices;

namespace DotPython {

    public ref class Python sealed
    {
    
    public:

        static void Initialize();
        

        static void Initialize(String^ pythonDllPath);
        

    };
}
