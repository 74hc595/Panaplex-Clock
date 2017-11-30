#ifndef SRC_DISPLAY_MACROS_H_
#define SRC_DISPLAY_MACROS_H_

/**
 * Segment mapping.
 * Digits 0-4:
 * bit 0 - a
 * bit 1 - b
 * bit 2 - f
 * bit 3 - g
 * bit 4 - dp
 * bit 5 - d
 * bit 6 - e
 * bit 7 - c
 *
 * digits 5-9:
 * bit 0 - a
 * bit 1 - f
 * bit 2 - b
 * bit 3 - g
 * bit 4 - dp
 * bit 5 - d
 * bit 6 - c
 * bit 7 - e
 */
enum segment_bits_for_left_half {
  sA = 1<<0,
  sB = 1<<1,
  sF = 1<<2,
  sG = 1<<3,
  sP = 1<<4, /* decimal point */
  sD = 1<<5,
  sE = 1<<6,
  sC = 1<<7
};

#define NUM_DIGITS 10
#define SEGMENT_SWAP(n) ((n) & 0b00111001) | (((n) & 0b10000100)>>1) | (((n) & 0b01000010)<<1)



/**
 * Unrolled macros to display fixed-length strings.
 * If all arguments are constants, they will compile to optimized MOVLW/MOVWF sequences.
 */
#define disp2(pos,a,b)                  do { display[(pos)] = (a); display[(pos)+1] = (b); } while (0)
#define disp3(pos,a,b,c)                do { display[(pos)] = (a); disp2((pos)+1,(b),(c)); } while (0)
#define disp4(pos,a,b,c,d)              do { display[(pos)] = (a); disp3((pos)+1,(b),(c),(d)); } while (0)
#define disp5(pos,a,b,c,d,e)            do { display[(pos)] = (a); disp4((pos)+1,(b),(c),(d),(e)); } while (0)
#define disp6(pos,a,b,c,d,e,f)          do { display[(pos)] = (a); disp5((pos)+1,(b),(c),(d),(e),(f)); } while (0)
#define disp7(pos,a,b,c,d,e,f,g)        do { display[(pos)] = (a); disp6((pos)+1,(b),(c),(d),(e),(f),(g)); } while (0)
#define disp8(pos,a,b,c,d,e,f,g,h)      do { display[(pos)] = (a); disp7((pos)+1,(b),(c),(d),(e),(f),(g),(h)); } while (0)
#define disp9(pos,a,b,c,d,e,f,g,h,i)    do { display[(pos)] = (a); disp8((pos)+1,(b),(c),(d),(e),(f),(g),(h),(i)); } while (0)
#define disp10(pos,a,b,c,d,e,f,g,h,i,j) do { display[(pos)] = (a); disp9((pos)+1,(b),(c),(d),(e),(f),(g),(h),(i),(j)); } while (0)

#define FLASH (!(flash_count & 4))
#define flash1(pos,a)                    do { if (FLASH) { display[(pos)] = (a); } else { display[(pos)] = 0; } } while(0)
#define flash2(pos,a,b)                  do { if (FLASH) { disp2((pos),(a)); } else { disp2((pos),0,0); } } while(0)
#define flash3(pos,a,b,c)                do { if (FLASH) { disp3((pos),(a),(b),(c)); } else { disp3((pos),0,0,0); } } while(0)
#define flash4(pos,a,b,c,d)              do { if (FLASH) { disp4((pos),(a),(b),(c),(d)); } else { disp4((pos),0,0,0,0); } } while(0)
#define flash5(pos,a,b,c,d,e)            do { if (FLASH) { disp5((pos),(a),(b),(c),(d),(e)); } else { disp5((pos),0,0,0,0,0); } } while(0)
#define flash6(pos,a,b,c,d,e,f)          do { if (FLASH) { disp6((pos),(a),(b),(c),(d),(e),(f)); } else { disp6((pos),0,0,0,0,0,0); } } while(0)
#define flash7(pos,a,b,c,d,e,f,g)        do { if (FLASH) { disp7((pos),(a),(b),(c),(d),(e),(f),(g)); } else { disp7((pos),0,0,0,0,0,0,0); } } while(0)
#define flash8(pos,a,b,c,d,e,f,g,h)      do { if (FLASH) { disp8((pos),(a),(b),(c),(d),(e),(f),(g),(h)); } else { disp8((pos),0,0,0,0,0,0,0,0); } } while(0)
#define flash9(pos,a,b,c,d,e,f,g,h,i)    do { if (FLASH) { disp9((pos),(a),(b),(c),(d),(e),(f),(g),(h),(i)); } else { disp9((pos),0,0,0,0,0,0,0,0,0); } } while(0)
#define flash10(pos,a,b,c,d,e,f,g,h,i,j) do { if (FLASH) { disp10((pos),(a),(b),(c),(d),(e),(f),(g),(h),(i),(j)); } else { disp10((pos),0,0,0,0,0,0,0,0,0,0); } } while(0)

/**
 * Segment patterns for characters.
 */
#define c_0 (sA|sB|sC|sD|sE|sF)
#define c_1 (sB|sC)
#define c_2 (sA|sB|sD|sE|sG)
#define c_3 (sA|sB|sC|sD|sG)
#define c_4 (sB|sC|sF|sG)
#define c_5 (sA|sC|sD|sF|sG)
#define c_6 (sA|sC|sD|sE|sF|sG)
#define c_7 (sA|sB|sC)
#define c_8 (sA|sB|sC|sD|sE|sF|sG)
#define c_9 (sA|sB|sC|sD|sF|sG)

#define c_A (sA|sB|sC|sE|sF|sG)
#define c_B (sC|sD|sE|sF|sG)
#define c_C (sA|sD|sE|sF)
#define c_D (sB|sC|sD|sE|sG)
#define c_E (sA|sD|sE|sF|sG)
#define c_F (sA|sE|sF|sG)
#define c_G (sA|sC|sD|sE|sF)
#define c_H (sB|sC|sE|sF|sG)
#define c_I (c_1)
#define c_J (sB|sC|sD|sE)
#define c_L (sD|sE|sF)
#define c_N (sA|sB|sC|sE|sF)
#define c_O (c_0)
#define c_P (sA|sB|sE|sF|sG)
#define c_R (sA|sE|sF)
#define c_S (c_5)
#define c_T (sD|sE|sF|sG)
#define c_U (sB|sC|sD|sE|sF)
#define c_Y (sB|sC|sD|sF|sG)
#define c_Z (c_2)

#define c_a (sA|sB|sC|sD|sE|sG)
#define c_b (c_B)
#define c_c (sD|sE|sG)
#define c_d (c_D)
#define c_e (sA|sB|sD|sE|sF|sG)
#define c_f (c_F)
#define c_g (c_9)
#define c_h (sC|sE|sF|sG)
#define c_i (sE)
#define c_j (sC|sD)
#define c_l (sE|sF)
#define c_n (sC|sE|sG)
#define c_o (sC|sD|sE|sG)
#define c_p (c_P)
#define c_q (sA|sB|sC|sF|sG)
#define c_r (sE|sG)
#define c_s (c_S)
#define c_t (c_T)
#define c_u (sC|sD|sE)
#define c_y (c_Y)
#define c_z (c_Z)

#endif
