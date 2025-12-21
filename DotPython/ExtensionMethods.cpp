#include "pch.h"
#include "ExtensionMethods.h"
#include "DynamicPyObject.h"

DotPython::ManagedPyObject^ DotPython::ExtensionMethods::ToManagedPyObject(Object^ managedObject)
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

    Type^ type = managedObject->GetType();
    if (type == System::Single::typeid)
    {
        float val = System::Convert::ToSingle(managedObject);
        auto pPyFloat = gcnew ManagedPyObject(PyFloat_FromDouble((double)val));


        if (pPyFloat->RawPointer == NULL)
        {
            return gcnew ManagedPyObject(NULL);
        }

        return pPyFloat;
    }

    if (type == double::typeid) {
        double dblValue = safe_cast<double>(managedObject);
        return gcnew ManagedPyObject(PyFloat_FromDouble(dblValue));
    }

    if (type == int::typeid) {
        int intValue = safe_cast<int>(managedObject);
        return gcnew ManagedPyObject(PyLong_FromLong(intValue));
    }

    if (type == String::typeid) {
        String^ strValue = safe_cast<String^>(managedObject);

        msclr::interop::marshal_context context;
        const wchar_t* wstr = context.marshal_as<const wchar_t*>(strValue);
        return gcnew ManagedPyObject(PyUnicode_FromWideChar(wstr, strValue->Length));
    }

    if (type == System::Boolean::typeid)
    {
        bool val = safe_cast<bool>(managedObject);
        auto pyBool = gcnew ManagedPyObject(val ? Py_True : Py_False);

        return pyBool;
    }

    if (type == System::Char::typeid)
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
            auto pyKey = ToManagedPyObject(entry.Key);
            auto pyValue = ToManagedPyObject(entry.Value);


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


    if (type->IsGenericType &&
        type->GetGenericTypeDefinition() == System::Collections::Generic::HashSet<Object^>::typeid->GetGenericTypeDefinition())
    {
        System::Collections::IEnumerable^ hashSet = dynamic_cast<System::Collections::IEnumerable^>(managedObject);
        if (hashSet != nullptr)
        {
            auto pPySet = gcnew ManagedPyObject(PySet_New(NULL));
            if (!pPySet->IsValid()) {
                return nullptr;
            }
            for each (System::Object ^ item in hashSet)
            {
                auto pPyItem = ToManagedPyObject(item);
                if (pPyItem != nullptr)
                {
                    PySet_Add(pPySet->RawPointer, pPyItem->RawPointer);
                }
            }
            return pPySet;
        }
    }

    System::Collections::ICollection^ collection = dynamic_cast<System::Collections::ICollection^>(managedObject);
    if (collection != nullptr)
    {
        auto pPyList = gcnew ManagedPyObject(PyList_New(collection->Count));

        Py_ssize_t i = 0;
        for each (System::Object ^ item in collection)
        {
            auto pPyItem = ToManagedPyObject(item);
            if (pPyItem != nullptr)
            {
                if (PyList_SetItem(pPyList->RawPointer, i, pPyItem->Release()) == -1)
                {
                    return nullptr;
                }
            }
            i++;
        }

        return pPyList;
    }

    System::Collections::IEnumerable^ enumerable = dynamic_cast<System::Collections::IEnumerable^>(managedObject);
    if (enumerable != nullptr)
    {
        auto pPyList = gcnew ManagedPyObject(PyList_New(0));

        for each (System::Object ^ item in enumerable)
        {
            auto pPyItem = ToManagedPyObject(item);
            if (pPyItem != nullptr)
            {
                PyList_Append(pPyList->RawPointer, pPyItem->RawPointer);
            }

        }

        return pPyList;
    }


    PyErr_SetString(PyExc_TypeError, "Unsupported managed type for conversion.");
    return gcnew ManagedPyObject(&_Py_NoneStruct);

}

System::Object^ DotPython::ExtensionMethods::ToPython(System::Object^ managedObject)
{
	return gcnew DynamicPyObject(ToManagedPyObject(managedObject));
}
