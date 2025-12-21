#include "pch.h"
#include "DynamicPyObject.h"


void DotPython::DynamicPyObject::Cleanup(bool isDeterministic)
{
    if (!m_disposed)
    {
        if (m_managedPyObject != nullptr)
        {
            if (isDeterministic)
            {
                delete m_managedPyObject;
            }
            m_managedPyObject = nullptr;
        }

        if (isDeterministic)
        {
            System::GC::SuppressFinalize(this);
        }
        m_disposed = true;
    }
}

bool DotPython::DynamicPyObject::TryInvokeCallable(ManagedPyObject^ pyCallable, cli::array<System::Object^>^ args, IReadOnlyCollection<String^>^ namedArgs, System::Object^% result)
{
    result = nullptr;
    if (!pyCallable->IsValid() || !PyCallable_Check(pyCallable->RawPointer))
    {
        if (PyErr_Occurred()) PyErr_Print();
        return false;
    }

    try
    {
        int totalArgs = args->Length;
        int namedArgCount = namedArgs->Count;
        int positionalArgCount = totalArgs - namedArgCount;
        if (!Py_IsInitialized()) {
            Console::WriteLine("Python not initialized!");
        }
        auto pyArgs = gcnew ManagedPyObject(PyTuple_New(positionalArgCount));
        for (int i = 0; i < positionalArgCount; i++)
        {
            auto managedPyObj = ExtensionMethods::ToPython(args[i]);
            PyTuple_SetItem(pyArgs->RawPointer, i, managedPyObj->Release());
        }

        auto pyKwargs = gcnew ManagedPyObject(PyDict_New());

        int i = 0;
        for each (auto argName in namedArgs)
        {
            auto argValue = args[positionalArgCount + i];

            auto managedPyKey = ExtensionMethods::ToPython(argName);
            auto mangedPyValue = ExtensionMethods::ToPython(argValue);

            PyDict_SetItem(pyKwargs->RawPointer, managedPyKey->Release(), mangedPyValue->Release());
            i++;
        }

        auto pyResult = gcnew ManagedPyObject(PyObject_Call(pyCallable->RawPointer, pyArgs->RawPointer, pyKwargs->RawPointer));

        if (!pyResult->IsValid())
        {
            if (PyErr_Occurred()) PyErr_Print();
            return false;
        }

        if (pyResult->RawPointer == Py_None)
        {
            return true;
        }


        result = ConvertToManagedObject(pyResult);

        return true;
    }
    catch (Exception^)
    {
        return false;
    }

}

DotPython::DynamicPyObject::DynamicPyObject(ManagedPyObject^ guard)
{
    PyObject* pyObject = guard->Release();
    m_managedPyObject = gcnew ManagedPyObject(pyObject);
}

DotPython::DynamicPyObject::~DynamicPyObject()
{
    Cleanup(true);
}

DotPython::DynamicPyObject::!DynamicPyObject()
{
    Cleanup(false);
}

System::String^ DotPython::DynamicPyObject::ToString()
{
    if (m_managedPyObject == nullptr)
    {
        return "Python Object (NULL)";
    }

    auto pyObjectRepr = gcnew ManagedPyObject(PyObject_Repr(m_managedPyObject->RawPointer));

    if (!pyObjectRepr->IsValid())
    {
        if (PyErr_Occurred()) PyErr_Print();
        return "Python Object (Error during repr)";
    }

    wchar_t* wideStr = PyUnicode_AsWideCharString(pyObjectRepr->RawPointer, nullptr);

    if (wideStr == nullptr)
    {
        if (PyErr_Occurred()) PyErr_Print();
        return "Python Object (Conversion Error)";
    }

    System::String^ managedString = gcnew System::String(wideStr);
    PyMem_Free(wideStr);

    return managedString;
}

bool DotPython::DynamicPyObject::TryGetMember(GetMemberBinder^ binder, Object^% result)
{
    result = nullptr;

    try
    {
        marshal_context context;
        const char* memberName = context.marshal_as<const char*>(binder->Name);
        auto pPyAttribute = gcnew ManagedPyObject(PyObject_GetAttrString(m_managedPyObject->RawPointer, memberName));

        if (!pPyAttribute->IsValid())
        {
            if (PyErr_Occurred()) {
                PyErr_Print();
                PyErr_Clear();
            }
            return false;
        }

        if (PyCallable_Check(pPyAttribute->RawPointer))
        {
            auto pyAttributeGuard = gcnew ManagedPyObject(pPyAttribute->RawPointer);
            result = gcnew DynamicPyObject(pyAttributeGuard);
        }
        else
        {
            result = ConvertToManagedObject(gcnew ManagedPyObject(pPyAttribute->RawPointer));
        }

        return true;
    }
    catch (Exception^ ex)
    {
        System::Diagnostics::Debug::WriteLine("Exception in TryGetMember: " + ex->Message);
        result = nullptr;
        return false;
    }

}

bool DotPython::DynamicPyObject::TrySetMember(SetMemberBinder^ binder, Object^ value)
{
    try
    {
        marshal_context context;
        const char* memberName = context.marshal_as<const char*>(binder->Name);

        auto pPyValue = ExtensionMethods::ToPython(value);
        if (!pPyValue->RawPointer) {
            return false;
        }


        int result = PyObject_SetAttrString(m_managedPyObject->RawPointer, memberName, pPyValue->RawPointer);

        return (result == 0);
    }
    catch (Exception^)
    {

        return false;
    }
}

bool DotPython::DynamicPyObject::TryInvokeMember(InvokeMemberBinder^ binder, array<Object^>^ args, Object^% result)
{
    result = nullptr;

    try
    {
        marshal_context context;
        const char* funcName = context.marshal_as<const char*>(binder->Name);
        auto pPyFunc = gcnew ManagedPyObject(PyObject_GetAttrString(m_managedPyObject->RawPointer, funcName), true);

        return this->TryInvokeCallable(pPyFunc, args, binder->CallInfo->ArgumentNames, result);
    }
    catch (Exception^)
    {
        return false;
    }

}

bool DotPython::DynamicPyObject::TryBinaryOperation(System::Dynamic::BinaryOperationBinder^ binder, System::Object^ arg, System::Object^% result)
{
    ManagedPyObject^ pPyLeft = m_managedPyObject;
    ManagedPyObject^ pPyRight = ExtensionMethods::ToPython(arg);
    ManagedPyObject^ pPyResult;

    result = nullptr;

    if (pPyRight == nullptr)
    {
        if (PyErr_Occurred()) PyErr_Print();
        return false;
    }

    try
    {
        switch (binder->Operation)
        {
        case System::Linq::Expressions::ExpressionType::Multiply:
            pPyResult = gcnew ManagedPyObject(PyNumber_Multiply(pPyLeft->RawPointer, pPyRight->RawPointer));
            break;
        case System::Linq::Expressions::ExpressionType::Divide:
            pPyResult = gcnew ManagedPyObject(PyNumber_TrueDivide(pPyLeft->RawPointer, pPyRight->RawPointer));
            break;
        case System::Linq::Expressions::ExpressionType::Add:
            pPyResult = gcnew ManagedPyObject(PyNumber_Add(pPyLeft->RawPointer, pPyRight->RawPointer));
            break;

        case System::Linq::Expressions::ExpressionType::Subtract:
            pPyResult = gcnew ManagedPyObject(PyNumber_Subtract(pPyLeft->RawPointer, pPyRight->RawPointer));
            break;
        case System::Linq::Expressions::ExpressionType::Modulo: // % operator
            pPyResult = gcnew ManagedPyObject(PyNumber_Remainder(pPyLeft->RawPointer, pPyRight->RawPointer));
            break;

        case System::Linq::Expressions::ExpressionType::LeftShift: // << operator
            pPyResult = gcnew ManagedPyObject(PyNumber_Lshift(pPyLeft->RawPointer, pPyRight->RawPointer));
            break;

        case System::Linq::Expressions::ExpressionType::RightShift: // >> operator
            pPyResult = gcnew ManagedPyObject(PyNumber_Rshift(pPyLeft->RawPointer, pPyRight->RawPointer));
            break;

        case System::Linq::Expressions::ExpressionType::And: // & operator
            pPyResult = gcnew ManagedPyObject(PyNumber_And(pPyLeft->RawPointer, pPyRight->RawPointer));
            break;

        case System::Linq::Expressions::ExpressionType::Or: // | operator
            pPyResult = gcnew ManagedPyObject(PyNumber_Or(pPyLeft->RawPointer, pPyRight->RawPointer));
            break;

        case System::Linq::Expressions::ExpressionType::ExclusiveOr: // ^ operator
            pPyResult = gcnew ManagedPyObject(PyNumber_Xor(pPyLeft->RawPointer, pPyRight->RawPointer));
            break;

        case System::Linq::Expressions::ExpressionType::Power: // ** operator
            pPyResult = gcnew ManagedPyObject(PyNumber_Power(pPyLeft->RawPointer, pPyRight->RawPointer, Py_None)); // The third argument is 'mod' and can be Py_None for standard power.
            break;

        case System::Linq::Expressions::ExpressionType::DivideAssign: // Floor division // operator
            pPyResult = gcnew ManagedPyObject(PyNumber_FloorDivide(pPyLeft->RawPointer, pPyRight->RawPointer));
            break;
        default:
            return false;
        }

        if (pPyResult->RawPointer == nullptr)
        {
            if (PyErr_Occurred()) PyErr_Print();
            return false;
        }

        result = ConvertToManagedObject(pPyResult);
        return true;
    }
    catch (System::Exception^)
    {
        return false;
    }
}

bool DotPython::DynamicPyObject::TryInvoke(InvokeBinder^ binder, array<System::Object^>^ args, System::Object^% result)
{
    return TryInvokeCallable(m_managedPyObject, args, binder->CallInfo->ArgumentNames, result);
}

DotPython::DynamicPyObject^ DotPython::DynamicPyObject::GetAttr(String^ attributeName)
{
    try
    {
        Interop::StringMarshal nativeAttributeName(attributeName);

        auto result = PyObject_GetAttrString(m_managedPyObject->RawPointer, nativeAttributeName);

        if (result == nullptr)
        {
            throw gcnew Exception(
                String::Format("Python object does not have attribute '{0}'.", attributeName));
        }

        return gcnew DynamicPyObject(gcnew ManagedPyObject(result));
    }
    catch (Exception^ ex)
    {
        Console::WriteLine("Managed Exception during GetAttr: {0}", ex->Message);
        return nullptr;
    }
}

bool DotPython::DynamicPyObject::TryGetIndex(System::Dynamic::GetIndexBinder^ binder, array<Object^>^ indexes, Object^% result)
{
    if (indexes == nullptr || indexes->Length != 1)
    {
        result = nullptr;
        return false;
    }

    Object^ managedKey = indexes[0];

    auto pyKey = ExtensionMethods::ToPython(managedKey);

    if (pyKey->RawPointer == nullptr)
    {
        result = nullptr;
        return false;
    }

    auto pyValue = gcnew ManagedPyObject(PyObject_GetItem(m_managedPyObject->RawPointer, pyKey->RawPointer));

    if (pyValue->RawPointer == nullptr)
    {
        PyErr_Clear();
        result = nullptr;
        return false;
    }

    result = ConvertToManagedObject(pyValue);

    return true;
}

bool DotPython::DynamicPyObject::TrySetIndex(System::Dynamic::SetIndexBinder^ binder, array<Object^>^ indexes, Object^ value)
{
    if (indexes == nullptr || indexes->Length != 1)
    {
        return false;
    }

    Object^ managedKey = indexes[0];
    auto pyKey = ExtensionMethods::ToPython(managedKey);
    auto pyValue = ExtensionMethods::ToPython(value);

    if (pyKey->RawPointer == nullptr || pyValue->RawPointer == nullptr)
    {
        return false;
    }

    int result = PyObject_SetItem(m_managedPyObject->RawPointer, pyKey->Release(), pyValue->Release());

    if (result == -1)
    {
        PyErr_Clear();
        return false;
    }

    return true;
}


//DynamicPyObject::DynamicPyObject(PyObject* pyObject)
//{
//    ManagedPyObject m_managedPyObject(pyObject);
//    m_pPyObject = pyObject;
//    Py_XINCREF(m_pPyObject);
//    m_disposed = false;
//}
