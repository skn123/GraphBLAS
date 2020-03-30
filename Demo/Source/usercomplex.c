//------------------------------------------------------------------------------
// GraphBLAS/Demo/Source/usercomplex.c:  complex numbers as a user-defined type
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

#include "GraphBLAS.h"

#if defined __INTEL_COMPILER
#pragma warning (disable: 58 167 144 161 177 181 186 188 589 593 869 981 1418 1419 1572 1599 2259 2282 2557 2547 3280 )
#elif defined __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#endif

#define GB_LIBRARY
#include "usercomplex.h"

GrB_BinaryOp Complex_first = NULL, Complex_second = NULL, Complex_min = NULL,
             Complex_max   = NULL, Complex_plus   = NULL, Complex_minus = NULL,
             Complex_times = NULL, Complex_div    = NULL, Complex_rminus = NULL,
             Complex_rdiv  = NULL, Complex_pair   = NULL ;

GrB_BinaryOp Complex_iseq = NULL, Complex_isne = NULL,
             Complex_isgt = NULL, Complex_islt = NULL,
             Complex_isge = NULL, Complex_isle = NULL ;

GrB_BinaryOp Complex_or = NULL, Complex_and = NULL, Complex_xor = NULL ;

GrB_BinaryOp Complex_eq = NULL, Complex_ne = NULL,
             Complex_gt = NULL, Complex_lt = NULL,
             Complex_ge = NULL, Complex_le = NULL ;

GrB_BinaryOp Complex_complex = NULL ;

GrB_UnaryOp  Complex_identity = NULL, Complex_ainv = NULL, Complex_minv = NULL,
             Complex_not = NULL,      Complex_conj = NULL,
             Complex_one = NULL,      Complex_abs  = NULL ;

GrB_UnaryOp Complex_real = NULL, Complex_imag = NULL,
            Complex_cabs = NULL, Complex_angle = NULL ;

GrB_UnaryOp Complex_complex_real = NULL, Complex_complex_imag = NULL ;

GrB_Type Complex = NULL ;
GrB_Monoid   Complex_plus_monoid = NULL, Complex_times_monoid = NULL ;
GrB_Semiring Complex_plus_times = NULL ;

#if GxB_STDC_VERSION >= 201112L

#define ONE  CMPLX(1,0)
#define ZERO CMPLX(0,0)
#define C double complex

#define X *x
#define Y *y
#define Z *z

#define T ONE
#define F ZERO
#define BOOL(X) (X != ZERO)

//------------------------------------------------------------------------------
// binary functions, z=f(x,y), where CxC -> C
//------------------------------------------------------------------------------

GB_PUBLIC void complex_first  (C Z, const C X, const C Y) { Z = X ; }
GB_PUBLIC void complex_second (C Z, const C X, const C Y) { Z = Y ; }
GB_PUBLIC void complex_pair   (C Z, const C X, const C Y) { Z = ONE ; }
GB_PUBLIC void complex_plus   (C Z, const C X, const C Y) { Z = X + Y ; }
GB_PUBLIC void complex_minus  (C Z, const C X, const C Y) { Z = X - Y ; }
GB_PUBLIC void complex_rminus (C Z, const C X, const C Y) { Z = Y - X ; }
GB_PUBLIC void complex_times  (C Z, const C X, const C Y) { Z = X * Y ; }
GB_PUBLIC void complex_div    (C Z, const C X, const C Y) { Z = X / Y ; }
GB_PUBLIC void complex_rdiv   (C Z, const C X, const C Y) { Z = Y / X ; }

GB_PUBLIC
void complex_min (C Z, const C X, const C Y)
{
    // min (x,y): complex number with smallest magnitude.  If tied, select the
    // one with the smallest phase angle (same as MATLAB definition).
    // No special cases for NaNs.
    double absx = cabs (X) ;
    double absy = cabs (Y) ;
    if (absx < absy)
    {
        Z = X ;
    }
    else if (absx > absy)
    {
        Z = Y ;
    }
    else
    {
        if (carg (X) < carg (Y))
        {
            Z = X ;
        }
        else
        {
            Z = Y ;
        }
    }
}

GB_PUBLIC
void complex_max (C Z, const C X, const C Y)
{
    // max (x,y): complex number with largest magnitude.  If tied, select the
    // one with the largest phase angle (same as MATLAB definition).
    // No special cases for NaNs.
    double absx = cabs (X) ;
    double absy = cabs (Y) ;
    if (absx > absy)
    {
        Z = X ;
    }
    else if (absx < absy)
    {
        Z = Y ;
    }
    else
    {
        if (carg (X) > carg (Y))
        {
            Z = X ;
        }
        else
        {
            Z = Y ;
        }
    }
}

//------------------------------------------------------------------------------
// 6 binary functions, z=f(x,y), where CxC -> C ; (1,0) = true, (0,0) = false
//------------------------------------------------------------------------------

// inequality operators follow the MATLAB convention

#define R(x) creal(x)

GB_PUBLIC
void complex_iseq (C Z, const C X, const C Y) { Z = (X == Y) ? T : F ; }
GB_PUBLIC
void complex_isne (C Z, const C X, const C Y) { Z = (X != Y) ? T : F ; }
GB_PUBLIC
void complex_isgt (C Z, const C X, const C Y) { Z = (R(X) >  R(Y)) ? T : F ; }
GB_PUBLIC
void complex_islt (C Z, const C X, const C Y) { Z = (R(X) <  R(Y)) ? T : F ; }
GB_PUBLIC
void complex_isge (C Z, const C X, const C Y) { Z = (R(X) >= R(Y)) ? T : F ; }
GB_PUBLIC
void complex_isle (C Z, const C X, const C Y) { Z = (R(X) <= R(Y)) ? T : F ; }

//------------------------------------------------------------------------------
// binary boolean functions, z=f(x,y), where CxC -> C
//------------------------------------------------------------------------------

GB_PUBLIC
void complex_or (C Z, const C X, const C Y)
{
    Z = (BOOL (X) || BOOL (Y)) ? T : F ;
}

GB_PUBLIC
void complex_and (C Z, const C X, const C Y)
{
    Z = (BOOL (X) && BOOL (Y)) ? T : F ;
}

GB_PUBLIC
void complex_xor (C Z, const C X, const C Y)
{
    Z = (BOOL (X) != BOOL (Y)) ? T : F ;
}

//------------------------------------------------------------------------------
// 6 binary functions, z=f(x,y), where CxC -> bool
//------------------------------------------------------------------------------

// inequality operators follow the MATLAB convention

GB_PUBLIC
void complex_eq (bool Z, const C X, const C Y) { Z = (X == Y) ; }
GB_PUBLIC
void complex_ne (bool Z, const C X, const C Y) { Z = (X != Y) ; }
GB_PUBLIC
void complex_gt (bool Z, const C X, const C Y) { Z = (R (X) >  R (Y)) ;}
GB_PUBLIC
void complex_lt (bool Z, const C X, const C Y) { Z = (R (X) <  R (Y)) ;}
GB_PUBLIC
void complex_ge (bool Z, const C X, const C Y) { Z = (R (X) >= R (Y)) ;}
GB_PUBLIC
void complex_le (bool Z, const C X, const C Y) { Z = (R (X) <= R (Y)) ;}

//------------------------------------------------------------------------------
// binary functions, z=f(x,y), where double x double -> complex
//------------------------------------------------------------------------------

GB_PUBLIC
void complex_complex (C Z, const double X, const double Y) { Z = CMPLX (X,Y) ; }

//------------------------------------------------------------------------------
// unary functions, z=f(x) where C -> C
//------------------------------------------------------------------------------

GB_PUBLIC
void complex_one      (C Z, const C X) { Z =       1. ; }
GB_PUBLIC
void complex_identity (C Z, const C X) { Z =       X  ; }
GB_PUBLIC
void complex_ainv     (C Z, const C X) { Z =      -X  ; }
GB_PUBLIC
void complex_abs      (C Z, const C X) { Z = CMPLX (cabs (X), 0) ; }
GB_PUBLIC
void complex_minv     (C Z, const C X) { Z =  1. / X  ; } 
GB_PUBLIC
void complex_not      (C Z, const C X) { Z = BOOL (X) ? F : T ; }
GB_PUBLIC
void complex_conj     (C Z, const C X) { Z = conj (X) ; }

//------------------------------------------------------------------------------
// unary functions, z=f(x) where C -> double
//------------------------------------------------------------------------------

GB_PUBLIC
void complex_real  (double Z, const C X) { Z = creal (X) ; }
GB_PUBLIC
void complex_imag  (double Z, const C X) { Z = cimag (X) ; }
GB_PUBLIC
void complex_cabs  (double Z, const C X) { Z = cabs  (X) ; }
GB_PUBLIC
void complex_angle (double Z, const C X) { Z = carg  (X) ; }

//------------------------------------------------------------------------------
// unary functions, z=f(x) where double -> C
//------------------------------------------------------------------------------

GB_PUBLIC
void complex_complex_real (C Z, const double X) { Z = CMPLX (X, 0) ; }
GB_PUBLIC
void complex_complex_imag (C Z, const double X) { Z = CMPLX (0, X) ; }

#else

//------------------------------------------------------------------------------
// Pre-ANSI C11: just make to easier to write this file
//------------------------------------------------------------------------------

#define ONE  0
#define ZERO 1
#define C double

#endif

//------------------------------------------------------------------------------
// OK: check if a method fails
//------------------------------------------------------------------------------

#define OK(method)              \
    info = method ;             \
    if (info != GrB_SUCCESS)    \
    {                           \
        Complex_finalize ( ) ;  \
        return (info) ;         \
    }

//------------------------------------------------------------------------------
// Complex_init: create the complex type, operators, monoids, and semiring
//------------------------------------------------------------------------------

GB_PUBLIC
GrB_Info Complex_init ( )
{

    //--------------------------------------------------------------------------
    // create the Complex type
    //--------------------------------------------------------------------------

#if GxB_STDC_VERSION >= 201112L

    GrB_Info info ;
    OK (GrB_Type_new (&Complex, sizeof (C))) ;    

    #undef C
    #undef D
    #define C Complex
    #define D GrB_FP64

    //--------------------------------------------------------------------------
    // create the Complex binary operators, CxC->C
    //--------------------------------------------------------------------------

    OK (GrB_BinaryOp_new (&Complex_first  , complex_first  , C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_second , complex_second , C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_pair   , complex_pair   , C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_min    , complex_min    , C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_max    , complex_max    , C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_plus   , complex_plus   , C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_minus  , complex_minus  , C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_rminus , complex_rminus , C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_times  , complex_times  , C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_div    , complex_div    , C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_rdiv   , complex_rdiv   , C, C, C)) ;

    //--------------------------------------------------------------------------
    // create the Complex binary comparison operators, CxC -> C
    //--------------------------------------------------------------------------

    OK (GrB_BinaryOp_new (&Complex_iseq , complex_iseq ,  C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_isne , complex_isne ,  C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_isgt , complex_isgt ,  C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_islt , complex_islt ,  C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_isge , complex_isge ,  C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_isle , complex_isle ,  C, C, C)) ;

    //--------------------------------------------------------------------------
    // create the Complex boolean operators, CxC -> C
    //--------------------------------------------------------------------------

    OK (GrB_BinaryOp_new (&Complex_or  , complex_or  ,  C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_and , complex_and ,  C, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_xor , complex_xor ,  C, C, C)) ;

    //--------------------------------------------------------------------------
    // create the Complex binary operators, CxC -> bool
    //--------------------------------------------------------------------------

    OK (GrB_BinaryOp_new (&Complex_eq , complex_eq ,  GrB_BOOL, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_ne , complex_ne ,  GrB_BOOL, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_gt , complex_gt ,  GrB_BOOL, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_lt , complex_lt ,  GrB_BOOL, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_ge , complex_ge ,  GrB_BOOL, C, C)) ;
    OK (GrB_BinaryOp_new (&Complex_le , complex_le ,  GrB_BOOL, C, C)) ;

    //--------------------------------------------------------------------------
    // create the Complex binary operator, double x double -> C
    //--------------------------------------------------------------------------

    OK (GrB_BinaryOp_new (&Complex_complex, complex_complex, C, D, D)) ;

    //--------------------------------------------------------------------------
    // create the Complex unary operators, C->C
    //--------------------------------------------------------------------------

    OK (GrB_UnaryOp_new (&Complex_one     , complex_one     , C, C)) ;
    OK (GrB_UnaryOp_new (&Complex_identity, complex_identity, C, C)) ;
    OK (GrB_UnaryOp_new (&Complex_ainv    , complex_ainv    , C, C)) ;
    OK (GrB_UnaryOp_new (&Complex_abs     , complex_abs     , C, C)) ;
    OK (GrB_UnaryOp_new (&Complex_minv    , complex_minv    , C, C)) ;
    OK (GrB_UnaryOp_new (&Complex_not     , complex_not     , C, C)) ;
    OK (GrB_UnaryOp_new (&Complex_conj    , complex_conj    , C, C)) ;

    //--------------------------------------------------------------------------
    // create the unary functions, C -> double
    //--------------------------------------------------------------------------

    OK (GrB_UnaryOp_new (&Complex_real  , complex_real  , D, C)) ;
    OK (GrB_UnaryOp_new (&Complex_imag  , complex_imag  , D, C)) ;
    OK (GrB_UnaryOp_new (&Complex_cabs  , complex_cabs  , D, C)) ;
    OK (GrB_UnaryOp_new (&Complex_angle , complex_angle , D, C)) ;

    //--------------------------------------------------------------------------
    // create the unary functions, double -> C
    //--------------------------------------------------------------------------

    OK (GrB_UnaryOp_new (&Complex_complex_real , complex_complex_real , C, D)) ;
    OK (GrB_UnaryOp_new (&Complex_complex_imag , complex_complex_imag , C, D)) ;

    //--------------------------------------------------------------------------
    // create the Complex monoids
    //--------------------------------------------------------------------------

    double complex Complex_1 = ONE ;
    double complex Complex_0 = ZERO ;

    OK (GrB_Monoid_new_UDT (&Complex_plus_monoid,  Complex_plus,  &Complex_0)) ;
    OK (GrB_Monoid_new_UDT (&Complex_times_monoid, Complex_times, &Complex_1)) ;

    //--------------------------------------------------------------------------
    // create the Complex plus-times semiring
    //--------------------------------------------------------------------------

    // more could be created, but this suffices for testing GraphBLAS
    OK (GrB_Semiring_new
        (&Complex_plus_times, Complex_plus_monoid, Complex_times)) ;
#endif

    return (GrB_SUCCESS) ;
}


//------------------------------------------------------------------------------
// Complex_finalize: free all complex types, operators, monoids, and semiring
//------------------------------------------------------------------------------

GB_PUBLIC
GrB_Info Complex_finalize ( )
{

#if GxB_STDC_VERSION >= 201112L

    //--------------------------------------------------------------------------
    // free the Complex plus-times semiring
    //--------------------------------------------------------------------------

    GrB_Semiring_free (&Complex_plus_times) ;

    //--------------------------------------------------------------------------
    // free the Complex monoids
    //--------------------------------------------------------------------------

    GrB_Monoid_free (&Complex_plus_monoid ) ;
    GrB_Monoid_free (&Complex_times_monoid) ;

    //--------------------------------------------------------------------------
    // free the Complex binary operators, CxC->C
    //--------------------------------------------------------------------------

    GrB_BinaryOp_free (&Complex_first ) ;
    GrB_BinaryOp_free (&Complex_second) ;
    GrB_BinaryOp_free (&Complex_pair  ) ;
    GrB_BinaryOp_free (&Complex_min   ) ;
    GrB_BinaryOp_free (&Complex_max   ) ;
    GrB_BinaryOp_free (&Complex_plus  ) ;
    GrB_BinaryOp_free (&Complex_minus ) ;
    GrB_BinaryOp_free (&Complex_rminus) ;
    GrB_BinaryOp_free (&Complex_times ) ;
    GrB_BinaryOp_free (&Complex_div   ) ;
    GrB_BinaryOp_free (&Complex_rdiv  ) ;

    GrB_BinaryOp_free (&Complex_iseq) ;
    GrB_BinaryOp_free (&Complex_isne) ;
    GrB_BinaryOp_free (&Complex_isgt) ;
    GrB_BinaryOp_free (&Complex_islt) ;
    GrB_BinaryOp_free (&Complex_isge) ;
    GrB_BinaryOp_free (&Complex_isle) ;

    GrB_BinaryOp_free (&Complex_or) ;
    GrB_BinaryOp_free (&Complex_and) ;
    GrB_BinaryOp_free (&Complex_xor) ;

    //--------------------------------------------------------------------------
    // free the Complex binary operators, CxC -> bool
    //--------------------------------------------------------------------------

    GrB_BinaryOp_free (&Complex_eq) ;
    GrB_BinaryOp_free (&Complex_ne) ;
    GrB_BinaryOp_free (&Complex_gt) ;
    GrB_BinaryOp_free (&Complex_lt) ;
    GrB_BinaryOp_free (&Complex_ge) ;
    GrB_BinaryOp_free (&Complex_le) ;

    //--------------------------------------------------------------------------
    // free the Complex binary operator, double x double -> complex
    //--------------------------------------------------------------------------

    GrB_BinaryOp_free (&Complex_complex) ;

    //--------------------------------------------------------------------------
    // free the Complex unary operators, C->C
    //--------------------------------------------------------------------------

    GrB_UnaryOp_free (&Complex_one     ) ;
    GrB_UnaryOp_free (&Complex_identity) ;
    GrB_UnaryOp_free (&Complex_ainv    ) ;
    GrB_UnaryOp_free (&Complex_abs     ) ;
    GrB_UnaryOp_free (&Complex_minv    ) ;
    GrB_UnaryOp_free (&Complex_not     ) ;
    GrB_UnaryOp_free (&Complex_conj    ) ;

    //--------------------------------------------------------------------------
    // free the unary functions, C -> double
    //--------------------------------------------------------------------------

    GrB_UnaryOp_free (&Complex_real ) ;
    GrB_UnaryOp_free (&Complex_imag ) ;
    GrB_UnaryOp_free (&Complex_cabs ) ;
    GrB_UnaryOp_free (&Complex_angle) ;

    //--------------------------------------------------------------------------
    // free the unary functions, double -> C
    //--------------------------------------------------------------------------

    GrB_UnaryOp_free (&Complex_complex_real) ;
    GrB_UnaryOp_free (&Complex_complex_imag) ;

    //--------------------------------------------------------------------------
    // free the Complex type
    //--------------------------------------------------------------------------

    GrB_Type_free (&Complex) ;
#endif

    return (GrB_SUCCESS) ;
}

