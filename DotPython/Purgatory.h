#pragma once
#include <Python.h>
#include <vector>
#include <memory>
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>
#include "pch.h"


class Purgatory
{
private:
    static std::vector<PyObject*> m_markedForDecremention;

public:
    static void MarkForDecrementation(PyObject* pyObject);
        

    static void DecrementAndClear();
};
