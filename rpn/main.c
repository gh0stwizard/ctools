#include <stdio.h>
#include <stdlib.h>     /* для объявления atof() */
#include <math.h>       /* для объявления fmod(), pow(), exp(), sin() */
#include "calc.h"

/* калькулятор с обратной польской записью */
int
main (int argc, char *argv[])
{
        int i, type, var = 0;
        double op1, op2;
        double variable[26];
        char *s;

        for (i = 0; i < 26; i++)
                variable[i] = 0.0;

        while (--argc != 0) {
                s = *++argv;
                type = getop (s);
                switch (type) {
                case NUMBER:
                        push (atof (s));
                        break;
                case NAME:
                        mathfnc (s);
                        break;
                case '+':
                        push (pop () + pop ());
                        break;
                case '*':
                        push (pop () * pop ());
                        break;
                case '-':
                        op2 = pop ();
                        push (pop () - op2);
                        break;
                case '/':
                        op2 = pop ();
                        if (op2 != 0.0)
                                push (pop () / op2);
                        else
                                printf ("error: zero divisor\n");
                        break;
                case '%':
                        op2 = pop ();
                        if (op2 != 0.0)
                                push (fmod (pop (), op2));
                        else
                                printf ("error: zero divisor\n");
                        break;
                case 'c':       /* очистить стек */
                        clear ();
                        break;
                case '?':       /* напечатать верхний элемент стека */
                        op2 = pop ();
                        printf ("\t%.8g\n", op2);
                        push (op2);
                        break;
                case 'd':       /* продублировать верхний элемент стека */
                        op2 = pop ();
                        push (op2);
                        push (op2);
                        break;
                case 's':       /* поменять местами два верхних элемента */
                        op1 = pop ();
                        op2 = pop ();
                        push (op1);
                        push (op2);
                        break;
                case '=':       /* запомнить значение в переменную */
                        pop ();
                        if (var >= 'A' && var <= 'Z')
                                variable[var - 'A'] = pop ();
                        else
                                printf ("error: variable name not found\n");
                        break;
                default:
                        if (type >= 'A' && type <= 'Z')
                                push (variable[type - 'A']);
                        else
                                printf ("error: unknown command %s (%c)\n", s, type);
                        break;
                }
                /* запоминаем букву */
                var = type;
        }

        printf ("%.8g\n", pop ());
        return 0;
}
