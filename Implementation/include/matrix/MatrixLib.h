#pragma once
#include <td/Types.h>
#include <compiler/Definitions.h>

#ifdef MU_WINDOWS
    #ifdef MATRIX_EXPORTS
        #define MATRIX_API __declspec(dllexport)
        #define MATRIX_EXTERN
    #else
        #define MATRIX_API __declspec(dllimport)
        #define MATRIX_EXTERN extern
    #endif
#else
    #ifdef MATRIX_EXPORTS
        #define MATRIX_API __attribute__((visibility("default")))
        #define MATRIX_EXTERN
    #else
        #define MATRIX_API
        #define MATRIX_EXTERN extern
    #endif
#endif
