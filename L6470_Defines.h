#pragma once

/// REGISTERS

#define ABS_POS_REG 0x01
struct abs_pos_reg {
	unsigned unused:2;
	unsigned abs_pos:22;
};

#define EL_POS_REG 0x02
struct el_pos_reg {
	unsigned unused:7;
	unsigned step:2;
	unsigned microstep:7;
};
#define MARK_REG 0x03
struct mark_reg {
	unsigned unused:2;
	unsigned mark:22;
};

#define SPEED_REG 0x04
struct speed_reg {
	unsigned unused:4;
	unsigned speed:20;
};

#define ACC_REG 0x05
struct acc_reg {
	unsigned unused:4;
	unsigned acc:12;
};

#define DEC_REG 0x06
struct dec_reg {
	unsigned unused:4;
	unsigned dec:12;
};

#define MAX_SPEED_REG 0x07
struct max_speed_reg {
	unsigned unused:6;
	unsigned max_speed:10;
};

#define MIN_SPEED_REG 0x08
struct min_speed_reg {
	unsigned unused:3;
	unsigned min_speed:13;
};

#define KVAL_HOLD_REG 0x09
struct kval_hold_reg {
	unsigned kval_hold:8;
};

#define KVAL_RUN_REG 0x0a
struct kval_run_reg {
	unsigned kval_run:8;
};
#define KVAL_ACC_REG 0x0b
struct kval_acc_reg {
	unsigned kval_acc:8;
};
#define KVAL_DEC_REG 0x0c
struct kval_dec_reg {
	unsigned kval_dec:8;
};

#define ST_SLP_REG 0x0e
#define FN_SLP_ACC_REG 0x0f
#define FN_SLP_DEC_REG 0x10
#define K_THERM_REG 0x11
#define ADC_OUT_REG 0x12
#define OCD_TH_REG 0x13
#define STALL_TH 0x14
#define FS_SPD_REG 0x15
struct fs_spd_reg {
	unsigned unused:6;
	unsigned fs_spd:10;
};

#define STEP_MODE_REG 0x16
#define ALARM_EN_REG 0x17
#define CONFIG_REG 0x18
struct config_reg {
	unsigned f_pwm_int:3; // 15..13
	unsigned f_pwm_dec:3; // 12..10
	unsigned pow_sr:2; // 9..8
	unsigned oc_sd:1; // 7
	unsigned reserved:1; // 6
	unsigned en_vscomp:1; // 5
	unsigned sw_mode:1; // 4
	unsigned ext_clk:1; // 3
	unsigned osc_sel:3; // 2..0
};

#define STATUS_REG 0x19
struct status_reg {
	unsigned sck_mod:1;     // 15
	unsigned step_loss_b:1; // 14
	unsigned step_loss_a:1; // 13
	unsigned ocd:1;         // 12
	unsigned th_sd:1;       // 11
	unsigned th_wrn:1;      // 10
	unsigned uvlo:1;        //  9
	unsigned wrong_cmd:1;   //  8
	unsigned notperf_cmd:1; //  7
	unsigned mot_status:2;  //  6..5
	unsigned dir:1;         //  4
	unsigned sw_evn:1;      //  3
	unsigned sw_f:1;        //  2
	unsigned busy:1;        //  1
	unsigned hiz:1;         //  0
};
