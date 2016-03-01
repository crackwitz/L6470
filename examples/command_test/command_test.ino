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

float const acceleration = 100.0; // distribute to a_x**2 + a_y**2 = a**2
// resolution 14.55 st/s^2

float const velocity = 1000.0;
// resolution 15.25 st/s (goto)
// resolution 0.015 st/s (run)

void drivers_setParam(reg_def param, uint32_t data)
{
  driver1.setParam(param, data);
  driver2.setParam(param, data);
}

void receive_speeds_serial()
{
  drivers_setParam(ACC_REG, ACC_VAL(5000));
  drivers_setParam(DEC_REG, ACC_VAL(5000));
  drivers_setParam(MAX_SPEED_REG, MAX_SPEED_VAL(8000));

  while (true)
  {
    if (Serial.available() <= 0)
      continue;
    
    char sel = Serial.read();
    bool doset = (Serial.peek() != '\n');
    float val = 0;
    if (doset) switch (sel)
    {
      case 'a':
      case 'x':
      case 'y':
        val = Serial.parseFloat();
        break;
      case '!':
        break;
      default:
        continue;
    }
   
    int dir = (val > 0);
    Serial.print(sel);
    Serial.print(" ");
    Serial.println(val);
    switch (sel)
    {
      case 'a':
        drivers_setParam(ACC_REG, ACC_VAL(abs(val)));
        drivers_setParam(DEC_REG, ACC_VAL(abs(val)));
        break;
      case 'x':
        driver1.command(CMD_RUN(dir), SPEED_VAL(abs(val)) & 0xfffff);
        break;
      case 'y':
        driver2.command(CMD_RUN(dir), SPEED_VAL(abs(val)) & 0xfffff);
        break;
      case '!':
        driver1.command(CMD_SOFT_HIZ);
        driver2.command(CMD_SOFT_HIZ);
        break;
    }
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hallo!");

  driver1.command(CMD_HARD_HIZ);
  driver2.command(CMD_HARD_HIZ);

//  step_mode_reg tmp;
//  tmp.step_sel = 7; // 2**k = 1..128
//  driver.setParam(STEP_MODE_REG, tmp.raw );
  drivers_setParam(STEP_MODE_REG, (step_mode_reg){ .step_sel = 7 }.raw);

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

  drivers_setParam(OCD_TH_REG, OCD_TH_AMPS(1.0));
  //drivers_setParam(MAX_SPEED_REG, MAX_SPEED_VAL(15000)); // 0x3ff, steps/s = (val * 2**-18 / 250ns)
  drivers_setParam(MAX_SPEED_REG, 0x3ff); // 0x3ff, steps/s = (val * 2**-18 / 250ns)
  
  drivers_setParam(MIN_SPEED_REG, LSPD_OPT | MIN_SPEED_VAL(900)); // 0x3ff, steps/s = (val * 2**-18 / 250ns)

  drivers_setParam(FS_SPD_REG, 0x3ff); // 0x3ff

  // default acc
  drivers_setParam(ACC_REG, 20);
  drivers_setParam(DEC_REG, 20);
  
  /*
  // 12V
  drivers_setParam(KVAL_ACC_REG,  30); // 0x29
  drivers_setParam(KVAL_RUN_REG,  30); // 0x29
  drivers_setParam(KVAL_DEC_REG,  30); // 0x29
  drivers_setParam(KVAL_HOLD_REG, 30); // 0x29
  drivers_setParam(INT_SPD_REG, INT_SPD_VAL(800)); // intersect speed 0x0408 = 15 fs/s, max 0x3fff
  drivers_setParam(ST_SLP_REG, 35); // 0x19
  drivers_setParam(FN_SLP_ACC_REG, 255); // 0x29
  drivers_setParam(FN_SLP_DEC_REG, 255); // 0x29
  /*/
  // 24V
  drivers_setParam(KVAL_ACC_REG,  20); // 0x29
  drivers_setParam(KVAL_RUN_REG,  20); // 0x29
  drivers_setParam(KVAL_DEC_REG,  20); // 0x29
  drivers_setParam(KVAL_HOLD_REG, 20); // 0x29
  // 1060 looks to be it, but value limited to 976.5
  drivers_setParam(INT_SPD_REG, INT_SPD_VAL(1060)); // intersect speed 0x0408 = 15 fs/s, max 0x3fff
  drivers_setParam(ST_SLP_REG, 20);
  drivers_setParam(FN_SLP_ACC_REG, 55);
  drivers_setParam(FN_SLP_DEC_REG, 55);
  //*/

  receive_speeds_serial();

  // speed sweep
  /*
  driver1.setParam(ACC_REG, ACC_VAL(10));
  driver1.setParam(MAX_SPEED_REG, MAX_SPEED_VAL(8000));
  driver1.command(CMD_RUN(1), SPEED_VAL(5) & 0xfffff);
  return;
  //*/

  /*
  // single steps
  long int dx = 1;
  int pos = 0;
  long int xmax = 200;
  long int xmin = -xmax;
  while (true)
  {
    if ((pos >= xmax && dx > 0) || (pos <= xmin && dx < 0))
      dx = -dx;

    pos += dx;
    driver1.command(CMD_GO_TO, pos);
    Serial.println(pos);
    while (is_busy(driver1));
    delay(200);
  }
  
  
  //*/
  float const goldenratio = (1 + sqrt(5)) / 2.0;

  // waypoints
  struct {
    float x;
    float y;
  } waypoints[] = {
    //*
    {-1.0,  1 / goldenratio},
    { 1.0, -1 / goldenratio},
    /*/
    { 0.0,  0.0},
    { 0.5,  1.0},
    { 1.0,  0.0},
    { 0.5, -1.0},
    { 0.0,  0.0},
    {-0.5,  1.0},
    {-1.0,  0.0},
    {-0.5, -1.0}
    //*/
  };
  int const pointcount = sizeof(waypoints) / sizeof(*waypoints);
  float radius = degrees_to_steps(1.0);
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
  static int32_t prev_x = 0, prev_y = 0;

  // figure out speeds and set
  int32_t dx = x - prev_x;
  int32_t dy = y - prev_y;

  float const angle = atan2(dy, dx);
  float const ca = abs(cos(angle));
  float const sa = abs(sin(angle));

  driver1.setParam(ACC_REG, ACC_VAL(ca * acceleration + 0.5));
  driver1.setParam(DEC_REG, ACC_VAL(ca * acceleration + 0.5));
  driver2.setParam(ACC_REG, ACC_VAL(sa * acceleration + 0.5));
  driver2.setParam(DEC_REG, ACC_VAL(sa * acceleration + 0.5));

  driver1.setParam(MAX_SPEED_REG, MAX_SPEED_VAL(ca * velocity + 0.5));
  driver2.setParam(MAX_SPEED_REG, MAX_SPEED_VAL(sa * velocity + 0.5));

  //Serial.print("vx "); Serial.println(VAL_MAX_SPEED(driver1.getParam(MAX_SPEED_REG)));
  //Serial.print("vy "); Serial.println(VAL_MAX_SPEED(driver2.getParam(MAX_SPEED_REG)));

  prev_x = x;
  prev_y = y;

  // go there
  driver1.command(CMD_GO_TO, x & 0x3fffff);
  driver2.command(CMD_GO_TO, y & 0x3fffff);
}

void loop()
{
  return;
  Serial.print("Speeds: ");
  Serial.print(VAL_SPEED(driver1.getParam(SPEED_REG)));
  Serial.print(" ");
  Serial.println(VAL_SPEED(driver2.getParam(SPEED_REG)));
  delay(100);
}
