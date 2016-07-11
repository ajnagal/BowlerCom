/*
 * EEPROM.c
 *
 *  Created on: Feb 17, 2010
 *      Author: hephaestus
 */
#include <BowlerCom.h>


extern MAC_ADDR MyMAC;

uint8_t eeReadByte(uint16_t addr);
void eeWriteByte(uint16_t addr,uint8_t val);

#define MACKEY 42
#define LOCKKEY 37
#define KEYADDR 0
#define MACSTART 	(KEYADDR+1)
#define MODESTART 	(MACSTART+6)
#define VALUESTART 	(MODESTART+24)
#define UARTBAUD 	(VALUESTART+24)
#define DATASTART_AVR 	(UARTBAUD+4)


void EEInitMAC(void){
	uint8_t i;
	uint8_t key=0;
	key=eeReadByte(KEYADDR);
	if ( (key == LOCKKEY) ){
		for (i=0;i<6;i++){
			MyMAC.v[i]=eeReadByte(i+MACSTART);
		}
	}
}

boolean ProvisionMAC(uint8_t * m){
	uint8_t i;
	uint8_t key=0;
	key=eeReadByte(KEYADDR);
	if (key==LOCKKEY){
		return false; 
	}else{
		for (i=0;i<6;i++){
			MyMAC.v[i]=m[i];
			eeWriteByte(i+MACSTART,MyMAC.v[i]);
		}
		eeWriteByte(KEYADDR,LOCKKEY);
		return true; 
	}
}

boolean checkMode(uint8_t mode){
	if(IS_DI<=mode && IO_MODE_MAX >mode){
		return true;
	}
	return false;
}

void _EEWriteMode(uint8_t pin,uint8_t mode){
	//getBcsIoDataTable(pin)->PIN.currentChannelMode = mode;
	//SetChannelModeDataTable(pin,mode);
	if(!checkMode( mode)){
		mode = getDefaultMode( pin);
	}
	while(EEReadMode(pin) != mode){
		//println_W("Mode Set Pin :");p_int_W(pin);printMode(mode,WARN_PRINT);

		eeWriteByte((uint16_t)(MODESTART+pin),mode);
	}
}

uint8_t EEReadMode(uint8_t pin){
	uint8_t mode =eeReadByte((uint16_t)(MODESTART+pin));
	if(!checkMode( mode)){
		mode =  getDefaultMode( pin);
	}

	return mode;
}


void EEWriteValue(uint16_t pin,uint8_t value){
	if (value == 255)
		value = 254;
	eeWriteByte((uint16_t)(VALUESTART+pin),value);
}

uint8_t EEReadValue(uint16_t pin){
	uint8_t val = eeReadByte((uint16_t)(VALUESTART+pin));
	if (val == 255)
		EEWriteValue(pin,128);
	return eeReadByte((uint16_t)(VALUESTART+pin));
}

uint32_t EEReadBaud(void){
	UINT32_UNION v;
	v.byte.FB= eeReadByte((UARTBAUD+0));
	v.byte.TB= eeReadByte((UARTBAUD+1));
	v.byte.SB= eeReadByte((UARTBAUD+2));
	v.byte.LB= eeReadByte((UARTBAUD+3));
	return v.Val;
}

void EEWriteBaud(uint32_t val){
	UINT32_UNION v;
	v.Val = val;
	eeWriteByte((UARTBAUD+0),v.byte.FB);
	eeWriteByte((UARTBAUD+1),v.byte.TB);
	eeWriteByte((UARTBAUD+2),v.byte.SB);
	eeWriteByte((UARTBAUD+3),v.byte.LB);
}

uint8_t EEReadData(uint16_t addr){
	uint8_t val=0;
	if((addr+DATASTART_AVR)>=4096){
		println_E("Attempting Overwrite of EEPROM!")
		return 0;
	}
	val =  eeReadByte((DATASTART_AVR+addr));
//	if(addr == 189){
//		println_W("R eeprom addr: ");p_int_W(addr);print_W(" is: ");p_int_W(val);
//	}
	return val;
}
void EEWriteData(uint16_t addr,uint8_t data){
	if((addr+DATASTART_AVR)>=4096){
		println_E("Attempting Overwrite of EEPROM!")
		return;
	}
	eeWriteByte((DATASTART_AVR+addr),data);
//	if(addr == 189){
//		println_E("W eeprom addr: ");p_int_W(addr);print_W(" is: ");p_int_W(data);
//	}
}


uint8_t eeReadByte(uint16_t addr){

//	/* Wait for completion of previous write */
//	while(EECR & (1<<EEPE));
//	EECR=0;
//	/* Set up address register */
//	EEAR = addr;
//	/* Start eeprom read by writing EERE */
//	EECR |= (1<<EERE);
//	/* Return data from Data Register */
//	return EEDR;
	return EEPROM.read(addr);
}

void eeWriteByte(uint16_t addr,uint8_t val){
	if (eeReadByte(addr)==val)
		return;
	EEPROM.write(addr, val);
//	/* Wait for completion of previous write */
//	while(EECR & (1<<EEPE));
//	EECR=0;
//	/* Set up address and Data Registers */
//	EEAR = addr;
//	EEDR = val;
//	/* Write logical one to EEMPE */
//	EECR |= (1<<EEMPE);
//	/* Start eeprom write by setting EEPE */
//	EECR |= (1<<EEPE);

}
//pid_vales pidEEPRomVal[NUM_PID_GROUPS];

pid_vales tmpVals;

uint8_t loadEEDone=false;
//void LoadEEstore(void){
//	if(loadEEDone)
//		return;
//	//println_I("Loading eeprom data");
//	loadEEDone=true;
//	int i;
//	for (i=0;i<NUM_PID_GROUPS;i++){
//		GetEEPRomData((pidValSize*i),(pidValSize*i)+pidValSize,tmpVals.stream);
//	}
//	//println_I("Done loading eeprom data");
//}
void LoadPIDvals(AbsPID * pid, DYIO_PID * dy,int group){

	GetEEPRomData((pidValSize*group),(pidValSize*group)+pidValSize,tmpVals.stream);
	//uint8_t i = group;
	if(lockCodeValue !=tmpVals.data.lockCode ){
		WritePIDvalues( pid,  dy, group);
		return;
	}
	if(tmpVals.data.outputChannel==tmpVals.data.inputChannel)
		return;
	if(tmpVals.data.outputChannel>=GetNumberOfIOChannels() ||tmpVals.data.inputChannel>=GetNumberOfIOChannels() )
		return;
	if(tmpVals.data.outputMode==tmpVals.data.inputMode)
		return;
	//println_I("Using values for chan: ");p_int_I(i);
	pid->config.Enabled=tmpVals.data.Enabled;
	pid->config.Polarity=tmpVals.data.Polarity;
	//pidChans->Async=tmpVals.data.Async;
	dy->inputMode=tmpVals.data.inputMode;
	dy->outputMode=tmpVals.data.outputMode;
	dy->outputChannel=tmpVals.data.outputChannel;
	dy->inputChannel=tmpVals.data.inputChannel;

	pid->config.K.P=tmpVals.data.K.P;
	pid->config.K.I=tmpVals.data.K.I;
	pid->config.K.D=tmpVals.data.K.D;
}

void WritePIDvalues(AbsPID * pid, DYIO_PID * dy,int group){
	uint8_t i = group;
	pid_vales tmpVals;
	tmpVals.data.lockCode =lockCodeValue;
	tmpVals.data.Enabled= pid->config.Enabled;
	tmpVals.data.Polarity=pid->config.Polarity;
	tmpVals.data.Async=pid->config.Async;
	tmpVals.data.inputMode=dy->inputMode;
	tmpVals.data.outputMode=dy->outputMode;
	tmpVals.data.inputChannel=dy->inputChannel;
	tmpVals.data.outputChannel=dy->outputChannel;
	tmpVals.data.K.P=pid->config.K.P;
	tmpVals.data.K.I=pid->config.K.I;
	tmpVals.data.K.D=pid->config.K.D;
	SetEEPRomData((pidValSize*i),(pidValSize*i)+pidValSize,tmpVals.stream);
}
void writePPMLink(uint8_t * vals){
	SetEEPRomData(PID_VAL_END,PID_VAL_END+NUM_PPM_CHAN,vals);
}
void readPPMLink(uint8_t * vals){
	GetEEPRomData(PID_VAL_END,PID_VAL_END+NUM_PPM_CHAN,vals);
}

void setEEBrownOutDetect(boolean b){
	uint8_t tmp = b?1:0;
	setCoProcBrownOutMode(b);
	SetEEPRomData(BROWNOUT_START,BROWNOUT_END,&tmp);
}
boolean getEEBrownOutDetect(){
	uint8_t tmp =0;
	GetEEPRomData(BROWNOUT_START,BROWNOUT_END,&tmp);
	return tmp?true:false;
}

void GetEEPRomData(uint8_t start, uint8_t stop, uint8_t * data) {
//	println_W("Getting eeprom page: ");p_int_W(start+DATASTART);print_W(" to ");p_int_W(stop+DATASTART);
	//WORD_VAL raw;
	uint8_t i = 0;
	if (start > stop) {
		//println_W("###ERROR eeprom");
		return;
	}
	for (i = 0; i < (stop - start); i++) {
		data[i] = EEReadData(i+start);
	}
//	println_W("Rx<<");
//	printBowlerPacketDEBUG(&downstreamPacketTemp,WARN_PRINT);
}

void SetEEPRomData(uint8_t start, uint8_t stop, uint8_t * data) {
	//println_W("Setting eeprom page: ");p_int_W(start+DATASTART);print_W(" to ");p_int_W(stop+DATASTART);
	//WORD_VAL raw;
	uint8_t i = 0;
	if (start >= stop)
		return;

	for (i = 0; i < (stop - start); i++) {
		EEWriteData(i+start,data[i]);
	}
}

uint8_t isAsciiMine(char  str) {
	if (str < 48) {
		return false;
	}
	if (str > 122) {
		return false;
	}
	return true;
}

boolean GetName(char * name){
	//WORD_VAL raw;
	uint8_t i = 0;
	//printPacket(&downstreamPacketTemp,WARN_PRINT);
	if( isAsciiMine(EEReadData(NAMESTART))){
		while (isAsciiMine(EEReadData(NAMESTART+i))) {
			name[i] = EEReadData(NAMESTART+i);
			i++;
			if (i == NAMESIZE)
				break;
		}
		name[i] = 0;// Null terminated strings
	}
	println_I("Getting name: ");
	printfDEBUG(name,INFO_PRINT);
	return isAsciiMine(name[0]);
}
void SetName(char * name){
	uint8_t i = 0;
	println_W("Setting name: ");
	printfDEBUG(name,WARN_PRINT);
	while (isAsciiMine(name[i])) {
		EEWriteData(NAMESTART+i,name[i]);
	}
}

