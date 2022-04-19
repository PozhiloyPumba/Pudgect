#include <iostream>

#include "driver.hpp"

int main ()
{
    yy::Form driver;
    driver.parse ();


    driver.toCNF ();
    std::cout << driver.toString () << std::endl;
    // driver.callDump (std::cout);
    std::cout << std::endl;
    return 0;
}