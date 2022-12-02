#ifndef MPT_PARSING
#define MPT_PARSING

#include "multiple_precision_type.h"

typedef int (*parse_char)(char);

int mpt_parse_char(mpt *target, const char c, const enum bases base);

int mpt_parse_str(mpt **target, const char *str, const enum bases base);

#endif