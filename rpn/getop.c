#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "calc.h"

/**
 * getop: извлекает следующий операнд или знак операции
 */
int
getop (char s[])
{
        char *c = s;

        while (*c == ' ' || *c == '\t') /* пропуск пробелов */
                c++;

        if (islower (*c)) { /* команда или имя */
                while (islower (*++c));
                if (*c == '\0')
                        return NAME;    /* > 1 это имя */
                else
                        return *s;      /* возможно, это команда */
        }

        if (!isdigit (*c) && *c != '.' && *c != '-')
                return *c; /* не число */

        if (*c == '-')
                c++;

        if (isdigit (*c) || *c == '.')
                c++; /* отрицательное число */
        else
                return '-'; /* знак минус */

        if (isdigit (*c)) /* накопление целой части */
                while (isdigit (*++c));

        if (*c == '.') /* накопление дробной части */
                while (isdigit (*++c));

        return NUMBER;
}
