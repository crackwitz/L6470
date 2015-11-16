#include <SPI.h>
#include <L6470.h>

L6470 driver(10, 7, 6);

void get_status()
{
  status_reg status = { .raw = driver.getParam(STATUS_REG) };

  Serial.print("STATUS: "); Serial.println(status.raw, 16);
  Serial.print("SCK_MOD:      "); Serial.println(status.sck_mod);
  Serial.print("STEP_LOSS_B/: "); Serial.println(status.step_loss_b);
  Serial.print("STEP_LOSS_A/: "); Serial.println(status.step_loss_a);
  Serial.print("OCD/:         "); Serial.println(status.ocd);
  Serial.print("TH_SD/:       "); Serial.println(status.th_sd);
  Serial.print("TH_WRN/:      "); Serial.println(status.th_wrn);
  Serial.print("UVLO/:        "); Serial.println(status.uvlo);
  Serial.print("WRONG_CMD:    "); Serial.println(status.wrong_cmd);
  Serial.print("NOTPERF_CMD:  "); Serial.println(status.notperf_cmd);
  Serial.print("MOT_STATUS:   "); Serial.print(status.mot_status); Serial.println("(0:stopped, 1:acc, 2:dec, 3:const)");
  Serial.print("DIR:          "); Serial.println(status.dir ? "fwd" : "rev");
  Serial.print("SW_EVN:       "); Serial.println(status.sw_evn ? "yes" : "no");
  Serial.print("SW_F:         "); Serial.println(status.sw_f ? "closed" : "open");
  Serial.print("BUSY:         "); Serial.println(status.busy ? "no" : "yes");
  Serial.print("HIZ:          "); Serial.println(status.hiz ? "yes" : "no");
}

void get_config()
{
  config_reg config = { .raw = driver.getParam(CONFIG_REG) };
  
  Serial.print("CONFIG: "); Serial.println(config.raw, 16);
  Serial.print("OSC_SEL:      "); Serial.println(config.osc_sel, 2);
  Serial.print("EXT_CLK:      "); Serial.println(config.ext_clk);
  Serial.print("SW_MODE:      "); Serial.println(config.sw_mode ? "user" : "hardstop interrupt");
  Serial.print("EN_VSCOMP:    "); Serial.println(config.en_vscomp);
  Serial.print("reserved:     "); Serial.println(config.reserved);
  Serial.print("OC_SD:        "); Serial.println(config.oc_sd ? "disable bridges" : "don't disable bridges");
  Serial.print("POW_SR:       "); Serial.println(config.pow_sr, 2);
  Serial.print("F_PWM_MUL     "); Serial.println(config.f_pwm_mul, 3);
  Serial.print("F_PWM_DIV     "); Serial.println(config.f_pwm_div, 3);

}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hallo!");

  //Serial.println(driver.sendSPI(0x00), 16);

  driver.sendSPI(0b10101000); // hard hiz

  get_status();
  get_config();

  step_mode_reg tmp;
  tmp.step_sel = 7; // 2**k = 1..128
  driver.setParam(STEP_MODE_REG, tmp.raw );

  Serial.print("step mode ");
  Serial.println(driver.getParam(STEP_MODE_REG));

  get_status();

  config_reg config = { .raw = driver.getParam(CONFIG_REG) };
  //config.pow_sr = 0b11;
  config.oc_sd = 1;
  //config.f_pwm_div = F_PWM_DIV(7);
  //config.f_pwm_mul = F_PWM_MUL(0.625);
  //config.pow_sr = 0b00;
  config.en_vscomp = 1;
  driver.setParam(CONFIG_REG, config.raw);

 // driver.setParam(ACC_REG, 0x3ff);
  driver.setParam(ACC_REG, 0x007);
  driver.setParam(FS_SPD_REG, 0x3ff);
  driver.setParam(MAX_SPEED_REG, 0x3ff);
  driver.setParam(OCD_TH_REG, OCD_TH_AMPS(1.0));

//  driver.setParam(KVAL_HOLD_REG, 0x29);
  //driver.setParam(KVAL_RUN_REG, 0x09); // default 0x29
//  driver.setParam(KVAL_ACC_REG, 0x29);
//  driver.setParam(KVAL_DEC_REG, 0x29);
//  
//  driver.setParam(INT_SPD_REG, 0x0408);
//  driver.setParam(ST_SLP_REG, 0x19);
//  driver.setParam(FN_SLP_ACC_REG, 0x29);
//  driver.setParam(FN_SLP_DEC_REG, 0x29);

  //driver.sendSPI(0b10110000); // soft stop, enable bridges

  driver.sendSPI(0b01010001); // run forward
  driver.sendValue(20, SPEED_VAL(200 * 8 * 1));

}

uint32_t speed = 200 * 8 * 1;

void loop() {
  return;
  if (Serial.available())
  {
    int value = Serial.parseInt();
    if (value == 0) return;
    
    driver.sendSPI(0b10101000); // soft hiz
    //driver.setParam(KVAL_RUN_REG, value);
    //driver.setParam(ST_SLP_REG, value);
    //driver.setParam(FN_SLP_ACC_REG, value);
    //driver.setParam(FN_SLP_DEC_REG, value);
    Serial.print("st_slp: "); Serial.println(value);
  }
  //speed += 1;
  driver.sendSPI(0b01010001); // run forward
  driver.sendValue(20, SPEED_VAL(speed));
  //Serial.print("Speed: "); Serial.println(speed);
  delay(100);
}
/*
  return;
  // put your main code here, to run repeatedly:
 
//  driver.sendSPI(0b01000001); // move forward
//  driver.sendValue(22, 200 * 128);
//  delay(2000);
  
  driver.sendSPI(0b01010001); // run forward
  speed += 1000;
  driver.sendValue(20, speed);
  Serial.println(speed);
  delay(1000);


}

*/
