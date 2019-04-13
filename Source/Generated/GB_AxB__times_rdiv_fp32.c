//------------------------------------------------------------------------------
// GB_AxB:  hard-coded functions for semiring: C<M>=A*B or A'*B
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2019, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// Unless this file is Generator/GB_AxB.c, do not edit it (auto-generated)

#include "GB.h"
#ifndef GBCOMPACT
#include "GB_AxB__include.h"

// The C=A*B semiring is defined by the following types and operators:

// A*B function (Gustavon):  GB_AgusB__times_rdiv_fp32
// A'*B function (dot):      GB_AdotB__times_rdiv_fp32
// A*B function (heap):      GB_AheapB__times_rdiv_fp32

// C type:   float
// A type:   float
// B type:   float

// Multiply: z = (bkj / aik)
// Add:      cij *= z
// MultAdd:  cij *= (bkj / aik)
// Identity: 1
// Terminal: ;

#define GB_ATYPE \
    float

#define GB_BTYPE \
    float

// aik = Ax [pA]
#define GB_GETA(aik,Ax,pA) \
    float aik = Ax [pA]

// bkj = Bx [pB]
#define GB_GETB(bkj,Bx,pB) \
    float bkj = Bx [pB]

#define GB_CX(p) Cx [p]

// multiply operator
#define GB_MULT(z, x, y)        \
    z = (y / x) ;

// multiply-add
#define GB_MULTADD(z, x, y)     \
    z *= (y / x) ;

// copy scalar
#define GB_COPY(z,x) z = x ;

// monoid identity value (Gustavson's method only, with no mask)
#define GB_IDENTITY \
    1

// break if cij reaches the terminal value (dot product only)
#define GB_DOT_TERMINAL(cij) \
    ;

// cij is not a pointer but a scalar; nothing to do
#define GB_CIJ_REACQUIRE(cij,cnz) ;

// save the value of C(i,j)
#define GB_CIJ_SAVE(cij) Cx [cnz] = cij ;

#define GB_SAUNA_WORK(i) Sauna_Work [i]

//------------------------------------------------------------------------------
// C<M>=A*B and C=A*B: gather/scatter saxpy-based method (Gustavson)
//------------------------------------------------------------------------------

GrB_Info GB_AgusB__times_rdiv_fp32
(
    GrB_Matrix C,
    const GrB_Matrix M,
    const GrB_Matrix A, bool A_is_pattern,
    const GrB_Matrix B, bool B_is_pattern,
    GB_Sauna Sauna
)
{ 
    float *restrict Sauna_Work = Sauna->Sauna_Work ;
    float *restrict Cx = C->x ;
    GrB_Info info = GrB_SUCCESS ;
    #include "GB_AxB_Gustavson_meta.c"
    return (info) ;
}

//------------------------------------------------------------------------------
// C<M>=A'*B, C<!M>=A'*B or C=A'*B: dot product
//------------------------------------------------------------------------------

GrB_Info GB_AdotB__times_rdiv_fp32
(
    GrB_Matrix *Chandle,
    const GrB_Matrix M, const bool Mask_comp,
    const GrB_Matrix A, bool A_is_pattern,
    const GrB_Matrix B, bool B_is_pattern
)
{ 
    GrB_Matrix C = (*Chandle) ;
    float *restrict Cx = C->x ;
    float cij ;
    GrB_Info info = GrB_SUCCESS ;
    #define GB_SINGLE_PHASE
    #include "GB_AxB_dot_meta.c"
    #undef GB_SINGLE_PHASE
    return (info) ;
}

//------------------------------------------------------------------------------
// C<M>=A'*B, C<!M>=A'*B or C=A'*B: dot product (phase 2)
//------------------------------------------------------------------------------

GrB_Info GB_Adot2B__times_rdiv_fp32
(
    GrB_Matrix *Chandle,
    const GrB_Matrix M, const bool Mask_comp,
    const GrB_Matrix A, bool A_is_pattern,
    const GrB_Matrix B, bool B_is_pattern,
    const int64_t *restrict C_count_start,
    const int64_t *restrict C_count_end
)
{ 
    GrB_Matrix C = (*Chandle) ;
    float *restrict Cx = C->x ;
    float cij ;
    #define GB_PHASE_2_OF_2
    #include "GB_AxB_dot_meta.c"
    #undef GB_PHASE_2_OF_2
    return (GrB_SUCCESS) ;
}

//------------------------------------------------------------------------------
// C<M>=A*B and C=A*B: heap saxpy-based method
//------------------------------------------------------------------------------

#include "GB_heap.h"

GrB_Info GB_AheapB__times_rdiv_fp32
(
    GrB_Matrix *Chandle,
    const GrB_Matrix M,
    const GrB_Matrix A, bool A_is_pattern,
    const GrB_Matrix B, bool B_is_pattern,
    int64_t *restrict List,
    GB_pointer_pair *restrict pA_pair,
    GB_Element *restrict Heap,
    const int64_t bjnz_max
)
{ 
    GrB_Matrix C = (*Chandle) ;
    float *restrict Cx = C->x ;
    float cij ;
    int64_t cvlen = C->vlen ;
    GrB_Info info = GrB_SUCCESS ;
    #include "GB_AxB_heap_meta.c"
    return (info) ;
}

#endif
