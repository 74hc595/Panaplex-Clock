#ifndef SRC_PIN_ASSIGNMENTS_H_
#define SRC_PIN_ASSIGNMENTS_H_

#define HV_EN         C5 /* pin 5 */
#define MISO          B4 /* pin 13 */
#define MOSI          C7 /* pin 9 */
#define SCK           B6 /* pin 11 */
#define nRTC_SS       C2 /* pin 14 */
#define nDISP_SS      C4 /* pin 6 */
#define SQW           C6 /* pin 8 */
#define ANODE3        A0 /* pin 19 */
#define ANODE1        A1 /* pin 18 */
#define ANODE0        A2 /* pin 17 */
#define ANODE2        C0 /* pin 16 */
#define ANODE4        C1 /* pin 15 */
#define ANODE012_PORT A
#define ANODE012_PINS (_PBIT(ANODE0)|_PBIT(ANODE1)|_PBIT(ANODE2))
#define ANODE34_PORT  C
#define ANODE34_PINS  (_PBIT(ANODE3)|_PBIT(ANODE4))

#define SENSE1        C3
#define SENSE1_ADC_CH 7
#define SENSE2        A4
#define SENSE2_ADC_CH 3

#endif
