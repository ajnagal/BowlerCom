
#include <BowlerCom.h>
//int maxNumCharsInModes=0;
//char *  printModeStrings[]={
//		"NO_CHANGE",
//		"HIGH_IMPEDANCE",
//		"IS_DI",
//		"IS_DO",
//		"IS_ANALOG_IN",
//		"IS_ANALOG_OUT",
//		"PWM",
//		"SERVO",
//		"UART_TX",
//		"UART_RX",
//		"SPI_MOSI",
//		"SPI_MISO",
//		"SPI_SCK",
//		"SPI_SS",
//		"COUNTER_INPUT_INT",
//		"COUNTER_INPUT_DIR",
//		"COUNTER_INPUT_HOME",
//		"COUNTER_OUTPUT_INT",
//		"COUNTER_OUTPUT_DIR",
//		"COUNTER_OUTPUT_HOME",
//		"IS_DC_MOTOR_DIR",
//		"IS_DC_MOTOR_VEL",
//		"IS_PPM_IN"
//};

int mystrlen(const char * s){
	int i=0;
	while(s[i++]);
	return i;
}

//char * unknown = "UNKNOWN";
void printMode(uint8_t mode, Print_Level l){
	print_nnl(" Mode 0x",l);prHEX8(mode,l);

	switch(mode){
case NO_CHANGE	 	 		:
		b_println("NO_CHANGE",l);
		break;

case HIGH_IMPEDANCE 	 	:
		b_println("HIGH_IMPEDANCE",l);
		break;
case IS_DI	 		 		:
		b_println("IS_DI",l);
		break;
case IS_DO		 			:
		b_println("IS_DO",l);
		break;
case IS_ANALOG_IN 			:
		b_println("IS_ANALOG_IN",l);
		break;
case IS_ANALOG_OUT			:
		b_println("IS_ANALOG_OUT",l);
		break;
case IS_PWM	 				:
		b_println("IS_PWM",l);
		break;
case IS_SERVO 				:
		b_println("IS_SERVO",l);
		break;
case IS_UART_TX		 		:
		b_println("IS_UART_TX",l);
		break;
case IS_UART_RX	 			:
		b_println("IS_UART_RX",l);
		break;
case IS_SPI_MOSI			:
		b_println("IS_SPI_MOSI",l);
		break;
case IS_SPI_MISO			:
		b_println("IS_SPI_MISO",l);
		break;
case IS_SPI_SCK	 			:
		b_println("IS_SPI_SCK",l);
		break;
case IS_COUNTER_INPUT_INT	:
		b_println("IS_COUNTER_INPUT_INT",l);
		break;
case IS_COUNTER_INPUT_DIR	:
		b_println("IS_COUNTER_INPUT_DIR",l);
		break;
case IS_COUNTER_INPUT_HOME	:
		b_println("IS_COUNTER_INPUT_HOME",l);
		break;
case IS_COUNTER_OUTPUT_INT	:
		b_println("IS_COUNTER_OUTPUT_INT",l);
		break;
case IS_COUNTER_OUTPUT_DIR	:
		b_println("IS_COUNTER_OUTPUT_DIR",l);
		break;
case IS_COUNTER_OUTPUT_HOME	:
		b_println("IS_COUNTER_OUTPUT_HOME",l);
		break;
case IS_DC_MOTOR_VEL		:
		b_println("IS_DC_MOTOR_VEL",l);
		break;
case IS_DC_MOTOR_DIR		:
		b_println("IS_DC_MOTOR_DIR",l);
		break;
case IS_PPM_IN				:
		b_println("IS_PPM_IN",l);
		break;

	}

}
