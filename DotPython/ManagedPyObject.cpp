#include "pch.h"
#include "ManagedPyObject.h"
#include "UnmanagedPyObject.h"
#include "Purgatory.h"

DotPython::ManagedPyObject::ManagedPyObject()
{
    _pyObject = nullptr;
}

DotPython::ManagedPyObject::ManagedPyObject(PyObject* pyObject)
{

    _pyObject = pyObject;
    _isBorrowed = false;
}

DotPython::ManagedPyObject::ManagedPyObject(PyObject* pyObject, bool isBorrowed)
{
    _pyObject = pyObject;
	_isBorrowed = isBorrowed;
}

DotPython::ManagedPyObject::~ManagedPyObject()
{
    this->!ManagedPyObject();
    GC::SuppressFinalize(this);
}

DotPython::ManagedPyObject::!ManagedPyObject()
{
    if (_isBorrowed)
    {
        _pyObject = nullptr;
        return;
	}
    if (_pyObject != nullptr)
    {
        try
        {
            if (PyGILState_Check())
            {
                long ref_count = Py_REFCNT(_pyObject);

                if (ref_count <= 0) {
                    // This should never happen, but just in case

                    return;
                }
                Py_DecRef(_pyObject);
                _pyObject = nullptr;
            }
            else
            {
                Purgatory::MarkForDecrementation(_pyObject);
            }
        }
        catch (Exception^)
        {
            int i = 0;
        }
    }
    
}


bool DotPython::ManagedPyObject::IsValid()
{
    return _pyObject != nullptr;
}

PyObject* DotPython::ManagedPyObject::RawPointer::get()
{
    return _pyObject;
}

PyObject* DotPython::ManagedPyObject::Release()
{
    auto temp = _pyObject;
    _pyObject = nullptr;
    return temp;
}
