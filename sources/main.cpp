#include <iostream>

#include "form.hpp"

int main ()
{
    SAT::Form form;
    if (form.input ())
        return 0;

    form.toCNF ();
    form.simplify ();
    // std::cout << form.toString () << std::endl;
    form.callDump (std::cout);
    std::cout << std::endl;
    return 0;
}