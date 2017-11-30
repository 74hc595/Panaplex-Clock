#ifndef SRC_BIT_MACROS_H_
#define SRC_BIT_MACROS_H_

#define set_bits(x,bits)    (x) |= (bits)
#define clear_bits(x,bits)  (x) &= (~(bits))
#define set(x,bit)          set_bits(x,(1<<(bit)))
#define clear(x,bit)        clear_bits(x,(1<<(bit)))
#define bit_is_set(x,bit)   ((x) & (1<<(bit)))
#define bit_is_clear(x,bit) (!bit_is_set(x,bit))

/* for __bit lvalues */
#define set_bit(bit)        (bit) = 1
#define clear_bit(bit)      (bit) = 0

#endif
