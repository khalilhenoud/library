/**
 * @file core.h
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-12-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_H
#define CORE_H

#ifndef __FUNCTION_NAME__
    #ifdef WIN32
        #define __FUNCTION_NAME__   __FUNCTION__  
    #else
        #define __FUNCTION_NAME__   __func__ 
    #endif
#endif

// NOTE: this code is provided by Joe Lowe and initially found at:
// https://stackoverflow.com/questions/1113409/attribute-constructor-equivalent-in-vc
// NOTE: also see for more info on pragma(comment(...)):
// https://learn.microsoft.com/en-us/cpp/preprocessor/comment-c-cpp?view=msvc-170
// NOTE: we have to force the linker to include the symbol as the optimized
// builds will usually strip the symbols away.
#ifdef __cplusplus
    #define INITIALIZER(f)                                            \
        static void f(void);                                          \
        struct f##_t_ { f##_t_(void) { f(); } }; static f##_t_ f##_;  \
        static void f(void)
#elif defined(_MSC_VER)
    #pragma section(".CRT$XCU",read)
    #define INITIALIZER2_(f, p)                                       \
        static void f(void);                                          \
        __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f;      \
        __pragma(comment(linker,"/include:" p #f "_"))                \
        static void f(void)
    #ifdef _WIN64
        #define INITIALIZER(f) INITIALIZER2_(f, "")
    #else
        #define INITIALIZER(f) INITIALIZER2_(f, "_")
    #endif
#else
    #define INITIALIZER(f) \
        static void f(void) __attribute__((constructor));            \
        static void f(void)
#endif

#endif