#include <iostream>

#include "driver.hpp"

int main ()
{
    yy::Form driver;
    driver.parse ();

    // std::cout << driver.toString () << std::endl;

    driver.toCNF ();
    driver.callDump (std::cout);
    std::cout << std::endl;
    return 0;
}