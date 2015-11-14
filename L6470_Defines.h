#pragma once

typedef struct reg_def
{
	uint8_t addr;
	uint8_t bits;
} reg_def;

#define ABS_POS_REG    (reg_def) { 0x01, 22 }
#define EL_POS_REG     (reg_def) { 0x02,  9 }
#define MARK_REG       (reg_def) { 0x03, 22 }
#define SPEED_REG      (reg_def) { 0x04, 20 }
#define VAL_SPEED(stepspertick) (stepspertick * 0.014901161193847656)
#define SPEED_VAL(stepspersec) ((uint32_t)( stepspersec * 67.108864 + 0.5 ))
#define ACC_REG        (reg_def) { 0x05, 12 }
#define DEC_REG        (reg_def) { 0x06, 12 }
#define MAX_SPEED_REG  (reg_def) { 0x07, 10 }
#define MIN_SPEED_REG  (reg_def) { 0x08, 13 }
#define KVAL_HOLD_REG  (reg_def) { 0x09,  8 }
#define KVAL_RUN_REG   (reg_def) { 0x0a,  8 }
#define KVAL_ACC_REG   (reg_def) { 0x0b,  8 }
#define KVAL_DEC_REG   (reg_def) { 0x0c,  8 }
#define INT_SPD_REG    (reg_def) { 0x0d, 14 }
#define INT_SPD_VAL(steps) ((uint16_t)( steps * 4.194304 + 0.5 )) // step/s = INT_SPEED * 2**-24 / 250ns
#define ST_SLP_REG     (reg_def) { 0x0e,  8 }
#define FN_SLP_ACC_REG (reg_def) { 0x0f,  8 }
#define FN_SLP_DEC_REG (reg_def) { 0x10,  8 }
#define K_THERM_REG    (reg_def) { 0x11,  4 }
#define ADC_OUT_REG    (reg_def) { 0x12,  5 }

#define OCD_TH_REG     (reg_def) { 0x13,  4 }
#define OCD_TH_AMPS(amps) ((uint8_t)(amps / 6.0 * 16 - 0.5))

#define STALL_TH_REG   (reg_def) { 0x14,  7 }
#define FS_SPD_REG     (reg_def) { 0x15, 10 }
#define STEP_MODE_REG  (reg_def) { 0x16,  8 }
#define ALARM_EN_REG   (reg_def) { 0x17,  8 }
#define CONFIG_REG     (reg_def) { 0x18, 16 }
#define STATUS_REG     (reg_def) { 0x19, 16 }

typedef union step_mode_reg {
	uint8_t raw;
	struct {
		unsigned step_sel:3;
		unsigned reserved:1;
		unsigned sync_sel:3;
		unsigned sync_en:1;
	};
} step_mode_reg;

typedef union config_reg {
	uint16_t raw;
	struct {
		unsigned osc_sel:3; // 2..0
		unsigned ext_clk:1; // 3
		unsigned sw_mode:1; // 4
		unsigned en_vscomp:1; // 5
		unsigned reserved:1; // 6
		unsigned oc_sd:1; // 7
		unsigned pow_sr:2; // 9..8
		unsigned f_pwm_mul:3; // 12..10
		unsigned f_pwm_div:3; // 15..13
	};
} config_reg;

#define F_PWM_DIV(divisor) ((divisor >= 1 && divisor <= 7) ? divisor - 1 : 0)
#define F_PWM_MUL(factor) ((uint8_t)( factor <= 1 ? (factor * 8 - 4.5) : (factor * 4 - 0.5)))

typedef union status_reg {
	uint16_t raw;
	struct {
		unsigned hiz:1;         //  0
		unsigned busy:1;        //  1
		unsigned sw_f:1;        //  2
		unsigned sw_evn:1;      //  3
		unsigned dir:1;         //  4
		unsigned mot_status:2;  //  6..5
		unsigned notperf_cmd:1; //  7
		unsigned wrong_cmd:1;   //  8
		unsigned uvlo:1;        //  9
		unsigned th_wrn:1;      // 10
		unsigned th_sd:1;       // 11
		unsigned ocd:1;         // 12
		unsigned step_loss_a:1; // 13
		unsigned step_loss_b:1; // 14
		unsigned sck_mod:1;     // 15
	};
} status_reg;

