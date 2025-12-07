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

DotPython::DynamicPyObject::DynamicPyObject(ManagedPyObject^ guard)
{
    PyObject* pyObject = guard->Release();
    m_managedPyObject = gcnew ManagedPyObject(pyObject);
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


//DynamicPyObject::DynamicPyObject(PyObject* pyObject)
//{
//    ManagedPyObject m_managedPyObject(pyObject);
//    m_pPyObject = pyObject;
//    Py_XINCREF(m_pPyObject);
//    m_disposed = false;
//}
