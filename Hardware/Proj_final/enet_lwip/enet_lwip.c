//*****************************************************************************
// IoT Gateway
// Author : CHEN Muyao
// 06/06/2018
//*****************************************************************************

// Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

// Hardware Libraries
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

// Driver Libraries
#include "driverlib/flash.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "driverlib/eeprom.h"
#include "driverlib/flash.h"
#include "drivers/pinout.h"

// Libraries used by LwIP
#include "utils/locator.h"
#include "utils/lwiplib.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"

// TCP Serever Library
#include "tcp_server.h"



//*****************************************************************************
//
// Define for maximum acceptable bluetooth devices
//
//*****************************************************************************

#define MAX_BLUETOOTH_DEVICE    3

//*****************************************************************************
//
// Defines for setting up the system clock.
//
//*****************************************************************************
#define SYSTICKHZ               100
#define SYSTICKMS               (1000 / SYSTICKHZ)

//*****************************************************************************
//
// Interrupt priority definitions.  The top 3 bits of these values are
// significant with lower values indicating higher priority interrupts.
//
//*****************************************************************************
#define SYSTICK_INT_PRIORITY    0x80    // 1000 0000
#define ETHERNET_INT_PRIORITY   0xC0    // 1100 0000

//*****************************************************************************
//
// The current IP address.
//
//*****************************************************************************
uint32_t g_ui32IPAddress;

//*****************************************************************************
//
// The system clock frequency.
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


//*****************************************************************************
//
// Declarartion of variables
//
//*****************************************************************************

//
// ID of the Bluetooth device being connected
//
int BLInService = 0;

//
// Flag distinguishing the AT Mode(0) and the Transmission Mode(1)
//
int flagMode = 0;

//
// Flag checking the received command OK
//
int flagcmd = 0;

//
// Current eeprom address
//
uint32_t eepromAddr = 0;

//
// Buffer for incoming bluetooth command
//
char BL_cmd_receive[100];

//
// Bluetooth device structure
//
struct Bluetooth{
    int id;
    char addr[20];
};

//
// List of Bluetooth devices
//
struct Bluetooth Bluetooth_list[MAX_BLUETOOTH_DEVICE];


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
        // Read the next character from the UART and write it into Bluetooth receive buffer.
        //
        bufferBLReceive[var]=ROM_UARTCharGet(UART7_BASE);
        ROM_UARTCharPut(UART0_BASE,bufferBLReceive[var]);
        var++;
    }

    //
    // Processing the response from Bluetooth Device
    //

    //
    // if Bluetooth is in Transmission Mode
    //
    if (flagMode == 1) {

        //
        // transfer the buffer into a string type
        //
        bufferBLReceive[var] = '\0';

        //
        // transfer the received string into integer
        //
        unsigned char *p = (void *)bufferBLReceive;
        uint32_t data = p[0] + 256U*p[1] + 65536U*p[2] + 16777216U*p[3];

        //
        // Save the received data in the EEPROM
        //
        EEPROMRead(&eepromAddr,0,4);    // Find the current eeprom address
        if(eepromAddr == 4294967295) {  // If overflow, begin from the 0 address
            eepromAddr = 0;
        }
        EEPROMProgram(&BLInService,eepromAddr+4,4); // Save the current device ID being interrogated
        eepromAddr+=4;
        EEPROMProgram(&data,eepromAddr+4,4);        // Save the Integer into the next address
        eepromAddr+=4;
        EEPROMProgram(&eepromAddr, 0, 4);           // Update the current address

        //
        // Display the data information on the screen (for debugging)
        //
        uint32_t temp;
        EEPROMRead(&temp,eepromAddr,4);
        UARTprintf("%d ",temp);

        //
        // if Bluetooth is in AT Mode (Configuring the bluetooth)
        //
    } else {
        bufferBLReceive[var] = '\0';
        if(strstr(bufferBLReceive,"OK")){
            flagcmd = 1;
            UARTprintf("Command sending success!\n");
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

//*****************************************************************************
//
// Configure the UART for Bluetooth module
//
//*****************************************************************************
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
// Display an lwIP type IP Address.
//
//*****************************************************************************
void
DisplayIPAddress(uint32_t ui32Addr)
{
    char pcBuf[16];

    //
    // Convert the IP Address into a string.
    //
    usprintf(pcBuf, "%d.%d.%d.%d", ui32Addr & 0xff, (ui32Addr >> 8) & 0xff,
            (ui32Addr >> 16) & 0xff, (ui32Addr >> 24) & 0xff);

    //
    // Display the string.
    //
    UARTprintf(pcBuf);
}

//*****************************************************************************
//
// Required by lwIP library to support any host-related timer functions.
//
//*****************************************************************************
void
lwIPHostTimerHandler(void)
{
    uint32_t ui32Idx, ui32NewIPAddress;

    //
    // Get the current IP address.
    //
    ui32NewIPAddress = lwIPLocalIPAddrGet();

    //
    // See if the IP address has changed.
    //
    if(ui32NewIPAddress != g_ui32IPAddress)
    {
        //
        // See if there is an IP address assigned.
        //
        if(ui32NewIPAddress == 0xffffffff)
        {
            //
            // Indicate that there is no link.
            //
            UARTprintf("Waiting for link.\n");
        }
        else if(ui32NewIPAddress == 0)
        {
            //
            // There is no IP address, so indicate that the DHCP process is
            // running.
            //
            UARTprintf("Waiting for IP address.\n");
        }
        else
        {
            //
            // Display the new IP address.
            //
            UARTprintf("IP Address: ");
            DisplayIPAddress(ui32NewIPAddress);
            UARTprintf("\nWaiting for connection...\n");
        }

        //
        // Save the new IP address.
        //
        g_ui32IPAddress = ui32NewIPAddress;

        //
        // Turn GPIO off.
        //
        MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, ~GPIO_PIN_1);
    }

    //
    // If there is not an IP address.
    //
    if((ui32NewIPAddress == 0) || (ui32NewIPAddress == 0xffffffff))
    {
        //
        // Loop through the LED animation.
        //

        for(ui32Idx = 1; ui32Idx < 17; ui32Idx++)
        {

            //
            // Toggle the GPIO
            //
            MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1,
                    (MAP_GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1) ^
                     GPIO_PIN_1));

            SysCtlDelay(g_ui32SysClock/(ui32Idx << 1));
        }
    }
}

//*****************************************************************************
//
// The interrupt handler for the SysTick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Call the lwIP timer handler.
    //
    lwIPTimer(SYSTICKMS);
}


//*****************************************************************************
//
// Add Bluetooth device into the device list
//
//*****************************************************************************
void
addBluetooth(int id,char* addr,int size){
    strncpy(Bluetooth_list[id-1].addr,addr,size);
    Bluetooth_list[id-1].id = id;
}

//*****************************************************************************
//
// Configure the Bluetooth module to connect with different bluetooth devices
// If configuration succeeds return 1
//
//*****************************************************************************
int
consultBluetooth(int id){

    //
    // Turn off the Bluetooth
    //
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0); // Key
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0); // VCC

    SysCtlDelay(g_ui32SysClock / (100 * 3)); // delay 10ms

    flagMode = 0;

    //
    // Enter the AT Mode
    //
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_PIN_6);
    SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_PIN_7); // VCC

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
            UARTprintf("Error Sending AT");
            return NULL;
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
            UARTprintf("Error Configuring AT ROLE");
            return NULL;
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
            UARTprintf("Error Configuring AT CMODE");
            return NULL;
        }
    }
    flagcmd = 0;

    t=1000;
    char* BL_RMAAD = "AT+RMAAD";
    UARTSend(BL_RMAAD, strlen(BL_RMAAD));
    while(!flagcmd && t--){
        SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
    }
    if(!flagcmd){
        UARTSend(BL_RMAAD, strlen(BL_RMAAD));
        t=1000;
        while(!flagcmd && t--){
           SysCtlDelay(g_ui32SysClock / (1000 * 3)); // delay 1ms
        }
        if(!flagcmd){
            UARTprintf("Error Configuring RMAAD");
            return NULL;
        }
    }
    flagcmd = 0;

    t=10000;
    char BL_BIND[25];
    memcpy(BL_BIND, "AT+BIND=", 9*sizeof(char));
    strcat(BL_BIND, Bluetooth_list[id-1].addr);
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
            UARTprintf("Error Configuring AT BIND");
            return NULL;
        }
    }
    flagcmd = 0;
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
    BLInService = id;
    return 1;
}

//*****************************************************************************
//
// Send the request message ("1") to the Bluetooth device so that
// the target device would send back the data
// Received data will be received in the BTIntHandler fonction
// Here we wait for several time and check if we have received the data (return 1 or O)
//
//*****************************************************************************
int RequestBluetooth(){
    uint32_t AddrToRead = 0;
    EEPROMRead(&AddrToRead,0,4);
    UARTSend("1",2);
    UARTprintf("Message Requiring Data Sent\n");
    int t = 1000;
    while(t--) SysCtlDelay(g_ui32SysClock / (1000 * 3));
    uint32_t NewAddrToRead = 0;
    EEPROMRead(&NewAddrToRead,0,4);
    if(AddrToRead == NewAddrToRead) return NULL;
    return 1;
}

//*****************************************************************************
//
// Main fonction
//
//*****************************************************************************
int
main(void)
{
    uint32_t ui32User0, ui32User1;
    uint8_t pui8MACArray[8];

    //
    // Make sure the main oscillator is enabled because this is required by
    // the PHY.  The system must have a 25MHz crystal attached to the OSC
    // pins. The SYSCTL_MOSC_HIGHFREQ parameter is used when the crystal
    // frequency is 10MHz or higher.
    //
    SysCtlMOSCConfigSet(SYSCTL_MOSC_HIGHFREQ);

    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);
    //
    // Configure the device pins.
    //
    PinoutSet(true, false);

    //
    // Configure UART.
    //
    UARTStdioConfig(0, 115200, g_ui32SysClock);

    //
    // Clear the terminal and print banner.
    //
    UARTprintf("\033[2J\033[H");
    UARTprintf("IoT Gateway Rules!\n\n");


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

    ConfigureUART();
    ConfigureUARTBluetooth();

    //
    // Initialize eeprom
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    EEPROMInit();
    EEPROMRead(&eepromAddr,0,4);
    if(eepromAddr == 4294967295)
    {
        uint32_t zero = 0;
        EEPROMProgram(&zero, 0, 4);
        EEPROMRead(&eepromAddr,0,4);
    }
    UARTprintf("%d\n",eepromAddr);

/***********************************************************************************/

    //
    // Configure Port N1 for as an output for the animation LED.
    //
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    //
    // Initialize LED to OFF (0)
    //
    MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, ~GPIO_PIN_1);

    //
    // Configure SysTick for a periodic interrupt.
    //
    MAP_SysTickPeriodSet(g_ui32SysClock / SYSTICKHZ);
    MAP_SysTickEnable();
    MAP_SysTickIntEnable();

    //
    // Configure the hardware MAC address for Ethernet Controller filtering of
    // incoming packets.  The MAC address will be stored in the non-volatile
    // USER0 and USER1 registers.
    //
    MAP_FlashUserGet(&ui32User0, &ui32User1);
    if((ui32User0 == 0xffffffff) || (ui32User1 == 0xffffffff))
    {
        //
        // We should never get here.  This is an error if the MAC address has
        // not been programmed into the device.  Exit the program.
        // Let the user know there is no MAC address
        //
        UARTprintf("No MAC programmed!\n");
        while(1)
        {
        }
    }

    //
    // Tell the user what we are doing just now.
    //
    UARTprintf("Waiting for IP.\n");

    //
    // Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
    // address needed to program the hardware registers, then program the MAC
    // address into the Ethernet Controller registers.
    //
    pui8MACArray[0] = ((ui32User0 >>  0) & 0xff);
    pui8MACArray[1] = ((ui32User0 >>  8) & 0xff);
    pui8MACArray[2] = ((ui32User0 >> 16) & 0xff);
    pui8MACArray[3] = ((ui32User1 >>  0) & 0xff);
    pui8MACArray[4] = ((ui32User1 >>  8) & 0xff);
    pui8MACArray[5] = ((ui32User1 >> 16) & 0xff);

    //
    // Initialize the lwIP library, using DHCP.
    //
    lwIPInit(g_ui32SysClock, pui8MACArray, 0, 0, 0, IPADDR_USE_DHCP);
    //
    // Setup the device locator service.
    //
    LocatorInit();
    LocatorMACAddrSet(pui8MACArray);
    LocatorAppTitleSet("EK-TM4C1294XL enet_io");


    //
    // Initialize a tcp server
    //
    tcp_server_test();



    //
    // Set the interrupt priorities.  We set the SysTick interrupt to a higher
    // priority than the Ethernet interrupt to ensure that the file system
    // tick is processed if SysTick occurs while the Ethernet handler is being
    // processed.  This is very likely since all the TCP/IP and HTTP work is
    // done in the context of the Ethernet interrupt.
    //
    MAP_IntPrioritySet(INT_EMAC0, ETHERNET_INT_PRIORITY);
    MAP_IntPrioritySet(FAULT_SYSTICK, SYSTICK_INT_PRIORITY);

    //
    // Loop forever.  All the work is done in interrupt handlers.
    //

    addBluetooth(1, "2016,11,282119",14);
    addBluetooth(2, "2016,11,295802",14);


    while(1)
    {

        if(consultBluetooth(1)){
            int delay_t = 30;
            while(delay_t--){
                            SysCtlDelay(g_ui32SysClock / 3); // delay 1s
                            UARTprintf(".");
                        }
            if(!RequestBluetooth()) UARTprintf("\n No Response from Bluetooth 1 \n");
        }


        if(consultBluetooth(2)){
            int delay_t = 30;
            while(delay_t--){
                            SysCtlDelay(g_ui32SysClock / 3); // delay 1s
                            UARTprintf(".");
                        }
            if(!RequestBluetooth()) UARTprintf("\n No Response from Bluetooth 2\n");
        }

    }
}
