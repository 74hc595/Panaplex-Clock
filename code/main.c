/**
 *
 * Panaplex clock
 *
 * Matt Sarnoff (msarnoff.org)
 * November 22, 2017
 *
 * Features:
 * - 10-digit gas discharge display
 * - DS3234 real-time clock with battery backup
 * - Time display with seconds
 * - Date, month, year, and day of week displays
 * - Sleep mode with display fade in/out effects
 * - Automatic daylight saving time correction (US only) up to 2099
 * - Leap year correction up to 2099
 * - Customizable display:
 *   - 12- or 24-hour format
 *   - Choice of colon "separators" with optional flashing
 *   - Choice of date separators (dashes or dots)
 * - Two capacitive touch sensors
 * - Menu interface
 * - Can be set to turn on/off at scheduled times
 *   - Selectable schedule for each weekday
 *   - Display can be on continuously 24 hours a day, off for 24 hours a day
 *     (activated by the touch sensors), or on during a choice of two time spans
 *
 * I have not taken the time to split the code into multiple files or add
 * decent comments.
 */

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover Mode (Internal/External Switchover Mode is disabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

#include <xc.h>
#include <stdint.h>

#include "io_macros.h"
#include "bit_macros.h"
#include "pin_assignments.h"
#include "dst_dates.h"
#include "display_macros.h"

#define _XTAL_FREQ 16000000

//void putch(unsigned char ch) {
//  while (!TXSTAbits.TRMT) {}
//  TXREG = ch;
//}

static inline void io_init(void)
{
  ALL_OUTPUTS = 0;

  pin_digital(ANODE0);
  pin_digital(ANODE1);
  pin_digital(ANODE2);
  pin_digital(ANODE3);
  pin_digital(ANODE4);
  pin_digital(MOSI);
  pin_digital(nRTC_SS);
  pin_digital(MISO);
  pin_digital(SQW);
  pin_analog(SENSE1);
  pin_analog(SENSE2);

  /* Slave selects are active low */
  pin_high(nDISP_SS);
  pin_high(nRTC_SS);
  
  pin_output(ANODE0);
  pin_output(ANODE1);
  pin_output(ANODE2);
  pin_output(ANODE3);
  pin_output(ANODE4);
  pin_output(MOSI);
  pin_output(SCK);
  pin_output(nRTC_SS);
  pin_output(nDISP_SS);
  pin_input(MISO);
  pin_input(SQW);
  pin_output(HV_EN);
  pin_input(SENSE1);
  pin_input(SENSE2);
}


static inline void osc_init(void)
{
  /* enable high frequency internal oscillator at 16 MHz */
  OSCCON = 0b01111010;
  while (!(OSCSTATbits.HFIOFS && OSCSTATbits.HFIOFR)) {}
}


static inline void hv_on(void)
{
  pin_high(HV_EN);
}


static void hv_off(void)
{
  pin_low(HV_EN);
}


static inline void spi_init(void)
{
  SSPCONbits.SSPM = 1; /* SPI Master mode, clock = Fosc/16 (1 MHz) */
  /* SPI mode 3, compatible with both TPIC6B595 and DS3234 */
  CKP = 1;
  CKE = 0; /* CKE is the inverse of what's usually called "CPHA" */
  SSPEN = 1;
}


static uint8_t spi_out(uint8_t n)
{
  do {
    WCOL = 0;
    SSPBUF = n;
  } while (WCOL); 
  while (!BF) {}
  return SSPBUF;
}


static inline void adc_init(void)
{
  ADFM = 1; /* Right-justified; bits 15:10 of result always 0 */
}


static inline void mux_timer_init(void)
{
  /* use Timer2 to drive digit multiplexing. */
  T2CONbits.T2CKPS = 0b01; /* 1/4 prescaler */
  PR2 = 250; /* 4 KHz. Each anode is on for ~250us */
  TMR2IE = 1;
  TMR2ON = 1;
}


static inline void mux_start(void)
{
  PEIE = 1;
  GIE = 1;
}


#define BRIGHTNESS_MAX  16
#define BRIGHTNESS_MIN  0

static uint8_t display[NUM_DIGITS] = {0};
static uint8_t fade_counter = 0;
static uint8_t fade_level = 0; /* fade in on power-up */
static uint8_t fade_target = BRIGHTNESS_MAX;
static void display_digits(uint8_t low_digit, uint8_t high_digit)
{
  pin_low(nDISP_SS);
  if ((fade_counter & 15) < fade_level) {
    spi_out(display[low_digit]);
    uint8_t n = display[high_digit];
    spi_out(SEGMENT_SWAP(n));
  } else {
    spi_out(0);
    spi_out(0);
  }
  pin_high(nDISP_SS);
}


static uint8_t mux_digit = 0;
static volatile bit blanked = 0;
static inline void timer2_interrupt(void)
{
  if (!TMR2IF) { return; }
  TMR2IF = 0;
  /* all anodes off */
  pins_low(ANODE012_PORT, ANODE012_PINS);
  pins_low(ANODE34_PORT, ANODE34_PINS);
  if (blanked || pin_is_low(nRTC_SS)) {
    return;
  }

  switch (mux_digit++) {
    /* Stagger the digits to prevent blue "streamers" due to insufficient */
    /* interdigit blanking time between neighboring digits. */
    case 0:
      display_digits(4,5);
      pin_high(ANODE0);
      return;
    case 1:
      display_digits(2,7);
      pin_high(ANODE2);
      return;
    case 2:
      display_digits(0,9);
      pin_high(ANODE4);
      return;
    case 3:
      display_digits(3,6);
      pin_high(ANODE1);
      return;
    case 4:
      display_digits(1,8);
      pin_high(ANODE3);
      mux_digit = 0;
      fade_counter++;
      return;
  }
}


static const uint8_t bcd_to_segments_left_half[16] = {
  c_0, c_1, c_2, c_3, c_4, c_5, c_6, c_7,
  c_8, c_9, 0,   0,   0,   0,   0,   0
};


static void display_clear(void)
{
  display[0] = 0; display[1] = 0; display[2] = 0; display[3] = 0; display[4] = 0;
  display[5] = 0; display[6] = 0; display[7] = 0; display[8] = 0; display[9] = 0;
}


static void set_bcd_byte(uint8_t pos, uint8_t byte)
{
  display[pos] = bcd_to_segments_left_half[byte >> 4];
  display[pos+1] = bcd_to_segments_left_half[byte & 0x0F];
}


typedef uint8_t bcd;
typedef uint16_t bcd2;

static bcd bcd_increment(bcd n) { return n + (((n & 0xF) == 9) ? 7 : 1); }
static bcd bcd_decrement(bcd n) { return n - (((n & 0xF) == 0) ? 7 : 1); }

static bcd increment_hour(bcd hr)                      { return (hr == 0x23) ? 0x00 : bcd_increment(hr); }
static bcd increment_minute(bcd min)                   { return (min == 0x59) ? 0x00 : bcd_increment(min); }
static bcd increment_month(bcd month)                  { return (month == 0x12) ? 0x01 : bcd_increment(month); }
static bcd increment_date(bcd date, bcd days_in_month) { return (date >= days_in_month) ? 0x01 : bcd_increment(date); }
static bcd increment_year(bcd year)                    { return (year == 0x99) ? 0x00 : bcd_increment(year); }

static bcd decrement_hour(bcd hr)                      { return (hr == 0x00) ? 0x23 : bcd_decrement(hr); }
static bcd decrement_minute(bcd min)                   { return (min == 0x00) ? 0x59 : bcd_decrement(min); }
static bcd decrement_month(bcd month)                  { return (month == 0x01) ? 0x12 : bcd_decrement(month); }
static bcd decrement_date(bcd date, bcd days_in_month) { return (date == 0x01) ? days_in_month : bcd_decrement(date); }
static bcd decrement_year(bcd year)                    { return (year == 0x00) ? 0x99 : bcd_decrement(year); }

/* Fast multiplication by 10 by adding (n<<1)+(n<<3). */
static uint8_t times10(uint8_t n)
{
  n <<= 1;
  uint8_t times2 = n;
  n <<= 2;
  return n+times2;  
}


static bcd leading_zero_suppressed(bcd n)
{
  return (n >= 0x10) ? n : n|0xF0; /* blank */
}


static uint8_t bcd_to_byte(bcd n)
{
  return times10(n >> 4) + (n & 0xF);
}


/* Returns 1 if the 2-digit BCD year is a leap year. */
/* Will only work for the range 2000-2099. (2100 is not a leap year) */
static bit is_leap_year(bcd year)
{
  year = bcd_to_byte(year);
  return (year & 3) == 0; /* leap year if divisble by 4 */
}


static const bcd days_in_month_table[] = {
  0x31, 0x28, 0x31, 0x30, 0x31, 0x30, 0x31, 0x31, 0x30, 0x31, 0x30
};

static bcd days_in_month(bcd month, uint8_t leap_year)
{
  /* let's just get this out of the way */
  if (leap_year && month == 2) { return 0x29; }
  /* hasty bcd to binary conversion */
  if (month & 0x10) { month -= 6; }
  if (month >= 11) { return 0x31; }
  return days_in_month_table[month-1];
}


static bcd to_12_hour(bcd hour)
{
  if (hour == 0x00) { /* midnight */
    return 0x12;
  } else if (hour <= 0x12) {
    return hour;
  } else {
    return hour - ((hour == 0x20 || hour == 0x21) ? 24 : 18);
  }
}


static uint8_t am_pm_indicator_for_hour(bcd hour_24)
{
  return (hour_24 < 0x12) ? c_A : c_P; /* "A"/"P" indicator */
}


/* Sunday=1, Monday=2, ..., Saturday=7 */
static uint8_t weekday_for_date(bcd month, bcd date, bcd year)
{
  /* Sakamoto's method. */
  /* Not fast, but it only needs to be called when the user sets the date. */
  static const uint8_t t[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  month = bcd_to_byte(month);
  date = bcd_to_byte(date);
  uint16_t full_year = 2000 + bcd_to_byte(year);
  full_year -= (month < 3);
  /* If year is restricted to the range 2000-2099, */
  /* the (y/100) term is always 20 and the (y/400) term is always 5. */
  uint16_t sum = full_year + (full_year>>2) - 20 + 5 + t[month-1] + date;
  /* Sum will be in the range [2484, 2645] inclusive. */
  /* Subtract 2484. Add 6 because (2484%7)==6. */
  uint8_t day = sum - 2478;
  /* Compute mod 7 without using the mod operator, which increases program size. */
  while (day >= 7) { day -= 7; }
  return day+1;
}


static void rtc_set_register(uint8_t addr, uint8_t value)
{
  while (pin_is_low(nDISP_SS)) {}
  pin_low(nRTC_SS);
  spi_out(0x80|addr);
  spi_out(value);
  pin_high(nRTC_SS);
}


static uint8_t rtc_get_register(uint8_t addr)
{
  while (pin_is_low(nDISP_SS)) {}
  pin_low(nRTC_SS);
  spi_out(addr);
  uint8_t value = spi_out(0);
  pin_high(nRTC_SS);
  return value;
}


struct ds3234_time {
  union {
    struct {
      unsigned int seconds:8;
      unsigned int minutes:8;
      unsigned int hour:6;
      unsigned int mode_12hr:1;
      unsigned int :1;
      unsigned int weekday:3;
      unsigned int :5;
      unsigned int date:8;
      unsigned int month:5;
      unsigned int :2;
      unsigned int century:1;
      unsigned int year:8;
    };
    uint8_t bytes[7];
  };
};
/* Globally readable time. Updated once per second. */
static struct ds3234_time current_time;
static bit need_time_update = 0;
static bit need_time_redraw = 0;
/* Alarm 2 flag. Set daily at 02:00. Used to check for DST transitions. */
static bit a2f = 0;

static void update_sleep_state(void);
static void rtc_get_time(void)
{
  while (pin_is_low(nDISP_SS)) {}
  pin_low(nRTC_SS);
  spi_out(0); /* set register to 0 */
  for (uint8_t i = 0; i < sizeof(current_time); i++) {
    current_time.bytes[i] = spi_out(0); /* clock out bytes */
  }
  pin_high(nRTC_SS);
  need_time_update = 0;
  need_time_redraw = 1;
  a2f = (rtc_get_register(0x0F) & 0x2) != 0;
  update_sleep_state();
}


static void rtc_set_time_full(void)
{
  current_time.weekday = weekday_for_date(current_time.month, current_time.date, current_time.year);
  while (pin_is_low(nDISP_SS)) {}
  pin_low(nRTC_SS);
  spi_out(0x80); /* set register to 0 */
  for (uint8_t i = 0; i < sizeof(current_time); i++) {
    spi_out(current_time.bytes[i]); /* clock out bytes */
  }
  pin_high(nRTC_SS);
  rtc_get_time();
}


static void date_add_hour(void)
{
  /* Straightforward. Increment the smallest component. If it wraps around, */
  /* increment the next larger component, etc. */
  if ((current_time.hour = increment_hour(current_time.hour))           != 0x00) { goto done; }
  uint8_t max_date = days_in_month(current_time.month, is_leap_year(current_time.year));
  if ((current_time.date = increment_date(current_time.date, max_date)) != 0x01) { goto done; }
  if ((current_time.month = increment_month(current_time.month))        != 0x01) { goto done; }
  current_time.year = increment_year(current_time.year);
done:
  rtc_set_time_full();
}


static void date_subtract_hour(void)
{
  if ((current_time.hour = decrement_hour(current_time.hour)) != 0x23) {
    goto done;
  }
  /* Things are more complex if we need to roll back to the previous month */
  if (current_time.date == 0x01) {
    /* First, roll back one month */
    current_time.month = decrement_month(current_time.month);
    /* Roll back one year if we wrapped around */
    if (current_time.month == 0x12) {
      current_time.year = decrement_year(current_time.year);
    }
    /* Now set the date to the maximum value for the new month/year */
    current_time.date = days_in_month(current_time.month, is_leap_year(current_time.year));
  }
  /* If not, just roll back one day. `max_date` parameter will be ignored. */
  else {
    current_time.date = decrement_date(current_time.date, 0);
  }
done:
  rtc_set_time_full();
}


/* Settings stored in the RTC's battery-backed RAM. */
struct settings
{
  union {
    struct {
      unsigned int time_sep_mode:2;
      unsigned int time_sep_style:2;
      unsigned int date_sep_style:1;
      unsigned int display_12hr:1;
      unsigned int auto_dst:1;
      unsigned int in_dst:1;
    };
    uint8_t byte;
  };
};


struct schedule_range
{
  bcd on_hour;
  bcd off_hour;
};


struct schedule
{
  union {
    struct {
      uint8_t sleep_mode;
      uint8_t days[7];
      struct schedule_range ranges[2];
    };
    uint8_t bytes[12];
  };
};
static struct settings current_settings;
static struct schedule current_schedule;
static bit settings_need_commit = 0;
static bit schedule_needs_commit = 0;
static bit sleep = 0;
static bit sleep_now = 0;
static bit wake_now = 0;

/* Save settings to the RTC's battery-backed SRAM. */
static void save_settings(void)
{
  if (settings_need_commit) {
    rtc_set_register(0x18, 0); /* SRAM address 0 */
    while (pin_is_low(nDISP_SS)) {}
    pin_low(nRTC_SS);
    spi_out(0x99); /* SRAM data register */
    spi_out(current_settings.byte);
    spi_out(current_settings.byte ^ 0xFF); /* save an inverted copy in lieu of a checksum */
    pin_high(nRTC_SS);
    settings_need_commit = 0;
  }
}


/* Read settings from the RTC's battery-backed SRAM. */
static void read_settings(void)
{
  rtc_set_register(0x18, 0); /* set SRAM address register */
  while (pin_is_low(nDISP_SS)) {}
  pin_low(nRTC_SS);
  spi_out(0x19); /* SRAM data register */
  uint8_t data = spi_out(0);
  uint8_t data_inv = spi_out(0);
  pin_high(nRTC_SS);
  if (data == (data_inv ^ 0xFF)) {
    current_settings.byte = data;
  } else {
    /* No saved data. Set defaults. */
    current_settings.time_sep_mode = 1;
    current_settings.auto_dst = 1;
    settings_need_commit = 1;
    save_settings();
  }
}


static const struct schedule_range on_24h_range  = { 0x00, 0x00 }; /* midnight to midnight */
static const struct schedule_range off_24h_range = { 0x24, 0x25 }; /* all out of range */

static struct schedule_range sleep_range_for_weekday(uint8_t day)
{
  uint8_t sleep_mode_for_day = current_schedule.days[day];
  if (sleep_mode_for_day == 0) {
    return off_24h_range;
  } else if (sleep_mode_for_day == 1) {
    return current_schedule.ranges[0];
  } else if (sleep_mode_for_day == 2) {
    return current_schedule.ranges[1];
  } else {
    return on_24h_range;
  }
}


static void update_sleep_state(void)
{
  uint8_t old_sleep = sleep;
  uint8_t sleep_mode = current_schedule.sleep_mode;
  if (sleep_mode < 2) {
    /* Scheduled sleep disabled; directly on or off */
    sleep = (sleep_mode & 1);
  } else {
    bcd hour = current_time.hour;
    uint8_t today = current_time.weekday-1;
    uint8_t yesterday = (today == 0) ? 6 : today-1;
    struct schedule_range today_range = sleep_range_for_weekday(today);
    struct schedule_range yesterday_range = sleep_range_for_weekday(yesterday);
    /* If off hour <= on hour, then the range extends past midnight, and */
    /* the range ends when the off hour is reached on the following day. */
    /* So we may need to make two range checks; one against today's range, */
    /* and another against the sliver between midnight and yesterday's */
    /* post-midnight off hour. (This sliver will be an empty range if */
    /* yesterday's off hour > on hour. */
    if (yesterday_range.off_hour <= yesterday_range.on_hour) {
      yesterday_range.on_hour = 0x00;
    } else {
      yesterday_range.on_hour = 0x00;
      yesterday_range.off_hour = 0x00;
    }
    /* Also, if today's range extends past midnight (off time <= on time) */
    /* we don't need to do the "after off time" check. */
    if (today_range.off_hour <= today_range.on_hour) {
      today_range.off_hour = 0x24;
    }
    /* We are awake if the current hour falls within the "today" range */
    /* or the remnant of yesterday's range if it extended past midnight. */
    sleep = !((hour >= today_range.on_hour && hour < today_range.off_hour) ||
             (hour >= yesterday_range.on_hour && hour < yesterday_range.off_hour));
  }
  /* Sleep or wake? */
  if (sleep && !old_sleep) {
    sleep_now = 1;
  } else if (!sleep && old_sleep) {
    wake_now = 1;
  }
}


/* Save schedule to the RTC's battery-backed SRAM. */
static void save_schedule(void)
{
  if (schedule_needs_commit) {
    rtc_set_register(0x18, 2); /* SRAM address 2 */
    while (pin_is_low(nDISP_SS)) {}
    pin_low(nRTC_SS);
    spi_out(0x99); /* SRAM data register */
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < sizeof(struct schedule); i++) {
      uint8_t byte = current_schedule.bytes[i];
      spi_out(byte);
      checksum += byte;
    }
    checksum ^= 0xFF;
    spi_out(checksum);
    pin_high(nRTC_SS);
    schedule_needs_commit = 0;
  }
}


/* Read schedule from the RTC's battery-backed SRAM. */
static void read_schedule(void)
{
  rtc_set_register(0x18, 2); /* set SRAM address register */
  while (pin_is_low(nDISP_SS)) {}
  pin_low(nRTC_SS);
  spi_out(0x19); /* SRAM data register */
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < sizeof(struct schedule); i++) {
    uint8_t byte = spi_out(0);
    current_schedule.bytes[i] = byte;
    checksum += byte;
  }
  checksum ^= 0xFF;
  uint8_t expected_checksum = spi_out(0);
  pin_high(nRTC_SS);
  if (checksum != expected_checksum) {
    /* unrolled, this uses *less* program space than a loop */
    current_schedule.sleep_mode = 0;
    current_schedule.days[0] = 0;
    current_schedule.days[1] = 0;
    current_schedule.days[2] = 0;
    current_schedule.days[3] = 0;
    current_schedule.days[4] = 0;
    current_schedule.days[5] = 0;
    current_schedule.days[6] = 0;
    current_schedule.ranges[0].on_hour = 0x09;
    current_schedule.ranges[0].off_hour = 0x17;
    current_schedule.ranges[1].on_hour = 0x20;
    current_schedule.ranges[1].off_hour = 0x23;
    schedule_needs_commit = 1;
    save_schedule();
  }
  update_sleep_state();
}



/* Type suitable for month/date/hour comparisons. */
#ifdef __XC8__
typedef unsigned long short int u24;
#else
typedef uint32_t u24;
#endif
__pack struct month_date_hour {
  union {
    struct {
      bcd hour;
      bcd date;
      bcd month;
    };
    struct {
      bcd __hour;
      bcd2 date_and_month;
    };
    u24 value;
  };
};


static bit current_date_is_in_dst(void)
{
  uint8_t bin_year = bcd_to_byte(current_time.year);
  if (bin_year >= 100) { return 0; } /* shouldn't happen... */
  struct dst_date_pair indexes = dst_dates_for_year[bin_year];

  struct month_date_hour now;
  now.hour = current_time.hour;
  now.date = current_time.date;
  now.month = current_time.month;

  /* need to check against spring or fall? */
  if (current_time.month <= 4) {
    struct month_date_hour other;
    other.hour = 0x02;
    other.date_and_month = spring_dst_dates[indexes.spring];
    return now.value >= other.value;
  } else {
    struct month_date_hour other;
    other.hour = 0x02;
    other.date_and_month = fall_dst_dates[indexes.fall];
    return now.value < other.value;
  }
}


/* Updates in_dst flag based on current date/time without modifying them. */
static void update_in_dst_flag(void)
{
  uint8_t new_dst = (current_settings.auto_dst) ? current_date_is_in_dst() : 0;
  if (new_dst != current_settings.in_dst) {
    current_settings.in_dst = new_dst;
    settings_need_commit = 1;
    save_settings();
  }
}


/* If A2F flag is set (daily at 02:00), check if we've crossed a DST transition. */
static void dst_transition_check(void)
{
  if (a2f) {
    if (current_settings.auto_dst) {
      uint8_t was_in_dst = current_settings.in_dst;
      update_in_dst_flag();
      if (was_in_dst && !current_settings.in_dst) {
        /* Fall transition: roll back one hour */
        date_subtract_hour();
      } else if (!was_in_dst && current_settings.in_dst) {
        /* Spring transition: jump ahead one hour */
        date_add_hour();
      }
    }
    /* clear A2F flag */
    rtc_set_register(0x0F, 0);
    a2f = 0;
  }
}


static void rtc_set_time(bcd hour, bcd minutes, bcd seconds)
{
  while (pin_is_low(nDISP_SS)) {}
  pin_low(nRTC_SS);
  spi_out(0x80); /* seconds register */
  spi_out(seconds);
  spi_out(minutes);
  spi_out(hour);
  pin_high(nRTC_SS);
  rtc_get_time();
  update_in_dst_flag();
}


static void rtc_set_date(bcd month, bcd date, bcd year)
{
  /* Handle pathological case of trying to set February 29th on a non-leap year. */
  if (month == 0x02 && date == 0x29 && !is_leap_year(year)) {
    /* advance to March 1st */
    month = 0x03;
    date = 0x01;
  }
  uint8_t weekday = weekday_for_date(month, date, year);
  while (pin_is_low(nDISP_SS)) {}
  pin_low(nRTC_SS);
  spi_out(0x83); /* weekday register */
  spi_out(weekday);
  spi_out(date);
  spi_out(month);
  spi_out(year);
  pin_high(nRTC_SS);
  rtc_get_time();
  update_in_dst_flag();
}


static void rtc_poll(void)
{
  /* Read the new time on the falling edge of the square wave input. */
  /* The SQW pin doesn't support interrupt-on-change, so we look for edges manually. */
  if (pin_is_low(SQW)) {
    if (need_time_update) {
      rtc_get_time();
      dst_transition_check();
    }
  } else {
    need_time_update = 1;
  }
}


static void rtc_init(void)
{
  /* TODO: also check OSF flag and clear time if set? */
  a2f = (rtc_get_register(0x0F) & 0x2) != 0;

  while (pin_is_low(nDISP_SS)) {}
  pin_low(nRTC_SS);
  spi_out(0x8B); /* Address of Alarm 2 Minutes register */
  spi_out(0x00); /* 0x0B: A2M2=0, alarm 2 minutes=00 */
  spi_out(0x02); /* 0x0C: A2M3=0, alarm 2 hours=02 */
  spi_out(0x80); /* 0x0D: A2M4=1, alarm 2 day=n/a */
  spi_out(0x00); /* 0x0E: Enable 1Hz square wave output */
  spi_out(0x00); /* 0x0F: Disable 32kHz output, clear oscillator stop flag */
  pin_high(nRTC_SS);
  /* Read settings and schedule from SRAM */
  read_settings();
  read_schedule();
}



static uint16_t read_touch_sensor(uint8_t sensor_num)
{
  if (!sensor_num) {
    pin_high(SENSE2);
    pin_output(SENSE2);
    ADCON0 = (SENSE2_ADC_CH<<2)|1;
    pin_low(SENSE1);
    pin_output(SENSE1);
    pin_input(SENSE1);
    ADCON0 = (SENSE1_ADC_CH<<2)|1;
  } else {
    pin_high(SENSE1);
    pin_output(SENSE1);
    ADCON0 = (SENSE1_ADC_CH<<2)|1;
    pin_low(SENSE2);
    pin_output(SENSE2);
    pin_input(SENSE2);
    ADCON0 = (SENSE2_ADC_CH<<2)|1;    
  }
  ADCON0bits.GO = 1;
//  while (GO) {}
  return (ADRESH<<8) | ADRESL;
}


static inline void slew_rate_limited_update(uint16_t *curr_value, uint16_t reading)
{
  uint16_t curr = *curr_value;
  if (reading < curr) {
    curr--;
  } else if (reading > curr) {
    curr++;   
  }
  *curr_value = curr;
}


struct touch_sensor {
  uint16_t baseline;  /* calibrated baseline (no touch) value */
  uint16_t current;   /* current level (slew limited) */
  uint8_t sensor_num; /* which touch sensor (0=left, 1=right) */
  uint8_t counts;     /* number of "pressed" samples in the current sampling period */
  uint8_t last_count; /* "counts" value from previous sampling period */
  uint8_t state;
};


static void touch_sensor_calibrate(struct touch_sensor *ts)
{
  ts->baseline = 0;
  for (uint16_t i = 32768; i != 0; i++) {
    slew_rate_limited_update(&(ts->baseline), read_touch_sensor(ts->sensor_num));
  }
  ts->current = ts->baseline;
}


static void touch_sensor_read(struct touch_sensor *ts)
{
  slew_rate_limited_update(&(ts->current), read_touch_sensor(ts->sensor_num));
  if (ts->current < ts->baseline) {
    /* In practice, a touch shouldn't cause the delta to be more than 255 */
    uint8_t diff = ts->baseline - ts->current;
    uint8_t n = ts->counts;
    if (n < 255 && diff >= 20) {
      n++;
    }
    ts->counts = n;
  }
}


/* Defined as a macro because pointers don't optimize well */
#define touch_sensor_start_reading(ts) do { ts.last_count = ts.counts; ts.counts = 0; } while(0)


enum touch_event {
  EVENT_PRESS   = (1 << 0), /* sent on initial touch down */
  EVENT_RELEASE = (1 << 1), /* sent on touch up */
  EVENT_TAP     = (1 << 2), /* sent after a short touch down/up */
  EVENT_HOLD    = (1 << 3), /* sent after touch has persisted past a duration threshold */
  EVENT_REPEAT  = (1 << 4)  /* sent repeatedly after touch has persisted past a duration threshold */
};

/* Hold durations */
#define TOUCH_HOLD_POINT          16
#define TOUCH_REPEAT_POINT        16
#define TOUCH_FAST_REPEAT_POINT   48
/* Repeat speeds. (must be 1 less than a power of 2) */
#define TOUCH_REPEAT_PERIOD       1
#define TOUCH_FAST_REPEAT_PERIOD  0

static uint8_t touch_sensor_update_and_get_events(struct touch_sensor *ts)
{
  uint8_t events = 0;
  uint8_t state = ts->state;

  /* "Down" events */
  if (ts->last_count == 255) {
    /* Sensor is now pressed; increment press count and handle wraparound. */
    if (++state == 0) { state = TOUCH_FAST_REPEAT_POINT; }

    /* first touch down */
    if (state == 1) {
      events |= EVENT_PRESS;
    }
    /* start of hold */
    else if (state == TOUCH_HOLD_POINT) {
      events |= EVENT_HOLD;
    }
    /* emit fast repeat events */
    if ((state >= TOUCH_FAST_REPEAT_POINT && !(state & TOUCH_FAST_REPEAT_PERIOD)) ||
             (state >= TOUCH_REPEAT_POINT && !(state & TOUCH_REPEAT_PERIOD))) {
      events |= EVENT_REPEAT;
    }
  }
 
  /* "Up" events */
  else {
    /* If the sensor was pressed but now isn't, emit a release */
    if (state) {
      events |= EVENT_RELEASE;
      /* If the sensor was pressed for a short time only, emit a tap */
      if (state < TOUCH_HOLD_POINT) {
        events |= EVENT_TAP;
      }
    }
    state = 0; /* reset to un-pressed state */
  }
  ts->state = state;
  return events;
}


static struct touch_sensor ts1 = { 0, 0, 0, 0, 0, 0 };
static struct touch_sensor ts2 = { 0, 0, 1, 0, 0, 0 };
static uint8_t touch1_events = 0;
static uint8_t touch2_events = 0;

#define TOUCH1_PRESS    (touch1_events & EVENT_PRESS)
#define TOUCH1_RELEASE  (touch1_events & EVENT_RELEASE)
#define TOUCH1_TAP      (touch1_events & EVENT_TAP)
#define TOUCH1_HOLD     (touch1_events & EVENT_HOLD)
#define TOUCH1_REPEAT   (touch1_events & EVENT_REPEAT)
#define TOUCH2_PRESS    (touch2_events & EVENT_PRESS)
#define TOUCH2_RELEASE  (touch2_events & EVENT_RELEASE)
#define TOUCH2_TAP      (touch2_events & EVENT_TAP)
#define TOUCH2_HOLD     (touch2_events & EVENT_HOLD)
#define TOUCH2_REPEAT   (touch2_events & EVENT_REPEAT)

static uint16_t tick = 0;
static volatile bit boop = 0;

/* Main loop; called once every 64 microseconds. */
/* On every iteration, one touch sensor is read. */
/* The selected touch sensor alternates every 512 iterations. */
static void timer0_interrupt()
{
  if (!TMR0IE || !TMR0IF) { return; }
  TMR0IF = 0;

  if (tick == 0) {
    /* Begin a new reading of sensor 1 */
    touch_sensor_start_reading(ts1);
  } else if (tick == 512) {
    /* Begin a new reading of sensor 2 */
    touch_sensor_start_reading(ts2);
    boop = 1; /* update the UI */
  }
  
  struct touch_sensor *ts = ((tick & 512) == 0) ? &ts1 : &ts2;
  touch_sensor_read(ts);
  
  tick++;
  tick &= 1023;
}


static inline void touch_loop_start()
{
  TMR0CS = 0; /* Fosc/4 clock, no prescaler; one interrupt every 64us */
  TMR0IE = 1;
}


void interrupt isr(void)
{
  timer0_interrupt();
  timer2_interrupt();
}



struct mode {
  void (*init)(void);
  void (*update)(void);
};


#define FIRST_MENU_ITEM set_sleep_mode

static void time_display_init(void);
static void time_display_update(void);
static const struct mode time_display = {
  time_display_init,
  time_display_update
};

static void date_display_init(void);
static void date_display_update(void);
static const struct mode date_display = {
  date_display_init,
  date_display_update
};

static void weekday_display_update(void);
static const struct mode weekday_display = {
  date_display_init,
  weekday_display_update
};

static void animator_init(void);
static void animator_update(void);
static const struct mode animate = {
  animator_init,
  animator_update
};

static void off_update(void);
static const struct mode off = {
  hv_off,
  off_update
};

static void prompt_start(void);
static void prompt_start_with_timeout(void);

static void set_sleep_mode_update(void);
static const struct mode set_sleep_mode = {
  prompt_start_with_timeout,
  set_sleep_mode_update
};

static void set_time_prompt_update(void);
static const struct mode set_time_prompt = {
  prompt_start_with_timeout,
  set_time_prompt_update
};

static void set_date_prompt_update(void);
static const struct mode set_date_prompt = {
  prompt_start_with_timeout,
  set_date_prompt_update
};

static void exit_menu_prompt_update(void);
static const struct mode exit_menu_prompt = {
  prompt_start_with_timeout,
  exit_menu_prompt_update
};

static void set_hour_update(void);
static const struct mode set_hour = {
  prompt_start,
  set_hour_update
};

static void set_minute_update(void);
static const struct mode set_minute = {
  prompt_start,
  set_minute_update
};

static void set_month_update(void);
static const struct mode set_month = {
  prompt_start,
  set_month_update
};

static void set_date_update(void);
static const struct mode set_date = {
  prompt_start,
  set_date_update
};

static void set_year_init(void);
static void set_year_update(void);
static const struct mode set_year = {
  set_year_init,
  set_year_update
};

static void set_12hr_24hr_update(void);
static const struct mode set_12hr_24hr = {
  prompt_start_with_timeout,
  set_12hr_24hr_update
};

static void set_auto_dst_update(void);
static const struct mode set_auto_dst = {
  prompt_start_with_timeout,
  set_auto_dst_update
};

static void set_time_separator_mode_update(void);
static const struct mode set_time_separator_mode = {
  prompt_start_with_timeout,
  set_time_separator_mode_update
};

static void set_time_separator_style_update(void);
static const struct mode set_time_separator_style = {
  prompt_start_with_timeout,
  set_time_separator_style_update
};

static void set_date_separator_style_update(void);
static const struct mode set_date_separator_style = {
  prompt_start_with_timeout,
  set_date_separator_style_update
};

static void set_schedule_prompt_update(void);
static const struct mode set_schedule_prompt = {
  prompt_start_with_timeout,
  set_schedule_prompt_update
};

static void set_schedule_menu_update(void);
static const struct mode set_schedule_menu = {
  prompt_start_with_timeout,
  set_schedule_menu_update
};

static void schedule_edit_days_update(void);
static const struct mode schedule_edit_days = {
  prompt_start,
  schedule_edit_days_update
};

static void schedule_edit_range_init(void);
static void schedule_edit_range_on_time_update(void);
static const struct mode schedule_edit_range_on_time = {
  schedule_edit_range_init,
  schedule_edit_range_on_time_update
};

static void schedule_edit_range_off_time_update(void);
static const struct mode schedule_edit_range_off_time = {
  prompt_start,
  schedule_edit_range_off_time_update
};

static void none(void) {}
static void do_exit_menu(void);
static const struct mode exit_menu = {
  do_exit_menu,
  none
};

static const struct mode *current_mode = &time_display;
static const struct mode *next_mode = &time_display;
static const struct mode *animator_dest;
static void (*animator_draw_fn)(void);
static bit display_cycling = 0;
static uint8_t flash_count = 0;
static uint8_t timeout = 0;
static uint8_t sleep_timer = 0;

#define TIMEOUT (timeout == 1)

void main(void)
{
  io_init();
  osc_init();
  spi_init();
  rtc_init();
  adc_init();
  mux_timer_init();

  /* startup delay so we don't rapidly toggle HV_EN during programming */
  __delay_ms(500);
  hv_on();     /* danger, danger! high voltage! when we touch, when we kiss! */
  mux_start(); /* enables interrupts */

  /* calibrate touch sensors; do this after enabling the HV supply so */
  /* environmental noise is taken into account */
  touch_sensor_calibrate(&ts1);
  touch_sensor_calibrate(&ts2);
  touch_loop_start();

  /* force initial time update */
  rtc_get_time();
  /* if we start up in scheduled sleep, don't go to sleep immediately */
  sleep_now = 0;

  current_mode->init();

  while (1) {
    while (!boop) {}
    boop = 0;
    touch1_events = touch_sensor_update_and_get_events(&ts1);
    touch2_events = touch_sensor_update_and_get_events(&ts2);
    rtc_poll();
    
    /* The flash counter increments every frame. */
    flash_count++;
    /* The timeout counter decrements every frame, but is paused if a */
    /* touch sensor is pressed. */
    if (timeout > 0 && !ts1.state && !ts2.state) {
      timeout--;
    }

    /* Handle fades. */
    if (fade_level < fade_target) {
      fade_level++;
    } else if (fade_level > fade_target) {
      fade_level--;
    }
 
    current_mode->update();
    if (next_mode != current_mode) {
      current_mode = next_mode;
      current_mode->init();
      /* Redraw screen without processing any events */
      touch1_events = touch2_events = 0;
      current_mode->update();
    }

    if (ts1.state) { set_bits(display[0], sA); } else { clear_bits(display[0], sA); }
    if (ts2.state) { set_bits(display[0], sG); } else { clear_bits(display[0], sG); }
 }
}



/**** Main display drawing routines ****/

static const uint8_t time_separators[4] = {
  sG,    /* middle dash */
  sP,    /* dot */
  sD|sG, /* pseudo-colon */
  sA|sD, /* tall pseudo-colon */
};


static uint8_t weekday_abbrs[8][4] = {
  { 0, 0, 0, 0 },
  { 0,   c_S, c_u, c_n|sP }, /* "Sun." */
  { c_1, c_N, c_o, c_n|sP }, /* "Mon." */
  { 0,   c_7, c_u, c_e|sP }, /* "Tue." */
  { c_1, c_J, c_e, c_d|sP }, /* "Wed." */
  { 0,   c_7, c_h, c_u|sP }, /* "Thu." */
  { 0,   c_F, c_r, c_i|sP }, /* "Fri." */
  { 0,   c_S, c_a, c_t|sP }, /* "Sat." */
};


static void time_draw(void)
{
 if (need_time_redraw) {
    if (current_settings.display_12hr) {
      uint8_t hour = to_12_hour(current_time.hour);
      set_bcd_byte(0, leading_zero_suppressed(hour));
      set_bcd_byte(3, current_time.minutes);
      set_bcd_byte(6, current_time.seconds);
      display[8] = 0;
      display[9] = am_pm_indicator_for_hour(current_time.hour);
    } else {
      display[0] = 0;
      set_bcd_byte(1, current_time.hour);
      set_bcd_byte(4, current_time.minutes);
      set_bcd_byte(7, current_time.seconds);
      display[9] = 0;
    }
    need_time_redraw = 0;
  }

  if (current_settings.time_sep_mode & 1) {
    uint8_t sep = (!(current_settings.time_sep_mode & 2) || pin_is_low(SQW)) ? time_separators[current_settings.time_sep_style] : 0;
    if (current_settings.display_12hr) {
      display[2] = display[5] = sep;
    } else {
      display[3] = display[6] = sep;
    }
  }
}


static void weekday_draw(void)
{
  if (need_time_redraw) {
    uint8_t *wkd = weekday_abbrs[current_time.weekday];
    display[0] = wkd[0];
    display[1] = wkd[1];
    display[2] = wkd[2];
    display[3] = wkd[3];
    display[4] = 0;
    set_bcd_byte(5, leading_zero_suppressed(current_time.month));
    set_bcd_byte(8, current_time.date);
  }
  if (current_settings.date_sep_style == 0) {
    display[7] = sG;
  } else {
    display[7] = 0;
    display[6] |= sP;
  }
}


static void date_draw(void)
{
  if (need_time_redraw) {
    set_bcd_byte(0, leading_zero_suppressed(current_time.month));
    set_bcd_byte(3, current_time.date);
    set_bcd_byte(6, 0x20);
    set_bcd_byte(8, current_time.year);
  }
  if (current_settings.date_sep_style == 0) {
    display[2] = display[5] = sG;
  } else{
    display[1] |= sP;
    display[2] = 0;
    display[4] |= sP;
    display[5] = 0;
  }
}



/**** Main display input handling ****/

static display_update_common(const struct mode *tap_next_mode, void (*anim_draw_fn)(void))
{
  wake_now = 0;
  if (fade_level == 0) {
    display_cycling = 0;
    next_mode = &off;
    return;
  }

  if (TOUCH2_HOLD || sleep_timer >= 154 || sleep_now) {
    /* Begin fade out; power off when display is blank. */
    fade_target = 0;
  }

  if (TOUCH1_HOLD) {
    display_cycling = 0;
    next_mode = &FIRST_MENU_ITEM;
    return;
  } else if (TOUCH1_TAP || TOUCH2_TAP) {
    display_cycling = 0;
    next_mode = tap_next_mode;
    return;
  } else if (TIMEOUT) {
    if (display_cycling) {
      animator_dest = tap_next_mode;
      animator_draw_fn = anim_draw_fn;
      next_mode = &animate;
    } else {
      next_mode = tap_next_mode;
    }
    return;
  }

  /* Advance sleep timer if we're in sleep mode */
  /* and there is no user input. */
  if (sleep && ts1.state == 0 && ts2.state == 0) {
    sleep_timer++;
  }
}



/**** Main display init/update callbacks ****/

static void time_display_init(void)
{
  display_clear();
  need_time_redraw = 1;
  display_cycling = 0;
  timeout = 0;
  sleep_timer = 0;
}


static void time_display_update(void)
{
  display_update_common(&weekday_display, weekday_draw);

  /* At the 26-second mark of even minutes, do an animated transition and */
  /* show the date. */
  if (current_time.seconds == 0x26 && (current_time.minutes & 1) == 0) {
    display_cycling = 1;
    animator_dest = &weekday_display;
    animator_draw_fn = weekday_draw;
    next_mode = &animate;
    return;
  }

  time_draw();
}


static void date_display_init(void)
{
  display_clear();
  need_time_redraw = 1;
  timeout = 47; /* 3 seconds */
}


static void weekday_display_update(void)
{
  if (TOUCH1_PRESS) { timeout = 47; }
  display_update_common(&date_display, date_draw);
  weekday_draw();
}


static void date_display_update(void)
{
  if (TOUCH1_PRESS) { timeout = 47; }
  display_update_common(&time_display, time_draw);
  date_draw();
}


static void animator_init(void)
{
  timeout = 15;
}


static void animator_update(void)
{
  uint8_t t = timeout & 7;
  uint8_t mask = sD;
  if (t <= 6) { mask |= sE; }
  if (t <= 5) { mask |= sC; }
  if (t <= 4) { mask |= sG; }
  if (t <= 3) { mask |= sF; }
  if (t <= 2) { mask |= sB; }
  if (t <= 1) { mask |= sA; }
  if (timeout >= 8) {
    for (uint8_t i = 0; i < NUM_DIGITS; i++) {
      display[i] |= mask;
    }
  } else {
    need_time_redraw = 1;
    animator_draw_fn();
    for (uint8_t i = 0; i < NUM_DIGITS; i++) {
      display[i] |= ((~mask) & (~sP));
    }
  }

  if (TIMEOUT) {
    next_mode = animator_dest;
  }
}


static void off_update(void)
{
  sleep_now = 0;
  /* Tapping either sensor powers the display back on. */
  if (TOUCH1_TAP || TOUCH2_TAP || wake_now) {
    hv_on();
    fade_target = BRIGHTNESS_MAX;
    fade_level = 1;
    next_mode = &time_display;
  }
}


static void prompt_start(void)
{
  display_clear();
  flash_count = 0;
}


static void prompt_start_with_timeout(void)
{
  display_clear();
  flash_count = 0;
  timeout = 154; /* 10 seconds */
}


static void display_set_message(void)
{
  disp3(1, c_S, c_E, c_T);
}


static uint8_t temp1 = 0;
static uint8_t temp2 = 0;
static bit temp_is_leap_year = 0;

static void set_year_init(void)
{
  prompt_start_with_timeout();
  temp1 = current_time.year;
}


static void set_time_draw(uint8_t flash_field)
{
  if (FLASH || flash_field != 0) {
    display_set_message();
  } else {
    disp3(1, 0, 0, 0);
  }
  if (FLASH || flash_field != 1) {
    if (current_settings.display_12hr) {
      set_bcd_byte(5, leading_zero_suppressed(to_12_hour(temp1)));
      display[9] = am_pm_indicator_for_hour(temp1);
    } else {
      set_bcd_byte(5, temp1);
    }
  } else {
    display[5] = display[6] = display[9] = 0;
  }
  if (FLASH || flash_field != 2) {
    set_bcd_byte(7, temp2);
  } else {
    display[7] = display[8] = 0;
  }
  if (pin_is_low(SQW) || flash_field > 0) { display[6] |= sP; }
}


static void set_time_prompt_update(void)
{
  if (TOUCH1_TAP) {
    next_mode = &set_date_prompt;
    return;
  } else if (TOUCH2_TAP) {
    next_mode = &set_hour;
  } else if (TOUCH1_HOLD || TIMEOUT) {
    next_mode = &exit_menu;
    return;
  }
  temp1 = current_time.hour;
  temp2 = current_time.minutes;
  temp_is_leap_year = is_leap_year(current_time.year);
  set_time_draw(0);
}


static void set_hour_update(void)
{
  if (TOUCH1_TAP) {
    next_mode = &set_minute;
    return;
  } else if (TOUCH1_HOLD) {
    next_mode = &exit_menu;
    return;
  } else if (TOUCH2_PRESS|TOUCH2_REPEAT) {
    flash_count = 0;
    temp1 = increment_hour(temp1);
  }
  set_time_draw(1);
}


static void set_minute_update(void)
{
  if (TOUCH1_TAP) {
    /* commit changes */
    rtc_set_time(temp1, temp2, 0);
    next_mode = &set_time_prompt;
    return;
  } else if (TOUCH1_HOLD) {
    next_mode = &exit_menu;
    return;
  } else if (TOUCH2_PRESS|TOUCH2_REPEAT) {
    flash_count = 0;
    temp2 = increment_minute(temp2);
  }
  set_time_draw(2);
}


static void set_date_draw(uint8_t flash_field)
{
  if (FLASH || flash_field != 0) {
    display_set_message();
  } else {
    disp3(1, 0, 0, 0);
  }
  if (FLASH || flash_field != 1) {
    set_bcd_byte(5, temp1);
  } else {
    disp2(5, 0, 0);
  }
  if (FLASH || flash_field != 2) {
    set_bcd_byte(8, temp2);
  } else {
    disp2(8, 0, 0);
  }
  display[7] = sG;
}


static void set_date_prompt_update(void)
{
  if (TOUCH1_TAP) {
    next_mode = &set_year;
    return;
  } else if (TOUCH2_TAP) {
    next_mode = &set_month;
    return;
  } else if (TOUCH1_HOLD || TIMEOUT) {
    next_mode = &exit_menu;
    return;
  }
  temp1 = current_time.month;
  temp2 = current_time.date;
  set_date_draw(0);
}


static void set_month_update(void)
{
  if (TOUCH1_TAP) {
    next_mode = &set_date;
    return;
  } else if (TOUCH1_HOLD) {
    next_mode = &exit_menu;
    return;
  } else if (TOUCH2_PRESS|TOUCH2_REPEAT) {
    flash_count = 0;
    temp1 = increment_month(temp1);
    /* prevent invalid dates */
    uint8_t max_date = days_in_month(temp1, temp_is_leap_year);
    if (temp2 > max_date) {
      temp2 = max_date;
    }
  }
  set_date_draw(1);
}


static void set_date_update(void)
{
  if (TOUCH1_TAP) {
    /* commit changes */
    rtc_set_date(temp1, temp2, current_time.year);
    next_mode = &set_year;
    return;
  } else if (TOUCH1_HOLD) {
    next_mode = &exit_menu;
    return;
  } else if (TOUCH2_PRESS|TOUCH2_REPEAT) {
    flash_count = 0;
    uint8_t max_date = days_in_month(temp1, temp_is_leap_year);
    temp2 = increment_date(temp2, max_date);
  }
  set_date_draw(2);
}


static void set_year_update(void)
{
  if (TOUCH1_TAP) {
    /* commit changes */
    next_mode = &set_12hr_24hr;
    rtc_set_date(current_time.month, current_time.date, temp1);
    return;
  } else if (TOUCH2_PRESS|TOUCH2_REPEAT) {
    timeout = 154;
    flash_count = 0;
    temp1 = increment_year(temp1);
  } else if (TOUCH1_HOLD || TIMEOUT) {
    next_mode = &exit_menu;
    rtc_set_date(current_time.month, current_time.date, temp1);
    return;
  }
  
  disp4(1, c_Y, c_E, c_A, c_R); /* "YEAR" */
  if (FLASH) {
    set_bcd_byte(6, 0x20);
    set_bcd_byte(8, temp1);
  } else {
    display[6] = display[7] = display[8] = display[9] = 0;
  }
}


static void settings_update_common(void (*advance_fn)(void), const struct mode *tap_next_mode)
{
  if (TOUCH1_TAP) {
    next_mode = tap_next_mode;
    save_settings();
    save_schedule();
  } else if (TOUCH2_TAP) {
    timeout = 154;
    flash_count = 0;
    advance_fn();
  } else if (TOUCH1_HOLD || TIMEOUT) {
    next_mode = &exit_menu;
    save_settings();
    save_schedule();
  }
}


static void set_12hr_24hr_advance(void)
{
  current_settings.display_12hr++;
  settings_need_commit = 1;
}


static void set_12hr_24hr_update(void)
{
  settings_update_common(set_12hr_24hr_advance, &set_auto_dst);

  /* "DISP." */
  disp4(1, c_D, c_I, c_S, c_P|sP);
  if (FLASH) {
    if (current_settings.display_12hr) {
      disp2(6, c_1, c_2); /* "12" */
    } else {
      disp2(6, c_2, c_4); /* "24" */
    }
    disp2(8, c_h, c_r); /* "hr" */
  } else {
    disp4(6, 0,0,0,0);
  }
}


static void set_auto_dst_advance(void)
{
  current_settings.auto_dst++;
  settings_need_commit = 1;
  update_in_dst_flag();
}


static void set_auto_dst_update(void)
{
  settings_update_common(set_auto_dst_advance, &set_time_separator_mode);

  /* "D.S.T." */
  disp3(1, c_D|sP, c_S|sP, c_T|sP);

  if (FLASH) {
    if (current_settings.auto_dst) {
      disp4(6, c_A, c_U, c_T, c_O); /* "AUTO" */
    } else {
      disp4(6, 0, c_O, c_F, c_F); /* "OFF" */
    }
  } else {
    disp4(6, 0,0,0,0);
  }
  /* + sign illuminated if currently in DST */
  if (current_settings.in_dst) {
    display[0] = sF;
  }
}


static void set_time_separator_mode_advance(void)
{
  current_settings.time_sep_mode--;
  /* off with flash enabled doesn't make sense, skip it */
  if (current_settings.time_sep_mode == 2) {
    current_settings.time_sep_mode = 1;
  }
  settings_need_commit = 1;
}


static void set_time_separator_mode_update(void)
{
  settings_update_common(set_time_separator_mode_advance, &set_time_separator_style);
  
  /* "SEP." */
  disp3(1, c_S, c_E, c_P|sP);
  
  if (FLASH) {
    switch (current_settings.time_sep_mode) {
      case 0:
      case 2:
        disp5(5, 0, 0, c_O, c_F, c_F); /* "OFF" */
        break;
      case 3:
        disp5(5, c_F, c_L, c_A, c_S, c_H); /* "FLASH" */
        break;
      case 1:
        disp5(5, c_S, c_O, c_L, c_I, c_D); /* "SOLID" */
        break;
    }
  } else {
    display[5] = display[6] = display[7] = display[8] = display[9] = 0;
  }
}


static void set_time_separator_style_advance(void)
{
  current_settings.time_sep_style++;
  settings_need_commit = 1;
}


static void set_time_separator_style_update(void)
{
  settings_update_common(set_time_separator_style_advance, &set_date_separator_style);
  /* "SEP.STYLE" */
  disp8(1, c_S, c_E, c_P|sP, c_S, c_T, c_Y, c_L, c_E);
  flash1(9, time_separators[current_settings.time_sep_style]);
}


static void set_date_separator_style_advance(void)
{
  current_settings.date_sep_style++;
  settings_need_commit = 1;
}


static void set_date_separator_style_update(void)
{
  settings_update_common(set_date_separator_style_advance, &set_schedule_prompt);
  /* "DATESEP." */
  disp7(1, c_D, c_A, c_T, c_E, c_S, c_E, c_P|sP);
  flash1(9, (current_settings.date_sep_style) ? sP : sG);
}


static void set_sleep_mode_advance(void)
{
  uint8_t new_sleep_mode = current_schedule.sleep_mode+1;
  if (new_sleep_mode >= 3) {
    new_sleep_mode = 0;
  }
  current_schedule.sleep_mode = new_sleep_mode;
  schedule_needs_commit = 1;
  update_sleep_state();
}


static uint8_t sleep_state_indicator(void)
{
  return (sleep) ? sE : sF;
}


static void set_sleep_mode_update(void)
{
  settings_update_common(set_sleep_mode_advance, &set_time_prompt);
  /* "SLEEP." */
  disp5(1, c_S, c_L, c_E, c_E, c_P);

  if (FLASH) {
    switch (current_schedule.sleep_mode) {
      case 0:
        disp3(7, c_O, c_F, c_F); /* "OFF" */
        break;
      case 1:
        disp3(7, 0, c_O, c_N); /* "ON" */
        break;
      case 2:
      case 3:
        disp3(7, c_S, c_C, c_H|sP); /* "SCH." */
        break;
    }
  } else {
    display[7] = display[8] = display[9] = 0;
  }
  display[0] = sleep_state_indicator();
}



static uint8_t sched_menu_field = 0;
static struct schedule_range *editing_range;

static void set_schedule_enter(void)
{
  sched_menu_field = 0;
  next_mode = &set_schedule_menu;
}


static void set_schedule_prompt_update(void)
{
  settings_update_common(set_schedule_enter, &exit_menu_prompt);
  flash9(1, c_S, c_E, c_T, 0, c_S, c_C, c_H, c_E, c_D|sP);
  display[0] = sleep_state_indicator();
}


static void set_schedule_menu_update(void)
{
  if (TOUCH1_TAP) {
    timeout = 154;
    flash_count = 0;
    sched_menu_field++;
    if (sched_menu_field == 3) {
      sched_menu_field = 0;
    }
  } else if (TOUCH2_TAP) {
    if (sched_menu_field == 0) {
      next_mode = &schedule_edit_days;
    } else if (sched_menu_field == 1) {
      editing_range = &current_schedule.ranges[0];
      next_mode = &schedule_edit_range_on_time;
    } else if (sched_menu_field == 2) {
      editing_range = &current_schedule.ranges[1];
      next_mode = &schedule_edit_range_on_time;
    }
  } else if (TOUCH1_HOLD || TIMEOUT) {
    next_mode = &exit_menu;
  }

  if (FLASH || sched_menu_field != 0) {
    disp4(1, c_D, c_A, c_Y, c_S);
  } else {
    display[1] = display[2] = display[3] = display[4] = 0;
  }
  display[6] = (FLASH || sched_menu_field != 1) ? c_1 : 0;
  display[8] = (FLASH || sched_menu_field != 2) ? c_2 : 0;
  display[0] = sleep_state_indicator();
}


static const uint8_t sched_day_indicators[4] = {
  sD,      /* 24h off */
  c_1,     /* Use range 1 */
  c_2,     /* Use range 2 */
  sA|sD|sG /* 24h on */
};

static void schedule_edit_days_update(void)
{
  if (TOUCH1_TAP) {
    flash_count = 0;
    sched_menu_field++;
    if (sched_menu_field == 7) {
      sched_menu_field = 0;
    }
  } else if (TOUCH2_TAP) {
    flash_count = 0;
    uint8_t day = (current_schedule.days[sched_menu_field] + 1) & 3;
    current_schedule.days[sched_menu_field] = day;
  } else if (TOUCH1_HOLD) {
    sched_menu_field = 0;
    next_mode = &set_schedule_menu;
    schedule_needs_commit = 1;
    update_sleep_state();
    save_schedule();
    return;
  }

  for (uint8_t i = 0; i < 7; i++) {
    uint8_t day = current_schedule.days[i] & 3;
    display[i+3] = (FLASH || sched_menu_field != i) ? sched_day_indicators[day] : 0;
  }

  uint8_t *wkd = weekday_abbrs[sched_menu_field+1];
  display[0] = wkd[0];
  display[1] = wkd[1];
  display[2] = wkd[2]|sP;
  display[0] |= sleep_state_indicator();
}


static void schedule_edit_range_init(void)
{
  prompt_start();
  temp1 = editing_range->on_hour;
  temp2 = editing_range->off_hour;
}


static void schedule_edit_range_draw(uint8_t flash_field)
{
  display[1] = sP | ((sched_menu_field == 1) ? c_1 : c_2);

  if (FLASH || flash_field != 0) {
    if (!current_settings.display_12hr) {
      set_bcd_byte(3, temp1);
    } else {
      set_bcd_byte(2, leading_zero_suppressed(to_12_hour(temp1)));
      display[4] = am_pm_indicator_for_hour(temp1);
    }
  } else {
    display[2] = display[3] = display[4] = 0;
  }
  
  if (FLASH || flash_field != 1) {
    if (!current_settings.display_12hr) {
      set_bcd_byte(7, temp2);
    } else {
      set_bcd_byte(6, leading_zero_suppressed(to_12_hour(temp2)));
      display[8] = am_pm_indicator_for_hour(temp2);
    }
    /* "prime" indicator if off time is <= on time */
    display[9] = (temp2 <= temp1) ? sF : 0;
  } else {
    display[6] = display[7] = display[8] = display[9] = 0;
  }
  
  if (!current_settings.display_12hr) {
    display[6] = sG;
  }
  display[5] = sG;
  display[0] = sleep_state_indicator();
}


static void edit_range_finish(void)
{
  editing_range->on_hour = temp1;
  editing_range->off_hour = temp2;
  schedule_needs_commit = 1;
  save_schedule();
  update_sleep_state();
  next_mode = &set_schedule_menu;
}


static void schedule_edit_range_on_time_update(void)
{
  if (TOUCH1_TAP) {
    next_mode = &schedule_edit_range_off_time;
    return;
  } else if (TOUCH1_HOLD) {
    edit_range_finish();
    return;
  } else if (TOUCH2_PRESS|TOUCH2_REPEAT) {
    flash_count = 0;
    temp1 = increment_hour(temp1);
  }
  schedule_edit_range_draw(0);
}


static void schedule_edit_range_off_time_update(void)
{
  if (TOUCH1_TAP || TOUCH1_HOLD) {
    /* commit changes */
    edit_range_finish();
    return;
  } else if (TOUCH2_PRESS|TOUCH2_REPEAT) {
    flash_count = 0;
    temp2 = increment_hour(temp2);
  }
  schedule_edit_range_draw(1);
}


static void do_exit_menu(void)
{
  /* Don't go to sleep immediately after exiting the menus. */
  sleep_now = 0;
  next_mode = &time_display;
}


static void exit_menu_prompt_update(void)
{
  settings_update_common(do_exit_menu, &FIRST_MENU_ITEM);
  flash4(1, c_D, c_O, c_N, c_E);
}
