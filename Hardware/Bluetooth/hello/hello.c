#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "drivers/pinout.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/eeprom.h"
#include "driverlib/flash.h"
#include "utils/uartstdio.h"

//*****************************************************************************
//
// System clock rate in Hz.
//
//*****************************************************************************
uint32_t g_ui32SysClock;


//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

int flagMode = 0;
int flagcmd = 0;
int BL_id = 0;
uint32_t eepromAddr = 0;
char BL_cmd_receive[100];



//*****************************************************************************
//
// The UART7 Bluetooth interrupt handler.
//
//*****************************************************************************
void
BTIntHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the interrupt status.
    //
    ui32Status = ROM_UARTIntStatus(UART7_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART7_BASE, ui32Status);

    int var = 0;
    char bufferBLReceive[100];
    int i = 0;
    for (i = 0; i < 100; ++i) {
        bufferBLReceive[i]=0;
    }
    //
    // Loop while there are characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART7_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART0.
        //
//        ROM_UARTCharPutNonBlocking(UART0_BASE,
//                                   ROM_UARTCharGetNonBlocking(UART7_BASE));
        bufferBLReceive[var]=ROM_UARTCharGet(UART7_BASE);
        ROM_UARTCharPut(UART0_BASE,bufferBLReceive[var]);
        var++;
    }

    if (flagMode == 1) {
        bufferBLReceive[var] = '\0';
        uint32_t data = atoi(bufferBLReceive);
        if(eepromAddr == 4294967295) {
            eepromAddr = 0;
        }
        EEPROMProgram(&data,eepromAddr+4,4);
        eepromAddr+=4;
        EEPROMProgram(&eepromAddr, 0, 4);

        uint32_t temp;
        EEPROMRead(&temp,eepromAddr,4);
        UARTprintf("%d ",temp);

    } else {
        bufferBLReceive[var] = '\0';
        if(strstr(bufferBLReceive,"OK")){
            flagcmd = 1;
            UARTprintf("YES!\n");
        }
    }



}

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(char *pui8Buffer, uint32_t ui32Count)
{
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        ROM_UARTCharPut(UART7_BASE, *pui8Buffer++);

    }
    ROM_UARTCharPut(UART7_BASE, '\r');
    ROM_UARTCharPut(UART7_BASE, '\n');
}


//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}

void
ConfigureUARTBluetooth(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    //
    // Enable UART7
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);

    //
    // Enable processor interrupts.
    //
    ROM_IntMasterEnable();

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PC4_U7RX);
    ROM_GPIOPinConfigure(GPIO_PC5_U7TX);
    ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART7_BASE, g_ui32SysClock, 38400,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));


    //
    // Enable the UART interrupt.
    //
    ROM_IntEnable(INT_UART7);
    ROM_UARTIntEnable(UART7_BASE, UART_INT_RX | UART_INT_RT);

}

//*****************************************************************************
//
// Print "Hello World!" to the UART on the Intelligent UART Module.
//
//*****************************************************************************
int
main(void)
{
    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);

    //
    // Configure the device pins.
    //
    PinoutSet(false, false);

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    //
    // Enable the GPIO pins for the LED (PN0).
    //
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);


    //
    // Enable the GPIO port that is used for KEY and VCC.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    //
    // Enable the GPIO pins for the VCC (PC7).
    //
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_7);

    //
    // Enable the GPIO pins for the KEY (PC6).
    //
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6);

    //
    // Turn off the Bluetooth
    //
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0); // Key
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0); // VCC

    SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms

    //
    // Enter the AT Mode
    //
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_PIN_6);
    SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_PIN_7); // VCC



    //
    // Enable the GPIO pins for the LED D1 (PN1).
    //
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    //
    // Initialize the UARTs.
    //
    ConfigureUART();
    ConfigureUARTBluetooth();


    //
    // Initialize eeprom
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    EEPROMInit();
    //UARTprintf("%d\n",EEPROMSizeGet());
    //UARTprintf("%d\n",EEPROMBlockCountGet());
    EEPROMRead(&eepromAddr,0,4);
    if(eepromAddr == 4294967295)
    {
        uint32_t zero = 0;
        EEPROMProgram(&zero, 0, 4);
        EEPROMRead(&eepromAddr,0,4);
    }
    UARTprintf("%d\n",eepromAddr);

    //
    // Hello!
    //
    UARTprintf("Hello, world!\n");
    int var = 0;
    for (var = 0; var < 1000; ++var) {
        SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
    }

    flagMode = 0;

    //
    // We are finished.  Hang around flashing D1.
    //

    int t = 1000;
    char *BL_test = "AT";
    UARTSend(BL_test, strlen(BL_test));
    while(!flagcmd && t--){
        SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
    }
    if(!flagcmd){
        UARTSend(BL_test, strlen(BL_test));
        t=1000;
        while(!flagcmd && t--){
           SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
        }
        if(!flagcmd){
            UARTprintf("Error Connecting Bluetooth");
        }
    }
    flagcmd = 0;

    t=1000;
    char *BL_role = "AT+ROLE=1";
    UARTSend(BL_role, strlen(BL_role));
    while(!flagcmd && t--){
        SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
    }
    if(!flagcmd){
        UARTSend(BL_role, strlen(BL_role));
        t=1000;
        while(!flagcmd && t--){
           SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
        }
        if(!flagcmd){
            UARTprintf("Error Connecting Bluetooth");
        }
    }
    flagcmd = 0;

    t=1000;
    char* BL_CMODE = "AT+CMODE=0";
    UARTSend(BL_CMODE, strlen(BL_CMODE));
    while(!flagcmd && t--){
        SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
    }
    if(!flagcmd){
        UARTSend(BL_CMODE, strlen(BL_CMODE));
        t=1000;
        while(!flagcmd && t--){
           SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
        }
        if(!flagcmd){
            UARTprintf("Error Connecting Bluetooth");
        }
    }
    flagcmd = 0;

//    t=1000;
//        char* BL_UART = "AT+UART=38400£¬0£¬0";
//        UARTSend(BL_UART, strlen(BL_UART));
//        while(!flagcmd && t--){
//            SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
//        }
//        if(!flagcmd){
//            UARTSend(BL_UART, strlen(BL_UART));
//            t=1000;
//            while(!flagcmd && t--){
//               SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
//            }
//            if(!flagcmd){
//                UARTprintf("Error Connecting Bluetooth");
//            }
//        }
//        flagcmd = 0;

    t=10000;
    char* BL_BIND = "AT+BIND=2016,11,282119";
    UARTSend(BL_BIND, strlen(BL_BIND));
    while(!flagcmd && t--){
        SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
    }
    if(!flagcmd){
        UARTSend(BL_BIND, strlen(BL_BIND));
        t=5000;
        while(!flagcmd && t--){
           SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
        }
        if(!flagcmd){
            UARTprintf("Error Connecting Bluetooth");
        }
    }
    flagcmd = 0;




    int a = 0;
    while(1)
    {
        char bufferReceive[100];

        if (UARTgets(bufferReceive, 100)) {
            if(bufferReceive[0] == 'F'){
                //
                // Turn off the Bluetooth
                //
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0); // Key
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0); // VCC

                SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms

                //
                // Enter the Nomal Mode
                //
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_PIN_7); // VCC

                ROM_UARTConfigSetExpClk(UART7_BASE, g_ui32SysClock, 38400,
                                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                         UART_CONFIG_PAR_NONE));
                flagMode = 1;
            }
            else {
//                int var;
//                for (var = 0;  var < 100; var++) {
//                    if(bufferReceive[var] == '\0'){
//                        break;
//                    }
//                }
                //UARTprintf(bufferReceive);
                UARTSend(bufferReceive, strlen(bufferReceive));
                LEDWrite(CLP_D1, 1-a);
            }
        }
    }
}
