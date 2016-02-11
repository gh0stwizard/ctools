#include <stdio.h>
#define YES     1
#define NO      0

long long htoi (char s[]);

int
main (int argc, char *argv[])
{
        if (argc == 2) {
                printf ("%lli\n", htoi (argv[1]));
                return 0;
        }
        else {
                fprintf (stderr, "Usage: %s hex (max 0x7fffffffffffffff)\n",
                        argv[0]);
                return 1;
        }
}

/* преобразование строки 16-ных цифр в число */
long long
htoi (char s[])
{
        int i, hexdigit;
        int ishex, changesign;
        long long n;

        i = 0;
        changesign = NO;
        if (s[i] == '-') {
                ++i;
                changesign = YES;
        }

        if (s[i] == '0') {
                ++i;
                if (s[i] == 'x' || s[i] == 'X')
                        ++i;
        }

        for (n = 0, ishex = YES; ishex == YES; ++i) {
                if (s[i] >= '0' && s[i] <= '9')
                        hexdigit = s[i] - '0';
                else if (s[i] >= 'a' && s[i] <= 'f')
                        hexdigit = s[i] - 'a' + 10;
                else if (s[i] >= 'A' && s[i] <= 'F')
                        hexdigit = s[i] - 'A' + 10;
                else
                        ishex = NO;

                if (ishex == YES)
                        n = 16.0L * n + hexdigit;
        }

        if (changesign == YES)
                n = -n;

        return n;
}
