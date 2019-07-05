#include "ltm_parser.h"


static char *skip(char *pos)
{
    while (isspace(*pos))
        pos++;
    return pos;
}

static char *consume(char *pos, char sym)
{
    if (*pos == sym)
        return ++pos;
    else
        return NULL;
}

static char *number(char *pos, int max, int *result)
{
    bool valid = false;
    *result = 0;

    while (isdigit(*pos)) {
        valid = true;
        *result = (*result * 10) + (*pos - '0');
        pos++;
    }

    if (!valid || *result >= max)
        return NULL;
    else
        return pos;
}

char *ltm_decode(char *s, uint8_t *sym)
{
    if (*s == '\\' && *(s + 1) != '\0') {
        char esc = *(s + 1);

        if (isdigit(esc)) {
            *sym = esc - '0';
            return s + 2;
        } else if (esc == 'n') {
            *sym = '\n';
            return s + 2;
        }
    }

    *sym = *s;
    return s + 1;
}

ltm_status ltm_msgvalidate(struct ltm *data, char *str)
{
    int i;

    for (; *str != '\0'; str++) {
        if (!isprint(*str))
            return LTM_BAD;

        if (*str == '\\' && isdigit(*(str + 1))) {
            if ((str = number(++str, GLYPH_MEMLOC, &i)) != NULL) {
                if (!data->active_glyphs[i])
                    return LTM_BAD;
            } else {
                return LTM_BAD;
            }
        }
    }

    return LTM_OK;
}

ltm_status ltm_parse(struct ltm *data, char *sentence)
{
    int memloc;
    char *pos = skip(sentence);
    memset(data, 0, sizeof(*data));

    switch (*pos++) {
    case '!':
        while (*pos != '\0') {
            pos = skip(pos);

            switch (*pos++) {
            case 'M':
                if ((pos = consume(pos, ':')) == NULL)
                    return LTM_BAD;
                if (*pos == '\0')
                    return LTM_BAD;

                if (ltm_msgvalidate(data, pos) == LTM_BAD) {
                    return LTM_BAD;
                } else {
                    data->text = pos;
                    return LTM_OK;
                }
                break;

            case 'C':
                if ((pos = number(pos, GLYPH_MEMLOC, &memloc)) == NULL)
                    return LTM_BAD;
                if (data->active_glyphs[memloc])
                    return LTM_BAD;
                if ((pos = consume(pos, ':')) == NULL)
                    return LTM_BAD;

                for (int i = 0; i < GLYPH_ROWS; i++) {
                    pos = skip(pos);
                    if ((pos = number(pos, (1 << GLYPH_COLS), (int*)&data->glyphs[memloc][i])) == NULL)
                        return LTM_BAD;
                    if ((pos = consume(pos, ',')) == NULL)
                        return LTM_BAD;
                }
                data->active_glyphs[memloc] = true;
                break;
            }
        }
        break;

    case '?':
        pos = skip(pos);
        if (*pos == '\0')
            return LTM_ECHO;
        else
            return LTM_BAD;
        break;
    }

    return LTM_BAD;
}


/* #include <stdio.h>

int main(void)
{
    char buffer[1024];
    struct ltm remote;

    fgets(buffer, 1024, stdin);
    buffer[strcspn(buffer, "\r\n")] = 0;
    ltm_status s = ltm_parse(&remote, buffer);
    if (s == LTM_OK) {
        printf("Message: %s\n", remote.text);
        for (int i = 0; i < 8; i++) {
            if (remote.active_glyphs[i])
                printf("Glyph: %d\n", remote.active_glyphs[i]);
        }
    } else if (s == LTM_BAD) {
        puts("Parse Error");
    } else {
        puts("Response");
    }

    putchar('\n');
} */
