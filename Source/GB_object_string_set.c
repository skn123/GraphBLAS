//------------------------------------------------------------------------------
// GB_object_string_set: set the name or defn of a user-defined object
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2023, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

#include "GB_get_set.h"
#include <ctype.h>
#include "GB_jitifyer.h"

GrB_Info GB_object_string_set
(
    // input:
    bool user_defined,
    bool jitable,
    char *value,
    int field,
    // output:
    char *name,
    int32_t *name_len,
    char **defn,
    size_t *defn_size,
    uint64_t *hash
) 
{

    //--------------------------------------------------------------------------
    // quick return for built-in types
    //--------------------------------------------------------------------------

    if (!user_defined)
    { 
        // built-in type
        return (GrB_ALREADY_SET) ;
    }

    //--------------------------------------------------------------------------
    // set the name or defn of a user-defined type
    //--------------------------------------------------------------------------

    size_t len = strlen (value) ;
    bool compute_hash = false ;

    switch (field)
    {
        case GrB_NAME : 

            if (name [0] != '[')    // default name: "[unnamed_user_*]"
            { 
                // name already defined
                return (GrB_ALREADY_SET) ;
            }

            if (value [0] == '[' || len == 0 || len >= GxB_MAX_NAME_LEN)
            { 
                // invalid name: "[" denotes an unnamed user type, the name
                // cannot be empty, and the name cannot exceed
                // GxB_MAX_NAME_LEN-1 characters.
                return (GrB_INVALID_VALUE) ;
            }

            // set the name
            strncpy (name, value, GxB_MAX_NAME_LEN-1) ;
            name [GxB_MAX_NAME_LEN-1] = '\0' ;
            (*name_len) = (int32_t) len ;
            // compute the hash if the type defn has also been set
            compute_hash = ((*defn) != NULL) ;
            break ;

        case GxB_DEFINITION : 

            if ((*defn) != NULL)
            { 
                // name already defined
                return (GrB_ALREADY_SET) ;
            }

            // allocate space for the definition
            (*defn) = GB_MALLOC (len+1, char, defn_size) ;
            if ((*defn) == NULL)
            { 
                // out of memory
                return (GrB_OUT_OF_MEMORY) ;
            }

            // copy the definition into the new operator
            memcpy ((*defn), value, len+1) ;
            // compute the hash if the type name has also been set
            compute_hash = (name [0] != '[') ;
            break ;

        default : 
            return (GrB_INVALID_VALUE) ;
    }

    //--------------------------------------------------------------------------
    // compute the operator hash, if type->name and type->defn are now both set
    //--------------------------------------------------------------------------

    if (compute_hash)
    { 
        // the type name and defn have been set
        (*hash) = GB_jitifyer_hash (name, (*name_len), jitable) ;
    }

    return (GrB_SUCCESS) ;
}

