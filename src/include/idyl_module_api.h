/* =============================================================================
 * idyl_module_api.h — C ABI for external idyl modules (.so / .dll / .dylib)
 *
 * External modules are shared libraries placed in ~/.idyl/modules/
 * and loaded at runtime via:
 *
 *     module("my_module")           // merge into global scope
 *     m = module("my_module")       // namespaced: m::func(...)
 *
 * A valid module must export an `idyl_module_init` function (see below).
 *
 * To create an external module in C:
 *
 *   #include "idyl_module_api.h"
 *
 *   static idyl_value my_func(const idyl_value* args, int argc) {
 *       idyl_value result = { IDYL_NUMBER, 0, 0, NULL };
 *       result.number = args[0].number + args[1].number;
 *       return result;
 *   }
 *
 *   static idyl_fn_entry entries[] = {
 *       { "my_func", my_func, 2, 2 },
 *   };
 *
 *   IDYL_MODULE_INIT {
 *       *count = 1;
 *       return entries;
 *   }
 *
 * ========================================================================== */

#ifndef IDYL_MODULE_API_H
#define IDYL_MODULE_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── Value type tags (mirrors core::value_t) ─────────────────────────────── */
typedef enum {
    IDYL_NUMBER  = 0,
    IDYL_TIME    = 1,
    IDYL_TRIGGER = 2,
    IDYL_STRING  = 3,
    IDYL_HANDLE  = 6,
    IDYL_NIL     = 7
} idyl_value_type;

/* ── Value passed to/from module functions ────────────────────────────────── */
/* For IDYL_STRING: `string` is a null-terminated pointer valid for the       */
/* duration of the function call.  Modules must copy if they need to keep it. */
/* For IDYL_HANDLE: `handle` carries an opaque intptr_t (C pointer or ID).   */
typedef struct {
    idyl_value_type type;
    double          number;     /* used by NUMBER, TIME                 */
    int             trigger;    /* used by TRIGGER (1 = bang, 0 = rest) */
    const char*     string;     /* used by STRING (NULL when unused)    */
    intptr_t        handle;     /* used by HANDLE (opaque C pointer)    */
} idyl_value;

/* ── Module function signature ───────────────────────────────────────────── */
typedef idyl_value (*idyl_fn)(const idyl_value* args, int argc);

/* ── Function descriptor ─────────────────────────────────────────────────── */
typedef struct {
    const char* name;
    idyl_fn     fn;
    int         min_arity;
    int         max_arity;  /* -1 = variadic (no upper bound) */
} idyl_fn_entry;

/* ── Module init function ────────────────────────────────────────────────── */
/* Must be exported as `idyl_module_init`.  Sets *count and returns a        */
/* pointer to an array of fn entries (static lifetime).                       */
typedef idyl_fn_entry* (*idyl_module_init_fn)(int* count);

/* Helper macro for defining the init function with correct linkage.         */
#ifdef _WIN32
#define IDYL_MODULE_INIT __declspec(dllexport) idyl_fn_entry* idyl_module_init(int* count)
#else
#define IDYL_MODULE_INIT __attribute__((visibility("default"))) idyl_fn_entry* idyl_module_init(int* count)
#endif

#ifdef __cplusplus
}
#endif

#endif /* IDYL_MODULE_API_H */
