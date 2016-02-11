/* Программа расчета кол-ва дней между двумя датами */
#include <stdio.h>

struct date {
        int month;
        int day;
        int year;
};

int
main (int argc, char *argv[])
{
        int days(struct date);
        struct date formatDate(const char s[]);

        if (argc == 3) {
                struct date date1 = formatDate(argv[1]);
                struct date date2 = formatDate(argv[2]);

                int n1 = days (date1);
                int n2 = days (date2);

                if (n1 > n2) {
                        n1 ^= n2;
                        n2 ^= n1;
                        n1 ^= n2;
                }

                printf("%i\n", n2 - n1);
                return 0;
        }
        else {
                fprintf (stderr, "Usage: %s mmddyyyy mmddyyyy\n",
                        argv[0]);
                return 1;
        }
}

/* Формула (модификация алгоритма Гаусса вычисления даты Пасхи):
 *   N = 1461 * f(year, month) / 4 + 153 * g(month) / 5 + day
 * где
 *   f(year, month) = year - 1 при month <= 2
 *   иначе year;
 *   g(month) = month + 13 при month <= 2
 *   иначе month + 1
 * для всех дат после March 1, 1900
 * N + 1 для дат от March 1, 1800 до February 28, 1900
 * N + 2 для дат от March 1, 1700 до February 28, 1800
 */
int
days (struct date d)
{
        int f (int year, int month);
        int g (int month);
        int delta (struct date d);
        int n;

        n = (1461 * f(d.year, d.month)) / 4 + (153 * g(d.month)) / 5 + d.day;
        return n + delta(d);
}

int
f (int year, int month)
{
        if (month <= 2)
                return year - 1;
        else
                return year;
}

int
g (int month)
{
        if (month <= 2)
                return month + 13;
        else
                return month + 1;
}

int
delta (struct date d)
{
        if (d.year > 1900)
                return 0;
        else if (d.year == 1900) {
                if (d.month >= 3)
                        return 1;       /* добавляем 1 день */
                else
                        return 0;
        }
        else if (d.year > 1800)
                return 1;               /* добавляем 1 день */
        else if (d.year == 1800) {
                if (d.month >= 3)
                        return 1;       /* добавляем 1 день */
                else
                        return 2;       /* добавляем 2 дня */
        }
        else if (d.year > 1700)
                return 2;               /* добавляем 2 дня */
        else if (d.year == 1700) {
                if (d.month >= 3)
                        return 2;       /* добавляем 2 дня */
                else
                        return 0;       /* XXX: неизвестно */
        }
        else
                return 0;               /* XXX: неизвестно */
}

struct date
formatDate(const char s[])
{
        struct date d;
        sscanf(s, "%2i%2i%4i", &d.month, &d.day, &d.year);
        return d;
}
