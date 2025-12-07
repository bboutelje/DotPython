#pragma once
#include <Python.h>
#include <optional>

using namespace System;

#include "pch.h"


public class UnmanagedPyObject {
private:
    PyObject* _pyObject;
    bool _isBorrowed;
public:

    UnmanagedPyObject();
    UnmanagedPyObject(PyObject* pyObject);
    UnmanagedPyObject(PyObject* pyObject, bool takeOwnership);
    ~UnmanagedPyObject();
    
    PyObject* RawPointer();

    bool IsValid();

    // Method to release ownership (e.g., for returning to Python)
    PyObject* Release();
};
