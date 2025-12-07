#include "pch.h"
#include "PyContext.h"
#include "Purgatory.h"

DotPython::PyContext::PyContext()
{

    m_state = PyGILState_Ensure();
    Purgatory::DecrementAndClear();
}

DotPython::PyContext::~PyContext()
{
    PyGILState_Release(m_state);
}

Object^ DotPython::PyContext::Import(String^ moduleName)
{
    Interop::StringMarshal nativeModuleNameString(moduleName);

    auto pName = gcnew ManagedPyObject(PyUnicode_DecodeFSDefault(nativeModuleNameString));
    auto pModule = gcnew ManagedPyObject(PyImport_Import(pName->RawPointer));

    if (!pModule->IsValid())
    {
        PyErr_Print();

        if (PyErr_Occurred()) {
            PyErr_Clear();
        }

        throw gcnew InvalidOperationException(
            String::Format("Python module import failed for '{0}'. See console output for Python traceback.", moduleName)
        );
    }

    DynamicPyObject^ wrapper = gcnew DynamicPyObject(pModule);

    return wrapper;
}

Object^ DotPython::PyContext::List(System::Collections::IEnumerable^ input)
{
    if (input == nullptr) {
        throw gcnew ArgumentNullException("input", "Input enumerable cannot be null");
    }

    auto pyList = gcnew ManagedPyObject(PyList_New(0));
    if (pyList == nullptr) {
        throw gcnew InvalidOperationException("Failed to create Python list");
    }

    try {
        for each (Object ^ item in input) {

            auto pyItem = ConvertToPythonObject(item);

            if (!pyItem->IsValid()) {
                throw gcnew InvalidOperationException("Failed to convert item to Python object");
            }

            if (PyList_Append(pyList->RawPointer, pyItem->RawPointer) != 0) {
                throw gcnew InvalidOperationException("Failed to append to Python list");
            }
        }

        return gcnew DynamicPyObjectCollection(pyList);
    }
    catch (Exception^) {
        throw;
    }
}

Object^ DotPython::PyContext::Dict(System::Collections::IDictionary^ managedDict)
{
    auto pDict = gcnew ManagedPyObject(PyDict_New());
    if (!pDict->IsValid()) {
        Console::WriteLine("Error: Failed to create new Python dictionary.");
        return nullptr;
    }

    IDictionaryEnumerator^ enumerator = managedDict->GetEnumerator();
    try {
        while (enumerator->MoveNext()) {
            DictionaryEntry entry = enumerator->Entry;
            Object^ managedKey = entry.Key;
            Object^ managedValue = entry.Value;

            auto pKey = ConvertToPythonObject(managedKey);

            if (!pKey->IsValid() || pKey->RawPointer == Py_None) {
                Console::WriteLine("Error: Converted key resulted in a non-hashable/null Python object. Skipping entry.");
                continue;
            }

            auto pValue = ConvertToPythonObject(managedValue);

            if (!pValue->IsValid()) {
                Console::WriteLine("Error: Failed to convert value for key '{0}'.", managedKey);
                return nullptr;
            }

            if (PyDict_SetItem(pDict->RawPointer, pKey->Release(), pValue->Release()) < 0) {
                Console::WriteLine("Error: Failed to set item in Python dictionary.");
                return nullptr;
            }
        }
    }
    catch (Exception^ ex) {
        Console::WriteLine("An unexpected managed error occurred during iteration: {0}", ex->Message);
        return nullptr;
    }

    return gcnew DynamicPyObjectCollection(pDict);
}

void DotPython::PyContext::Execute(String^ code)
{
    Interop::StringMarshal nativeCodeString(code);

    int result = PyRun_SimpleString(nativeCodeString);
    if (result != 0) {

    }
    
}
