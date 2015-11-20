#include <SPI.h>
#include <L6470.h>

L6470 driver1(6, 2, 8);
L6470 driver2(7, 3, 9);
// avoid 10 (nCC) which is the chip select that SPI.begin() uses
// flags: 5, 6
// step clocks: unused

status_reg get_status(L6470 &driver)
{
  return { .raw = driver.getParam(STATUS_REG) };
}

bool is_busy(L6470 &driver)
{
  return get_status(driver).busy == 0;
}

void print_status(L6470 &driver)
{
  status_reg status = get_status(driver);

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

void get_config(L6470 &driver)
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

  driver1.sendSPI(CMD_HARD_HIZ);
  driver2.sendSPI(CMD_HARD_HIZ);

//  step_mode_reg tmp;
//  tmp.step_sel = 7; // 2**k = 1..128
//  driver.setParam(STEP_MODE_REG, tmp.raw );
  driver1.setParam(STEP_MODE_REG, (step_mode_reg){ .step_sel = 7 }.raw);
  driver2.setParam(STEP_MODE_REG, (step_mode_reg){ .step_sel = 7 }.raw);

  Serial.print("step modes ");
  Serial.print(driver1.getParam(STEP_MODE_REG));
  Serial.print(", ");
  Serial.println(driver2.getParam(STEP_MODE_REG));

  //print_status();

  config_reg config1 = { .raw = driver1.getParam(CONFIG_REG) };
  config1.oc_sd = 1;
  config1.en_vscomp = 1;
  //config1.pow_sr = 0b11;
  //config1.f_pwm_div = F_PWM_DIV(7);
  //config1.f_pwm_mul = F_PWM_MUL(0.625);
  //config1.pow_sr = 0b00;
  driver1.setParam(CONFIG_REG, config1.raw);

  config_reg config2 = { .raw = driver2.getParam(CONFIG_REG) };
  config2.oc_sd = 1;
  config2.en_vscomp = 1;
  //config2.pow_sr = 0b11;
  //config2.f_pwm_div = F_PWM_DIV(7);
  //config2.f_pwm_mul = F_PWM_MUL(0.625);
  //config2.pow_sr = 0b00;
  driver2.setParam(CONFIG_REG, config2.raw);

  driver1.setParam(OCD_TH_REG, OCD_TH_AMPS(1.0));
  driver2.setParam(OCD_TH_REG, OCD_TH_AMPS(1.0));

  driver1.setParam(MAX_SPEED_REG, MAX_SPEED_VAL(1000)); // 0x3ff, steps/s = (val * 2**-18 / 250ns)
  driver2.setParam(MAX_SPEED_REG, MAX_SPEED_VAL(1000)); // 0x3ff, steps/s = (val * 2**-18 / 250ns)
  
  driver1.setParam(MIN_SPEED_REG, LSPD_OPT | MIN_SPEED_VAL(900)); // 0x3ff, steps/s = (val * 2**-18 / 250ns)
  driver2.setParam(MIN_SPEED_REG, LSPD_OPT | MIN_SPEED_VAL(900)); // 0x3ff, steps/s = (val * 2**-18 / 250ns)

  driver1.setParam(FS_SPD_REG, 0x3ff); // 0x3ff
  driver2.setParam(FS_SPD_REG, 0x3ff); // 0x3ff

  driver1.setParam(ACC_REG, 20);
  driver1.setParam(DEC_REG, 20);
  driver2.setParam(ACC_REG, 20);
  driver2.setParam(DEC_REG, 20);

  driver1.setParam(KVAL_ACC_REG,  30); // 0x29
  driver1.setParam(KVAL_RUN_REG,  30); // 0x29
  driver1.setParam(KVAL_DEC_REG,  30); // 0x29
  driver1.setParam(KVAL_HOLD_REG, 30); // 0x29
  driver2.setParam(KVAL_ACC_REG,  30); // 0x29
  driver2.setParam(KVAL_RUN_REG,  30); // 0x29
  driver2.setParam(KVAL_DEC_REG,  30); // 0x29
  driver2.setParam(KVAL_HOLD_REG, 30); // 0x29

  driver1.setParam(INT_SPD_REG, INT_SPD_VAL(800)); // intersect speed 0x0408 = 15 fs/s, max 0x3fff
  driver1.setParam(ST_SLP_REG, 35); // 0x19
  driver1.setParam(FN_SLP_ACC_REG, 255); // 0x29
  driver1.setParam(FN_SLP_DEC_REG, 255); // 0x29

  driver2.setParam(INT_SPD_REG, INT_SPD_VAL(800)); // intersect speed 0x0408 = 15 fs/s, max 0x3fff
  driver2.setParam(ST_SLP_REG, 35); // 0x19
  driver2.setParam(FN_SLP_ACC_REG, 255); // 0x29
  driver2.setParam(FN_SLP_DEC_REG, 255); // 0x29

  struct {
    float x;
    float y;
  } waypoints[] = {
    { 0.0,  0.0},
    { 0.5,  1.0},
    { 1.0,  0.0},
    { 0.5, -1.0},
    { 0.0,  0.0},
    {-0.5,  1.0},
    {-1.0,  0.0},
    {-0.5, -1.0}
  };
  int const pointcount = sizeof(waypoints) / sizeof(*waypoints);
  float radius = degrees_to_steps(20.0);
  int index = 0;
  while (true)
  {
    int32_t ix = waypoints[index].x * radius;
    int32_t iy = waypoints[index].y * radius;

    Serial.print("Index ");
    Serial.print(index);
    Serial.print(": ");
    Serial.print(ix);
    Serial.print(" : ");
    Serial.println(iy);
    
    go_to(ix, iy);

    while (get_status(driver1).mot_status != MOT_STOPPED);
    while (get_status(driver2).mot_status != MOT_STOPPED);

    index = (index + 1) % pointcount;
  }
}

void go_to(int32_t x, int32_t y)
{
  static int32_t cur_x = 0, cur_y = 0;

  // figure out speeds and set

  driver1.sendSPI(CMD_GO_TO);
  driver1.sendValue(22, x & 0x3fffff);
  driver2.sendSPI(CMD_GO_TO);
  driver2.sendValue(22, y & 0x3fffff);
}

void loop()
{

}
