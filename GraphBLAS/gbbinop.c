//------------------------------------------------------------------------------
// gbbinop: create a GraphBLAS binary op and print it (for illustration only)
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2019, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// Usage:

// A = gbbinop (binop)
// A = gbbinop (binop, type)

#include "gb_matlab.h"

void mexFunction
(
    int nargout,
    mxArray *pargout [ ],
    int nargin,
    const mxArray *pargin [ ]
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    gb_usage (nargin <= 2 && nargout == 0,
        "usage: gbbinop (binop) or gbbinop (binop,type)") ;

    //--------------------------------------------------------------------------
    // construct the GraphBLAS binary operator and print it
    //--------------------------------------------------------------------------

    GrB_Type type = NULL ;
    if (nargin == 2)
    {
        type = gb_mxstring_to_type (pargin [1]) ;
    }

    GrB_BinaryOp op = gb_mxstring_to_binop (pargin [0], type) ;
    GxB_BinaryOp_fprint (op, "", GxB_COMPLETE, stdout) ;
}
