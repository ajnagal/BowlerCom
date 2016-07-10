#include <BowlerCom.h>

DATA_STRUCT * DyioPinFunctionData;
Servo myservo[MAX_SERVOS];  // create servo object to control a servo
boolean startupFlag=false;
int32_t GetConfigurationDataTable(uint8_t pin) {
	return EEReadValue(pin);
}

boolean setMode(uint8_t pin, uint8_t mode) {
	if (GetChannelMode(pin) == mode &&startupFlag)
		return true;
	if (GetChannelMode(pin) == IS_SERVO) {
		myservo[PIN_TO_SERVO(pin)].detach();
	}

	_EEWriteMode(pin, mode);
	//getBcsIoDataTable(pin)->PIN.currentChannelMode = mode;
	println_I("New Mode set: ");
	printMode(pin, INFO_PRINT);

	switch (GetChannelMode(pin)) {
	case IS_DI:
		pinMode(PIN_TO_DIGITAL(pin), INPUT);
		break;
	case IS_DO:
		pinMode(PIN_TO_DIGITAL(pin), OUTPUT);
		break;
	case IS_ANALOG_IN:
		// arduino analogs are not changable
		break;
	case IS_SERVO:
		myservo[PIN_TO_SERVO(pin)].attach(PIN_TO_SERVO(pin));
		break;
	}

	return true;
}

/**
 * Set Channel Values
 * This function takes a
 * @param pin pin index
 * @param numValues a number of values to be delt with
 * @param data an array of data values
 * @param ms the time for the transition to take
 *
 */
boolean SetChanelValueHW(uint8_t pin, uint8_t numValues, int32_t * data,
		float ms) {
	//uint8_t mode = GetChannelMode(pin);
	SetChanVal(pin, data[0], ms);
	return true;
}

/**
 * Set Channel Values
 * This function takes a pin index, a number of values to be delt with, and an array of data values
 * Data is stored into numValues and data
 */
boolean GetChanelValueHW(uint8_t pin, uint8_t * numValues, int32_t * data) {
	//uint8_t mode = GetChannelMode(pin);

	numValues[0] = 1;

	data[0] = GetChanVal(pin);

	return true;

}

/**
 * Set Channel Values
 * This function takes a
 * @param data an array of data values
 * @param ms the time for the transition to take
 *
 */
boolean SetAllChanelValueHW(int32_t * data, float ms) {
	int i;
	for (i = 0; i < GetNumberOfIOChannels(); i++) {
		SetChanelValueHW(i, 1, &data[i], ms);
	}
	return true;
}

/**
 * Set Channel Values
 * This function takes a pin index, a number of values to be delt with, and an array of data values
 * Data is stored into numValues and data
 */
boolean GetAllChanelValueHW(int32_t * data) {
	int i;
	uint8_t numValues;
	for (i = 0; i < GetNumberOfIOChannels(); i++) {
		GetChanelValueHW(i, &numValues, &data[i]);
	}
	return true;
}

/**
 * Configure Channel
 * @param pin the index of the channel to configure
 * @param numValues The number of values passed in to deal with
 * @param data the array of values to use in the configuration step
 */

boolean ConfigureChannelHW(uint8_t pin, uint8_t numValues, int32_t * data) {

	EEWriteValue(pin, data[0]);

	return true;
}

/**
 * Set Stream
 * This function takes a
 * @param pin pin index
 * @param numValues a number of values to be dealt with
 * @param data an array of data values
 *
 */
boolean SetStreamHW(uint8_t pin, uint8_t numValues, uint8_t * data) {

	return true;

}

/**
 * Get Stream
 * This function takes a pin index, a number of values to be dealt with, and an array of data values
 * Data is stored into numValues and data
 */
boolean GetStreamHW(uint8_t pin, uint8_t* numValues, uint8_t * data) {

	return true;
}

boolean SetChanVal(uint8_t pin, int32_t bval, float time) {
	print_W("\r\n");
	p_int_E(pin);
	print_W(" Set value of pin to ");
	p_int_W(bval);
	switch (GetChannelMode(pin)) {

	case IS_DO:
		digitalWrite(PIN_TO_DIGITAL(pin), bval == 0 ? LOW : HIGH);
		break;
	case IS_ANALOG_IN:
		// arduino analogs are not changable
	case IS_SERVO:
		myservo[PIN_TO_SERVO(pin)].write(bval);
		break;
	}

	return true;
}
int32_t GetChanVal(uint8_t pin) {
	switch (GetChannelMode(pin)) {
	case IS_DI:
		return digitalRead(PIN_TO_DIGITAL(pin));
	case IS_ANALOG_IN:
		return analogRead(PIN_TO_ANALOG(pin));
	case IS_SERVO:
		return GetServoPos(pin);
	}

	return getDataTableCurrentValue(pin);
}
void InitPinFunction(DATA_STRUCT * functionData) {
	DyioPinFunctionData = functionData;
	int i;
	for (i = 0; i < TOTAL_PINS; i++) {
		DyioPinFunctionData[i].FUNCTION.HAS_ANALOG_IN = IS_PIN_ANALOG(i);
		DyioPinFunctionData[i].FUNCTION.HAS_PWM = IS_PIN_PWM(i);
		DyioPinFunctionData[i].FUNCTION.HAS_UART_T = IS_PIN_DIGITAL(i)
				&& i % 2 != 0;  //odd pins are tx
		DyioPinFunctionData[i].FUNCTION.HAS_UART_R = IS_PIN_DIGITAL(i)
				&& i % 2 == 0;  // even pins are rx
		DyioPinFunctionData[i].FUNCTION.HAS_SPI_C = false;
		DyioPinFunctionData[i].FUNCTION.HAS_SPI_I = false;
		DyioPinFunctionData[i].FUNCTION.HAS_SPI_O = false;
		DyioPinFunctionData[i].FUNCTION.HAS_COUNTER_INPUT_I = false;
		DyioPinFunctionData[i].FUNCTION.HAS_COUNTER_OUTPUT_I = false;
		DyioPinFunctionData[i].FUNCTION.HAS_COUNTER_INPUT_D = false;
		DyioPinFunctionData[i].FUNCTION.HAS_COUNTER_OUTPUT_D = false;
		DyioPinFunctionData[i].FUNCTION.HAS_COUNTER_INPUT_H = false;
		DyioPinFunctionData[i].FUNCTION.HAS_COUNTER_OUTPUT_H = false;
		DyioPinFunctionData[i].FUNCTION.HAS_DC_MOTOR = false;
		DyioPinFunctionData[i].FUNCTION.HAS_PPM = false;
		//Get mode from EEPROm
		uint8_t mode = GetChannelMode(i);
		//Set up hardware in startup mode so it forces a hardware set
		setMode(i,mode);
		// Get value using hardware setting.
		int32_t currentValue;
		if(isOutputMode(mode)==true){
			currentValue=GetConfigurationDataTable(i);
		}else{
			currentValue= GetChanVal(i);
		}
		setDataTableCurrentValue(i,currentValue);
		DyioPinFunctionData[i].PIN.asyncDataPreviousVal = currentValue;
	}

	InitilizeBcsIo( TOTAL_PINS, DyioPinFunctionData, &SetChanelValueHW,
			&GetChanelValueHW, &SetAllChanelValueHW, &GetAllChanelValueHW,
			&ConfigureChannelHW, &SetStreamHW, &GetStreamHW);

	InitilizeBcsIoSetmode(&setMode);

	initAdvancedAsync();  // after the IO namespace is set up
	startupFlag=true;
}
