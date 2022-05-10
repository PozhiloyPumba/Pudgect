#include <iostream>

#include "form.hpp"

int main ()
{
    SAT::Form form;
    if (form.input ())
        return 0;

    // form.toCNF ();
    // form.simplify ();
    // SAT::Form eval = form.evaluate ();
    std::cout << form.toString () << std::endl;
    // eval.callDump (std::cout);
    // std::cout << std::endl;
    SAT::CNF_3 cool (form);
    std::cout << cool.toString () << std::endl;

    return 0;
}