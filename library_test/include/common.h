/**
 * @file common.h
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2024-12-04
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <iostream>

#ifndef __FUNCTION_NAME__
    #ifdef WIN32   //WINDOWS
        #define __FUNCTION_NAME__   __FUNCTION__
    #else          //*NIX
        #define __FUNCTION_NAME__   __func__
    #endif
#endif

#define TABS std::string(tabs, '\t')
#define CTABS std::cout << TABS
#define PRINT_FUNCTION CTABS << "-------[" __FUNCTION_NAME__ "]-------" \
  << std::endl
#define PRINT_DESC(x) CTABS << ">>>>>>> " << x << " <<<<<<<" << std::endl
#define PRINT_BOOL(x) #x << " = " << (x ? "true" : "false")
#define PRINT(x) #x << " = " << x
#define NEWLINE std::cout << std::endl
