//------------------------------------------------------------------------------
// GB_bitmap_assign_notM_noaccum_whole:  assign to C bitmap
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2023, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------
// C<!M> = A       assign
// C<!M> = A       subassign

// C<!M,repl> = A       assign
// C<!M,repl> = A       subassign
//------------------------------------------------------------------------------

// C:           bitmap
// M:           present, hypersparse or sparse (not bitmap or full)
// Mask_comp:   true
// Mask_struct: true or false
// C_replace:   true or false
// accum:       not present
// A:           matrix (hyper, sparse, bitmap, or full), or scalar
// kind:        assign or subassign (same action)

// JIT: needed.

#include "assign/GB_bitmap_assign_methods.h"
#define GB_GENERIC
#include "assign/include/GB_assign_shared_definitions.h"

#undef  GB_FREE_ALL
#define GB_FREE_ALL                         \
{                                           \
    GB_WERK_POP (M_ek_slicing, int64_t) ;   \
}

GrB_Info GB_bitmap_assign_notM_noaccum_whole
(
    // input/output:
    GrB_Matrix C,               // input/output matrix in bitmap format
    // inputs:
    const bool C_replace,       // descriptor for C
    #define I NULL              /* I index list */
    #define ni 0
    #define nI 0
    #define Ikind GB_ALL
    #define Icolon NULL
    #define J NULL              /* J index list */
    #define ni 0
    #define nI 0
    #define Jkind GB_ALL
    #define Jcolon NULL
    const GrB_Matrix M,         // mask matrix
    #define Mask_comp true      /* true here, for !M only */
    const bool Mask_struct,     // true if M is structural, false if valued
    #define accum NULL          /* not present */
    const GrB_Matrix A,         // input matrix, not transposed
    const void *scalar,         // input scalar
    const GrB_Type scalar_type, // type of input scalar
    #define assign_kind         GB_ASSIGN
    GB_Werk Werk
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GBURBLE_BITMAP_ASSIGN ("bit8:whole", M, true, NULL,
        GB_ALL, GB_ALL, GB_ASSIGN) ;
    ASSERT (GB_IS_HYPERSPARSE (M) || GB_IS_SPARSE (M)) ;
    ASSERT_MATRIX_OK (C, "C for bitmap assign, !M, noaccum", GB0) ;
    ASSERT_MATRIX_OK (M, "M for bitmap assign, !M, noaccum", GB0) ;
    ASSERT_MATRIX_OK_OR_NULL (A, "A for bitmap assign, !M, noaccum", GB0) ;

    int nthreads_max = GB_Context_nthreads_max ( ) ;
    double chunk = GB_Context_chunk ( ) ;

    //--------------------------------------------------------------------------
    // get inputs
    //--------------------------------------------------------------------------

    GB_GET_C_BITMAP ;           // C must be bitmap
    GB_SLICE_M
    GB_GET_A_AND_SCALAR_FOR_BITMAP

    //--------------------------------------------------------------------------
    // scatter M into the bitmap of C
    //--------------------------------------------------------------------------

    // Cb [pC] += 2 for each entry M(i,j) in the mask
    GB_bitmap_M_scatter_whole (C, M, GB_MASK_STRUCT,
        GB_BITMAP_M_SCATTER_PLUS_2, M_ek_slicing, M_ntasks, M_nthreads) ;
    // the bitmap of C now contains:
    //  Cb (i,j) = 0:   cij not present, mij zero
    //  Cb (i,j) = 1:   cij present, mij zero
    //  Cb (i,j) = 2:   cij not present, mij 1
    //  Cb (i,j) = 3:   cij present, mij 1

    //--------------------------------------------------------------------------
    // do the assignment
    //--------------------------------------------------------------------------

    if (GB_SCALAR_ASSIGN)
    {

        //----------------------------------------------------------------------
        // scalar assignment: C<!M, replace or !replace> = scalar
        //----------------------------------------------------------------------

        if (C_replace)
        { 

            //------------------------------------------------------------------
            // C<!M,replace> = scalar
            //------------------------------------------------------------------

            #undef  GB_CIJ_WORK
            #define GB_CIJ_WORK(pC)                                 \
            {                                                       \
                switch (Cb [pC])                                    \
                {                                                   \
                    case 0: /* C(i,j) not present, !M(i,j) = 1 */   \
                        /* Cx [pC] = scalar */                      \
                        GB_COPY_cwork_to_C (Cx, pC, cwork, C_iso) ; \
                        Cb [pC] = 1 ;                               \
                        task_cnvals++ ;                             \
                        break ;                                     \
                    case 1: /* C(i,j) present, !M(i,j) = 1 */       \
                        /* Cx [pC] = scalar */                      \
                        GB_COPY_cwork_to_C (Cx, pC, cwork, C_iso) ; \
                        break ;                                     \
                    case 2: /* C(i,j) not present, !M(i,j) = 0 */   \
                        /* clear the mask from C */                 \
                        Cb [pC] = 0 ;                               \
                        break ;                                     \
                    case 3: /* C(i,j) present, !M(i,j) = 0 */       \
                        /* delete this entry */                     \
                        Cb [pC] = 0 ;                               \
                        task_cnvals-- ;                             \
                        break ;                                     \
                    default: ;                                      \
                }                                                   \
            }
            #include "assign/factory/GB_bitmap_assign_C_whole_template.c"

        }
        else
        { 

            //------------------------------------------------------------------
            // C<!M> = scalar
            //------------------------------------------------------------------

            #undef  GB_CIJ_WORK
            #define GB_CIJ_WORK(pC)                                 \
            {                                                       \
                switch (Cb [pC])                                    \
                {                                                   \
                    case 0: /* C(i,j) not present, !M(i,j) = 1 */   \
                        /* Cx [pC] = scalar */                      \
                        GB_COPY_cwork_to_C (Cx, pC, cwork, C_iso) ; \
                        Cb [pC] = 1 ;                               \
                        task_cnvals++ ;                             \
                        break ;                                     \
                    case 1: /* C(i,j) present, !M(i,j) = 1 */       \
                        /* Cx [pC] = scalar */                      \
                        GB_COPY_cwork_to_C (Cx, pC, cwork, C_iso) ; \
                        break ;                                     \
                    case 2: /* C(i,j) not present, !M(i,j) = 0 */   \
                        /* clear the mask from C */                 \
                        Cb [pC] = 0 ;                               \
                        break ;                                     \
                    case 3: /* C(i,j) present, !M(i,j) = 0 */       \
                        /* C(i,j) remains; clear the mask from C */ \
                        Cb [pC] = 1 ;                               \
                        break ;                                     \
                    default: ;                                      \
                }                                                   \
            }
            #include "assign/factory/GB_bitmap_assign_C_whole_template.c"
        }

    }
    else
    {

        //----------------------------------------------------------------------
        // matrix assignment: C<!M, replace or !replace> = A
        //----------------------------------------------------------------------

        if (GB_IS_BITMAP (A) || GB_IS_FULL (A))
        {

            //------------------------------------------------------------------
            // C<!M, replace or !replace> = A where A is bitmap or full
            //------------------------------------------------------------------

            if (C_replace)
            { 

                //--------------------------------------------------------------
                // C<!M, replace> = A where A is bitmap or full
                //--------------------------------------------------------------

                #undef  GB_CIJ_WORK
                #define GB_CIJ_WORK(pC)                                       \
                {                                                             \
                    switch (Cb [pC])                                          \
                    {                                                         \
                        case 0: /* C(i,j) not present, !M(i,j) = 1 */         \
                            if (GBB_A (Ab, pC))                               \
                            {                                                 \
                                /* Cx [pC] = Ax [pC] */                       \
                                GB_COPY_aij_to_C (Cx,pC,Ax,pC,A_iso,cwork,    \
                                    C_iso) ;                                  \
                                Cb [pC] = 1 ;                                 \
                                task_cnvals++ ;                               \
                            }                                                 \
                            break ;                                           \
                        case 1: /* C(i,j) present, !M(i,j) = 1 */             \
                            if (GBB_A (Ab, pC))                               \
                            {                                                 \
                                /* Cx [pC] = Ax [pC] */                       \
                                GB_COPY_aij_to_C (Cx,pC,Ax,pC,A_iso,cwork,    \
                                    C_iso) ;                                  \
                            }                                                 \
                            else                                              \
                            {                                                 \
                                /* delete this entry */                       \
                                Cb [pC] = 0 ;                                 \
                                task_cnvals-- ;                               \
                            }                                                 \
                            break ;                                           \
                        case 2: /* C(i,j) not present, !M(i,j) = 0 */         \
                            /* clear the mask from C */                       \
                            Cb [pC] = 0 ;                                     \
                            break ;                                           \
                        case 3: /* C(i,j) present, !M(i,j) = 0 */             \
                            /* delete this entry */                           \
                            Cb [pC] = 0 ;                                     \
                            task_cnvals-- ;                                   \
                            break ;                                           \
                        default: ;                                            \
                    }                                                         \
                }
                #include "assign/factory/GB_bitmap_assign_C_whole_template.c"

            }
            else
            { 

                //--------------------------------------------------------------
                // C<!M> = A where A is bitmap or full
                //--------------------------------------------------------------

                #undef  GB_CIJ_WORK
                #define GB_CIJ_WORK(pC)                                       \
                {                                                             \
                    switch (Cb [pC])                                          \
                    {                                                         \
                        case 0: /* C(i,j) not present, !M(i,j) = 1 */         \
                            if (GBB_A (Ab, pC))                               \
                            {                                                 \
                                /* Cx [pC] = Ax [pC] */                       \
                                GB_COPY_aij_to_C (Cx,pC,Ax,pC,A_iso,cwork,    \
                                    C_iso) ;                                  \
                                Cb [pC] = 1 ;                                 \
                                task_cnvals++ ;                               \
                            }                                                 \
                            break ;                                           \
                        case 1: /* C(i,j) present, !M(i,j) = 1 */             \
                            if (GBB_A (Ab, pC))                               \
                            {                                                 \
                                /* Cx [pC] = Ax [pC] */                       \
                                GB_COPY_aij_to_C (Cx,pC,Ax,pC,A_iso,cwork,    \
                                    C_iso) ;                                  \
                            }                                                 \
                            else                                              \
                            {                                                 \
                                /* delete this entry */                       \
                                Cb [pC] = 0 ;                                 \
                                task_cnvals-- ;                               \
                            }                                                 \
                            break ;                                           \
                        case 2: /* C(i,j) not present, !M(i,j) = 0 */         \
                            /* clear the mask from C */                       \
                            Cb [pC] = 0 ;                                     \
                            break ;                                           \
                        case 3: /* C(i,j) present, !M(i,j) = 0 */             \
                            /* keep the entry */                              \
                            Cb [pC] = 1 ;                                     \
                            break ;                                           \
                        default: ;                                            \
                    }                                                         \
                }
                #include "assign/factory/GB_bitmap_assign_C_whole_template.c"
            }

        }
        else
        {

            //------------------------------------------------------------------
            // C<!M, replace or !replace> = A where A is sparse or hyper
            //------------------------------------------------------------------

            // assign entries from A into C
            #undef  GB_AIJ_WORK
            #define GB_AIJ_WORK(pC,pA)                                  \
            {                                                           \
                int8_t cb = Cb [pC] ;                                   \
                if (cb <= 1)                                            \
                {                                                       \
                    /* Cx [pC] = Ax [pA] */                             \
                    GB_COPY_aij_to_C (Cx, pC, Ax, pA, A_iso, cwork, C_iso) ; \
                    Cb [pC] = 4 ;                                       \
                    task_cnvals += (cb == 0) ;                          \
                }                                                       \
            }
            #include "assign/factory/GB_bitmap_assign_A_whole_template.c"

            // clear the mask and delete entries not assigned
            if (C_replace)
            { 
                #undef  GB_CIJ_WORK
                #define GB_CIJ_WORK(pC)                     \
                {                                           \
                    int8_t cb = Cb [pC] ;                   \
                    Cb [pC] = (cb == 4) ;                   \
                    task_cnvals -= (cb == 1 || cb == 3) ;   \
                }
                #include "assign/factory/GB_bitmap_assign_C_whole_template.c"
            }
            else
            { 
                #undef  GB_CIJ_WORK
                #define GB_CIJ_WORK(pC)                     \
                {                                           \
                    int8_t cb = Cb [pC] ;                   \
                    Cb [pC] = (cb == 4 || cb == 3) ;        \
                    task_cnvals -= (cb == 1) ;              \
                }
                #include "assign/factory/GB_bitmap_assign_C_whole_template.c"
            }
        }
    }

    //--------------------------------------------------------------------------
    // free workspace and return result
    //--------------------------------------------------------------------------

    C->nvals = cnvals ;
    GB_FREE_ALL ;
    ASSERT_MATRIX_OK (C, "final C for bitmap assign, !M, noaccum, whole", GB0) ;
    return (GrB_SUCCESS) ;
}

