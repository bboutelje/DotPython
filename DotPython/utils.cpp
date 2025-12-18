#include "pch.h"
#include "utils.h"
#include "Marshal.h"
#include "DynamicPyObject.h"
#include "DynamicPyObjectCollection.h"
#include <Python.h>
#include <abstract.h>

Object^ DotPython::ConvertToManagedObject(ManagedPyObject^ pyObjectGuard)
{
    if (pyObjectGuard == nullptr || pyObjectGuard->RawPointer == Py_None) {
        return nullptr;
    }

    if (PyLong_Check(pyObjectGuard->RawPointer)) {
        return PyLong_AsLong(pyObjectGuard->RawPointer);
    }

    if (PyFloat_Check(pyObjectGuard->RawPointer)) {
        return PyFloat_AsDouble(pyObjectGuard->RawPointer);
    }

    if (PyUnicode_Check(pyObjectGuard->RawPointer)) {
        Py_ssize_t size;
        wchar_t* buffer = PyUnicode_AsWideCharString(pyObjectGuard->RawPointer, &size);
        if (buffer != nullptr) {
            String^ result = gcnew String(buffer, 0, (int)size);
            PyMem_Free((void*)buffer); // Clean up the wide char string
            return result;
        }
    }

    if (PySequence_Check(pyObjectGuard->RawPointer))
    {
        return gcnew DotPython::DynamicPyObjectCollection(pyObjectGuard);
    }

    
    return gcnew DotPython::DynamicPyObject(pyObjectGuard);
    
}

DotPython::ManagedPyObject^ DotPython::ConvertToPythonObject(Object^ managedObject)
{
    if (managedObject == nullptr) {
        Py_IncRef(Py_None);
        return gcnew ManagedPyObject(Py_None);
    }

    DotPython::DynamicPyObject^ dynamicWrapper = dynamic_cast<DotPython::DynamicPyObject^>(managedObject);

    if (dynamicWrapper != nullptr)
    {
        auto pPyObject = gcnew ManagedPyObject(dynamicWrapper->GetManagedPyObject()->RawPointer);
        return pPyObject;
    }
   
    if (managedObject->GetType() == System::Single::typeid)
    {
        float val = System::Convert::ToSingle(managedObject);
        auto pPyFloat = gcnew ManagedPyObject(PyFloat_FromDouble((double)val));


        if (pPyFloat->RawPointer == NULL)
        {
            return gcnew ManagedPyObject(NULL);
        }

        return pPyFloat;
    }

    if (managedObject->GetType() == double::typeid) {
        double dblValue = safe_cast<double>(managedObject);
        return gcnew ManagedPyObject(PyFloat_FromDouble(dblValue));
    }

    if (managedObject->GetType() == int::typeid) {
        int intValue = safe_cast<int>(managedObject);
        return gcnew ManagedPyObject(PyLong_FromLong(intValue));
    }

    if (managedObject->GetType() == String::typeid) {
        String^ strValue = safe_cast<String^>(managedObject);

        msclr::interop::marshal_context context;
        const wchar_t* wstr = context.marshal_as<const wchar_t*>(strValue);
        return gcnew ManagedPyObject(PyUnicode_FromWideChar(wstr, strValue->Length));
    }

    if (managedObject->GetType() == System::Boolean::typeid)
    {
        bool val = safe_cast<bool>(managedObject);
        auto pyBool = gcnew ManagedPyObject(val ? Py_True : Py_False);
        
        return pyBool;
    }

    if (managedObject->GetType() == System::Char::typeid)
    {
        wchar_t managedChar = System::Convert::ToChar(managedObject);

        auto pPyString = gcnew ManagedPyObject(PyUnicode_FromWideChar(&managedChar, 1));

        return pPyString;
    }

    System::Collections::IDictionary^ netDictionary = dynamic_cast<System::Collections::IDictionary^>(managedObject);
    if (netDictionary != nullptr)
    {
        auto pyDict = gcnew ManagedPyObject(PyDict_New());
        if (pyDict->RawPointer == nullptr) {
            return nullptr;
        }

        for each (System::Collections::DictionaryEntry entry in netDictionary)
        {
            auto pyKey = ConvertToPythonObject(entry.Key);
            auto pyValue = ConvertToPythonObject(entry.Value);

            
            if (pyKey->RawPointer && pyValue->RawPointer)
            {
                if (PyDict_SetItem(pyDict->RawPointer, pyKey->Release(), pyValue->Release()) == -1)
                {
                    return nullptr;
                }
            }

        }

        return pyDict;
    }


    System::Collections::ICollection^ collection = dynamic_cast<System::Collections::ICollection^>(managedObject);
    if (collection != nullptr)
    {
        auto pPyList = gcnew ManagedPyObject(PyList_New(collection->Count));

        Py_ssize_t i = 0;
        for each (System::Object ^ item in collection)
        {
            auto pPyItem = ConvertToPythonObject(item);
            if (pPyItem != nullptr)
            {
                PyList_SetItem(pPyList->RawPointer, i, pPyItem->Release());
            }
            i++;
        }

        return pPyList;
    }


    PyErr_SetString(PyExc_TypeError, "Unsupported managed type for conversion.");
    return gcnew ManagedPyObject(& _Py_NoneStruct);

}
