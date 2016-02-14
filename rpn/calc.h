#define NUMBER '0'      /* сигнал, что обнаружено число */
#define NAME 'n'        /* сигнал, что обнаружено имя */

int getop (char s[]);
void push (double);
double pop (void);
void clear (void);
void mathfnc (const char s[]);
