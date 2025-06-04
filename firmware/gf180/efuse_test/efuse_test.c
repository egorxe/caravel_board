/*
 *  Simple test for the eFuse block in Ophelia FPGA OpenMPW chip.
 * 
 *  At first it reads all eFuse addresses and prints any nonzero values,
 *  afterwards it attempts to write a counter value to all eFuse bytes 
 *  and read it back printing nonzero values again.
 * 
 */ 

#include <defs.h>

// User Wishbone address eFuse block is mapped to
#define USER_WB_ADDR            0x30000000
#define OFF_EFUSE_ADDR          0x30000

// User wishbone access helpers
#define write_user_reg(off, val) {(*(volatile uint32_t*)(USER_WB_ADDR + (off))) = (val);}
#define write_user_byte(off, val) {(*(volatile uint8_t*)(USER_WB_ADDR + (off))) = (val);}
#define read_user_reg(off) (*(volatile uint32_t*)(USER_WB_ADDR + (off)))
#define read_user_byte(off) (*(volatile uint8_t*)(USER_WB_ADDR + (off)))


// --------------------------------------------------------
// IO routines
// --------------------------------------------------------

void configure_io()
{

//  ======= Useful GPIO mode values =============

//      GPIO_MODE_MGMT_STD_INPUT_NOPULL
//      GPIO_MODE_MGMT_STD_INPUT_PULLDOWN
//      GPIO_MODE_MGMT_STD_INPUT_PULLUP
//      GPIO_MODE_MGMT_STD_OUTPUT
//      GPIO_MODE_MGMT_STD_BIDIRECTIONAL
//      GPIO_MODE_MGMT_STD_ANALOG

//      GPIO_MODE_USER_STD_INPUT_NOPULL
//      GPIO_MODE_USER_STD_INPUT_PULLDOWN
//      GPIO_MODE_USER_STD_INPUT_PULLUP
//      GPIO_MODE_USER_STD_OUTPUT
//      GPIO_MODE_USER_STD_BIDIRECTIONAL
//      GPIO_MODE_USER_STD_ANALOG


//  ======= set each IO to the desired configuration =============

    //  GPIO 0 is turned off to prevent toggling the debug pin; For debug, make this an output and
    //  drive it externally to ground.

    reg_mprj_io_0 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;

    // Changing configuration for IO[1-4] will interfere with programming flash. if you change them,
    // You may need to hold reset while powering up the board and initiating flash to keep the process
    // configuring these IO from their default values.

    reg_mprj_io_1 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_2 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;
    reg_mprj_io_3 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;
    reg_mprj_io_4 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;

    // -------------------------------------------

    reg_mprj_io_5 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;     // UART Rx
    reg_mprj_io_6 = GPIO_MODE_MGMT_STD_OUTPUT;           // UART Tx
    reg_mprj_io_7 = GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_8 = GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_9 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_10 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_11 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_12 = GPIO_MODE_USER_STD_INPUT_PULLUP;
    reg_mprj_io_13 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_14 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_15 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_16 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_17 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_18 = GPIO_MODE_MGMT_STD_OUTPUT;

    reg_mprj_io_19 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_20 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_21 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_22 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_23 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_24 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_25 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_26 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_27 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_28 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_29 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_30 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_31 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_32 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_33 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_34 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_35 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_36 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_37 = GPIO_MODE_MGMT_STD_OUTPUT;

    // Initiate the serial transfer to configure IO
    reg_mprj_xfer = 1;
    while (reg_mprj_xfer == 1);
}

void putchar(char c)
{
    if (c == '\n')
        putchar('\r');
    while (reg_uart_txfull == 1);
        reg_uart_data = c;
}

void print_hex(uint32_t v, int digits)
{
	for (int i = digits - 1; i >= 0; i--) 
    {
		char c = "0123456789abcdef"[(v >> (4*i)) & 15];
		putchar(c);
	}
}

void print(const char *p)
{
    while (*p)
        putchar(*(p++));
}

// Sleep for some time
void delay(const int d)
{

    // Configure timer for a single-shot countdown 
	reg_timer0_config = 0;
	reg_timer0_data = d;
    reg_timer0_config = 1;

    // Loop, waiting for value to reach zero
    reg_timer0_update = 1;  // latch current value
    while (reg_timer0_value > 0) 
        reg_timer0_update = 1;

}

// --------------------------------------------------------
// eFuse test routines
// --------------------------------------------------------

void read_efuse(int size)
{
    print("Reading efuse, printing non default values...\n");
    for (int i = 0; i < size; i+=4) 
    {
        // read eFuse via memory mapped Wishbone addresses
        uint8_t data = read_user_byte(OFF_EFUSE_ADDR + i);
        if (data)
        {
            print_hex(i, 8);
            print(" : ");
            print_hex(data, 2);
            print("\n");
        }
    }
    print("Efuse read finished\n");
}

void write_efuse(int size)
{
    print("Writing counter to efuse...\n");
    for (int i = 0; i < size; i+=4) 
    {
        // write eFuse via memory mapped Wishbone addresses
        write_user_reg(OFF_EFUSE_ADDR + i, (i>>2) & 0xFF);
    }
    print("Efuse write finished\n");
}

// --------------------------------------------------------
// Main
// --------------------------------------------------------

void main()
{
	int i, j, k;

    // Configure IO pads & enable GPIO, UART and user Wishbone
    reg_gpio_mode1 = 1;
    reg_gpio_mode0 = 0;
    reg_gpio_ien = 1;
    reg_gpio_oeb = 0;

    configure_io();

    reg_uart_enable = 1;
    reg_wb_enable = 1;

    // Read, write and read eFuse again 
    print("Efuse test started\n");
    
    read_efuse(0x2400);
    write_efuse(0x2400);
    read_efuse(0x2400);
       
    print("\nTest finished\n");

    // Just blink GPIO LED after finish
	while (1) 
    {
        reg_gpio_out = 1; 

		delay(2000000);

        reg_gpio_out = 0; 

		delay(2000000);
    }
}

