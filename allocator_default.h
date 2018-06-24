#ifndef ALLOCATOR_DEFAULT_H
#define ALLOCATOR_DEFAULT_H

//#include <cstdio> // For vsnprintf
//#include <stdlib.h> // For malloc
//#include "EASTL/allocator.h"
//#include "EASTL/string.h"

#include "EASTL/map.h"
#include "EASTL/set.h"
#include "EASTL/vector.h"
#include "EASTL/algorithm.h"
#include "EASTL/sort.h"
#include "EASTL/iterator.h"
#include "EASTL/functional.h"


// EASTL expects us to define these, see allocator.h line 194
//void* operator new[](size_t size, const char* /*pName*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
/*{
        return malloc(size);
}*/
//void* operator new[](size_t size, size_t alignment, size_t /*alignmentOffset*/, const char* /*pName*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
/*{
        // this allocator doesn't support alignment
        EASTL_ASSERT(alignment <= 8);
        return malloc(size);
}*/

// EASTL also wants us to define this (see string.h line 197)
//int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
/*{
        return vsnprintf(pDestination, n, pFormat, arguments);
}*/

// EASTL expects us to define these, see allocator.h line 194
/*void* operator new[](size_t size, const char* pName, int flags,
             unsigned debugFlags, const char* file, int line)
{
   (void)pName; (void)flags; (void)debugFlags; (void)file; (void)line;
    return malloc(size);
}
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset,
    const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    // Alternate to -Wunused-parameter which I don't want to disable
    (void)alignment; (void)alignmentOffset; (void)pName; (void)flags;
    (void)debugFlags; (void)file; (void)line;

    // this allocator doesn't support alignment
    EASTL_ASSERT(alignment <= 8);
    return malloc(size);
}

// EASTL also wants us to define this (see string.h line 197)
int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
{
    return vsnprintf(pDestination, n, pFormat, arguments);
}*/

#endif // ALLOCATOR_DEFAULT_H
