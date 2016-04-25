#include "Arduino.h"
#include <unabto/unabto_external_environment.h>

#define MAX_STAMP_DIFF 0x7fffffff;

bool nabtoIsStampPassed(nabto_stamp_t *stamp)
{
    return *stamp - nabtoGetStamp() > (uint32_t)MAX_STAMP_DIFF;
}

nabto_stamp_diff_t nabtoStampDiff(nabto_stamp_t * newest, nabto_stamp_t * oldest)
{
    return (*newest - *oldest);
}

int nabtoStampDiff2ms(nabto_stamp_diff_t diff)
{
    return (int) diff;
}
