#pragma once
#include <Python.h>
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>
#include "ManagedPyObject.h"

using namespace System;

namespace DotPython {

	private ref class Utils {
	public:
		static Object^ ToManagedObject(ManagedPyObject^ pyObject);
		static ManagedPyObject^ ToPythonObject(Object^ managedObject);
	};
}


