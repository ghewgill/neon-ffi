#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <ffi.h>

#include <dlfcn.h>

#include "neonext.h"

typedef void (*void_function_t)();

const struct Ne_MethodTable *Ne;

class ForeignFunctionStub {
public:
    void *library;
    void_function_t fp;
    ffi_cif cif;
};

Ne_EXPORT int Ne_INIT(const struct Ne_MethodTable *methodtable)
{
    Ne = methodtable;
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_bind)
{
    std::string library = Ne_PARAM_STRING(0);
    std::string name = Ne_PARAM_STRING(1);
    std::string returntype = Ne_PARAM_STRING(2);

    auto *ffs = new ForeignFunctionStub();
    ffs->library = dlopen(library.c_str(), RTLD_LAZY);
    ffs->fp = reinterpret_cast<void_function_t>(dlsym(ffs->library, name.c_str()));
    ffi_type *rtype = &ffi_type_void;
    ffi_status status = ffi_prep_cif(&ffs->cif, FFI_DEFAULT_ABI, 0, rtype, NULL);
    if (status != FFI_OK) {
        fprintf(stderr, "internal ffi error %d\n", status);
        exit(1);
    }

    Ne_RETURN_POINTER(ffs);
}

Ne_FUNC(Ne_invoke)
{
    auto *ffs = Ne_PARAM_POINTER(ForeignFunctionStub, 0);

    if (ffs->library == NULL || ffs->fp == NULL) {
        Ne_RAISE_EXCEPTION("InvalidParameter", 0, 0);
    }

    ffi_call(&ffs->cif, ffs->fp, NULL, NULL);

    return Ne_SUCCESS;
}

Ne_FUNC(Ne_unbind)
{
    auto *ffs = Ne_PARAM_POINTER(ForeignFunctionStub, 0);

    if (ffs->library != NULL) {
        dlclose(ffs->library);
        ffs->fp = NULL;
    }

    return Ne_SUCCESS;
}
