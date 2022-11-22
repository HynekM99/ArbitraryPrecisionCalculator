#ifndef MPT_PARSING
#define MPT_PARSING
#include "multiple_precision_type.h"

int mpt_parse_dec_char(mpt *target, const char c);

int mpt_parse_str(mpt **target, const char *str);

#endif