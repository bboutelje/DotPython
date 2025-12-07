#pragma once

#include <string>
#include <msclr/marshal_cppstd.h>

namespace Interop
{
    class StringMarshal
    {
    private:
        std::string m_nativeString;

    public:
        StringMarshal(System::String^ managedString)
        {
            if (managedString != nullptr)
            {
                msclr::interop::marshal_context context;

                m_nativeString = context.marshal_as<std::string>(managedString);
            }
        }

        operator const char* () const
        {
            return m_nativeString.c_str();
        }

        char* ToCharPointer()
        {
            return m_nativeString.empty() ? nullptr : &m_nativeString[0];
        }
    };
}