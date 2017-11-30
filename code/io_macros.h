#ifndef SRC_IO_MACROS_H_
#define	SRC_IO_MACROS_H_

#include "bit_macros.h"

#define _PASTE2(x,y)    x##y
#define _PASTE3(x,y,z)  x##y##z
#define _PASTE2X(x,y)   _PASTE2(x,y)
#define _PASTE3X(x,y,z) _PASTE3(x,y,z)
#define _PORT(p)        _PASTE2(p,_PORT)
#define _PIN(p)         _PASTE2(p,_PIN)
#define _PBIT(p)        (1<<_PIN(p))
#define ANSEL(p)        _PASTE2X(ANSEL,_PORT(p))
#define TRIS(p)         _PASTE2X(TRIS,_PORT(p))
#define PORT(p)         _PASTE2X(PORT,_PORT(p))
#define LAT(p)          _PASTE2X(LAT,_PORT(p))
#define WPU(p)          _PASTE2X(WPU,_PORT(p))

#define ANS_BIT(p)      _PASTE3X(ANS,_PORT(p),_PIN(p))
#define TRIS_BIT(p)     _PASTE3X(TRIS,_PORT(p),_PIN(p))
#define PORT_BIT(p)     _PASTE3X(R,_PORT(p),_PIN(p))
#define LAT_BIT(p)      _PASTE3X(LAT,_PORT(p),_PIN(p))
#define WPU_BIT(p)      _PASTE3X(WPU,_PORT(p),_PIN(p))

#define pins_digital(p,bits)   clear_bits(ANSEL(p),bits)
#define pins_analog(p,bits)    set_bits(ANSEL(p),bits)
#define pins_output(p,bits)    clear_bits(TRIS(p),bits)
#define pins_input(p,bits)     set_bits(TRIS(p),bits)
#define pins_low(p,bits)       clear_bits(LAT(p),bits)
#define pins_high(p,bits)      set_bits(LAT(p),bits)
#define pins_no_pullup(p,bits) clear_bits(WPU(p),bits)
#define pins_pullup(p,bits)    set_bits(WPU(p),bits)

#define pin_digital(p)         clear_bit(ANS_BIT(p))
#define pin_analog(p)          set_bit(ANS_BIT(p))
#define pin_output(p)          clear_bit(TRIS_BIT(p))
#define pin_input(p)           set_bit(TRIS_BIT(p))
#define pin_low(p)             clear_bit(LAT_BIT(p))
#define pin_high(p)            set_bit(LAT_BIT(p))
#define pin_no_pullup(p)       clear_bit(WPU_BIT(p))
#define pin_pullup(p)          set_bit(WPU_BIT(p))
#define pin_is_low(p)          (PORT_BIT(p) == 0)
#define pin_is_high(p)         (PORT_BIT(p) == 1)

#define A_PORT A
#define B_PORT B
#define C_PORT C

#define A0_PORT A
#define A1_PORT A
#define A2_PORT A
#define A3_PORT A
#define A4_PORT A
#define A5_PORT A
#define A6_PORT A
#define A7_PORT A
#define A0_PIN  0
#define A1_PIN  1
#define A2_PIN  2
#define A3_PIN  3
#define A4_PIN  4
#define A5_PIN  5
#define A6_PIN  6
#define A7_PIN  7
#define B0_PORT B
#define B1_PORT B
#define B2_PORT B
#define B3_PORT B
#define B4_PORT B
#define B5_PORT B
#define B6_PORT B
#define B7_PORT B
#define B0_PIN  0
#define B1_PIN  1
#define B2_PIN  2
#define B3_PIN  3
#define B4_PIN  4
#define B5_PIN  5
#define B6_PIN  6
#define B7_PIN  7
#define C0_PORT C
#define C1_PORT C
#define C2_PORT C
#define C3_PORT C
#define C4_PORT C
#define C5_PORT C
#define C6_PORT C
#define C7_PORT C
#define C0_PIN  0
#define C1_PIN  1
#define C2_PIN  2
#define C3_PIN  3
#define C4_PIN  4
#define C5_PIN  5
#define C6_PIN  6
#define C7_PIN  7

#define ALL_OUTPUTS LAT(A) = LAT(B) = LAT(C)

#endif