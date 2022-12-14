#ifndef MPT_PARSING
#define MPT_PARSING

#include "multiple_precision_type.h"

typedef int (*char_parser)(const char);

typedef mpt *(*str_parser)(const char **);

int parse_char(const char c, const enum bases base);

mpt *mpt_parse_str_bin(const char **str);

mpt *mpt_parse_str_dec(const char **str);

mpt *mpt_parse_str_hex(const char **str);

mpt *mpt_parse_str(const char **str);

#endif