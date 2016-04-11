#ifdef __linux__
#define _BSD_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>

int
main (void)
{
    double avenrun[3];
    int i;

    if (getloadavg (avenrun, sizeof (avenrun) / sizeof (avenrun[0])) == -1)
        return (1);

    for (i = 0; i < (int)(sizeof (avenrun) / sizeof (avenrun[0])); i++)
        if (i > 0)
            printf (", %.2f", avenrun[i]);
        else
            printf ("%.2f", avenrun[i]);
    printf ("\n");

    return (0);
}
