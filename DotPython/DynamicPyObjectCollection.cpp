#include "pch.h"
#include "DynamicPyObjectCollection.h"
using namespace DotPython;

System::Collections::Generic::IEnumerator<DynamicPyObject^>^ DotPython::DynamicPyObjectCollection::GetEnumerator()
{
    return gcnew PyObjectEnumerator(m_managedPyObject);
}
