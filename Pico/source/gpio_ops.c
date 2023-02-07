#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/rtc.h"
#include "hardware/adc.h"

#include "rtc.h"

#include "sd_stdio.h"
#include "apricot-hd.h"
#include "pin_defs.h"

#ifdef APRIDEBUG
char *pin_names[] = {
	"D0",
	"D1",
	"D2",
	"D3",
	"D4",
	"D5",
	"D6",
	"D7",
	"GPIO8",
	"GPIO9",
	"SPI_CLK",
	"SPI_MOSI",
	"SPI_MISO",
	"SPI_CS",
	"SELECTED",
	"RW",
	"GPIO16",
	"GPIO17",
	"A1",
	"A2",
	"A3",
	"A4",
	"A8",
	"GPIO26",
	"INT2",
	"IOWAIT"
};
#endif

uint32_t busdata;

unsigned char pin_value(unsigned char pin)
{
	uint32_t mask;
	unsigned char retval;

	mask = 0x01 << pin;
	retval = ((busdata & mask) ? 1 : 0);
#if APRIDEBUG > 3
	printf("GPIO: pin_value: Pin %d (%s): Bus: %08x Mask: %08x Value: %1d\n", pin, pin_names[pin], busdata, mask, retval);
#endif

	return retval;
}

void bus_mode_write()
{
	gpio_set_dir_masked(BUS_GPIO_MASK, 0xFF);
#if APRIDEBUG > 3
	printf("GPIO: bus_mode_write: Data bus direction is now: %s\n", (gpio_get_dir(PIN_D0)) ? "out" : "in");
#endif
}

void bus_mode_read()
{
	gpio_set_dir_masked(BUS_GPIO_MASK, 0x00);
#if APRIDEBUG > 3
	printf("GPIO: bus_mode_read: Data bus direction is now: %s\n", (gpio_get_dir(PIN_D0)) ? "out" : "in");
#endif
}

void init_bus()
{
	enum gpio_function func;
	enum gpio_drive_strength strength;

	func = GPIO_FUNC_NULL;
	strength = GPIO_DRIVE_STRENGTH_12MA;
/*
 * Data bus.
 */
 
	gpio_init(PIN_D0);
	gpio_init(PIN_D1);
	gpio_init(PIN_D2);
	gpio_init(PIN_D3);
	gpio_init(PIN_D4);
	gpio_init(PIN_D5);
	gpio_init(PIN_D6);
	gpio_init(PIN_D7);

	gpio_set_dir(PIN_D0, GPIO_IN);
	gpio_pull_up(PIN_D0);

	gpio_set_dir(PIN_D1, GPIO_IN);
	gpio_pull_up(PIN_D1);

	gpio_set_dir(PIN_D2, GPIO_IN);
	gpio_pull_up(PIN_D2);

	gpio_set_dir(PIN_D3, GPIO_IN);
	gpio_pull_up(PIN_D3);

	gpio_set_dir(PIN_D4, GPIO_IN);
	gpio_pull_up(PIN_D4);

	gpio_set_dir(PIN_D5, GPIO_IN);
	gpio_pull_up(PIN_D5);

	gpio_set_dir(PIN_D6, GPIO_IN);
	gpio_pull_up(PIN_D6);

	gpio_set_dir(PIN_D7, GPIO_IN);
	gpio_pull_up(PIN_D7);
/*
 * Address bus.
 */

	gpio_init(PIN_A1); gpio_set_dir(PIN_A1, GPIO_IN); gpio_pull_up(PIN_A1);
	gpio_init(PIN_A2); gpio_set_dir(PIN_A2, GPIO_IN); gpio_pull_up(PIN_A2);
	gpio_init(PIN_A3); gpio_set_dir(PIN_A3, GPIO_IN); gpio_pull_up(PIN_A3);
	gpio_init(PIN_A4); gpio_set_dir(PIN_A4, GPIO_IN); gpio_pull_up(PIN_A4);
	gpio_init(PIN_A8); gpio_set_dir(PIN_A8, GPIO_IN); gpio_pull_up(PIN_A8);

/*
 * Control lines (input)
 */
	gpio_init(PIN_SELECTED); gpio_set_dir(PIN_SELECTED, GPIO_IN); gpio_pull_up(PIN_SELECTED);
	gpio_init(PIN_READ_WRITE); gpio_set_dir(PIN_READ_WRITE, GPIO_IN); gpio_pull_up(PIN_READ_WRITE);

/*
 * Control lines (output)
 */
	gpio_init(PIN_IOWAIT); gpio_set_dir(PIN_IOWAIT, GPIO_OUT);
	gpio_init(PIN_INT2); gpio_set_dir(PIN_INT2, GPIO_OUT);

	gpio_pull_up(PIN_IOWAIT);
	gpio_pull_up(PIN_INT2);

	gpio_init(PIN_LED);
	gpio_set_dir(PIN_LED, GPIO_OUT);
}

unsigned char data_pins[] = {
	PIN_D0,
	PIN_D1,
	PIN_D2,
	PIN_D3,
	PIN_D4,
	PIN_D5,
	PIN_D6,
	PIN_D7
};

unsigned char read_bus()
{
	unsigned char byte;

	busdata = gpio_get_all();

	byte = busdata & 0xff;

#if APRIDEBUG > 3
	printf("GPIO: read_bus: Bus data = %08x Byte = %02x\n", busdata, byte);
#endif

	return byte;
}

void write_bus(unsigned char byte)
{
	unsigned char bytevals[8];
	int i;
	volatile int j;

#if APRIDEBUG > 3
	printf("gpio: write_bus: Byte = ");
#endif

	for (i = 0; i < 8; i++)
	{
		bytevals[i] = ((byte >> i) & 0x01);
#if APRIDEBUG > 3
		printf("%1d", bytevals[i]);
#endif
		gpio_put(data_pins[i], bytevals[i]);
	}

/*
 * Add a delay to allow the data to propergate onto the bus through
 * the 74 series logic.
 */

	sleep_us(2);

#if APRIDEBUG > 3
	printf("\n");

#if APRIDEBUG > 3
	printf("Pico thinks it has output: ");

	for (i = 0; i < 8; i++)
	{
		printf("%1d", gpio_get_out_level(data_pins[i]));
	}
	printf("\n");
#endif
#endif
}

unsigned int bus_is_read()
{
	return ((gpio_get(PIN_READ_WRITE)) ? 0 : 1 );
}

struct bus_addr *accessed_address()
{
	static struct bus_addr addr;

	addr.highbits = (gpio_get(PIN_A8) << 1) + gpio_get(PIN_A4);
	addr.lowbits = (gpio_get(PIN_A3) << 2) + (gpio_get(PIN_A2) << 1) + gpio_get(PIN_A1);

	return &addr;
}

unsigned int is_selected()
{
	return ((gpio_get(PIN_SELECTED)) ? 0 : 1);
}

void assert_iowait()
{
	gpio_put(PIN_IOWAIT, false);
#if APRIDEBUG > 3
	printf("GPIO: assert_iowait: drive strength = %d, slew rate = %d, direction: %s out level: %s\n", gpio_get_drive_strength(PIN_IOWAIT), gpio_get_slew_rate(PIN_IOWAIT), (gpio_get_dir(PIN_IOWAIT)) ? "out" : "in", (gpio_get_out_level(PIN_IOWAIT)) ? "high" : "low");
#endif
}

void deassert_iowait()
{
	gpio_put(PIN_IOWAIT, true);
#if APRIDEBUG > 3
	printf("GPIO: deassert_iowait: drive strength = %d, slew rate = %d, direction: %s out level: %s\n", gpio_get_drive_strength(PIN_IOWAIT), gpio_get_slew_rate(PIN_IOWAIT), (gpio_get_dir(PIN_IOWAIT)) ? "out" : "in", (gpio_get_out_level(PIN_IOWAIT)) ? "high" : "low");
#endif
}

void assert_int2()
{
	gpio_put(PIN_INT2, 0);
}

void deassert_int2()
{
	gpio_put(PIN_INT2, 1);
}

void blink_led_short()
{
	gpio_put(PIN_LED, 1);

	sleep_ms(100);

	gpio_put(PIN_LED, 0);
}

void blink_led_long()
{
	gpio_put(PIN_LED, 1);

	sleep_ms(500);

	gpio_put(PIN_LED, 0);
}
