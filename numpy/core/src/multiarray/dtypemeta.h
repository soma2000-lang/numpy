#ifndef NUMPY_CORE_SRC_MULTIARRAY_DTYPEMETA_H_
#define NUMPY_CORE_SRC_MULTIARRAY_DTYPEMETA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "numpy/_dtype_api.h"

/* DType flags, currently private, since we may just expose functions */
#define NPY_DT_LEGACY 1 << 0
#define NPY_DT_ABSTRACT 1 << 1
#define NPY_DT_PARAMETRIC 1 << 2


typedef struct {
    /* DType methods, these could be moved into its own struct */
    discover_descr_from_pyobject_function *discover_descr_from_pyobject;
    is_known_scalar_type_function *is_known_scalar_type;
    default_descr_function *default_descr;
    common_dtype_function *common_dtype;
    common_instance_function *common_instance;
    ensure_canonical_function *ensure_canonical;
    /*
     * Currently only used for experimental user DTypes.
     */
    setitemfunction *setitem;
    getitemfunction *getitem;
    /*
     * The casting implementation (ArrayMethod) to convert between two
     * instances of this DType, stored explicitly for fast access:
     */
    PyObject *within_dtype_castingimpl;
    /*
     * Dictionary of ArrayMethods representing most possible casts
     * (structured and object are exceptions).
     * This should potentially become a weak mapping in the future.
     */
    PyObject *castingimpls;

    /*
     * Storage for `descr->f`, since we may need to allow some customizatoin
     * here at least in a transition period and we need to set it on every
     * dtype instance for backward compatibility.  (Keep this at end)
     */
    PyArray_ArrFuncs f;
} NPY_DType_Slots;


#define NPY_DTYPE(descr) ((PyArray_DTypeMeta *)Py_TYPE(descr))
#define NPY_DT_SLOTS(dtype) ((NPY_DType_Slots *)(dtype)->dt_slots)

#define NPY_DT_is_legacy(dtype) (((dtype)->flags & NPY_DT_LEGACY) != 0)
#define NPY_DT_is_abstract(dtype) (((dtype)->flags & NPY_DT_ABSTRACT) != 0)
#define NPY_DT_is_parametric(dtype) (((dtype)->flags & NPY_DT_PARAMETRIC) != 0)
#define NPY_DT_is_user_defined(dtype) (((dtype)->type_num == -1))

/*
 * Macros for convenient classmethod calls, since these require
 * the DType both for the slot lookup and as first arguments.
 *
 * (Macros may include NULL checks where appropriate)
 */
#define NPY_DT_CALL_discover_descr_from_pyobject(dtype, obj)  \
    NPY_DT_SLOTS(dtype)->discover_descr_from_pyobject(dtype, obj)
#define NPY_DT_CALL_is_known_scalar_type(dtype, obj)  \
    (NPY_DT_SLOTS(dtype)->is_known_scalar_type != NULL  \
        && NPY_DT_SLOTS(dtype)->is_known_scalar_type(dtype, obj))
#define NPY_DT_CALL_default_descr(dtype)  \
    NPY_DT_SLOTS(dtype)->default_descr(dtype)
#define NPY_DT_CALL_common_dtype(dtype, other)  \
    NPY_DT_SLOTS(dtype)->common_dtype(dtype, other)
#define NPY_DT_CALL_ensure_canonical(descr)  \
    NPY_DT_SLOTS(NPY_DTYPE(descr))->ensure_canonical(descr)
#define NPY_DT_CALL_getitem(descr, data_ptr)  \
    NPY_DT_SLOTS(NPY_DTYPE(descr))->getitem(descr, data_ptr)
#define NPY_DT_CALL_setitem(descr, value, data_ptr)  \
    NPY_DT_SLOTS(NPY_DTYPE(descr))->setitem(descr, value, data_ptr)


/*
 * This function will hopefully be phased out or replaced, but was convenient
 * for incremental implementation of new DTypes based on DTypeMeta.
 * (Error checking is not required for DescrFromType, assuming that the
 * type is valid.)
 */
static inline PyArray_DTypeMeta *
PyArray_DTypeFromTypeNum(int typenum)
{
    PyArray_Descr *descr = PyArray_DescrFromType(typenum);
    PyArray_DTypeMeta *dtype = NPY_DTYPE(descr);
    Py_INCREF(dtype);
    Py_DECREF(descr);
    return dtype;
}


NPY_NO_EXPORT int
python_builtins_are_known_scalar_types(
        PyArray_DTypeMeta *cls, PyTypeObject *pytype);

NPY_NO_EXPORT int
dtypemeta_wrap_legacy_descriptor(PyArray_Descr *dtypem);

#ifdef __cplusplus
}
#endif

#endif  /* NUMPY_CORE_SRC_MULTIARRAY_DTYPEMETA_H_ */
