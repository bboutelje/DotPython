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
        
        

        static void Initialize()
        {
            try
            {

                auto loader = gcnew PythonLoader();

                auto pythonDllPath = loader->FindPythonDll();
                loader->SetDllAndInitialize(pythonDllPath);
            }
            catch (Exception^ ex)
            {
                int i = 0;
            }
        }

        static void Initialize(String^ pythonDllPath)
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


        /*ref class GILContext : System::IDisposable
        {
        public:
            PyGILState_STATE m_state;

            GILContext()
            {   
                m_state = PyGILState_Ensure();
            }

            ~GILContext()
            {
                PyGILState_Release(m_state);
            }
        };

        static GILContext^ GIL()
        {
            return gcnew GILContext();
        }*/



        /*static Object^ Execute(String^ code)
        {
            Interop::StringMarshal nativeCodeString(code);

            int result = PyRun_SimpleString(nativeCodeString);
            if (result != 0) {
                return NULL;
            }
            return gcnew DynamicPyObject(gcnew ManagedPyObject(& _Py_NoneStruct));
        }*/

        /*[returnvalue:System::Runtime::CompilerServices::DynamicAttribute]
        static Object^ List(System::Collections::IEnumerable^ input)
        {
            if (input == nullptr) {
                throw gcnew ArgumentNullException("input", "Input enumerable cannot be null");
            }

            auto pyList = gcnew ManagedPyObject(PyList_New(0));
            if (pyList == nullptr) {
                throw gcnew InvalidOperationException("Failed to create Python list");
            }

            try {
                for each (Object^ item in input) {

                    auto pyItem = ConvertToPythonObject(item);

                    if (!pyItem->IsValid()) {
                        throw gcnew InvalidOperationException("Failed to convert item to Python object");
                    }

                    if (PyList_Append(pyList->RawPointer, pyItem->RawPointer) != 0) {
                        throw gcnew InvalidOperationException("Failed to append to Python list");
                    }
                }

                return gcnew DynamicPyObjectCollection(pyList);
            }
            catch (Exception^) {
                throw;
            }
        }*/

        //[returnvalue:System::Runtime::CompilerServices::DynamicAttribute]
        //static Object^ Dict(System::Collections::IDictionary^ managedDict)
        //{
        //    auto pDict = gcnew ManagedPyObject(PyDict_New());
        //    if (!pDict->IsValid()) {
        //        Console::WriteLine("Error: Failed to create new Python dictionary.");
        //        return nullptr;
        //    }

        //    IDictionaryEnumerator^ enumerator = managedDict->GetEnumerator();
        //    try {
        //        while (enumerator->MoveNext()) {
        //            DictionaryEntry entry = enumerator->Entry;
        //            Object^ managedKey = entry.Key;
        //            Object^ managedValue = entry.Value;

        //            auto pKey = ConvertToPythonObject(managedKey);

        //            if (!pKey->IsValid() || pKey->RawPointer == Py_None) {
        //                Console::WriteLine("Error: Converted key resulted in a non-hashable/null Python object. Skipping entry.");
        //                continue;
        //            }

        //            auto pValue = ConvertToPythonObject(managedValue);

        //            if (!pValue->IsValid()) {
        //                Console::WriteLine("Error: Failed to convert value for key '{0}'.", managedKey);
        //                return nullptr;
        //            }

        //            if (PyDict_SetItem(pDict->RawPointer, pKey->RawPointer, pValue->RawPointer) < 0) {
        //                Console::WriteLine("Error: Failed to set item in Python dictionary.");
        //                return nullptr;
        //            }
        //        }
        //    }
        //    catch (Exception^ ex) {
        //        Console::WriteLine("An unexpected managed error occurred during iteration: {0}", ex->Message);
        //        return nullptr;
        //    }

        //    return gcnew DynamicPyObjectCollection(pDict);
        //}

        //[returnvalue:System::Runtime::CompilerServices::DynamicAttribute]
        //static Object^ Import(String^ moduleName)
        //{
        //    Interop::StringMarshal nativeModuleNameString(moduleName);

        //    auto pName = gcnew ManagedPyObject(PyUnicode_DecodeFSDefault(nativeModuleNameString));
        //    auto pModule = gcnew ManagedPyObject(PyImport_Import(pName->RawPointer));

        //    if (!pModule->IsValid())
        //    {
        //        PyErr_Print();

        //        if (PyErr_Occurred()) {
        //            PyErr_Clear();
        //        }

        //        throw gcnew InvalidOperationException(
        //            String::Format("Python module import failed for '{0}'. See console output for Python traceback.", moduleName)
        //        );
        //    }

        //    DynamicPyObject^ wrapper = gcnew DynamicPyObject(pModule);

        //    return wrapper;
        //}

    private:
        
        

    };
}
