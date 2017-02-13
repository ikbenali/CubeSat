#include "i2c.h"

void i2c_init(TWI_t *twi, uint8_t baudRateRegisterSetting)
{
	twi->MASTER.BAUD   = baudRateRegisterSetting;
	twi->MASTER.CTRLC  = 0;
	twi->MASTER.CTRLA  = TWI_MASTER_ENABLE_bm;
	twi->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
}

uint8_t i2c_start(TWI_t *twi, uint8_t address, uint8_t rw)
{
	if ( (twi->MASTER.STATUS & TWI_MASTER_BUSSTATE_gm) !=                      // if bus available
	TWI_MASTER_BUSSTATE_IDLE_gc ) return I2C_STATUS_BUSY; //
	twi->MASTER.ADDR = (address << 1) | rw;                                    // send slave address
	while( ! (twi->MASTER.STATUS & (TWI_MASTER_WIF_bm << rw)) );               // wait until sent

	if ( twi->MASTER.STATUS & TWI_MASTER_RXACK_bm ) {                          // if no ack
		twi->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		return I2C_STATUS_NO_ACK;
	}

	return I2C_STATUS_OK;
}

uint8_t i2c_restart(TWI_t *twi, uint8_t address, uint8_t rw)
{
	twi->MASTER.ADDR = (address << 1) | rw;                                    // send slave address
	while( ! (twi->MASTER.STATUS & (TWI_MASTER_WIF_bm << rw)) );               // wait until sent

	if ( twi->MASTER.STATUS & TWI_MASTER_RXACK_bm ) {                          // if no ack
		twi->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		return I2C_STATUS_NO_ACK;
	}

	return I2C_STATUS_OK;
}

void i2c_stop(TWI_t *twi)
{
	twi->MASTER.CTRLC  = TWI_MASTER_CMD_STOP_gc;
	twi->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
}

uint8_t i2c_write(TWI_t *twi, uint8_t data)
{
	twi->MASTER.DATA = data;                                                   // send data
	while( ! (twi->MASTER.STATUS & TWI_MASTER_WIF_bm) );                       // wait until sent

	if ( twi->MASTER.STATUS & TWI_MASTER_RXACK_bm ) {                          // if no ack
		twi->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		return I2C_STATUS_NO_ACK;
	}

	return I2C_STATUS_OK;
}

uint8_t i2c_read(TWI_t *twi, uint8_t ack)
{
	uint8_t data;

	while( ! (twi->MASTER.STATUS & TWI_MASTER_RIF_bm) );                       // wait until received
	data = twi->MASTER.DATA;                                                   // read data
	twi->MASTER.CTRLC = ((ack==I2C_ACK) ? TWI_MASTER_CMD_RECVTRANS_gc :        // send ack (go on) or
	TWI_MASTER_ACKACT_bm|TWI_MASTER_CMD_STOP_gc); //     nack (and stop)

	if ( ack == I2C_NACK ) {
		while( ! (twi->MASTER.STATUS & TWI_MASTER_BUSSTATE_IDLE_gc) );
	}

	return data;
}

/*
 * read bytes from chip register
 */
int8_t i2c_readBytes(uint8_t regDevice,uint8_t regAddr, uint8_t length, uint8_t *data) {
	uint8_t i = 0;
	int8_t count = 0;
	if(length > 0) {
		i2c_start(&TWIE, regDevice, I2C_WRITE);
		i2c_write(&TWIE, regAddr);
		_delay_us(10);
		i2c_restart(&TWIE, regDevice, I2C_READ);
		//read data
		for(i=0; i<length; i++) {
			count++;
			if(i==length-1)
				data[i] = i2c_read(&TWIE, I2C_NACK);
			else
				data[i] = i2c_read(&TWIE, I2C_ACK);
		}
		i2c_stop(&TWIE);
	}
	return count;
}

/*
 * read 1 byte from chip register
 */
int8_t i2c_readByte(uint8_t regDevice,uint8_t regAddr, uint8_t *data) {
    return i2c_readBytes(regDevice,regAddr, 1, data);
}


/*
 * write bytes to chip register
 */
void i2c_writeBytes(uint8_t regDevice,uint8_t regAddr, uint8_t length, uint8_t* data) {
	if(length > 0) {
		//write data
		i2c_start(&TWIE, regDevice, I2C_WRITE);
		i2c_write(&TWIE, regAddr);
		for (uint8_t i = 0; i < length; i++) {
			i2c_write(&TWIE, (uint8_t) data[i]);
		}
		i2c_stop(&TWIE);
	}
}

/*
 * write 1 byte to chip register
 */
void i2c_writeByte(uint8_t regDevice,uint8_t regAddr, uint8_t data) {
    return i2c_writeBytes(regDevice,regAddr, 1, &data);
}

/*
 * read bits from chip register
 */
int8_t i2c_readBits(uint8_t regDevice,uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    int8_t count = 0;
    if(length > 0) {
		uint8_t b;
		if ((count = i2c_readByte(regDevice,regAddr, &b)) != 0) {
			uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
			b &= mask;
			b >>= (bitStart - length + 1);
			*data = b;
		}
    }
    return count;
}

/*
 * read 1 bit from chip register
 */
int8_t i2c_readBit(uint8_t regDevice,uint8_t regAddr, uint8_t bitNum, uint8_t *data) {
    uint8_t b;
    uint8_t count = i2c_readByte(regDevice,regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}

/*
 * write bit/bits to chip register
 */
void i2c_writeBits(uint8_t regDevice,uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
	if(length > 0) {
		uint8_t b = 0;
		if (i2c_readByte(regDevice,regAddr, &b) != 0) { //get current data
			uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
			data <<= (bitStart - length + 1); // shift data into correct position
			data &= mask; // zero all non-important bits in data
			b &= ~(mask); // zero all important bits in existing byte
			b |= data; // combine data with existing byte
			i2c_writeByte(regDevice,regAddr, b);
		}
	}
}

/*
 * write one bit to chip register
 */
void i2c_writeBit(uint8_t regDevice,uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    uint8_t b;
    i2c_readByte(regDevice,regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    i2c_writeByte(regDevice,regAddr, b);
}

