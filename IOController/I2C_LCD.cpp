#include "I2C_LCD.h"


I2C_LCD::I2C_LCD(uint8_t NumRow, uint8_t NumCol, uint8_t I2Caddress, TwoWire *twi)
{
	_NumRowsLCD = NumRow;
	_NumColsLCD = NumCol;
	_LCDSlaveAddresI2C  = I2Caddress;
	wire = twi;
}

void I2C_LCD::send_data(unsigned char data)
{
	const uint8_t LCDDataByteOn= 0x0D;
	const  uint8_t LCDDataByteOff = 0x09;
	
	unsigned char dataNibbleLower, dataNibbleUpper;
	uint8_t dataBufferI2C[4];
	uint8_t I2CReturnCode = 0;
	
	dataNibbleLower = (data << 4) & 0xf0;
	dataNibbleUpper = data & 0xf0;
	dataBufferI2C[0] = dataNibbleUpper | (LCDDataByteOn  & _LCDBackLight);
	dataBufferI2C[1] = dataNibbleUpper | (LCDDataByteOff & _LCDBackLight);
	dataBufferI2C[2] = dataNibbleLower | (LCDDataByteOn  & _LCDBackLight);
	dataBufferI2C[3] = dataNibbleLower | (LCDDataByteOff &  _LCDBackLight);
	dataBufferI2C[4] = d

	wire->beginTransmission(_LCDSlaveAddresI2C);
	wire->write(dataBufferI2C, 4) ;
	wire->endTransmission();
}

void I2C_LCD::send_cmd(unsigned char cmd)
{	
	const uint8_t LCDCmdByteOn = 0x0C;
	const uint8_t LCDCmdByteOff = 0x08;
	
	unsigned char cmdNibbleLower, cmdNibbleUpper;
	uint8_t cmdBufferI2C[4];
	uint8_t I2CReturnCode = 0;
	
	cmdNibbleLower = (cmd << 4) & 0xf0;
	cmdNibbleUpper = cmd & 0xf0;
	cmdBufferI2C[0] = cmdNibbleUpper | (LCDCmdByteOn & _LCDBackLight);
	cmdBufferI2C[1] = cmdNibbleUpper | (LCDCmdByteOff & _LCDBackLight);
	cmdBufferI2C[2] = cmdNibbleLower | (LCDCmdByteOn & _LCDBackLight);
	cmdBufferI2C[3] = cmdNibbleLower | (LCDCmdByteOff & _LCDBackLight);

	wire->beginTransmission(_LCDSlaveAddresI2C);
	wire->write(cmdBufferI2C, 4) ;
	I2CReturnCode = wire->endTransmission();
}

void I2C_LCD::clear_line(uint8_t lineNo)
{
	switch (lineNo) {
		case 0:
			send_cmd(LCDLineAddressOne);
			break;
		case 1:
			send_cmd(LCDLineAddressTwo); 
			break;
		case 2:
			switch (_NumColsLCD)
			{
				case 16: send_cmd(LCDLineAddress3Col16); break;
				case 20: send_cmd(LCDLineAddress3Col20); break;
			}
			break;
		case 3:
			switch (_NumColsLCD)
			{
				case 16: send_cmd(LCDLineAddress4Col16); break;
				case 20: send_cmd(LCDLineAddress4Col20); break;
			}
			break;
	}

	for (uint8_t i = 0; i < _NumColsLCD; i++) {
		send_data(' ');
	}
}

void I2C_LCD::clear(void)
{
	if (_NumRowsLCD < 1 || _NumRowsLCD >4)
		return;

	clear_line(0);
	
	if (_NumRowsLCD >= 2)
		clear_line(1);
	if (_NumRowsLCD >= 3)
		clear_line(2);
	if (_NumRowsLCD == 4)
		clear_line(3);
}

void I2C_LCD::reset(LCDCursorType_e CursorType)
{
	send_cmd(LCDModeFourBit);
	send_cmd(LCDDisplayOn);
	send_cmd(CursorType);
	send_cmd(LCDClearScreen);
	send_cmd(LCDEntryModeThree);
	delay(5);
}

void I2C_LCD::display(bool OnOff)
{
	OnOff ? send_cmd(LCDDisplayOn) : send_cmd(LCDDisplayOff);
	delay(5);
}

bool I2C_LCD::init(LCDCursorType_e cursorType)
{
	if (init_lcd_i2c() != true)
		return false;
	delay(15);

	send_cmd(LCDHomePosition);
	delay(5);
	send_cmd(LCDHomePosition);
	delay(5);
	send_cmd(LCDHomePosition);
	delay(5);

	send_cmd(LCDModeFourBit);
	send_cmd(LCDDisplayOn);
	send_cmd(cursorType);
	send_cmd(LCDEntryModeThree);
	send_cmd(LCDClearScreen);
	delay(5);

	return true;
}

void I2C_LCD::send_string(char *str)
{
	while (*str)
		send_data(*str++);
}

void I2C_LCD::send_char(char data)
{
	send_data(data);
}

void I2C_LCD::move_cursor(LCDDirectionType_e direction, uint8_t moveSize)
{
	uint8_t i = 0;
	const uint8_t LCDMoveCursorLeft = 0x10;
	const uint8_t LCDMoveCursorRight = 0x14;
	switch(direction)
	{
		case LCDMoveRight:
			for (i = 0; i < moveSize; i++)
				send_cmd(LCDMoveCursorRight);
		break;
		case LCDMoveLeft:
			for (i = 0; i < moveSize; i++)
				send_cmd(LCDMoveCursorLeft);
		break;
	}

}

void I2C_LCD::scroll(LCDDirectionType_e direction, uint8_t ScrollSize)
{
	uint8_t i = 0;

	const uint8_t LCDScrollRight = 0x1E;
	const uint8_t LCDScrollLeft = 0x18;
	
	switch(direction)
	{
		case LCDMoveRight:
			for (i = 0; i < ScrollSize; i++) {
				send_cmd(LCDScrollRight);
			}
		break;
		case LCDMoveLeft:
			for (i = 0; i < ScrollSize; i++) {
				send_cmd(LCDScrollLeft);
			}
		break;
	}
}

void I2C_LCD::go_to(uint8_t line, uint8_t col)
{
	switch (line)
	{
		case 0: send_cmd(LCDLineAddressOne| col); break;
		case 1: send_cmd(LCDLineAddressTwo | col); break;
		case 2:
			switch (_NumColsLCD)
			{
				case 16: send_cmd(LCDLineAddress3Col16 | col); break;
				case 20: send_cmd(LCDLineAddress3Col20 + col); break;
			}
		break;
		case 3:
			switch (_NumColsLCD)
			{
				case 16: send_cmd(LCDLineAddress4Col16 | col); break;
				case 20: send_cmd(LCDLineAddress4Col20+ col); break;
			}
		break;
	}
}

void I2C_LCD::create_custom_char(uint8_t location, uint8_t * charmap)
{
	const uint8_t LCD_CG_RAM = 0x40;
	if (location >= 8)
		return;
	 
	send_cmd(LCD_CG_RAM | (location<<3));
	for (uint8_t i=0; i<8; i++)
	{
		send_data(charmap[i]);
	}
}

void I2C_LCD::set_backlight(bool OnOff)
{
	OnOff ? (_LCDBackLight = LCDBackLightOnMask) : (_LCDBackLight = LCDBackLightOffMask);
}
	
bool I2C_LCD::get_backlight(void)
{
	if(_LCDBackLight == LCDBackLightOffMask)
		return false;
	else
		return true;
}

bool I2C_LCD::init_lcd_i2c()
{
	uint8_t I2CReturnCode= 0;

	wire->begin();
	wire->beginTransmission(_LCDSlaveAddresI2C);

	return !wire->endTransmission();
}

void I2C_LCD::print_custom_char(uint8_t location)
{
	if (location >= 8)
		return;
	send_data(location);
}

size_t I2C_LCD::write(uint8_t  character)
{
	send_char(character);
	return 1;
}

void I2C_LCD::clear_screen_cmd(void)
{
	send_cmd(LCDClearScreen);
	delay(3);
}

void I2C_LCD::home(void)
{
	send_cmd(LCDHomePosition);
	delay(3);
}

void I2C_LCD::change_entry_mode(LCDEntryMode_e newEntryMode)
{
	send_cmd(newEntryMode);
	delay(3);
}
