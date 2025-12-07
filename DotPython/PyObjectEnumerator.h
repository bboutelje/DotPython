#pragma once

#include <Python.h> // Include the CPython header
#include "DynamicPyObject.h" // Assuming this defines DynamicPyObject

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;

namespace DotPython {
    ref class PyObjectEnumerator : public DynamicPyObject, System::Collections::Generic::IEnumerator<DynamicPyObject^>
    {
    private:

        DynamicPyObject^ currentItem;
        ManagedPyObject^ pyIteratorObj;

    public:
        PyObjectEnumerator(ManagedPyObject^ pyEnumerable)
        {
            pyIteratorObj = gcnew ManagedPyObject(PyObject_GetIter(pyEnumerable->RawPointer));

            if (!pyIteratorObj->IsValid())
            {
                throw gcnew InvalidOperationException("The Python object is not iterable.");
            }

            currentItem = nullptr;
        }

        ~PyObjectEnumerator()
        {

        }

        virtual property DynamicPyObject^ Current
        {
            DynamicPyObject^ get()
            {
                if (currentItem == nullptr)
                {
                    throw gcnew InvalidOperationException("Enumeration not started or finished.");
                }
                return currentItem;
            }
        }

        virtual property Object^ Current_NonGeneric
        {
            System::Object^ get() = System::Collections::IEnumerator::Current::get
            {
                return Current;
            }
        }

        virtual bool MoveNext()
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

        virtual void Reset()
        {
            throw gcnew NotSupportedException("Python iterators cannot be reset.");
        }

    };
}