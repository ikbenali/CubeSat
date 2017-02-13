/*
 * MM7150.c
 *
 * Created: 2/6/2017 5:23:50 AM
 *  Author: Ali
 */ 

#include<inttypes.h>
#include "i2c.h"
#include "MM7150.h"


uint16_t buffer[14];

/*
 * test connection to chip
 */
uint16_t mm7150_testConnection() 
{
	i2c_readByte(MM7150_ADDRESS,0x36,(uint16_t*)buffer)
    printf("%" PRIu16 "\n",buffer[0]);
	
	if(buffer[0] == 0x36)
	{
		printf("Made connection, MPU6050 Address: 0x%x \n", buffer[0]);
		return 1;
	}
	else 
	{
		return 0;
	}
}
