#include <stdio.h>

#include "pico/stdlib.h"
#include "rtc.h"

#include "sd_stdio.h"
#include "apricot-hd.h"

void wd_access(struct bus_addr *address, unsigned char byte, unsigned int isread);
void latch_access(struct bus_addr *address, unsigned char byte, unsigned int isread);
void buffer_access(struct bus_addr *address, unsigned char byte, unsigned int isread);

extern volatile unsigned char brdy, cmdrdy;

extern volatile LATCH latch;
extern volatile unsigned char cur_drive, wd_cur_drive;

extern volatile WDREG host_reg_precomp, host_reg_sect_cnt, host_reg_sect_num;
extern volatile WDREG host_reg_cyl_low, host_reg_cyl_high, host_reg_sdh;
extern volatile WDREG wd_reg_errflg, wd_reg_sect_cnt, wd_reg_sect_num;
extern volatile WDREG wd_reg_cyl_low, wd_reg_cyl_high, wd_reg_sdh;
extern volatile WDCMD host_cmd;
extern volatile WDSTAT wd_reg_status;

extern CONFIG *hd_conf;

extern FIL *fps[2];

#ifdef APRIDEBUG
extern char *cmdmsg[8];
#endif

void host_interface()
{
	int i;
	volatile int j;
	unsigned int is_host_read;
	unsigned char byte;
	struct bus_addr *address;

#ifdef APRIDEBUG
	printf("Core1: Starting host interface.");
#endif

	wd_reg_status.byte = 0;

	bus_mode_read();
	
	assert_iowait();

	while(1)
	{
		while(!is_selected()){}
/*
 * Add a delay to allow the system to put data onto the bus after
 * writing the address.
 */
		sleep_us(2);

		byte = read_bus();

#if APRIDEBUG > 3
		printf("Core1: Data on bus: %02x\n", byte);
#endif

		is_host_read = bus_is_read();

		address = accessed_address();

#if APRIDEBUG > 3
		printf("Core1: Bus is %s\n", (is_host_read) ? "host read" : "host write");
#endif

		if (is_host_read)
		{
			bus_mode_write();
		}
		else
		{

/*
 * We now have all the data we need. We can let the host go do other things.
 */

#if APRIDEBUG > 3
			printf("Core1: Finished processing host write: deasserting iowait.\n");
#endif

			deassert_iowait();

			while(is_selected()){}

			assert_iowait();
		
#if APRIDEBUG > 3
			printf("Core1: host_write: Deselected.\n");
#endif

		}

#if APRIDEBUG > 3
		printf("Core1: Address: High = %x, Low = %x\n", address->highbits, address->lowbits);
#endif

		switch (address->highbits)
		{
			case 0:
				wd_access(address, byte, is_host_read);
				break;
			case 2:
				latch_access(address, byte, is_host_read);
				break;
			case 3:
				buffer_access(address, byte, is_host_read);
		}

		if (is_host_read)
		{
#if APRIDEBUG > 3
			printf("Core1: Finished processing host read: deasserting iowait.\n");
#endif

			deassert_iowait();

			while(is_selected()){}

			assert_iowait();
		
#if APRIDEBUG > 3
			printf("Core1: host_read: Deselected.\n");
#endif

			bus_mode_read();
		}
	}
}

void wd_access(struct bus_addr *address, unsigned char byte, unsigned int is_host_read)
{
#if APRIDEBUG > 3
		printf("Core1: wd_access called: host %s, opcode: %d\n", (is_host_read) ? "read" : "write", address->lowbits);
#endif
	int opcode, drive;

	opcode = address->lowbits;

	if (is_host_read)
	{
		switch (opcode)
		{
			case 0:
#if APRIDEBUG > 2
				printf("Core1: wd_access: host read: Called with opcode zero!\n");
#endif
				buffer_access(address, byte, is_host_read);
				break;
			case 1:
#if APRIDEBUG > 2
				printf("Core1: wd_access: read error flgs\n");
#endif
				write_bus(wd_reg_errflg.byte);
				break;
			case 2:
#if APRIDEBUG > 2
				printf("Core1: wd_access: read sector count\n");
#endif
				write_bus(wd_reg_sect_cnt.byte);
				break;
			case 3:
#if APRIDEBUG > 2
				printf("Core1: wd_access: read sector number\n");
#endif
				write_bus(wd_reg_sect_num.byte);
				break;
			case 4:
#if APRIDEBUG > 2
				printf("Core1: wd_access: read cyl low byte\n");
#endif
				write_bus(wd_reg_cyl_low.byte);
				break;
			case 5:
#if APRIDEBUG > 2
				printf("Core1: wd_access: read cyl high byte\n");
#endif
				write_bus(wd_reg_cyl_high.byte);
				break;
			case 6:
#if APRIDEBUG > 2
				printf("Core1: wd_access: read sdh\n");
#endif
				write_bus(wd_reg_sdh.byte);
				break;
			case 7:
				if ((DRV_EXISTS(cur_drive - 1) == 0) || (cur_drive == 0))
					wd_reg_status.status.rdy = 0;
				else
					wd_reg_status.status.rdy = 1;

#if APRIDEBUG > 2
				printf("Core1: wd_access: read status.\n");
				debug_print_status(wd_reg_status);
#endif
				write_bus(wd_reg_status.byte);

//				wd_reg_status.status.sc = 0;
				
				deassert_int2();
		}
	}
	else
	{
		switch (opcode)
		{
			case 0:
				printf("Core1: wd_access: host write: Called with opcode zero!\n");
				buffer_access(address, byte, is_host_read);
				break;
			case 1:
#if APRIDEBUG > 3
				printf("Core1: wd_access: write precomp (%08b)\n", byte);
#endif
				host_reg_precomp.byte = byte;
				break;
			case 2:
#if APRIDEBUG > 3
				printf("Core1: wd_access: write sector count (%08b)\n", byte);
#endif
				host_reg_sect_cnt.byte = byte;
				break;
			case 3:
#if APRIDEBUG > 3
				printf("Core1: wd_access: write sector number (%08b)\n", byte);
#endif
				host_reg_sect_num.byte = byte;
				break;
			case 4:
#if APRIDEBUG > 3
				printf("Core1: wd_access: write cyl low byte (%08b)\n", byte);
#endif
				host_reg_cyl_low.byte = byte;
				break;
			case 5:
#if APRIDEBUG > 3
				printf("Core1: wd_access: write cyl high byte (%08b)\n", byte);
#endif
				host_reg_cyl_high.byte = byte;
				break;
			case 6:
#if APRIDEBUG > 3
				printf("Core1: wd_access: write sdh (%08b)\n", byte);
#endif
				host_reg_sdh.byte = byte;
				wd_reg_sdh.byte = byte;
				drive = (byte >> 3) & 0x3;
				if (drive != wd_cur_drive)
				{
					wd_cur_drive = drive;
					wd_reg_sect_cnt.byte = 0;
					wd_reg_sect_num.byte = 0;
					wd_reg_cyl_low.byte = 0;
					wd_reg_cyl_high.byte = 0;
				}
				break;
			case 7:
				host_cmd.byte = byte;
				wd_reg_status.status.cip = 1;
				wd_reg_status.status.bsy = 1;
				wd_reg_status.status.sc = 0;
				cmdrdy = 1;
#if APRIDEBUG > 1
				printf("Core1: wd_access: Command: %s (%08b)\n", cmdmsg[host_cmd.command.cmd], host_cmd.byte);
#endif
		}
	}

}

void latch_access(struct bus_addr *address, unsigned char byte, unsigned int is_host_read)
{
	unsigned char bit, temp;

#if APRIDEBUG > 3
		printf("Core1: latch_access: address low bits = %x\n", address->lowbits);
#endif

	bit = byte & 0x01;

	if (is_host_read)
	{
#if APRIDEBUG > 3
		printf("Core1: latch_access: Trying buffer read instead.\n");
#endif
		buffer_access(address, byte, is_host_read);
		return;
	}

	switch (address->lowbits)
	{
		case 0:
#if APRIDEBUG > 1
			printf("Core1: latch_access: setting head bit 0 to %d\n", bit);
#endif
			latch.bits.heads.bits.zero = bit;
			break;
		case 1:
#if APRIDEBUG > 1
			printf("Core1: latch_access: setting head bit 1 to %d\n", bit);
#endif
			latch.bits.heads.bits.one = bit;
			break;
		case 2:
#if APRIDEBUG > 1
			printf("Core1: latch_access: setting head bit 2 to %d\n", bit);
#endif
			latch.bits.heads.bits.two = bit;
			break;
		case 3:
#if APRIDEBUG > 1
			printf("Core1: latch_access: Drive 1 select = %d\n", bit);
#endif
			latch.bits.drvsel1 = bit;
			temp = cur_drive & 0xfe;
			cur_drive = temp | bit;
			break;
		case 4:
#if APRIDEBUG > 1
			printf("Core1: latch_access: Data ready = %d\n", bit);
#endif
			latch.bits.data_rdy = bit;
			brdy = bit;
			break;
		case 5:
#if APRIDEBUG > 1
			printf("Core1: latch_access: Reset buffer counter = %d\n", bit);
#endif
			latch.bits.buf_reset = bit;
			if (bit != 0)
				buf_reset();
			break;
		case 6:
#if APRIDEBUG > 1
			printf("Core1: latch_access: Buffer master %s\n", (bit) ? "controller" : "host");
#endif
			latch.bits.buf_master = bit;
			break;
		case 7:
#if APRIDEBUG > 1
			printf("Core1: latch_access: Drive 2 select = %d\n", bit);
#endif
			latch.bits.drvsel2 = bit;
			temp = cur_drive & 0xfd;
			cur_drive = temp | (bit << 1);
			break;
	}
}

void buffer_access(struct bus_addr *address, unsigned char byte, unsigned int is_host_read)
{
	if (is_host_read)
	{
		buf_read_byte(&byte);
#if APRIDEBUG > 3
		printf("Core1: buffer_access: buffer -> host = %02x\n", byte);
#endif
		write_bus(byte);
	}
	else
	{
		buf_write_byte(&byte);
#if APRIDEBUG > 3
		printf("Core1: buffer_access: host -> buffer = %02x\n", byte);
#endif
	}
}

