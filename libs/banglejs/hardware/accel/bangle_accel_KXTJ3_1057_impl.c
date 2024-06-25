#ifdef ACCEL_DEVICE_KXTJ3_1057

const ESPR_cmd_i2c bangle_accel_init[] = {
    {ESPR_CMD_I2C_WRITE,0x1B,0b00101000}, // CNTL1 Off (top bit)
    {ESPR_CMD_I2C_WRITE,0x1D,0x80}, // CNTL2 Software reset
    {ESPR_CMD_I2C_SLEEP,0,2000},
    {ESPR_CMD_I2C_WRITE,0x21,0},// DATA_CTRL_REG - 12.5Hz out
    {ESPR_CMD_I2C_WRITE,0x1B,0b00101000}, // CNTL1 Off (top bit), low power, DRDYE=1, 4g, Wakeup=0,
    {ESPR_CMD_I2C_WRITE,0x1B,0b10101000} // CNTL1 On (top bit), low power, DRDYE=1, 4g, Wakeup=0,
};


void banglejs_accel_init_impl(bool firstRun){
    if(firstRun){
        banglejs_i2cCmd_list_process(ACCEL_I2C,ACCEL_ADDR, &bangle_accel_init, SIZE_OF_CMD_I2C(bangle_accel_init));
    }
}

void banglejs_accel_off_impl(){
    jswrap_banglejs_accelWr(0x1B,0);
}

bool banglejs_accel_state_impl(unsigned char *tapped)
{
    //no tapped data
    *tapped=0;
    unsigned char buf[2] ={0x16,0}; // INT_SOURCE1
    // read interrupt source data
    jsi2cWrite(ACCEL_I2C, ACCEL_ADDR, 1, buf, false);
    jsi2cRead(ACCEL_I2C, ACCEL_ADDR, 1, buf, true);
    return (buf[0]&16)!=0; // DRDY
}

void banglejs_accel_get_pos_impl(short *x, short *y,  short *z)
{
    unsigned char buf[6];
    buf[0] = 6;
    jsi2cWrite(ACCEL_I2C, ACCEL_ADDR, 1, buf, false);
    jsi2cRead(ACCEL_I2C, ACCEL_ADDR, 6, buf, true);
    *x = (buf[1]<<8)|buf[0];
    *y = (buf[3]<<8)|buf[2];
    *z = (buf[5]<<8)|buf[4];
}


#endif