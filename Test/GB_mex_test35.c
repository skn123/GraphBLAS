//------------------------------------------------------------------------------
// GB_mex_test35: test GrB_get for a serialized blob
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2023, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

#include "GB_mex.h"
#include "GB_mex_errors.h"

#define USAGE "GB_mex_test35"

#define FREE_ALL ;
#define GET_DEEP_COPY ;
#define FREE_DEEP_COPY ;

typedef struct { int32_t stuff ; } mytype ;
#define MYTYPE_DEFN \
"typedef struct { int32_t stuff ; } mytype ;"

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

    GrB_Info info, expected ;
    bool malloc_debug = GB_mx_get_global (true) ;
    GrB_Matrix A = NULL ;
    GrB_Scalar s = NULL, s_fp64 = NULL, s_int32 = NULL, s_fp32 = NULL ;
    GrB_Type type = NULL ;
    uint8_t stuff [256] ;
    void *nothing = stuff ;
    size_t size, blob_size = 0 ;
    char name [256] ;
    char defn [2048] ;
    int code, i ;
    float fvalue ;
    double dvalue ;
    void *blob = NULL ;

    OK (GrB_Scalar_new (&s_fp64, GrB_FP64)) ;
    OK (GrB_Scalar_new (&s_fp32, GrB_FP32)) ;
    OK (GrB_Scalar_new (&s_int32, GrB_INT32)) ;

    //--------------------------------------------------------------------------
    // GxB_Blob_get
    //--------------------------------------------------------------------------

    OK (GrB_Matrix_new (&A, GrB_FP32, 5, 5)) ;
    OK (GrB_Matrix_setElement (A, 0, 0, 1)) ;
    OK (GrB_Matrix_wait (A, GrB_MATERIALIZE)) ;
    OK (GxB_Matrix_serialize (&blob, &blob_size, A, NULL)) ;

    expected = GrB_NOT_IMPLEMENTED ;
    ERR (GxB_Blob_get_VOID_(blob, nothing, 0, blob_size)) ;

    OK (GxB_Blob_get_SIZE_(blob, &size, GrB_ELTYPE_STRING, blob_size)) ;
    CHECK (size == GxB_MAX_NAME_LEN) ;
    OK (GxB_Blob_get_String_(blob, name, GrB_ELTYPE_STRING, blob_size)) ;
    CHECK (MATCH (name, "GrB_FP32")) ;

    OK (GxB_Blob_get_String_(blob, name, GrB_NAME, blob_size)) ;
    CHECK (MATCH (name, "")) ;

    OK (GxB_Blob_get_ENUM_(blob, &code, GrB_ELTYPE_CODE, blob_size)) ;
    CHECK (code == GrB_FP32_CODE) ;

    i = -1 ;
    OK (GxB_Blob_get_Scalar_(blob, s_int32, GrB_ELTYPE_CODE, blob_size)) ;
    OK (GrB_Scalar_extractElement_INT32_(&i, s_int32)) ;
    CHECK (i == GrB_FP32_CODE) ;

    OK (GxB_Blob_get_ENUM_(blob, &i, GrB_STORAGE_ORIENTATION_HINT, blob_size)) ;
    printf ("blob storage: %d\n", i) ;
    CHECK (i == GrB_COLMAJOR) ;

    OK (GxB_Blob_get_ENUM_(blob, &i, GxB_FORMAT, blob_size)) ;
    printf ("blob storage: %d\n", i) ;
    CHECK (i == GxB_BY_COL) ;

    OK (GxB_Blob_get_ENUM_(blob, &i, GxB_SPARSITY_CONTROL, blob_size)) ;
    printf ("blob sparsity control: %d\n", i) ;
    CHECK (i == GxB_AUTO_SPARSITY) ;

    OK (GrB_assign (A, NULL, NULL, 1, GrB_ALL, 5, GrB_ALL, 5, NULL)) ;
    OK (GrB_Matrix_wait (A, GrB_MATERIALIZE)) ;

    // free the blob and recreate it
    mxFree (blob) ; blob = NULL ; blob_size = 0 ;
    OK (GxB_Matrix_serialize (&blob, &blob_size, A, NULL)) ;

    OK (GxB_Blob_get_ENUM_(blob, &i, GxB_SPARSITY_STATUS, blob_size)) ;
    printf ("blob sparsity status: %d\n", i) ;
    CHECK (i == GxB_FULL) ;

    expected = GrB_INVALID_VALUE ;
    ERR (GxB_Blob_get_ENUM_(blob, &i, 0, blob_size)) ;
    ERR (GxB_Blob_get_SIZE_(blob, &size, 0, blob_size)) ;

    fvalue = -1 ;
    OK (GxB_Blob_get_Scalar_(blob, s_fp32, GxB_BITMAP_SWITCH, blob_size)) ;
    OK (GrB_Scalar_extractElement_FP32_(&fvalue, s_fp32)) ;
    printf ("blob bitmap switch: %g\n", fvalue) ;
    CHECK (abs (fvalue - 0.04) < 1e-6) ;

    OK (GrB_Matrix_set_ENUM_(A, GxB_BITMAP, GxB_SPARSITY_CONTROL)) ;

    // free the blob and recreate it
    mxFree (blob) ; blob = NULL ; blob_size = 0 ;
    OK (GxB_Matrix_serialize (&blob, &blob_size, A, NULL)) ;

    OK (GxB_Blob_get_ENUM_(blob, &i, GxB_SPARSITY_STATUS, blob_size)) ;
    printf ("blob sparsity status: %d\n", i) ;
    CHECK (i == GxB_BITMAP) ;

    OK (GrB_Scalar_setElement_FP32_(s_fp32, 0.25)) ;
    OK (GrB_Matrix_set_Scalar_(A, s_fp32, GxB_HYPER_SWITCH)) ;

    // free the blob and recreate it
    mxFree (blob) ; blob = NULL ; blob_size = 0 ;
    OK (GxB_Matrix_serialize (&blob, &blob_size, A, NULL)) ;

    OK (GxB_Blob_get_Scalar_(blob, s_fp64, GxB_HYPER_SWITCH, blob_size)) ;
    OK (GrB_Scalar_extractElement_FP64_(&dvalue, s_fp64)) ;
    printf ("blob hyper switch: %g\n", dvalue) ;
    CHECK (abs (dvalue - 0.25) < 1e-6) ;

    OK (GrB_Matrix_set_ENUM_(A, GrB_ROWMAJOR, GrB_STORAGE_ORIENTATION_HINT)) ;
    OK (GrB_Matrix_get_ENUM_(A, &i, GrB_STORAGE_ORIENTATION_HINT)) ;
    CHECK (i == GrB_ROWMAJOR) ;
    OK (GrB_Matrix_get_ENUM_(A, &i, GxB_FORMAT)) ;
    CHECK (i == GxB_BY_ROW) ;
    GxB_print (A, 3) ;

    // free the blob and recreate it
    mxFree (blob) ; blob = NULL ; blob_size = 0 ;
    OK (GxB_Matrix_serialize (&blob, &blob_size, A, NULL)) ;

    OK (GxB_Blob_get_ENUM_(blob, &i, GrB_STORAGE_ORIENTATION_HINT, blob_size)) ;
    CHECK (i == GrB_ROWMAJOR) ;
    OK (GxB_Blob_get_ENUM_(blob, &i, GxB_FORMAT, blob_size)) ;
    CHECK (i == GxB_BY_ROW) ;
    GxB_print (A, 3) ;

    expected = GrB_INVALID_VALUE ;
    ERR (GrB_Matrix_get_String_(A, defn, 999)) ;
    ERR (GrB_Matrix_get_Scalar(A, s_int32, 999)) ;

    OK (GrB_Matrix_get_SIZE_(A, &size, GrB_NAME)) ;
    CHECK (size == GxB_MAX_NAME_LEN) ;

    //--------------------------------------------------------------------------
    // finalize GraphBLAS
    //--------------------------------------------------------------------------

    GrB_free (&A) ;
    GrB_free (&s) ;
    GrB_free (&s_fp64) ;
    GrB_free (&s_fp32) ;
    GrB_free (&s_int32) ;
    GrB_free (&type) ;
    if (blob != NULL) mxFree (blob) ;
    GB_mx_put_global (true) ;
    printf ("\nGB_mex_test35:  all tests passed.\n\n") ;
}

