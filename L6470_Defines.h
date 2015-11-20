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
#define SPEED_VAL(steps) ((uint32_t)( (steps) * 67.108864 + 0.5 )) // full steps per sec
#define VAL_SPEED(val) ((val) * 0.014901161193847656)

#define ACC_REG        (reg_def) { 0x05, 12 }
#define ACC_VAL(steps) ((uint32_t)((steps) * 0.068719476736 + 0.5)
#define VAL_ACC(val) ((val) * 14.551915228366852)

#define DEC_REG        (reg_def) { 0x06, 12 }
#define DEC_VAL(steps) ACC_VAL(steps)
#define VAL_DEC(val) VAL_ACC(val)

#define MAX_SPEED_REG  (reg_def) { 0x07, 10 }
#define MAX_SPEED_VAL(steps) ((uint32_t)((steps) * 0.065536 + 0.5))
#define VAL_MAX_SPEED(val) ((val) * 15.2587890625)

#define MIN_SPEED_REG  (reg_def) { 0x08, 13 }
#define LSPD_OPT (1<<12) // bit
#define MIN_SPEED_VAL(steps) ((uint32_t)((steps) * 4.194304 + 0.5))
#define VAL_MIN_SPEED(val) ((val) * 0.2384185791015625)

#define KVAL_HOLD_REG  (reg_def) { 0x09,  8 }
#define KVAL_RUN_REG   (reg_def) { 0x0a,  8 }
#define KVAL_ACC_REG   (reg_def) { 0x0b,  8 }
#define KVAL_DEC_REG   (reg_def) { 0x0c,  8 }

#define INT_SPD_REG    (reg_def) { 0x0d, 14 }
#define VAL_INT_SPD(stepspertick) (stepspertick * 0.059604644775390625) // 2**-26 for some reason
#define INT_SPD_VAL(stepspersec) ((uint32_t)( stepspersec * 16.777216 + 0.5 )) // full steps

#define ST_SLP_REG     (reg_def) { 0x0e,  8 }
#define FN_SLP_ACC_REG (reg_def) { 0x0f,  8 }
#define FN_SLP_DEC_REG (reg_def) { 0x10,  8 }
#define K_THERM_REG    (reg_def) { 0x11,  4 }
#define K_THERM_VAL(coeff) ((uint32_t)(((coeff)-1) * 32 + 0.5))
#define ADC_OUT_REG    (reg_def) { 0x12,  5 }

#define OCD_TH_REG     (reg_def) { 0x13,  4 }
#define OCD_TH_AMPS(amps) ((uint8_t)(amps / 6.0 * 16 - 0.5))

#define STALL_TH_REG   (reg_def) { 0x14,  7 }

#define FS_SPD_REG     (reg_def) { 0x15, 10 }
#define FS_SPD_VAL(steps) ((uint32_t)((steps) * 0.065536))
#define VAL_FS_SPD(val) ((val + 0.5) * 15.2587890625)

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

enum mot_status {
	MOT_STOPPED = 0b00,
	MOT_ACC = 0b01,
	MOT_DEC = 0b10,
	MOT_CONST = 0b11,
};

enum command {
	CMD_NOP = 0x00,
	CMD_SET_PARAM = 0x00,
	CMD_GET_PARAM = 0x20,
	CMD_RUN = 0x50,
	CMD_STEP_CLOCK = 0x58,
	CMD_MOVE = 0x40,
	CMD_GO_TO = 0x60, // 22 bits
	CMD_GO_TO_DIR = 0x68,
	CMD_GO_UNTIL = 0x82,
	CMD_RELEASE_SW = 0x92,
	CMD_GO_HOME = 0x70,
	CMD_GO_MARK = 0x78,
	CMD_RESET_POS = 0xD8,
	CMD_RESET_DEVICE = 0xC0,
	CMD_SOFT_STOP = 0xB0,
	CMD_HARD_STOP = 0xB8,
	CMD_SOFT_HIZ = 0xA0,
	CMD_HARD_HIZ = 0xA8,
	CMD_GET_STATUS = 0xD0,
	CMD_RESERVED_CMD1 = 0xEB,
	CMD_RESERVED_CMD2 = 0xF8
};
