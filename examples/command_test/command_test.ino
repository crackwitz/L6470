#include <SPI.h>
#include <L6470.h>

L6470 driver1(6, 2, 8);
L6470 driver2(7, 3, 9);
// avoid 10 (nCC) which is the chip select that SPI.begin() uses
// flags: 5, 6
// step clocks: unused

L6470 &driver = driver2;

bool is_busy()
{
  status_reg status = { .raw = driver.getParam(STATUS_REG) };
  return status.busy == 0;
}

status_reg get_status()
{
  return { .raw = driver.getParam(STATUS_REG) };
}

void print_status()
{
  status_reg status = get_status();

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

int32_t degrees_to_steps(float angle)
{
  return (int32_t)(angle * 568.8888888888889 + 0.5);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hallo!");

  //Serial.println(driver.sendSPI(0x00), 16);

  driver.sendSPI(0b10101000); // hard hiz

  print_status();
  get_config();

  step_mode_reg tmp;
  tmp.step_sel = 7; // 2**k = 1..128
  driver.setParam(STEP_MODE_REG, tmp.raw );

  Serial.print("step mode ");
  Serial.println(driver.getParam(STEP_MODE_REG));

  print_status();

  config_reg config = { .raw = driver.getParam(CONFIG_REG) };
  //config.pow_sr = 0b11;
  config.oc_sd = 1;
  //config.f_pwm_div = F_PWM_DIV(7);
  //config.f_pwm_mul = F_PWM_MUL(0.625);
  //config.pow_sr = 0b00;
  config.en_vscomp = 1;
  driver.setParam(CONFIG_REG, config.raw);

  driver.setParam(OCD_TH_REG, OCD_TH_AMPS(1.0));
  driver.setParam(MAX_SPEED_REG, MAX_SPEED_VAL(1000)); // 0x3ff, steps/s = (val * 2**-18 / 250ns)
  driver.setParam(MIN_SPEED_REG, LSPD_OPT | MIN_SPEED_VAL(900)); // 0x3ff, steps/s = (val * 2**-18 / 250ns)
  driver.setParam(FS_SPD_REG, 0x3ff); // 0x3ff
  driver.setParam(ACC_REG, 20);
  driver.setParam(DEC_REG, 20);
  //driver.setParam(MAX_SPEED_REG, 1);

  driver.setParam(KVAL_ACC_REG,  30); // 0x29
  driver.setParam(KVAL_RUN_REG,  30); // 0x29
  driver.setParam(KVAL_DEC_REG,  30); // 0x29
  driver.setParam(KVAL_HOLD_REG, 30); // 0x29

  driver.setParam(INT_SPD_REG, INT_SPD_VAL(800)); // intersect speed 0x0408 = 15 fs/s, max 0x3fff
  driver.setParam(ST_SLP_REG, 35); // 0x19
  driver.setParam(FN_SLP_ACC_REG, 255); // 0x29
  driver.setParam(FN_SLP_DEC_REG, 255); // 0x29


  //driver.sendSPI(0b10110000); // soft stop, enable bridges

  int32_t dir = degrees_to_steps(90.0);
  int32_t pos = dir / 2;
  int32_t maxpos = dir / 2;
  int32_t minpos = -dir / 2;

  /*
  maxpos = 1000;
  minpos = -1000;
  pos = 0;
  dir = +100;
  //*/
  while (true)
  {
    if ((dir > 0 && pos >= maxpos) || (dir < 0 && pos <= minpos))
      dir = -dir;
      
    pos += dir;
    Serial.println(pos);
    
    driver.sendSPI(CMD_GO_TO); // go
    driver.sendValue(22, pos & 0x3fffff);
    while (get_status().mot_status != MOT_STOPPED);
    //delay(500);
  }
}

uint32_t speed = 200 * 8 * 1;

void loop() {
  //Serial.println(analogRead(0));
  //print_status();
  Serial.println(VAL_SPEED(driver.getParam(SPEED_REG)));
  
  //return;
  if (Serial.available())
  {
    int value = Serial.parseInt();
    if (value == 0) return;
    
    //driver.sendSPI(0b10101000); // soft hiz
    //driver.setParam(ST_SLP_REG, value);
    //driver.setParam(FN_SLP_ACC_REG, value);
    //driver.setParam(FN_SLP_DEC_REG, value);
    Serial.print("kval_run: "); Serial.println(value);
  }
  //speed += 1;
  //driver.sendSPI(0b01010001); // run forward
  //driver.sendValue(20, SPEED_VAL(speed));
  //Serial.print("Speed: "); Serial.println(speed);
  delay(200);
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
