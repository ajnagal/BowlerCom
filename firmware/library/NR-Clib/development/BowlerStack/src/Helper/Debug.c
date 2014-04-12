/**
 *
 * Copyright 2009 Neuron Robotics, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "Bowler/Bowler.h"
void sendStr(const char *str);
Print_Level level=NO_PRINT;

#define bufferSize 64

static BOOL DebugINIT = FALSE;
const char AsciiHex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
//#if !defined(NO_PRINTING)
	//static const char  packet[] = "\tPacket = \t";
	//static const char get[] = "Get";
	//static const char post[]="Post ";
	//static const char stat[]= "Status";
	//static const char crit[]="Critical";
	//static const char unknown[] = "Unknown ";
	//static const char ver[] ="\tVersion = \t";
	//static const char mac[] = "\tMAC = \t\t";
	//static const char meth[] = "\tMethod = \t";
	//static const char id[] = "\tNamespace Index = \t";
	//static const char dataSise[] ="\tData Size = \t";
	//static const char crcval []= "\tCRC Value = \t";
//	static const char dval[] = "\tData = \t\t";
//	static const char rpc []="\tRPC code = \t";
//	static const char nodata[] = " no data";
	//static const char streamsize[] = " Stream: size=";
//#endif

const char errorColor[]="\033[31m";
const char infoColor[]="\033[39m";
const char warningColor[]="\033[93m";
//const char debugColor[]="\033[94m";
const char clearErrorColor[]="\033[39m";
static int (*sendToStream)(BYTE * ,int);

void setColor(Print_Level l){
	switch (l){
	case NO_PRINT:
		return;
	case INFO_PRINT:
		  sendStr(infoColor);
		  return;
	case WARN_PRINT:
		  sendStr(warningColor);
		  return;
	case ERROR_PRINT:
		  sendStr( errorColor);
		  return;
	default:
		return;
	}
}

int sendToStreamMine(BYTE * data ,int num){
	int i;
	i=0;
	if(num>=bufferSize){
		num=bufferSize-1;
	}
	for(i=0;i<num;i++){

		putCharDebug(data[i]);
	}
	return i;
}

void setPrintStream(int (*sendToStreamPtr)(BYTE * ,int)){
	if(sendToStreamPtr == 0){
		sendToStreamPtr = &sendToStreamMine;
	}
	sendToStream = sendToStreamPtr;
}

void sendToStreamLocal(BYTE * data ,int num){
	if(sendToStream == 0){
		sendToStream = &sendToStreamMine;
	}
	sendToStream(data,num);
}

Print_Level getPrintLevel(){
	return level;
}

Print_Level setPrintLevel(Print_Level l){
#if defined(NO_PRINTING)
	level=NO_PRINT;
#else
	level=l;
#endif
	return getPrintLevel();
}

BOOL okToPrint(Print_Level l){
	if(getPrintLevel()>=l){
		if(DebugINIT == FALSE){
			DebugINIT = TRUE;
			EnableDebugTerminal();
		}
		return TRUE;
	}
        
	return FALSE;
}
//
//void enableDebug(void){
//	setPrintLevel(INFO_PRINT);
//}
//void disableDebug(void){
//	setPrintLevel(NO_PRINT);
//}



char GetLowNib(BYTE b){
	return AsciiHex[b&0x0f];
}
char GetHighNib(BYTE b){
	return AsciiHex[((b&0xF0)>>4)];
}

void printfDEBUG( char *str,Print_Level l){
	if(!okToPrint(l)){
		return;
	}

	putCharDebug('\n');
	putCharDebug('\r');
	printfDEBUG_NNL(str,l);
	//sendToStreamLocal(data,i);
}
void sendStr(const char *str){
   int x=0;
    while(str[x]!='\0' ){
            putCharDebug(str[x++]);
    }
}
void printfDEBUG_BYTE(char b,Print_Level l){
	if(!okToPrint(l)){
		return;
	}
	setColor(l);
	putCharDebug(b);
	//sendToStreamLocal((BYTE *)&b,1);

}

void printfDEBUG_NNL(char *str,Print_Level l)
{
	if(!okToPrint(l)){
		return;
	}
	setColor(l);
        sendStr(str);
	//sendToStreamLocal(data,i);
}

void printfDEBUG_INT(INT32 val,Print_Level l){
	if(!okToPrint(l)){
		return;
	}
	setColor(l);
	int x;
	x=0;
	if (val<0){
		val *=-1;
		putCharDebug('-');
	}
	BYTE byteStr[12];
	ultoaMINE(val,byteStr);
	while(byteStr[x] != '\0' ){
		putCharDebug(byteStr[x++]);
		if(x==12){
                    if(l==ERROR_PRINT){
                         sendStr(clearErrorColor);
                    }
                    return;
                }
	}
	//sendToStreamLocal(data,i);
}

void printfDEBUG_FL(float f,Print_Level l){
	if(!okToPrint(l)){
		return;
	}
	INT32 upper = (INT32)f;// set up the upper section of the decimal by int casting to clip  off the decimal places
	INT32 shift =(INT32)(f*1000.0f);//shift up the decaml places as a float 3 places
	INT32 clip  = upper*1000;//clip off the upper section of the decimal
	printfDEBUG_INT(upper,l);
	printfDEBUG_BYTE('.',l);
	INT32 dec =shift-clip;
	//make positive and print zeros
	if (dec<0)
		dec*=-1;
	if(dec<100)
		printfDEBUG_BYTE('0',l);
	if(dec<10)
		printfDEBUG_BYTE('0',l);
	printfDEBUG_INT(dec,l);
}

#if defined(BOWLERSTRUCTDEF_H_)

void printPIDvals(int i){

	println("PID chan=",INFO_PRINT);
	int chan =      getPidGroupDataTable()[i].channel;
	int enabled=    getPidGroupDataTable()[i].config.Enabled;
	int polarity =  getPidGroupDataTable()[i].config.Polarity;
	int set =       getPidGroupDataTable()[i].SetPoint;
	p_int(chan,INFO_PRINT);
	print(" Enabled=",INFO_PRINT);     p_int(enabled,INFO_PRINT);
	print(" Polarity=",INFO_PRINT);    p_int(polarity,INFO_PRINT);
	print(" SET=",INFO_PRINT);    p_int(set,INFO_PRINT);
	print(" Kp=",INFO_PRINT);    p_fl(getPidGroupDataTable()[i].config.K.P,INFO_PRINT);
	print(" Ki=",INFO_PRINT);    p_fl(getPidGroupDataTable()[i].config.K.I,INFO_PRINT);
	print(" Kd=",INFO_PRINT);    p_fl(getPidGroupDataTable()[i].config.K.D,INFO_PRINT);
	print("\t Setpoint=",INFO_PRINT);    p_fl(getPidGroupDataTable()[i].SetPoint,INFO_PRINT);
	print("\t Current State=",INFO_PRINT);    p_fl(getPidGroupDataTable()[i].CurrentState,INFO_PRINT);
        print("\t Error=",ERROR_PRINT);    p_fl(getPidGroupDataTable()[i].PreviousError,ERROR_PRINT);
	print("\t Control Output: ",INFO_PRINT); p_fl(getPidGroupDataTable()[i].Output ,INFO_PRINT);
        print("\t Output Set: ",INFO_PRINT); p_fl(getPidGroupDataTable()[i].OutputSet ,INFO_PRINT);

}



void printBowlerPacketDEBUG(BowlerPacket * Packet,Print_Level l){
#if !defined(NO_PRINTING)
	if(!okToPrint(l)){
		return;
	}
		int i;
		BYTE s;
		println("\tPacket = \t",l);
		s = BowlerHeaderSize+Packet->stream[DataSizeIndex];
		printfDEBUG_BYTE('[',l);
		for (i=0;i<s;i++){
			prHEX8(Packet->stream[i],l);
			if (i<s-1)
				printfDEBUG_BYTE(',',l);
		}
		printfDEBUG_BYTE(']',l);
		println("\tVersion = \t",l);
		prHEX8(Packet->stream[0],l);
		println("\tMAC = \t\t",l);
		for (i=0;i<6;i++){
			prHEX8(Packet->stream[1+i],l);
			if (i<5)
				printfDEBUG_BYTE(':',l);
		}
		println("\tMethod = \t",l);
		switch (Packet->stream[MethodIndex]){
		case BOWLER_STATUS:
			print("Status",l);
			break;
		case BOWLER_GET:
			print("Get",l);
			break;
		case BOWLER_POST:
			print("Post ",l);
			break;
		case BOWLER_CRIT:
			print("Critical",l);
			break;
        case BOWLER_ASYN:
			print("ASYNCHRONUS",l);
			break;
		default:
			print("Unknown ",l);
			prHEX8(Packet->stream[MethodIndex],l);
		break;
	}
		println("\tNamespace Index = \t",l);
		p_int((Packet->stream[SessionIDIndex]&0x7f),l);
		println("\tData Size = \t",l);
		p_int((Packet->stream[DataSizeIndex]),l);
		println("\tCRC Value = \t",l);
		p_int((Packet->stream[CRCIndex]),l);
		println("\tCalculated CRC = \t",l);
		p_int(CalcCRC(Packet),l);
		if(Packet->use.head.DataLegnth>=4){
			println("\tRPC code = \t",l);
			for (i=0;i<4;i++){
				printfDEBUG_BYTE(Packet->stream[RPCCodeStart+i],l);
			}
		}
		if(Packet->use.head.DataLegnth>4){
			s= (Packet->use.head.DataLegnth-4);
			println( "\tData = \t\t",l);
			for (i=0;i<s;i++){
				prHEX8(Packet->use.data[i],l);
				if (i<(s-1))
					printfDEBUG_BYTE(',',l);
			}
		}else{
			println(" no data",l);
		}

		println("\n",l);
#endif
}
#endif

void printByteArray(BYTE * stream,UINT16 len,Print_Level l){
//#if !defined(NO_PRINTING)
	if(!okToPrint(l)){
		return;
	}
	UINT16 i;
	print(" Stream: size=",l);
	p_int(len,l);
	print(" [",l);
	for (i=0;i<len;i++){
		prHEX8(stream[i],l);
		if (i<(len-1))
			printfDEBUG_BYTE(',',l);
	}
	print("]",l);
//#endif
}

void ultoaMINE(UINT32 Value, BYTE* Buffer)
{
	BYTE i;
	UINT32 Digit;
	UINT32 Divisor;
	BOOL Printed = FALSE;

	if(Value)
	{
		for(i = 0, Divisor = 1000000000; i < 10; i++)
		{
			Digit = Value/Divisor;
			if(Digit || Printed)
			{
				*Buffer++ = '0' + Digit;
				Value -= Digit*Divisor;
				Printed = TRUE;
			}
			Divisor /= 10;
		}
	}
	else
	{
		*Buffer++ = '0';
	}

	*Buffer = 0;//null terminator
}
