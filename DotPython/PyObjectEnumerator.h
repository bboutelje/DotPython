#pragma once

#include "pch.h"
#include <Python.h>
#include "DynamicPyObject.h"

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
        PyObjectEnumerator(ManagedPyObject^ pyEnumerable);
        

        virtual property DynamicPyObject^ Current
        {
            DynamicPyObject^ get();
            
        }

        virtual property Object^ Current_NonGeneric
        {
            System::Object^ get() = System::Collections::IEnumerator::Current::get
            {
                return Current;
            }
        }

        virtual bool MoveNext() override;
        

        virtual void Reset()
        {
            throw gcnew NotSupportedException("Python iterators cannot be reset.");
        }

    };
}