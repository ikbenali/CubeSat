/*

 * cube.c

 *

 * Created: 16-1-2017 14:58:50

 * Author : Enes_Eryigit

 */



#define F_CPU 2000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <util/delay.h>

#include "i2c.h"
#include "stream.h"                            // or use serialF0.h
#include "uart.h"
#include "GY88.h"
#include "MM7150.h"



#define BAUD_100K 100000UL
#define MPU6050_ADDRESS 0x68



#define PERIOD 1000
#define BASEDUTY 0

extern USART_data_t uartF0;

// globals for current state


void cube_init(){

	//I2C

	i2c_init(&TWIE, TWI_BAUD(F_CPU, BAUD_100K));
	PORTE.DIRSET    = PIN0_bm|PIN1_bm;            // SDA 0 SCL 1
	PORTE.OUTSET = PIN2_bm;
	PORTE.PIN0CTRL  = PORT_OPC_WIREDANDPULL_gc;   // Pullup SDA
	PORTE.PIN1CTRL  = PORT_OPC_WIREDANDPULL_gc;   // Pullup SCL

	//PWM

	PORTD.DIRSET = PIN0_bm; // PD0 PWM MOTOR1
	PORTD.DIRSET = PIN1_bm; // PD2 PWM MOTOR2
	PORTD.DIRSET = PIN2_bm; // PD4 PWM MOTOR3

	TCD0.PER = PERIOD-1; // Define pwm-period
	TCD0.CTRLA = TC_CLKSEL_DIV256_gc; // Prescaling 256
	TCD0.CTRLB = TC0_CCAEN_bm | TC_WGMODE_SS_gc; // Single slope on

	TCD0.CCA = PERIOD-(BASEDUTY*10); // Define dutycycle
	TCD0.CCB = PERIOD-(BASEDUTY*10); // Define dutycycle
	TCD0.CCC = PERIOD-(BASEDUTY*10); // Define dutycycle
}

/*

void power_motor(){
	double upper = 0.0;
	double lower = 0.0;
	int duty_cycle = 0;

	if(rx > lower && rx < upper){
		duty_cycle = rx/3.6;
		TCD0.CCABUF = PERIOD-(duty_cycle*10); // Redefine dutycycle
	}



	if(ry > lower && ry < upper){
		duty_cycle = ry/3.6;
		TCD0.CCBBUF = PERIOD-(duty_cycle*10); // Redefine dutycycle
	}



	if(rz > lower && rz < upper){
		duty_cycle = rz/3.6;
		TCD0.CCCBUF = PERIOD-(duty_cycle*10); // Redefine dutycycle
	}
}

*/



int main(void){

	cube_init();

	init_stream(F_CPU);
	sei();

	 if (mm7150_testConnection()== 0)
	 {
		 printf("No connection");
	 }
	 
	 mpu6050_init();
	 _delay_ms(50);
	float axg;
	float ayg;
	float azg;
	float gxds;
	float gyds;
	float gzds;
	float ax_real;
	float ay_real;
	float az_real;
	double pitch;

	while(1) {

		mpu6050_getRawData(&axg, &ayg, &azg,&gxds, &gyds, &gzds);
		// printf("axg: %f, ayg: %f, azg: %f \n",axg, ayg, azg);
		// printf("gxds: %f, gyds: %f, gzds: %f \n",gxds, gyds, gzds);
		
		pitch = atan((axg / sqrt((ayg*ayg + azg*azg))));

		printf("pitch: %lf", pitch);

		//printf("ax_real: %f\n", ax_real);
		 _delay_ms(2000);

	}

}

