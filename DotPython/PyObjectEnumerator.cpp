#include "pch.h"
#include "PyObjectEnumerator.h"

DotPython::PyObjectEnumerator::PyObjectEnumerator(ManagedPyObject^ pyEnumerable)
{
    pyIteratorObj = gcnew ManagedPyObject(PyObject_GetIter(pyEnumerable->RawPointer));

    if (!pyIteratorObj->IsValid())
    {
        throw gcnew InvalidOperationException("The Python object is not iterable.");
    }

    currentItem = nullptr;
}

bool DotPython::PyObjectEnumerator::MoveNext()
{
    auto py_next_item = gcnew ManagedPyObject(PyIter_Next(pyIteratorObj->RawPointer));

    if (py_next_item->IsValid())
    {
        currentItem = gcnew DynamicPyObject(py_next_item);
        return true;
    }
    else if (PyErr_Occurred())
    {
        throw gcnew InvalidOperationException("An error occurred during Python iteration.");
    }

    currentItem = nullptr;
    return false;
}

DotPython::DynamicPyObject^ DotPython::PyObjectEnumerator::Current::get()
{
    if (currentItem == nullptr)
    {
        throw gcnew InvalidOperationException("Enumeration not started or finished.");
    }
    return currentItem;
}