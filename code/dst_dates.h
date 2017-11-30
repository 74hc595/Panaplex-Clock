#ifndef SRC_DST_DATES_H_
#define SRC_DST_DATES_H_

#include <stdint.h>

struct dst_date_pair {
  unsigned int spring:4;
  unsigned int fall:4;
};

extern const uint16_t spring_dst_dates[16];
extern const uint16_t fall_dst_dates[16];
extern const struct dst_date_pair dst_dates_for_year[100];

#endif

