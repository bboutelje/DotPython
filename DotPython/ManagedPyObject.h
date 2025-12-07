#pragma once
#include <Python.h>
#include <optional>

using namespace System;

namespace DotPython
{


    public ref class ManagedPyObject : IDisposable {
    private:
        PyObject* _pyObject;
        bool _isBorrowed;
    public:

        ManagedPyObject();
        ManagedPyObject(PyObject* pyObject);
        ManagedPyObject(PyObject* pyObject, bool isBorrowed);
        
        ~ManagedPyObject();
        !ManagedPyObject();
        property PyObject* RawPointer { PyObject* get(); }

        bool IsValid();

        PyObject* Release();



    };
}
