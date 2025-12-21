
#include <Python.h>
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>
#include "ManagedPyObject.h"

using namespace System;

namespace DotPython {

	Object^ ConvertToManagedObject(ManagedPyObject^ pyObject);
	//ManagedPyObject^ ConvertToPythonObject(Object^ managedObject);
}


