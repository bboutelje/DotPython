#pragma once
#include <Python.h>
#include "DynamicPyObject.h"
#include "DynamicPyObjectCollection.h"

using namespace System;
using namespace System::Collections;
using namespace System::Runtime::InteropServices;

namespace DotPython {

    public ref class PyContext : System::IDisposable
    {
    private:
        PyGILState_STATE m_state;
        
    public:
        PyContext();
        ~PyContext();

        [returnvalue:System::Runtime::CompilerServices::DynamicAttribute]
        Object^ Import(String^ moduleName);
        [returnvalue:System::Runtime::CompilerServices::DynamicAttribute]
        Object^ List(System::Collections::IEnumerable^ input);
        [returnvalue:System::Runtime::CompilerServices::DynamicAttribute]
        Object^ Dict(System::Collections::IDictionary^ managedDict);

        void Execute(String^ code);
        
        void AddToSysPath(String^ path);

    };


}