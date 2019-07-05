/*
 * LTM (Light Text Message)
 *
 * Parser for simple custom application layer protocol
 * over RFCOMM for chacter LCD.
 *
 * Command: Ask for currently displayed message
 * Text:    "?"
 * Regex:   ^\s*\?\s*$
 *
 * Command: Set new message
 * Text:    "! C%n:%d,%d,%d,%d,%d,%d,%d,%d C%n: M:Some message\n\0
 * Regex:   ^\s*!\s*(\s*C[0-7]:(\s*([0-9]|[1-2][0-9]|3[0-1]),){8}){0-7}\s*M:.+$
 *
 * Escaped one digit number is special character mark
 *
 */

#ifndef LTM_PARSER_H
#define LTM_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

typedef enum {
    LTM_OK,
    LTM_BAD,
    LTM_ECHO
} ltm_status;

#define LTM_RESPONSE_OK     ">OK\n"
#define LTM_RESPONSE_BAD    ">BAD\n"
#define LTM_RESPONSE_MSG    ">MSG "

#define GLYPH_COLS      5
#define GLYPH_ROWS      8
#define GLYPH_MEMLOC    8

struct ltm {
    char *text;
    uint8_t glyphs[GLYPH_MEMLOC][GLYPH_ROWS];
    bool active_glyphs[GLYPH_MEMLOC];
};

char *ltm_decode(char *s, uint8_t *sym);
ltm_status ltm_parse(struct ltm *data, char *sentence);
ltm_status ltm_msgvalidate(struct ltm *data, char *str);

#endif
