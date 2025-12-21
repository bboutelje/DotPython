#pragma once

#include <Python.h>
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>
#include "ManagedPyObject.h"

using namespace System;
using namespace System::Runtime::CompilerServices;

namespace DotPython
{

	[Extension]
	public ref class ExtensionMethods abstract sealed {
	public:

		[Extension]
		static ManagedPyObject^ ToPython(Object^ managedObject);
	};
}