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
#define ACC_VAL(steps) ((uint32_t)((steps) * 0.068719476736 + 0.5))
#define VAL_ACC(val) ((val) * 14.551915228366852)
// since revision 6: 0xfff should never be used, is reserved

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
#define VAL_INT_SPD(stepspertick) (stepspertick * 0.059604644775390625) // 2**-26, see revision 6 of data sheet
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

typedef struct cmd_def
{
	uint8_t code;
	int8_t argbits; // -1 = variable
} cmd_def;


#define CMD_NOP                  (cmd_def) { 0x00,  0 }
#define CMD_SET_PARAM            (cmd_def) { 0x00, -1 }
#define CMD_GET_PARAM            (cmd_def) { 0x20, -1 }
#define CMD_RUN(fwd)             (cmd_def) { 0x50 | !!fwd, 20 }
#define CMD_STEP_CLOCK(fwd)      (cmd_def) { 0x58 | !!fwd,  0 }
#define CMD_MOVE(fwd)            (cmd_def) { 0x40 | !!fwd,  0 }
#define CMD_GO_TO                (cmd_def) { 0x60, 22 }
#define CMD_GO_TO_DIR(fwd)       (cmd_def) { 0x68 | !!fwd,  22 }
#define CMD_GO_UNTIL(fwd, act)   (cmd_def) { 0x82 | !!fwd | (!!act << 3),  20 }
#define CMD_RELEASE_SW(fwd, act) (cmd_def) { 0x92 | !!fwd | (!!act << 3),  0 }
#define CMD_GO_HOME              (cmd_def) { 0x70,  0 }
#define CMD_GO_MARK              (cmd_def) { 0x78,  0 }
#define CMD_RESET_POS            (cmd_def) { 0xD8,  0 }
#define CMD_RESET_DEVICE         (cmd_def) { 0xC0,  0 }
#define CMD_SOFT_STOP            (cmd_def) { 0xB0,  0 }
#define CMD_HARD_STOP            (cmd_def) { 0xB8,  0 }
#define CMD_SOFT_HIZ             (cmd_def) { 0xA0,  0 }
#define CMD_HARD_HIZ             (cmd_def) { 0xA8,  0 }
#define CMD_GET_STATUS           (cmd_def) { 0xD0, 16 }
//#define CMD_RESERVED_CMD1        (cmd_def) { 0xEB,  0 }
//#define CMD_RESERVED_CMD2        (cmd_def) { 0xF8,  0 }
