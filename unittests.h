#ifndef UNITTESTS_H
#define UNITTESTS_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <stdlib.h> // aligned_alloc

#include "rendererinterface.h"

#include "filteredcubemapmanager.h"

#include <QDebug>
#include <QTimer>
#include <QLabel>

class Unit_tests
{
public:
    Unit_tests();
    void run_all_tests();
private:
    bool is_aligned(const void * pointer, size_t byte_count);
    bool malloc_tests();
    bool cmft_image_load_test();
    bool webkit_on_separate_process_test();
};

inline void* aligned_malloc(size_t size, size_t align) {
    void* result = NULL;

#if defined(WIN32)
    result = _aligned_malloc(size, align);
#else    
    switch (posix_memalign((void**)&result, align, size)) {
    case EINVAL:
        std::cerr << "aligned_malloc: The value of the alignment parameter is not a power of two multiple of sizeof( void *).";
        result = NULL;
        break;
    case ENOMEM:
        std::cerr << "aligned_malloc: There is insufficient memory available with the requested alignment.";
        result = NULL;
        break;
    }
#endif
    return result;
}

inline void aligned_free(void* ptr) {
#if defined(WIN32)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}
#endif // UNITTESTS_H
