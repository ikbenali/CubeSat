#include <avr/io.h>
#include <util/delay.h>


#define TWI_BAUD(F_SYS, F_TWI)   ((F_SYS / (2 * F_TWI)) - 5)

#define I2C_ACK     0
#define I2C_NACK    1
#define I2C_READ    1
#define I2C_WRITE   0

#define I2C_STATUS_OK      0
#define I2C_STATUS_BUSY    1
#define I2C_STATUS_NO_ACK  2

void    i2c_init(TWI_t *twi, uint8_t baudRateRegisterSetting);
uint8_t i2c_start(TWI_t *twi, uint8_t address, uint8_t rw);
uint8_t i2c_restart(TWI_t *twi, uint8_t address, uint8_t rw);
void    i2c_stop(TWI_t *twi);
uint8_t i2c_write(TWI_t *twi, uint8_t data);
uint8_t i2c_read(TWI_t *twi, uint8_t ack);

int8_t mpu6050_readBytes(uint8_t regDevice, uint8_t regAddr, uint8_t length, uint8_t *data);
int8_t mpu6050_readByte(uint8_t regDevice, uint8_t regAddr, uint8_t *data);
void mpu6050_writeBytes(uint8_t regDevice, uint8_t regAddr, uint8_t length, uint8_t* data);
void mpu6050_writeByte(uint8_t regDevice, uint8_t regAddr, uint8_t data);
int8_t mpu6050_readBits(uint8_t regDevice, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
int8_t mpu6050_readBit(uint8_t regDevice, uint8_t regAddr, uint8_t bitNum, uint8_t *data);
void mpu6050_writeBits(uint8_t regDevice, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
void mpu6050_writeBit(uint8_t regDevice, uint8_t regAddr, uint8_t bitNum, uint8_t data);