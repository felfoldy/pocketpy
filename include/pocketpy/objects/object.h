#pragma once

#include "pocketpy/objects/namedict.h"
#include "pocketpy/objects/base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyObject {
    py_Type type;  // we have a duplicated type here for convenience
    bool gc_is_large;
    bool gc_marked;
    int slots;  // number of slots in the object
    char flex[];
} PyObject;

// slots >= 0, allocate N slots
// slots == -1, allocate a dict

// | HEADER | <N slots> | <userdata>
// | HEADER | <dict>    | <userdata>

py_TValue* PyObject__slots(PyObject* self);
pk_NameDict* PyObject__dict(PyObject* self);
void* PyObject__userdata(PyObject* self);

#define PK_OBJ_SLOTS_SIZE(slots) ((slots) >= 0 ? sizeof(py_TValue) * (slots) : sizeof(pk_NameDict))

PyObject* PyObject__new(py_Type type, int slots, int size);
void PyObject__delete(PyObject* self);

#ifdef __cplusplus
}
#endif