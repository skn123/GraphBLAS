//------------------------------------------------------------------------------
// GB_mex_test37: index binary op tests
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2024, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

#include "GB_mex.h"
#include "GB_mex_errors.h"

#define GET_DEEP_COPY ;
#define FREE_DEEP_COPY ;

#define FREE_ALL                        \
{                                       \
    GrB_Scalar_free (&Theta) ;          \
    GrB_Scalar_free (&Alpha) ;          \
    GrB_Scalar_free (&Beta) ;           \
    GrB_Matrix_free (&A) ;              \
    GrB_Matrix_free (&C1) ;             \
    GrB_Matrix_free (&C2) ;             \
    GrB_Matrix_free (&B1) ;             \
    GrB_Matrix_free (&B2) ;             \
    GrB_Matrix_free (&D) ;              \
    GrB_BinaryOp_free (&Bop) ;          \
    GzB_IndexBinaryOp_free (&Iop) ;     \
}

void test37_idxbinop (double *z,
    const double *x, GrB_Index ix, GrB_Index jx,
    const double *y, GrB_Index iy, GrB_Index jy,
    const double *theta) ;

void test37_idxbinop (double *z,
    const double *x, GrB_Index ix, GrB_Index jx,
    const double *y, GrB_Index iy, GrB_Index jy,
    const double *theta)
{
    (*z) = (*x) + 2*(*y) - 42*ix + jx + 3*iy + 1000*jy - (*theta) ;
}

#define TEST37_IDXBINOP                                                     \
"void test37_idxbinop (double *z,                                       \n" \
"    const double *x, GrB_Index ix, GrB_Index jx,                       \n" \
"    const double *y, GrB_Index iy, GrB_Index jy,                       \n" \
"    const double *theta)                                               \n" \
"{                                                                      \n" \
"    (*z) = (*x) + 2*(*y) - 42*ix + jx + 3*iy + 1000*jy - (*theta) ;    \n" \
"}                                                                      \n"

void mexFunction
(
    int nargout,
    mxArray *pargout [ ],
    int nargin,
    const mxArray *pargin [ ]
)
{

    //--------------------------------------------------------------------------
    // startup GraphBLAS
    //--------------------------------------------------------------------------

    GrB_Info info ;
    bool malloc_debug = GB_mx_get_global (true) ;

    //--------------------------------------------------------------------------
    // test index binary ops
    //--------------------------------------------------------------------------

    GrB_Scalar Theta = NULL, Alpha = NULL, Beta = NULL ;
    GzB_IndexBinaryOp Iop = NULL ;
    GrB_BinaryOp Bop = NULL ;
    GrB_Matrix A = NULL, C1 = NULL, C2 = NULL, B1 = NULL, B2 = NULL, D = NULL ;

    OK (GrB_Matrix_new (&A, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_new (&C1, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_new (&C2, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_new (&B1, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_new (&B2, GrB_FP64, 10, 10)) ;

    double x = 1 ;
    for (int64_t i = 0 ; i < 9 ; i++)
    {
        OK (GrB_Matrix_setElement_FP64 (A, x, i, i)) ;
        x = x*1.2 ;
        OK (GrB_Matrix_setElement_FP64 (A, x, i, i+1)) ;
        x = x*1.2 ;
        OK (GrB_Matrix_setElement_FP64 (A, x, i+1, i)) ;
        x = x*1.2 ;
    }
    OK (GrB_Matrix_setElement_FP64 (A, x, 9, 9)) ;
    x = x - 1000 ;
    OK (GrB_Matrix_setElement_FP64 (A, x, 5, 2)) ;

    OK (GrB_Scalar_new (&Theta, GrB_FP64)) ;
    OK (GrB_Scalar_setElement_FP64 (Theta, x)) ;

    OK (GxB_print (A, 5)) ;
    OK (GxB_print (Theta, 5)) ;

    OK (GzB_IndexBinaryOp_new2 (&Iop,
        (GzB_index_binary_function) test37_idxbinop,
        GrB_FP64, GrB_FP64, GrB_FP64, GrB_FP64,
        "test37_idxbinop", TEST37_IDXBINOP)) ;
    OK (GxB_print (Iop, 5)) ;

    OK (GzB_BinaryOp_IndexOp_new (&Bop, Iop, Theta)) ;
    OK (GxB_print (Bop, 5)) ;

    OK (GrB_Scalar_new (&Alpha, GrB_FP64)) ;
    OK (GrB_Scalar_new (&Beta, GrB_FP64)) ;
    OK (GrB_Scalar_setElement_FP64 (Alpha, (double) 3.14159)) ;
    OK (GrB_Scalar_setElement_FP64 (Beta, (double) 42)) ;

    OK (GrB_Global_set_INT32 (GrB_GLOBAL, 1 , (GrB_Field) GxB_BURBLE)) ;

    OK (GrB_Matrix_eWiseAdd_BinaryOp (C1, NULL, NULL, Bop, A, A,
        GrB_DESC_T1)) ;
    OK (GxB_Matrix_eWiseUnion (B1, NULL, NULL, Bop, A, Alpha, A, Beta,
        GrB_DESC_T1)) ;

    // turn off the JIT
    OK (GrB_Global_set_INT32 (GrB_GLOBAL, GxB_JIT_OFF,
        (GrB_Field) GxB_JIT_C_CONTROL)) ;

    OK (GrB_Matrix_set_INT32 (A,  GrB_ROWMAJOR, GrB_STORAGE_ORIENTATION_HINT)) ;
    OK (GrB_Matrix_set_INT32 (C2, GrB_ROWMAJOR, GrB_STORAGE_ORIENTATION_HINT)) ;
    OK (GrB_Matrix_set_INT32 (B2, GrB_ROWMAJOR, GrB_STORAGE_ORIENTATION_HINT)) ;
    OK (GxB_print (A, 5)) ;

    OK (GrB_Matrix_eWiseAdd_BinaryOp (C2, NULL, NULL, Bop, A, A,
        GrB_DESC_T1)) ;
    OK (GxB_Matrix_eWiseUnion (B2, NULL, NULL, Bop, A, Alpha, A, Beta,
        GrB_DESC_T1)) ;

    OK (GxB_print (C1, 5)) ;
//  OK (GxB_print (C2, 5)) ;

    OK (GxB_print (B1, 5)) ;
//  OK (GxB_print (B2, 5)) ;

    OK (GrB_Matrix_set_INT32 (C2, GrB_COLMAJOR, GrB_STORAGE_ORIENTATION_HINT)) ;
    OK (GrB_Matrix_set_INT32 (B2, GrB_COLMAJOR, GrB_STORAGE_ORIENTATION_HINT)) ;

    // FIXME: check C1 and B1 matrices

    OK (GrB_Matrix_new (&D, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_eWiseAdd_BinaryOp (D, NULL, NULL, GrB_MINUS_FP64, C1, B1,
        NULL)) ;
    OK (GrB_Matrix_select_FP64 (D, NULL, NULL, GrB_VALUENE_FP64, D,
        (double) 0, NULL)) ;
    OK (GxB_print (D, 5)) ;

    CHECK (GB_mx_isequal (C1, C2, 0)) ;
    CHECK (GB_mx_isequal (B1, B2, 0)) ;

    //------------------------------------------------------------------------
    // finalize GraphBLAS
    //------------------------------------------------------------------------

    FREE_ALL ;
    GB_mx_put_global (true) ;
    printf ("\nGB_mex_test37:  all tests passed\n\n") ;
}

