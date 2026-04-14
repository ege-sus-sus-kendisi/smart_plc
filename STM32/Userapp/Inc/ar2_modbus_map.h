#ifndef INCLUDE_AR2_MODBUS_MAP_H_
#define INCLUDE_AR2_MODBUS_MAP_H_

#include <stdint.h>

/*
 * AR2 Modbus map
 *
 * Notes:
 * - These are 0-based protocol addresses to place into the Modbus PDU.
 * - The "human" addresses in the document are like 00001, 10001, 30001, 40001.
 * - Example:
 *     NetIn1 coil       => human 00001 => protocol address 0x0000
 *     DIn1 discrete     => human 10001 => protocol address 0x0000
 * - AR2 supports:
 *     - max 16 consecutive coils/discrete inputs in one request
 *     - max 1 register per read/write for input/holding registers
 */

/* =========================
 * Common limits
 * ========================= */
#define AR2_MAX_COILS_PER_REQUEST              16U
#define AR2_MAX_DISCRETE_PER_REQUEST           16U
#define AR2_MAX_INPUT_REGS_PER_REQUEST         1U
#define AR2_MAX_HOLDING_REGS_PER_REQUEST       1U

/* =========================
 * Function codes
 * ========================= */
//#define MODBUS_FC_READ_COILS                  0x01U
//#define MODBUS_FC_READ_DISCRETE_INPUTS        0x02U
//#define MODBUS_FC_READ_HOLDING_REGISTERS      0x03U
//#define MODBUS_FC_READ_INPUT_REGISTERS        0x04U
//#define MODBUS_FC_WRITE_SINGLE_COIL           0x05U
//#define MODBUS_FC_WRITE_SINGLE_REGISTER       0x06U
//#define MODBUS_FC_WRITE_MULTIPLE_COILS        0x0FU
//#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS    0x10U

/* =========================
 * 0x COILS (read/write)
 * ========================= */
#define AR2_MB_COIL_NETIN_BASE                0x0000U
#define AR2_MB_COIL_NETIN_COUNT               48U
#define AR2_MB_COIL_NETIN(n)  ((uint16_t)(AR2_MB_COIL_NETIN_BASE + ((n) - 1U)))

/* =========================
 * 1x DISCRETE INPUTS (read-only)
 * ========================= */

/* DIn1..28: base 10001 -> 0x0000 */
#define AR2_MB_DI_DIN_BASE                    0x0000U
#define AR2_MB_DI_DIN_COUNT                   28U
#define AR2_MB_DI_DIN(n)     ((uint16_t)(AR2_MB_DI_DIN_BASE + ((n) - 1U)))

/* DInFlt1..28: base 10257 -> 0x0100 */
#define AR2_MB_DI_DINFLT_BASE                 0x0100U
#define AR2_MB_DI_DINFLT_COUNT                28U
#define AR2_MB_DI_DINFLT(n)  ((uint16_t)(AR2_MB_DI_DINFLT_BASE + ((n) - 1U)))

/* DQ1..20: base 10513 -> 0x0200 */
#define AR2_MB_DI_DQ_BASE                     0x0200U
#define AR2_MB_DI_DQ_COUNT                    20U
#define AR2_MB_DI_DQ(n)      ((uint16_t)(AR2_MB_DI_DQ_BASE + ((n) - 1U)))

/* Aux1..48: base 10769 -> 0x0300 */
#define AR2_MB_DI_AUX_BASE                    0x0300U
#define AR2_MB_DI_AUX_COUNT                   48U
#define AR2_MB_DI_AUX(n)     ((uint16_t)(AR2_MB_DI_AUX_BASE + ((n) - 1U)))

/* Tmr1..32: base 11025 -> 0x0400 */
#define AR2_MB_DI_TMR_BASE                    0x0400U
#define AR2_MB_DI_TMR_COUNT                   32U
#define AR2_MB_DI_TMR(n)     ((uint16_t)(AR2_MB_DI_TMR_BASE + ((n) - 1U)))

/* Cnt1..32: base 11281 -> 0x0500 */
#define AR2_MB_DI_CNT_BASE                    0x0500U
#define AR2_MB_DI_CNT_COUNT                   32U
#define AR2_MB_DI_CNT(n)     ((uint16_t)(AR2_MB_DI_CNT_BASE + ((n) - 1U)))

/* CntCmp1..32: base 11537 -> 0x0600 */
#define AR2_MB_DI_CNTCMP_BASE                 0x0600U
#define AR2_MB_DI_CNTCMP_COUNT                32U
#define AR2_MB_DI_CNTCMP(n)  ((uint16_t)(AR2_MB_DI_CNTCMP_BASE + ((n) - 1U)))

/* SmAQ1..16: base 11793 -> 0x0700 */
#define AR2_MB_DI_SMAQ_BASE                   0x0700U
#define AR2_MB_DI_SMAQ_COUNT                  16U
#define AR2_MB_DI_SMAQ(n)    ((uint16_t)(AR2_MB_DI_SMAQ_BASE + ((n) - 1U)))

/* SmBQ1..16: base 12049 -> 0x0800 */
#define AR2_MB_DI_SMBQ_BASE                   0x0800U
#define AR2_MB_DI_SMBQ_COUNT                  16U
#define AR2_MB_DI_SMBQ(n)    ((uint16_t)(AR2_MB_DI_SMBQ_BASE + ((n) - 1U)))

/* Key1..3: base 12305 -> 0x0900 */
#define AR2_MB_DI_KEY_BASE                    0x0900U
#define AR2_MB_DI_KEY_COUNT                   3U
#define AR2_MB_DI_KEY(n)     ((uint16_t)(AR2_MB_DI_KEY_BASE + ((n) - 1U)))

/* KeyFlt1..3: base 12561 -> 0x0A00 */
#define AR2_MB_DI_KEYFLT_BASE                 0x0A00U
#define AR2_MB_DI_KEYFLT_COUNT                3U
#define AR2_MB_DI_KEYFLT(n)  ((uint16_t)(AR2_MB_DI_KEYFLT_BASE + ((n) - 1U)))

/* AnCmp1..16: base 12817 -> 0x0B00 */
#define AR2_MB_DI_ANCMP_BASE                  0x0B00U
#define AR2_MB_DI_ANCMP_COUNT                 16U
#define AR2_MB_DI_ANCMP(n)   ((uint16_t)(AR2_MB_DI_ANCMP_BASE + ((n) - 1U)))

/* RTCWA1..8: base 13073 -> 0x0C00 */
#define AR2_MB_DI_RTCWA_BASE                  0x0C00U
#define AR2_MB_DI_RTCWA_COUNT                 8U
#define AR2_MB_DI_RTCWA(n)   ((uint16_t)(AR2_MB_DI_RTCWA_BASE + ((n) - 1U)))

/* RTCYA1..8: base 13329 -> 0x0D00 */
#define AR2_MB_DI_RTCYA_BASE                  0x0D00U
#define AR2_MB_DI_RTCYA_COUNT                 8U
#define AR2_MB_DI_RTCYA(n)   ((uint16_t)(AR2_MB_DI_RTCYA_BASE + ((n) - 1U)))

/* Scr1..32: base 13585 -> 0x0E00 */
#define AR2_MB_DI_SCR_BASE                    0x0E00U
#define AR2_MB_DI_SCR_COUNT                   32U
#define AR2_MB_DI_SCR(n)     ((uint16_t)(AR2_MB_DI_SCR_BASE + ((n) - 1U)))

/* System flags: base 13841 -> 0x0F00 */
#define AR2_MB_DI_SYS_POWERONRST              0x0F00U
#define AR2_MB_DI_SYS_ERRRETENTION            0x0F01U
#define AR2_MB_DI_SYS_ERRPTC1                 0x0F02U
#define AR2_MB_DI_SYS_ERRPTC2                 0x0F03U
#define AR2_MB_DI_SYS_ERRCOMMANY              0x0F04U
#define AR2_MB_DI_SYS_ERRCOMMG1               0x0F05U
#define AR2_MB_DI_SYS_ERRRTC                  0x0F06U
#define AR2_MB_DI_SYS_ERRCOMMMODBUS           0x0F07U

/* CfgFlg1..4: base 14097 -> 0x1000 */
#define AR2_MB_DI_CFGFLG_BASE                 0x1000U
#define AR2_MB_DI_CFGFLG_COUNT                4U
#define AR2_MB_DI_CFGFLG(n)  ((uint16_t)(AR2_MB_DI_CFGFLG_BASE + ((n) - 1U)))

/* NetIn mirror as read-only discrete inputs: base 14353 -> 0x1100 */
#define AR2_MB_DI_NETIN_BASE                  0x1100U
#define AR2_MB_DI_NETIN_COUNT                 48U
#define AR2_MB_DI_NETIN(n)   ((uint16_t)(AR2_MB_DI_NETIN_BASE + ((n) - 1U)))

/* =========================
 * 3x INPUT REGISTERS (read-only, 16-bit)
 * ========================= */

/* Tmr current values: starts at 0x0000 in 3x map section */
#define AR2_MB_IR_TMR_BASE                    0x0000U
#define AR2_MB_IR_TMR_COUNT                   32U
#define AR2_MB_IR_TMR(n)      				  ((uint16_t)(AR2_MB_IR_TMR_BASE + ((n) - 1U)))

/* Cnt current values: starts at 0x0100 */
#define AR2_MB_IR_CNT_BASE                    0x0100U
#define AR2_MB_IR_CNT_COUNT                   32U
#define AR2_MB_IR_CNT(n)      				  ((uint16_t)(AR2_MB_IR_CNT_BASE + ((n) - 1U)))

/* State machine timer/current state area: starts at 0x0200 */
#define AR2_MB_IR_SMA_STATE_NO                0x0200U
#define AR2_MB_IR_SMA_TIMER                   0x0201U
#define AR2_MB_IR_SMB_STATE_NO                0x0202U
#define AR2_MB_IR_SMB_TIMER                   0x0203U

/* PTC area: starts at 0x0300 */
#define AR2_MB_IR_PTC_BASE                    0x0300U
#define AR2_MB_IR_PTC_COUNT                   2U
#define AR2_MB_IR_PTC(n) 					  ((uint16_t)(AR2_MB_IR_PTC_BASE + ((n) - 1U)))

/* Analog input area: starts at 0x0400 */
#define AR2_MB_IR_ANIN_BASE                   0x0400U
#define AR2_MB_IR_ANIN_COUNT                  4U
#define AR2_MB_IR_ANIN(n)     			  	  ((uint16_t)(AR2_MB_IR_ANIN_BASE + ((n) - 1U)))

/* MB1 device ID area: starts at 0x0500 */
#define AR2_MB_IR_MB1_ID                 	  0x0500U

/* =========================
 * 4x HOLDING REGISTERS (read/write, 16-bit)
 * ========================= */

/* Timer tA presets: starts at 0x0000 */
#define AR2_MB_HR_TMR_A_BASE                  0x0000U
#define AR2_MB_HR_TMR_A_COUNT                 32U
#define AR2_MB_HR_TMR_A(n)   				  ((uint16_t)(AR2_MB_HR_TMR_A_BASE + ((n) - 1U)))

/* Timer tB presets: starts at 0x0100 */
#define AR2_MB_HR_TMR_B_BASE                  0x0100U
#define AR2_MB_HR_TMR_B_COUNT                 32U
#define AR2_MB_HR_TMR_B(n)    				  ((uint16_t)(AR2_MB_HR_TMR_B_BASE + ((n) - 1U)))

/* Counter presets: starts at 0x0200 */
#define AR2_MB_HR_CNT_PRESET_BASE             0x0200U
#define AR2_MB_HR_CNT_PRESET_COUNT            32U
#define AR2_MB_HR_CNT_PRESET(n) 			  ((uint16_t)(AR2_MB_HR_CNT_PRESET_BASE + ((n) - 1U)))

/* Counter comparator presets: starts at 0x0300 */
#define AR2_MB_HR_CNTCMP_PRESET_BASE          0x0300U
#define AR2_MB_HR_CNTCMP_PRESET_COUNT         32U
#define AR2_MB_HR_CNTCMP_PRESET(n) 			  ((uint16_t)(AR2_MB_HR_CNTCMP_PRESET_BASE + ((n) - 1U)))

/* State machine A timer preset: starts at 0x0400 */
#define AR2_MB_HR_SMA_TIMER_PRESET_BASE       0x0400U
#define AR2_MB_HR_SMA_TIMER_PRESET_COUNT	  48U
#define AR2_MB_HR_SMA_TIMER_PRESET(n) 		  ((uint16_t)(AR2_MB_HR_SMA_TIMER_PRESET_BASE + ((n) - 1U)))

/* State machine B timer preset: starts at 0x0500 */
#define AR2_MB_HR_SMB_TIMER_PRESET_BASE       0x0500U
#define AR2_MB_HR_SMB_TIMER_PRESET_COUNT	  48U
#define AR2_MB_HR_SMB_TIMER_PRESET(n) 		  ((uint16_t)(AR2_MB_HR_SMB_TIMER_PRESET_BASE + ((n) - 1U)))

/* Analog comparator preset: starts at 0x0600 */
#define AR2_MB_HR_ANCMP_PRESET_BASE           0x0600U
#define AR2_MB_HR_ANCMP_PRESET_COUNT          16U
#define AR2_MB_HR_ANCMP_PRESET(n) 			  ((uint16_t)(AR2_MB_HR_ANCMP_PRESET_BASE + ((n) - 1U)))

/* Analog comparator hysteresis: starts at 0x0700 */
#define AR2_MB_HR_ANCMP_HYS_BASE              0x0700U
#define AR2_MB_HR_ANCMP_HYS_COUNT             16U
#define AR2_MB_HR_ANCMP_HYS(n)  			  ((uint16_t)(AR2_MB_HR_ANCMP_HYS_BASE + ((n) - 1U)))


#endif /* INCLUDE_AR2_MODBUS_MAP_H_ */
