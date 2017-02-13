/*

 * cube.c

 *

 * Created: 16-1-2017 14:58:50

 * Author : Enes_Eryigit & Ali Ul Haq
 * References:
 	- http://dolman-wim.nl/
	- http://davidegironi.blogspot.nl/2013/02/avr-atmega-mpu6050-gyroscope-and.html
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
 //#include "GY88.h"
 //#include "MM7150.h"

 #define BAUD_100K 100000UL
 #define MPU6050_ADDRESS 0x68
 #define MM7150_ADDRESS 0x40

 #define PERIOD 1000
 #define BASEDUTY 0

 extern USART_data_t uartF0;

 // globals for current state

 void cube_init(){

     //I2C
     i2c_init(&TWIE, TWI_BAUD(F_CPU, BAUD_100K));
     PORTE.DIRSET    = PIN0_bm|PIN1_bm|PIN2_bm;            // SDA 0 SCL 1
     PORTE.PIN0CTRL  = PORT_OPC_WIREDANDPULL_gc;   // Pullup SDA
     PORTE.PIN1CTRL  = PORT_OPC_WIREDANDPULL_gc;   // Pullup SCL
     PORTE.OUTSET = PIN2_bm; // Wake Up

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

 void imu_init(){
     _delay_ms(50);
     int16_t start = 0b0011100111110101;

     i2c_start(&TWIE, MM7150_ADDRESS, I2C_WRITE);
     i2c_write(&TWIE, 0x00);
     i2c_restart(&TWIE, MM7150_ADDRESS, I2C_WRITE);
     i2c_write(&TWIE, start);
     i2c_stop(&TWIE);
 }

 int16_t roll, pitch, yaw;
 int r_array[] = {45,28,180};
 int p_array[] = {3, 100, 200};
 int y_array[] = {100,250, 345};
 void simulate_sensor(int i){
     /*roll = rand() % 360;
     pitch = rand() % 360;
     yaw = rand() % 360;*/

     roll = r_array[i];
     pitch = p_array[i];
     yaw = y_array[i];

     printf("Roll : %d \n Pitch : %d \n Yaw : %d \n", roll, pitch, yaw);
 }

 void power_motor(axis){
     double upper = 330.0;
     double lower = 30.0;
     int duty_cycle = 0;

     if(roll > lower && roll < upper){
         printf("Power motor 1\n");
         duty_cycle = roll/3.6;
         TCD0.CCABUF = PERIOD-(duty_cycle*10); // Redefine dutycycle
     }

     if(pitch > lower && pitch < upper){
         printf("Power motor 2\n");
         duty_cycle = pitch/3.6;
         TCD0.CCBBUF = PERIOD-(duty_cycle*10); // Redefine dutycycle
     }

     if(yaw > lower && yaw < upper){
         printf("Power motor 3\n");
         duty_cycle = yaw/3.6;
         TCD0.CCCBUF = PERIOD-(duty_cycle*10); // Redefine dutycycle
     }
 }


 int main(void){

     init_stream(F_CPU);
     sei();

     cube_init();

     while (1)
     {
         for(int i = 0; i < 3; i++){
             simulate_sensor(i);
             _delay_ms(1000);
             power_motor();
             _delay_ms(1000);
         }
     }

     /*imu_init();

     while(1){
         printf("start\n");
         i2c_start(&TWIE, MM7150_ADDRESS, I2C_WRITE);
         i2c_write(&TWIE, 0x16);
         i2c_restart(&TWIE, MM7150_ADDRESS, I2C_READ);
         for(int i = 1; i < 27; i++){
             if(i != 26){
                 printf("value %d : %d\n", i, i2c_read(&TWIE, I2C_ACK) );
             }else{
                 printf("value %d : %d\n", i, i2c_read(&TWIE, I2C_NACK) );
             }


         }
         i2c_stop(&TWIE);
         printf("stop\n");
         power_motor();
         _delay_ms(1000);
     }*/

     /*if (mm7150_testConnection()== 0)
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

     }*/
 }
