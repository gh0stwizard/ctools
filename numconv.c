/* Программа преобразовывает положительное целое число в другую
   систему счисления. */
#include <stdio.h>
#include <stdlib.h>

int convertedNumber[64];
long int numberToConvert;
int base;
int digit = 0;


int
main (int argc, char *argv[])
{
        void    getNumberAndBase (const char n[], const char base[]),
                convertNumber (void),
                displayConvertedNumber (void);

        if (argc == 3) {
                getNumberAndBase(argv[1], argv[2]);
                convertNumber();
                displayConvertedNumber();
                return 0;
        }
        else {
                fprintf (stderr, "Usage: %s number base\n", argv[0]);
                return 1;
        }
}

void
getNumberAndBase (const char n[], const char b[])
{
        extern long int numberToConvert;
        extern int base;

        numberToConvert = atol (n);
        base = atoi (b);

        if (base < 2 || base > 16) {
                fprintf (stderr, "Base must be between 2 and 16. Using 10\n");
                base = 10;
        }
}

void
convertNumber (void)
{
        extern int convertedNumber[];
        extern long int numberToConvert;
        extern int base;

        do {
                convertedNumber[digit] = numberToConvert % base;
                ++digit;
                numberToConvert /= base;
        } while ( numberToConvert != 0 );
}

void
displayConvertedNumber (void)
{
        const char baseDigits[16] = {
                '0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
        };
        int nextDigit;
        extern int convertedNumber[];

        if (base == 16)
                printf ("0x");

        for (--digit; digit >= 0; --digit) {
                nextDigit = convertedNumber[digit];
                printf ("%c", baseDigits[nextDigit]);
        }
        printf("\n");
}
