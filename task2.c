#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

enum
{
    NPREF = 2,
    NHASH = 5,
    MAXGEN = 10000,
    MULTIPLIER = 31
};

typedef struct State State;
typedef struct Suffix Suffix;
typedef struct WordNode WordNode;

struct WordNode
{
    char *word;
    WordNode *next;
};

struct State
{
    WordNode *pref[NPREF];
    Suffix *suf;
    State *next;
};

struct Suffix
{
    WordNode *word;
    Suffix *next;
};

State *statetab[NHASH];
WordNode *wordtab[NHASH];

unsigned int hash(char *s)
{
    unsigned int h = 0;
    unsigned char *p;
    for (p = (unsigned char *)s; *p != '\0'; p++)
        h = MULTIPLIER * h + *p;
    return h % NHASH;
}

WordNode *lookupWord(char *s, int create)
{
    int h = hash(s);
    WordNode *wp;
    for (wp = wordtab[h]; wp != NULL; wp = wp->next)
        if (strcmp(s, wp->word) == 0)
            return wp;
    if (create)
    {
        wp = (WordNode *)malloc(sizeof(WordNode));
        if (wp == NULL)
        {
            perror("Memory allocation error.");
            exit(1);
        }
        wp->word = strdup(s);
        wp->next = wordtab[h];
        wordtab[h] = wp;
    }
    return wp;
}

unsigned int hashState(WordNode *pref[NPREF])
{
    unsigned int h = 0;
    for (int i = 0; i < NPREF; i++)
        h = MULTIPLIER * h + (unsigned long)pref[i];
    return h % NHASH;
}

State *lookupState(WordNode *prefix[NPREF], int create)
{
    int h = hashState(prefix);
    State *sp;
    for (sp = statetab[h]; sp != NULL; sp = sp->next)
    {
        int i;
        for (i = 0; i < NPREF; i++)
            if (prefix[i] != sp->pref[i])
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
        for (int i = 0; i < NPREF; i++)
            sp->pref[i] = prefix[i];
        sp->suf = NULL;
        sp->next = statetab[h];
        statetab[h] = sp;
    }
    return sp;
}

void addsuffix(State *sp, WordNode *suffix)
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

void add(WordNode *prefix[NPREF], WordNode *suffix)//додаємо суфікс до знайденого/створеного стану та оновлюємо масив префіксів додаючи до них суфікс
{
    State *sp;
    sp = lookupState(prefix, 1);
    addsuffix(sp, suffix);
    memmove(prefix, prefix + 1, (NPREF - 1) * sizeof(prefix[0]));
    prefix[NPREF - 1] = suffix;
}

void build(WordNode *prefix[NPREF], FILE *f)
{
    char buf[100], fmt[10];
    sprintf(fmt, "%%%lds", sizeof(buf) - 1);
    printf("fmt=\"%s\"\n", fmt);
    while (fscanf(f, fmt, buf) != EOF)
    {
        printf("%s ", buf);
        add(prefix, lookupWord(buf, 1)); 
    }
    putchar('\n');
}

void generate(int nwords)
{
    WordNode *NONWORD = lookupWord("\n", 1);
    State *sp;
    Suffix *suf;
    WordNode *prefix[NPREF], *w;
    int i, nmatch;
    for (i = 0; i < NPREF; i++)
        prefix[i] = NONWORD;
    for (i = 0; i < nwords; i++)
    {
        sp = lookupState(prefix, 0);
        nmatch = 0;
        for (suf = sp->suf; suf != NULL; suf = suf->next)
            if (rand() % ++nmatch == 0)
                w = suf->word;
        if (w == NONWORD)
            break;
        printf("%s\n", w->word);
        memmove(prefix, prefix + 1, (NPREF - 1) * sizeof(prefix[0]));
        prefix[NPREF - 1] = w;
    }
}

int main(void)
{
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    int i, nwords = MAXGEN;
    WordNode *prefix[NPREF];
    WordNode *NONWORD = lookupWord("\n", 1);
    for (i = 0; i < NPREF; i++)
        prefix[i] = NONWORD;

    FILE *file = fopen("brooks.txt", "r");
    if (file)
    {
        build(prefix, file);
        fclose(file);
    }
    add(prefix, NONWORD);
    generate(nwords);

    gettimeofday(&end_time, NULL);
    long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000L + (end_time.tv_usec - start_time.tv_usec);
    printf("Program execution time: %ld microseconds\n", elapsed_time);

    return 0;
}