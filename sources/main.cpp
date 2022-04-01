#include <iostream>

#include "driver.hpp"

int main ()
{
    yy::Form driver;
    driver.parse ();

    // driver.callDump (std::cout);
    std::cout << driver.toString () << std::endl;

    return 0;
}