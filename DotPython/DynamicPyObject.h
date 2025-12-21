#pragma once

#include <Python.h>
#include <msclr/marshal.h>
#include <msclr/auto_gcroot.h>
#include <cliext/vector>
#include <vcclr.h>
#include "utils.h"
#include "ManagedPyObject.h"
#include "StringMarshal.h"
#include "UnmanagedPyObject.h"
#include "ExtensionMethods.h"

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Dynamic;
using namespace System::Linq::Expressions;
using namespace msclr::interop;

namespace DotPython {

    [System::ComponentModel::EditorBrowsable(System::ComponentModel::EditorBrowsableState::Never)]
    public ref class DynamicPyObject : public DynamicObject, IDisposable
    {
    protected:
        ManagedPyObject^ m_managedPyObject;

    private:

        bool m_disposed;
        void Cleanup(bool disposing);
        bool TryInvokeCallable(ManagedPyObject^ pyCallable, cli::array<System::Object^>^ args,
            IReadOnlyCollection<String^>^ namedArgs, System::Object^% result);
        
    public:

        DynamicPyObject() {}
        DynamicPyObject(ManagedPyObject^ guard);
        ~DynamicPyObject();
        !DynamicPyObject();

        ManagedPyObject^ GetManagedPyObject() { return m_managedPyObject; }

        virtual System::String^ ToString() override;


        virtual bool TryGetMember(GetMemberBinder^ binder, [System::Runtime::InteropServices::OutAttribute] Object^% result) override;
        virtual bool TrySetMember(SetMemberBinder^ binder, Object^ value) override;

        virtual bool TryInvokeMember(InvokeMemberBinder^ binder, array<Object^>^ args, 
            [System::Runtime::InteropServices::OutAttribute] Object^% result) override;
        virtual bool TryBinaryOperation(System::Dynamic::BinaryOperationBinder^ binder, 
            System::Object^ arg, [System::Runtime::InteropServices::OutAttribute] System::Object^% result) override;

        virtual bool TryInvoke(InvokeBinder^ binder, array<System::Object^>^ args, [System::Runtime::InteropServices::OutAttribute] System::Object^% result) override;
        
        DynamicPyObject^ GetAttr(String^ attributeName);
        

        virtual bool TryGetIndex(
            System::Dynamic::GetIndexBinder^ binder,
            array<Object^>^ indexes,
            [System::Runtime::InteropServices::Out] Object^% result) override;
        

        virtual bool TrySetIndex(
            System::Dynamic::SetIndexBinder^ binder,
            array<Object^>^ indexes,
            Object^ value) override;
        
    };
}
