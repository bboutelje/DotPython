
#include <algorithm>
#include "Purgatory.h"
#include "pch.h"

std::vector<PyObject*> Purgatory::m_markedForDecremention;

void Purgatory::MarkForDecrementation(PyObject* pyObject)
{
    if (pyObject != nullptr) {
        m_markedForDecremention.push_back(pyObject);
    }
}

void Purgatory::DecrementAndClear()
{
    std::for_each(m_markedForDecremention.begin(), m_markedForDecremention.end(),
        [](PyObject* obj)
        {
            if (obj)
            {

                Py_DECREF(obj);
                auto ref_count = Py_REFCNT(obj);
                if (ref_count == 0) {
                    obj = nullptr;
				}
                
            }
        }
    );

    m_markedForDecremention.clear();
}
