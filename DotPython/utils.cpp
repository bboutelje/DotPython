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

    // Check if the object is a Python long (integer)
    if (PyLong_Check(pyObjectGuard->RawPointer)) {
        return PyLong_AsLong(pyObjectGuard->RawPointer);
    }

    // Check if the object is a Python float
    if (PyFloat_Check(pyObjectGuard->RawPointer)) {
        return PyFloat_AsDouble(pyObjectGuard->RawPointer);
    }


    // Check if the object is a Python string
    if (PyUnicode_Check(pyObjectGuard->RawPointer)) {
        Py_ssize_t size;
        wchar_t* buffer = PyUnicode_AsWideCharString(pyObjectGuard->RawPointer, &size);
        if (buffer != nullptr) {
            String^ result = gcnew String(buffer, 0, (int)size);
            PyMem_Free((void*)buffer); // Clean up the wide char string
            return result;
        }
    }

    


    if (PySequence_Check(pyObjectGuard->RawPointer)) // <-- Use the standard PySequence_Check function
    {
        return gcnew DotPython::DynamicPyObjectCollection(pyObjectGuard);
        /*if (PyList_Check(pyObjectGuard->RawPointer) || PyTuple_Check(pyObjectGuard->RawPointer))
        {
            Py_ssize_t size = PyList_Check(pyObjectGuard->RawPointer) ? PyList_Size(pyObjectGuard->RawPointer) : PyTuple_Size(pyObjectGuard->RawPointer);

            array<System::Object^>^ managedArray = gcnew array<System::Object^>((int)size);

            for (Py_ssize_t i = 0; i < size; ++i)
            {
                PyObject* pPyItem = PyList_Check(pyObjectGuard->RawPointer) ? PyList_GetItem(pyObjectGuard->RawPointer, i) : PyTuple_GetItem(pyObjectGuard->RawPointer, i);
                managedArray[i] = ConvertToManagedObject(gcnew ManagedPyObject(pPyItem, true));
            }
            return managedArray;
        }

        return gcnew DotPython::DynamicPyObjectCollection(pyObjectGuard);*/
    }

    
    return gcnew DotPython::DynamicPyObject(pyObjectGuard);
    
}

System::Object^ DotPython::ConvertListToManagedArray(PyObject* pPyList)
{
    // CRITICAL: Ensure it is a list before proceeding
    if (!PyList_Check(pPyList)) return nullptr;

    Py_ssize_t size = PyList_Size(pPyList);

    // Create a managed array of System::Object^ to hold the items
    array<System::Object^>^ managedArray = gcnew array<System::Object^>((int)size);

    for (Py_ssize_t i = 0; i < size; ++i)
    {
        // Get the list item (BORROWED reference)
        PyObject* pPyItem = PyList_GetItem(pPyList, i);

        // Recursively convert the item. Since the reference is BORROWED, 
        // we do NOT call Py_DECREF on pPyItem.
        managedArray[i] = ConvertToManagedObject(gcnew ManagedPyObject(pPyItem));

        // Error handling for recursive call
        if (managedArray[i] == nullptr && PyErr_Occurred())
        {
            // If the inner conversion failed, propagate the failure
            return nullptr;
        }
    }
    return managedArray;
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
   
    // Check for System::Single (C# float)
    if (managedObject->GetType() == System::Single::typeid)
    {
        // 1. Unbox the managed System::Single to a C++ float
        float val = System::Convert::ToSingle(managedObject);

        auto pPyFloat = gcnew ManagedPyObject(PyFloat_FromDouble((double)val));


        if (pPyFloat->RawPointer == NULL)
        {
            return gcnew ManagedPyObject(NULL);
        }

        return pPyFloat;
    }

    // Check if the managed object is a double
    if (managedObject->GetType() == double::typeid) {
        double dblValue = safe_cast<double>(managedObject);
        return gcnew ManagedPyObject(PyFloat_FromDouble(dblValue));
    }

    // Check if the managed object is an integer
    if (managedObject->GetType() == int::typeid) {
        int intValue = safe_cast<int>(managedObject);
        return gcnew ManagedPyObject(PyLong_FromLong(intValue));
    }

    // Check if the managed object is a String
    if (managedObject->GetType() == String::typeid) {
        String^ strValue = safe_cast<String^>(managedObject);

        // Convert the managed string to a Python Unicode string
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
        auto pPyList = gcnew ManagedPyObject(PyList_New(0));

        for each (System::Object ^ item in collection)
        {
            // Recursively convert each item in the collection
            auto pPyItem = ConvertToPythonObject(item);
            if (pPyItem != nullptr)
            {
                
                PyList_Append(pPyList->RawPointer, pPyItem->Release());
            }
        }

        return pPyList;
    }


    // --- End of NEW LOGIC ---

    // ... Handle unknown types, return Py_None, or throw ...
    PyErr_SetString(PyExc_TypeError, "Unsupported managed type for conversion.");
    return gcnew ManagedPyObject(& _Py_NoneStruct); // Default fallback


    // You would add more cases here for other managed types, such as arrays, lists, etc.
    // For now, if the type is not recognized, return an error.
    
    
}

//const std::unique_ptr<char> ConvertStringToConstChar(String^ managedString)
//{
//    IntPtr unmanagedPointer = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(managedString);
//    const char* c_string = static_cast<const char*>(unmanagedPointer.ToPointer());
//    return std::make_unique<char>(c_string);
//}
