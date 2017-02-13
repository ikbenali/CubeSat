/*
 * GY88.c
 *
 * Created: 16-1-2017 14:58:50
 * Author : Ali Ul Haq & Enes Eryigit
 *
 */ 

#include <stdio.h>
#include "i2c.h"
#include "GY88.h"

uint8_t buffer[14];

/*
 * test connection to chip
 */
uint8_t mpu6050_testConnection() 
{
	i2c_readBits(MPU6050_ADDRESS,WHO_AM_I_MPU6050, 6, 6, (uint8_t *)buffer); // WHO_AM_I has a length of 6 bits with 0 and 7 reserved, returns default address of MPU6050
	
	if(buffer[0] == 0x68)
	{
		printf("Made connection, MPU6050 Address: 0x%x \n", buffer[0]);
		return 1;
	}
	else 
	{
		return 0;
	}
}

/*
 * set sleep disabled
 */
void mpu6050_setSleepDisabled() 
{
	i2c_writeBit(MPU6050_ADDRESS,PWR_MGMT_1, 6, 0x00); // Select Bit 6 to write to SLEEP
}

/*
 * initialize the accel and gyro
 */
void mpu6050_init() 
{
	//set sleep disabled
	mpu6050_setSleepDisabled();
	//wake up delay needed sleep disabled
	_delay_ms(100);
	//set clock source of Z-axis of gyroscope as reference
	i2c_writeBits(MPU6050_ADDRESS, PWR_MGMT_1, 2, 3, 0x03); 
	//set DLPF bandwidth to 256 Hz
	i2c_writeBits(MPU6050_ADDRESS, CONFIG, 2, 3, 0x00);
	//set sample rate
	i2c_writeByte(MPU6050_ADDRESS,SMPLRT_DIV, 0x07); //8khz / (1 + 7) = 1000Hz
	//set gyro range
	i2c_writeBits(MPU6050_ADDRESS,GYRO_CONFIG, 4, 2, MPU6050_GYRO_FS);
	//set accel range
	i2c_writeBits(MPU6050_ADDRESS,ACCEL_CONFIG, 4, 2, MPU6050_ACCEL_FS);
	
}

/*
 * get raw data
 */
void mpu6050_getRawData(float* axg, float* ayg, float* azg, float* gxds, float* gyds, float* gzds) 
{
	
	int16_t ax = 0;
	int16_t ay = 0;
	int16_t az = 0;
	int16_t gx = 0;
	int16_t gy = 0;
	int16_t gz = 0;
	
	i2c_readBytes(MPU6050_ADDRESS,ACCEL_XOUT_H, 14, (uint8_t *)buffer);
		
    ax = (((int16_t)buffer[0]) << 8) | buffer[1];
    ay = (((int16_t)buffer[2]) << 8) | buffer[3];
    az = (((int16_t)buffer[4]) << 8) | buffer[5];
    gx = (((int16_t)buffer[8]) << 8) | buffer[9];
    gy = (((int16_t)buffer[10]) << 8) | buffer[11];
    gz = (((int16_t)buffer[12]) << 8) | buffer[13];
	
	*axg = (float)(ax)/MPU6050_AGAIN;
	*ayg = (float)(ay)/MPU6050_AGAIN;
	*azg = (float)(az)/MPU6050_AGAIN;
	*gxds = (float)(gx)/MPU6050_GGAIN;
	*gyds = (float)(gy)/MPU6050_GGAIN;
	*gzds = (float)(gz)/MPU6050_GGAIN;		
}






