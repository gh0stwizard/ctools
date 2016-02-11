/* print number in reverse */
#include <stdio.h>
#include <stdlib.h>


int
main (int argc, char *argv[])
{
        int number, right_digit;
        short negative;

        if (argc != 2) {
                fprintf (stderr, "Usage: %s number\n", argv[0]);
                return 1;
        }

        negative = 0;
        number = atoi (argv[1]);

        if (number < 0) {
                number = -number;
                negative = 1;
        }

        while (number != 0) {
                right_digit = number % 10;
                number /= 10;
                printf ("%i", right_digit);
        }

        if (negative)
                printf("-");

        printf("\n");
        return 0;
}
