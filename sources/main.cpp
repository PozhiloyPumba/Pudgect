#include <iostream>
#include "driver.hpp"

int yyFlexLexer::yywrap () { return 1; }

int main ()
{
    yy::Driver driver;
    driver.parse ();
    
    driver.callDump (std::cout);
    return 0;
}