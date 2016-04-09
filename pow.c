#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* long double powl(long double x, long double y); */

int
main (int argc, char *argv[])
{
#ifdef USE_POW
        double b, p;
#else
        long double b, p;
#endif

        if (argc == 3) {
                b = strtold (argv[1], NULL);
                p = strtold (argv[2], NULL);
#ifdef USE_POW
                printf ("%.0f\n", pow (b, p));
#else
                printf ("%.0Lf\n", powl (b, p));
#endif
        }
        else {
                fprintf (stderr, "Usage: %s base power\n", argv[0]);
                return 1;
        }

        return 0;
}
