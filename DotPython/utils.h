
#include <Python.h>
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>
#include "ManagedPyObject.h"

using namespace System;

namespace DotPython {
	// This helper function handles the conversion from a Python object
	// back to a managed .NET object.
	Object^ ConvertToManagedObject(ManagedPyObject^ pyObject);


	ManagedPyObject^ ConvertToPythonObject(Object^ managedObject);

	System::Object^ ConvertListToManagedArray(PyObject* pPyList);
	//const std::unique_ptr<char> ConvertStringToConstChar(String^ managedString);
}


