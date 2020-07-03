#ifndef CHECK_MACRO_HPP
#define CHECK_MACRO_HPP

#include <iostream>

#define CHECK(x, y) ((!x) ? std::cout << "ERROR : " << y << std::endl : std::cout << "      : " << y << std::endl )

#endif // CHECK_MACRO_HPP
