#define MAXVAL 100      /* максимальная глубина стека val */

static int sp = 0;              /* следующая свободная позиция в стеке */
static double val[MAXVAL];      /* стек операндов */

/**
 * push: помещает число f в стек операндов
 */
void
push (double f)
{
        if (sp < MAXVAL)
                val[sp++] = f;
        else
                printf ("error: stack full, can't push %g\n", f);
}

/**
 * pop: извлекает и возвращает верхнее число из стека
 */
double
pop (void)
{
        if (sp > 0)
                return val[--sp];
        else {
                printf ("error: stack empty\n");
                return 0.0;
        }
}

/**
 * clear: очищает стек
 */
void
clear (void)
{
        sp = 0;
}
