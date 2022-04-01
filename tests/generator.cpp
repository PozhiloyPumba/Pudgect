#include <stdlib.h>

#include <cstring>
#include <ctime>
#include <iostream>

/*
    first argument  = filename
    second argument = max count of variables
    third argument  = count of operation
*/

void itoa (int res, char *buf, int strl);
int sizeofnum (int res);

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
        if (!(rand () % 3)) // 33%
            result += "~";

        if (!(rand () % 3)) { // 33%
            result += "(";
            ++br;
        }

        if (!(rand () % 3)) // 33%
            result += "~";

        switch (rand () % 3) {
            case 0: result += ("v" + std::to_string (rand () % atoi (argv[2])) + "|"); break;
            case 1: result += ("v" + std::to_string (rand () % atoi (argv[2])) + "&"); break;
            case 2: result += ("v" + std::to_string (rand () % atoi (argv[2])) + "->"); break;
        }
    }

    if (rand () % 2)
        result += "~";

    result += ("v" + std::to_string (rand () % atoi (argv[2])));

    for (int i = 0; i < br; ++i)
        result += ")";

    fprintf (wrt, "%s", result.data ());
    fclose (wrt);

    return 0;
}