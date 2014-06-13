/*
 * HAL.c
 *
 *  Created on: Jan 4, 2010
 *      Author: hephaestus
 */
#include "arch/pic32/BowlerConfig.h"
#include "Bowler/Bowler.h"


static enum {
      EXCEP_IRQ = 0,            // interrupt
      EXCEP_AdEL = 4,            // address error exception (load or ifetch)
      EXCEP_AdES,                // address error exception (store)
      EXCEP_IBE,                // bus error (ifetch)
      EXCEP_DBE,                // bus error (load/store)
      EXCEP_Sys,                // syscall
      EXCEP_Bp,                // breakpoint
      EXCEP_RI,                // reserved instruction
      EXCEP_CpU,                // coprocessor unusable
      EXCEP_Overflow,            // arithmetic overflow
      EXCEP_Trap,                // trap (possible divide by zero)
      EXCEP_IS1 = 16,            // implementation specfic 1
      EXCEP_CEU,                // CorExtend Unuseable
      EXCEP_C2E                // coprocessor 2
  } _excep_code;


void __attribute__((nomips16)) _general_exception_handler() {
        //static unsigned int _epc_code;
        static unsigned int _excep_addr;
        asm volatile("mfc0 %0,$13" : "=r" (_excep_code));
        asm volatile("mfc0 %0,$14" : "=r" (_excep_addr));
        _excep_code = (_excep_code & 0x0000007C) >> 2;

        setPrintLevelInfoPrint();

        print_E("\r\n\r\n\r\nException handeler!! ");
        switch(_excep_code){
            case EXCEP_IRQ: print_E ("interrupt");break;
            case EXCEP_AdEL: print_E ("address error exception (load or ifetch)");break;
            case EXCEP_AdES: print_E ("address error exception (store)");break;
            case EXCEP_IBE: print_E ("bus error (ifetch)");break;
            case EXCEP_DBE: print_E ("bus error (load/store)");break;
            case EXCEP_Sys: print_E ("syscall");break;
            case EXCEP_Bp: print_E ("breakpoint");break;
            case EXCEP_RI: print_E ("reserved instruction");break;
            case EXCEP_CpU: print_E ("coprocessor unusable");break;
            case EXCEP_Overflow: print_E ("arithmetic overflow");break;
            case EXCEP_Trap: print_E ("trap (possible divide by zero)");break;
            case EXCEP_IS1: print_E ("implementation specfic 1");break;
            case EXCEP_CEU: print_E ("CorExtend Unuseable");break;
            case EXCEP_C2E: print_E ("coprocessor 2");break;
        }
        print_E(" at 0x");
        prHEX32(_excep_addr,ERROR_PRINT);
        print_E("\r\n");

        DelayMs(1000);
        Reset();
}

static BYTE_FIFO_STORAGE storeUSB;
static BYTE privateRXUSB[BOWLER_PacketSize*2];
static BYTE_FIFO_STORAGE storeUART;
static BYTE privateRXUART[BOWLER_PacketSize*2];

static BOOL init=FALSE;

static BOOL usbComs = FALSE;
static BOOL uartComs = FALSE;

static BOOL disableSerial=FALSE;

void disableSerialComs(BOOL state){
    disableSerial=state;
    uartComs = !state;
}
void Pic32_Bowler_HAL_Init(void){

	init=TRUE;

	println_W("Init USB fifo");
	InitByteFifo(&storeUSB,privateRXUSB,sizeof(privateRXUSB));
	println_W("Init UART fifo");
	InitByteFifo(&storeUART,privateRXUART,sizeof(privateRXUART));
	println_W("Setting Serial FIFO buffer");
	SetPICUARTFifo(&storeUART);
	println_W("Setting USB FIFO buffer");
	SetPICUSBFifo(&storeUSB);

	println_W("Init UART hal");
	Pic32UART_HAL_INIT(PRINT_BAUD);
	TickInit();

	println_W("Pic32 is initialized...");
}

//HAL init functions

void Get_HAL_Packet(BYTE * packet,WORD size){

    if(usbComs){
        if(FifoGetByteStream(&storeUSB,packet,size) != 0)
            return;
    }

    if(uartComs){
        if(FifoGetByteStream(&storeUART,packet,size) != 0)
            return;
    }
    print_E("Hal reported error in retriving packet __FILE__");print_E(__FILE__);
}

BOOL Send_HAL_Packet(BYTE * packet,WORD size){

    if(usbComs)
        SendPacketUSB(packet,size);
    if(uartComs)
        Pic32UARTPutArray(packet,size);
    return TRUE;
}
WORD Get_HAL_Byte_Count(){
#if defined(USB_POLLING)
        USBDeviceTasks();
#endif
	if( init==FALSE){
		println_W("***Initializing the PIC hal***");
		Pic32_Bowler_HAL_Init();
	}
	//println_I("Getting the USB bytes");

	if(GetNumUSBBytes()>0){
            usbComs=TRUE;
            //println_I("Found USB bytes");
            return FifoGetByteCount(&storeUSB);
	}
	else {
		//println_I("Getting the UART bytes");
		if(Pic32Get_UART_Byte_Count()>0){
			//println_I("Found the UART bytes");
			if(!disableSerial)
				uartComs=TRUE;
			return FifoGetByteCount(&storeUART);
		}
	}
	return 0;
}

BOOL GetBowlerPacket_arch(BowlerPacket * Packet){
	//println_I("Checking packet from HAL");
	Get_HAL_Byte_Count();//This runs other update tasks for the HAL
	//println_I("Getting packet from HAL");
        if(usbComs)
            if(GetBowlerPacketDebug(Packet,&storeUSB))
                return TRUE;
        if (uartComs)
            if(GetBowlerPacketDebug(Packet,&storeUART))
                return TRUE;
	return FALSE;
}


/**
 * send the array out the connection
 */
UINT16 putStream(BYTE *packet,UINT16 size){
	Send_HAL_Packet(packet,size);
	return TRUE;
}
/**
 * get the time in ms
 */
float getMs(void){
	return TickGetMS();
}
/**
 * send this char to the //print terminal
 */
void putCharDebug(char c){
	WriteUART_DEBUG(c);
}
/**
 * Start the scheduler
 */
void startScheduler(void){
	TickInit();
}


void EnableDebugTerminal(void){
	Pic32UART_HAL_INIT(PRINT_BAUD);
}

