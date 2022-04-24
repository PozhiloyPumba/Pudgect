#include <stdlib.h>

#include <cstring>
#include <ctime>
#include <iostream>

/*
    first argument  = filename
    second argument = max count of variables
    third argument  = count of operation
*/

int main (int argc, char **argv)
{
    srand (time (nullptr));

    if (argc != 4) {
        std::cout << "Oops - wrong arguments\n"
                  << std::endl;
        return -1;
    }

    FILE *wrt = fopen (argv[1], "w");
    int br = 0;
    std::string result = "";

    for (int i = 0, sz = atoi (argv[3]); i < sz; ++i) {
        if (!(rand () % 3))  // 33%
            result += "~";

        if (!(rand () % 3)) {  // 33%
            result += "(";
            ++br;
        }

        if (!(rand () % 3))  // 33%
            result += "~";

        result += "v" + std::to_string (rand () % atoi (argv[2]));

        if (!(rand () % 3) && br > 0) {  // 33%
            result += ")";
            --br;
        }

        switch (rand () % 3) {
            case 0:
                result += "|";
                break;
            case 1:
                result += "&";
                break;
            case 2:
                result += "->";
                break;
        }
    }

    if (rand () % 2)
        result += "~";

    result += ("v" + std::to_string (rand () % atoi (argv[2])));

    for (int i = 0; i < br; ++i)
        result += ")";

    result += ";";

    fprintf (wrt, "%s", result.data ());
    fclose (wrt);

    return 0;
}