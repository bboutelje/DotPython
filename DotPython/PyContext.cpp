#include "pch.h"
#include "PyContext.h"
#include "Purgatory.h"
#include "Python.h"

DotPython::PyContext::PyContext()
{
	if (!Py_IsInitialized())
    	Python::Initialize();
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
        PyObject* pType = nullptr;
        PyObject* pValue = nullptr;
        PyObject* pTraceback = nullptr;

        PyErr_Fetch(&pType, &pValue, &pTraceback);

        auto type = gcnew ManagedPyObject(pType);
        auto value = gcnew ManagedPyObject(pValue);
        auto traceback = gcnew ManagedPyObject(pTraceback);

        String^ errorMessage = "Unknown Python error";

        if (pValue != nullptr)
        {
            auto str = gcnew ManagedPyObject(PyObject_Str(pValue));
            if (str->IsValid())
            {
                const char* errorStr = PyUnicode_AsUTF8(str->RawPointer);
                if (errorStr != nullptr)
                {
                    errorMessage = gcnew String(errorStr);
                }
                
            }
        }

        if (type->IsValid())
        {
            if (PyErr_GivenExceptionMatches(type->RawPointer, PyExc_ImportError) ||
                PyErr_GivenExceptionMatches(type->RawPointer, PyExc_ModuleNotFoundError))
            {
                throw gcnew System::IO::FileNotFoundException(
                    String::Format("Python module '{0}' not found: {1}", moduleName, errorMessage)
                );
            }
            else if (PyErr_GivenExceptionMatches(type->RawPointer, PyExc_SyntaxError))
            {
                throw gcnew System::FormatException(
                    String::Format("Syntax error in Python module '{0}': {1}", moduleName, errorMessage)
                );
            }
        }

        throw gcnew Exception(
            String::Format("Python module import failed for '{0}': {1}", moduleName, errorMessage)
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
        throw gcnew OutOfMemoryException("Failed to create Python list");
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

void DotPython::PyContext::AddToSysPath(String^ path)
{
    // Convert managed string to native
    pin_ptr<const wchar_t> wchPath = PtrToStringChars(path);

    // Get sys module
    PyObject* sysModule = PyImport_ImportModule("sys");
    if (sysModule == nullptr) {
        PyErr_Print();
        throw gcnew InvalidOperationException("Failed to import sys module");
    }

    // Get sys.path list
    PyObject* sysPath = PyObject_GetAttrString(sysModule, "path");
    if (sysPath == nullptr) {
        Py_DECREF(sysModule);
        PyErr_Print();
        throw gcnew InvalidOperationException("Failed to get sys.path");
    }

    // Convert path to Python string
    PyObject* pyPath = PyUnicode_FromWideChar(wchPath, -1);
    if (pyPath == nullptr) {
        Py_DECREF(sysPath);
        Py_DECREF(sysModule);
        PyErr_Print();
        throw gcnew InvalidOperationException("Failed to convert path to Python string");
    }

    // Insert at beginning of sys.path (index 0 = highest priority)
    int result = PyList_Insert(sysPath, 0, pyPath);

    // Cleanup
    Py_DECREF(pyPath);
    Py_DECREF(sysPath);
    Py_DECREF(sysModule);

    if (result != 0) {
        PyErr_Print();
        throw gcnew InvalidOperationException(
            String::Format("Failed to add '{0}' to sys.path", path));
    }
}
