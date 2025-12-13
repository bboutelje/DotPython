#pragma once
#include "DynamicPyObject.h"
#include "PyObjectEnumerator.h"

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Dynamic;
using namespace System::Linq::Expressions;

namespace DotPython {
    public ref class DynamicPyObjectCollection : public DynamicPyObject, System::Collections::Generic::IEnumerable<DynamicPyObject^>
    {

    public:

        DynamicPyObjectCollection(ManagedPyObject^ pyObject) : DynamicPyObject(pyObject) {}

        virtual System::Collections::Generic::IEnumerator<DynamicPyObject^>^ GetEnumerator() override;
        

        virtual System::Collections::IEnumerator^ GetEnumerator_NonGeneric()
            = System::Collections::IEnumerable::GetEnumerator
        {
            return GetEnumerator();
        }

    };
}