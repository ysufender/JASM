#pragma once

#include "JASMConfig.hpp"

#ifdef TEST_MODE
    #define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#else
    #define DOCTEST_CONFIG_DISABLE
#endif // TEST_MODE

#include "doctest.h"
