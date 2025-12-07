
#include "pch.h"
#include "UnmanagedPyObject.h"

UnmanagedPyObject::UnmanagedPyObject()
{

}

UnmanagedPyObject::UnmanagedPyObject(PyObject* pyObject)
{
	_pyObject = pyObject;
}

UnmanagedPyObject::UnmanagedPyObject(PyObject* pyObject, bool isBorrowed)
{
    _pyObject = pyObject;
    if (isBorrowed)
    {
        Py_IncRef(_pyObject);
    }
}

UnmanagedPyObject::~UnmanagedPyObject()
{
    if (_pyObject != nullptr)
    {
        try
        {
            Py_DecRef(_pyObject);
        }
        catch (std::exception())
        {
            int i = 0;
        }
    }
    _pyObject = nullptr;
}

PyObject* UnmanagedPyObject::RawPointer()
{
    return _pyObject;
}

bool UnmanagedPyObject::IsValid()
{
    return _pyObject != nullptr;
}


