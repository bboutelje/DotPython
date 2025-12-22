#pragma once

#include <Python.h>
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>
#include "ManagedPyObject.h"
#include "DynamicPyObject.h"
#include "utils.h"

using namespace System;
using namespace System::Runtime::CompilerServices;

namespace DotPython
{
	[System::Runtime::CompilerServices::Extension]
	[System::ComponentModel::EditorBrowsable(System::ComponentModel::EditorBrowsableState::Never)]
	public ref class ExtensionMethods abstract sealed {
	internal:
		
		//static ManagedPyObject^ ToManagedPyObject(Object^ managedObject);
	public:
		[returnvalue:System::Runtime::CompilerServices::DynamicAttribute]
		[System::Runtime::CompilerServices::Extension]
		static Object^ ToPython(System::Object^ managedObject);

	};
}