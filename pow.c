#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* long double powl(long double x, long double y); */

int
main (int argc, char *argv[])
{
        long double b, p;

        if (argc == 3) {
                b = strtold (argv[1], NULL);
                p = strtold (argv[2], NULL);
                printf ("%.0Lf\n", powl (b, p));
        }
        else {
                fprintf (stderr, "Usage: %s base power\n", argv[0]);
                return 1;
        }

        return 0;
}
