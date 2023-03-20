//------------------------------------------------------------------------------
// GB_concat_bitmap_sparse: concatenate a sparse tile into a bitmap matrix
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2023, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

{

    #ifdef GB_JIT_KERNEL
    GB_WERK_DECLARE (A_ek_slicing, int64_t) ;
    #undef  GB_FREE_ALL
    #define GB_FREE_ALL GB_WERK_POP (A_ek_slicing, int64_t) ;
    #endif

    int A_nthreads, A_ntasks ;
    GB_SLICE_MATRIX (A, 1, chunk) ;
    const int64_t *restrict Ap = A->p ;
    const int64_t *restrict Ah = A->h ;
    const int64_t *restrict Ai = A->i ;
    int tid ;
    #pragma omp parallel for num_threads(A_nthreads) schedule(static)
    for (tid = 0 ; tid < A_ntasks ; tid++)
    {
        int64_t kfirst = kfirst_Aslice [tid] ;
        int64_t klast  = klast_Aslice  [tid] ;
        for (int64_t k = kfirst ; k <= klast ; k++)
        {
            int64_t j = GBH_A (Ah, k) ;
            int64_t jC = cvstart + j ;
            int64_t pC_start = cistart + jC * cvlen ;
            int64_t pA_start, pA_end ;
            GB_get_pA (&pA_start, &pA_end, tid, k,
                kfirst, klast, pstart_Aslice, Ap, avlen) ;
            GB_PRAGMA_SIMD
            for (int64_t pA = pA_start ; pA < pA_end ; pA++)
            { 
                int64_t i = Ai [pA] ;
                int64_t pC = pC_start + i ;
                // Cx [pC] = Ax [pA] ;
                GB_COPY (pC, pA, A_iso) ;
                Cb [pC] = 1 ;
            }
        }
    }

    #ifdef GB_JIT_KERNEL
    GB_FREE_ALL ;
    #endif
}

