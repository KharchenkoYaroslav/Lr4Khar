#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

enum
{
    NPREF = 2,
    NHASH = 5,
    MAXGEN = 10000
};

typedef struct State State;
typedef struct Suffix Suffix;

struct State
{
    char *pref[NPREF];
    Suffix *suf;
    State *next;
};

struct Suffix
{
    char *word;
    Suffix *next;
};

State *statetab[NHASH];

enum
{
    MULTIPLIER = 31
};

unsigned int hash(char *s[NPREF])//хешування
{
    unsigned int h = 0;
    unsigned char *p;
    for (int i = 0; i < NPREF; i++)
        for (p = (unsigned char *)s[i]; *p != '\0'; p++)
            h = MULTIPLIER * h + *p;
    return h % NHASH;
}

char *mystrdup(char *s)//метод для дуплікації
{
    char *t;
    t = (char *)malloc(strlen(s) + 1);
    if (t == NULL)
    {
        perror("Error: strdup failed.");
        exit(1); 
    }
    strcpy(t, s);
    return t;
}

State *lookup(char *prefix[NPREF], int create)//перевіряємо чи є прафікси в таблиці, якщо хоча б одного нема та create дозволяє то додаємо новий елемент в таблицю
{
    int i;
    State *sp;
    int h = hash(prefix);
    for (sp = statetab[h]; sp != NULL; sp = sp->next)
    {
        for (i = 0; i < NPREF; i++)
            if (strcmp(prefix[i], sp->pref[i]) != 0)
                break;
        if (i == NPREF)
            return sp;
    }
    if (create)
    {
        sp = (State *)malloc(sizeof(State));
        if (sp == NULL)
        {
            perror("Memory allocation error.");
            exit(1);
        }
        for (i = 0; i < NPREF; i++)
            sp->pref[i] = prefix[i];
        sp->suf = NULL;
        sp->next = statetab[h];
        statetab[h] = sp;
    }
    return sp;
}


void addsuffix(State *sp, char *suffix)//додаємо новий суфікс до елемента
{
    Suffix *suf;
    suf = (Suffix *)malloc(sizeof(Suffix));
    if (suf == NULL)
    {
        perror("Memory allocation error.");
        exit(1);
    }
    suf->word = suffix;
    suf->next = sp->suf;
    sp->suf = suf;
}

void add(char *prefix[NPREF], char *suffix)//додаємо суфікс до знайденого/створеного стану та оновлюємо масив префіксів додаючи до них суфікс
{
    State *sp;
    sp = lookup(prefix, 1);
    addsuffix(sp, suffix);
    memmove(prefix, prefix + 1, (NPREF - 1) * sizeof(prefix[0]));
    prefix[NPREF - 1] = suffix;
}

void build(char *prefix[NPREF], FILE *f) {
    char buf[100], fmt[10];
    sprintf(fmt, "%%%lds", sizeof(buf) - 1);
    printf("fmt=\"%s\"\n", fmt);
    while (fscanf(f, fmt, buf) != EOF) {
        printf("%s ", buf);
        add(prefix, mystrdup(buf));
    }
    
    add(prefix, mystrdup(""));
    putchar('\n');
}

void generate() {
    State *sp;
    Suffix *suf;
    char *prefix[NPREF], *w;
    int i, nmatch;
    for (i = 0; i < NPREF; i++) {
        prefix[i] = ""; 
    }
    for (i = 0; i < MAXGEN; i++) {
        sp = lookup(prefix, 0);
        if (sp == NULL || sp->suf == NULL) {
            break; 
        }
        nmatch = 0;
        for (suf = sp->suf; suf != NULL; suf = suf->next) {
            if (rand() % ++nmatch == 0) {
                w = suf->word;
            }
        }
        if (strcmp(w, "") == 0) {
            break; 
        }
        printf("%s \n", w);
        memmove(prefix, prefix + 1, (NPREF - 1) * sizeof(prefix[0]));
        prefix[NPREF - 1] = w;
    }
    putchar('\n');
}
int main(void) {
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    int i;
    char *prefix[NPREF];
    for (i = 0; i < NPREF; i++) {
        prefix[i] = ""; 
    }

    FILE *file = fopen("brooks.txt", "r");
    if (file) {
        build(prefix, file);
        fclose(file);
    }

    generate();

    gettimeofday(&end_time, NULL);
    long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000L + (end_time.tv_usec - start_time.tv_usec);
    printf("Program execution time: %ld microseconds\n", elapsed_time);

    return 0;
}