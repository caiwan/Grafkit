# QnD solution to find a path of a lib in a list os paths
# e.g. HAYSTACK

function(path_find_in_list RESULT NEEDLE)
    set(${RESULT} "" PARENT_SCOPE)

    foreach(_findpath ${ARGN})
        string(TOLOWER ${_findpath} _findpath_low)
        string(TOLOWER ${NEEDLE} _name_low)
        string(FIND ${_findpath_low} ${_name_low} _strres REVERSE)
        
        if (NOT ${_strres} EQUAL -1)
            set(${RESULT} ${_findpath} PARENT_SCOPE)
            return()
        elseif(EXISTS ${_findpath}/${NEEDLE} )
            set(${RESULT} "${_findpath}/${NEEDLE}" PARENT_SCOPE)
            return()
        endif()
        
    endforeach()

endfunction(path_find_in_list)
