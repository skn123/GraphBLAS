//------------------------------------------------------------------------------
// GrB_Vector_set_*: set a field in a vector
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2023, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

#include "GB_get_set.h"

//------------------------------------------------------------------------------
// GrB_Vector_set_Scalar
//------------------------------------------------------------------------------

GrB_Info GrB_Vector_set_Scalar
(
    GrB_Vector v,
    GrB_Scalar value,
    GrB_Field field
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GB_WHERE1 ("GrB_Vector_set_Scalar (v, value, field)") ;
    GB_RETURN_IF_NULL_OR_FAULTY (v) ;
    ASSERT_VECTOR_OK (v, "v to set option", GB0) ;

    //--------------------------------------------------------------------------
    // set the field
    //--------------------------------------------------------------------------

    double dvalue = 0 ;
    int ivalue = 0 ;
    GrB_Info info ;

    switch ((int) field)
    {
        case GxB_BITMAP_SWITCH : 
            info = GrB_Scalar_extractElement_FP64 (&dvalue, value) ;
            break ;
        default : 
            info = GrB_Scalar_extractElement_INT32 (&ivalue, value) ;
            break ;
    }

    if (info != GrB_SUCCESS)
    { 
        return ((info == GrB_NO_VALUE) ? GrB_EMPTY_OBJECT : info) ;
    } 
    return (GB_matvec_set ((GrB_Matrix) v, true, ivalue, dvalue, field, Werk)) ;
}

//------------------------------------------------------------------------------
// GrB_Vector_set_String
//------------------------------------------------------------------------------

GrB_Info GrB_Vector_set_String
(
    GrB_Vector v,
    char * value,
    GrB_Field field
)
{ 
    return (GrB_NOT_IMPLEMENTED) ;      // FIXME: set the name of a GrB_Vector
}

//------------------------------------------------------------------------------
// GrB_Vector_set_ENUM
//------------------------------------------------------------------------------

GrB_Info GrB_Vector_set_ENUM
(
    GrB_Vector v,
    int value,
    GrB_Field field
)
{ 

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GB_WHERE1 ("GrB_Vector_set_ENUM (v, value, field)") ;
    GB_RETURN_IF_NULL_OR_FAULTY (v) ;
    ASSERT_VECTOR_OK (v, "v to set option", GB0) ;

    //--------------------------------------------------------------------------
    // set the field
    //--------------------------------------------------------------------------

    return (GB_matvec_set ((GrB_Matrix) v, true, value, 0, field, Werk)) ;
}

//------------------------------------------------------------------------------
// GrB_Vector_set_VOID
//------------------------------------------------------------------------------

GrB_Info GrB_Vector_set_VOID
(
    GrB_Vector v,
    void * value,
    GrB_Field field,
    size_t size
)
{ 
    return (GrB_NOT_IMPLEMENTED) ;
}

