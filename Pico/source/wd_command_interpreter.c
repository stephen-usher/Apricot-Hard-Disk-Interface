#include <stdio.h>

#include "pico/stdlib.h"
#include "rtc.h"

#include "sd_stdio.h"
#include "apricot-hd.h"

#if APRIDEBUG > 1
char *cmdmsg[] = {
	"",
	"Restore",
	"Read",
	"Write",
	"ScanID",
	"Format",
	"",
	"Seek"
};
#endif

void wd_restore();
void wd_read();
void wd_write();
void wd_scanid();
void wd_format();
void wd_seek();

extern volatile unsigned char brdy, cmdrdy;

extern volatile LATCH latch;
extern volatile unsigned char cur_drive;

extern volatile WDREG host_reg_precomp, host_reg_sect_cnt, host_reg_sect_num;
extern volatile WDREG host_reg_cyl_low, host_reg_cyl_high, host_reg_sdh;
extern volatile WDREG wd_reg_errflg, wd_reg_sect_cnt, wd_reg_sect_num;
extern volatile WDREG wd_reg_cyl_low, wd_reg_cyl_high, wd_reg_sdh;
extern volatile WDCMD host_cmd;
extern volatile WDSTAT wd_reg_status;

extern CONFIG *hd_conf;

int drv_sects[] = { 16, 16, 16 };
int drv_heads[] = { 2, 4, 4 };
int drv_cyls[] = { 306, 306, 664 };

int sectsiztab[] = {
	256,
	512,
	1024,
	128
};

int sectorsiz;

/*#define SECTORSIZ 512*/
#define SECTORSIZ sectsiztab[((wd_reg_sdh.byte & 0x70) >> 5)]
#define MAXSECTORSIZ 1024

void wd_command_interpreter()
{
	int i;

#ifdef APRIDEBUG
	printf("Core0: Entering command interpreter.\n");
#endif

	while(1)
	{
		while(cmdrdy == 0){}

		cmdrdy = 0;
		
#if APRIDEBUG > 1
		printf("Core0: got command %s\n", cmdmsg[host_cmd.command.cmd]);
#endif
		switch(host_cmd.command.cmd)
		{
			case WDCMD_RESTORE:
				wd_restore();
				break;
			case WDCMD_READ:
				wd_read();
				break;
			case WDCMD_WRITE:
				wd_write();
				break;
			case WDCMD_SCANID:
				wd_scanid();
				break;
			case WDCMD_FORMAT:
				wd_format();
				break;
			case WDCMD_SEEK:
				wd_seek();
				break;
			default:
#if APRIDEBUG > 2
				printf("Core0: Command dispatch: unknown command.\n");
#endif
				break;
		}
#if APRIDEBUG > 2
		printf("Core0: Command end.\n");
#endif
	}
}

FRESULT spinup(int x)
{
	if (DRV_SPUNUP(x))
	{
#if APRIDEBUG > 2
		printf("Core0: spinup: Drive %d already spun up.\n", x);
#endif
		return FR_OK;
	}
	else
	{
#if APRIDEBUG > 2
		printf("Core0: spinup: Spinning up drive %d.\n", x);
#endif
		DRV_SPUNUP(x) = 1;
		return f_open(DRV_FP(x), DRV_FILE(x), FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	}
}

FRESULT spindown(int x)
{
	if (DRV_SPUNUP(x))
	{
#if APRIDEBUG > 2
		printf("Core0: spindown: Spinning down drive %d.\n", x);
#endif
		DRV_SPUNUP(x) = 0;
		return f_close(DRV_FP(x));
	}
	else
	{
#if APRIDEBUG > 2
		printf("Core0: spinup: Drive %d already spun down.\n", x);
#endif
		return FR_OK;
	}
}

FRESULT hdimg_seek(int drv, unsigned int head, unsigned int cylinder, unsigned int sector)
{
	unsigned long location;
	unsigned int track_length, cyl_size;
	unsigned int drvsects, drvcyls, drvheads;
	FRESULT retval;

	if ((drv >= hd_conf->numdrvs) || (drv == -1))
		return FR_NO_FILE;

	retval = spinup(drv);

	if (retval != FR_OK)
		return retval;

	drvsects = drv_sects[DRV_TYPE(drv)];
	drvcyls = drv_cyls[DRV_TYPE(drv)];
	drvheads = drv_heads[DRV_TYPE(drv)];

	track_length = drvsects * SECTORSIZ;
	cyl_size = track_length * drvheads;

	if (sector > drvsects)
		sector = 0;

	location = (cyl_size * cylinder) + (head * track_length) + (sector * SECTORSIZ);

#if APRIDEBUG > 2
	printf("Core0: hdimg_seek: drive = %d, head = %d, cyl = %d, sect = %d, location = %d, sector size = %d\n", drv, head, cylinder, sector, location / SECTORSIZ, SECTORSIZ);
#endif

	if ((head >= drvheads) || (cylinder >= drvcyls) || (sector >= drvsects))
		return FR_NO_FILE;

	retval = f_lseek(DRV_FP(drv), location);

	if (retval != FR_OK)
	{
#if APRIDEBUG > 2
		printf("Core0: wd_internal_seek failed: %s\n", FRESULT_str(retval));
#endif
	}
	
	return retval;
}

FRESULT wd_internal_seek()
{
	unsigned int drv, cyl, head, sector;
	FRESULT retval;

	cyl = (host_reg_cyl_high.byte << 8) + host_reg_cyl_low.byte;
	head = host_reg_sdh.byte & 0x07;
	sector = host_reg_sect_num.byte;

	retval = hdimg_seek(cur_drive - 1, head, cyl, sector);

	if (retval != FR_OK)
	{
		wd_reg_status.status.err = 1;
		wd_reg_errflg.byte = 0x10;
		hd_conf->numdrvs = 0;
		return retval;
	}

	wd_reg_sect_num.byte = sector;
	wd_reg_sdh.byte = (wd_reg_sdh.byte & 0xf8) + head;
	wd_reg_cyl_high.byte = host_reg_cyl_high.byte;
	wd_reg_cyl_low.byte = host_reg_cyl_low.byte;

	return retval;
}

void wd_restore()
{
#if APRIDEBUG > 2
	printf("Core0: wd_restore: Starting.\n");
#endif
	wd_reg_cyl_low.byte = 0;
	wd_reg_cyl_high.byte = 0;
	wd_reg_sect_num.byte = 0;
	wd_reg_sect_cnt.byte = 0;
	wd_reg_cyl_low.byte = 0;
	wd_reg_cyl_high.byte = 0;

#if APRIDEBUG > 2
	printf("Core0: wd_restore: asserting interrupt.\n");
#endif
	assert_int2();
	sleep_ms(1);
#if APRIDEBUG > 2
	printf("Core0: wd_restore: Slept and deasserting.\n");
#endif
	deassert_int2();

	wd_reg_status.status.drq = 1;
	wd_reg_status.status.sc = 1;
	wd_reg_status.status.bsy = 0;
	wd_reg_status.status.cip = 0;
#if APRIDEBUG > 2
	printf("Core0: wd_restore: Finishing.\n");
#endif
	assert_int2();
}

#define BRDY_TIMEOUT 1000

void wait_for_brdy()
{
	int i;

	i = 0;

	while (brdy == 1)
	{
//		sleep_ms(1);
		i++;
		if (i > BRDY_TIMEOUT)
			break;
	}

	i = 0;

	while (brdy == 0)
	{
//		sleep_ms(1);
		i++;
		if (i > BRDY_TIMEOUT)
			break;
	}
}
	

void wd_read()
{
	unsigned char sector[MAXSECTORSIZ];
	int multiple, interrupt;
	unsigned int byteswritten;
	int num_sects;
	int sects_written;
	FRESULT fresult;

	multiple = host_cmd.command.opts & 0b0100;
	interrupt = host_cmd.command.opts & 0b1000;

	if (multiple)
		num_sects = host_reg_sect_cnt.byte;
	else
		num_sects = 1;

	deassert_int2();

#if APRIDEBUG > 2
	printf("Core0: wd_read: drive %d: multiple = %d, interrupt = %d, num_sects = %d\n", cur_drive, multiple, interrupt, num_sects);
//	printf("Core0: wd_read: ");
//	debug_print_status(wd_reg_status);
#endif

	fresult = wd_internal_seek();

	if (fresult != FR_OK)
	{
#ifdef APRIDEBUG
		printf("Core0: wd_read: wd_internal_seek failed: %s\n", FRESULT_str(fresult));
#endif
		wd_reg_status.status.err = 1;
		wd_reg_errflg.byte = 0x10;
		hd_conf->numdrvs = 0;
		return;
	}

	for (; num_sects > 0; num_sects--)
	{
		fresult = f_read(DRV_FP(cur_drive - 1), sector, SECTORSIZ, &byteswritten);

		if (fresult != FR_OK)
		{
#ifdef APRIDEBUG
			printf("Core0: wd_read: f_read failed: %s\n", FRESULT_str(fresult));
#endif
			
			wd_reg_status.status.drq = 0;
			wd_reg_status.status.cip = 0;
			wd_reg_status.status.sc = 1;
			
			wd_reg_status.status.err = 1;
			wd_reg_errflg.byte = 0x10;
			hd_conf->numdrvs = 0;
			
			assert_int2();

			return;
		}

		sects_written = buf_write_sector(sector);
#if APRIDEBUG > 2
		if (sects_written != 512)
		{
			printf("Core0: wd_read: buffer short write: %d.\n", sects_written);
		}
#endif
		wd_reg_sect_cnt.byte = num_sects - 1;

#if APRIDEBUG > 3
		printf("Core0: wd_read: read %d bytes from file. and pushed to buffer.\n", byteswritten);
#endif

		if (num_sects > 1)
		{
			wd_reg_status.status.drq = 1;
			wd_reg_status.status.sc = 1;
#if APRIDEBUG > 3
			printf("Core0: wd_read: multiple: waiting for buffer ready signal.\n");
//			printf("Core0: wd_read: multiple: ");
//			debug_print_status(wd_reg_status);
#endif
			wait_for_brdy();

			wd_reg_status.status.drq = 0;
			wd_reg_status.status.sc = 1;
#if APRIDEBUG > 3
			printf("Core0: wd_read: multiple: Got it.\n");
			printf("Core0: wd_read: multiple: ");
//			debug_print_status(wd_reg_status);
#endif
		}
#if APRIDEBUG > 3
		printf("Core0: wd_read: Sector count now: %d\n", wd_reg_sect_cnt.byte);
#endif
	}

	wd_reg_status.status.drq = 1;
	wd_reg_status.status.bsy = 0;
	wd_reg_status.status.sc = 1;

	if (interrupt)
		assert_int2();

#if APRIDEBUG > 3
	printf("Core0: wd_read: end: waiting for buffer ready signal.\n");
//	printf("Core0: wd_read: end: ");
//	debug_print_status(wd_reg_status);
#endif

	wait_for_brdy();

	wd_reg_status.status.drq = 0;
	wd_reg_status.status.cip = 0;
	wd_reg_status.status.sc = 1;
#if APRIDEBUG > 3
	printf("Core0: wd_read: end: Got it.\n");
//	printf("Core0: wd_read: end: ");
//	debug_print_status(wd_reg_status);
	printf("Core0: wd_read: Sector count now: %d\n", wd_reg_sect_cnt.byte);
#endif
	assert_int2();
}

void wd_write()
{
	unsigned char sector[MAXSECTORSIZ];
	unsigned int byteswritten;
	int num_sects;
	int multiple;
	FRESULT fresult;

	multiple = host_cmd.command.opts & 0b0100;

	if (multiple)
	{
		num_sects = host_reg_sect_cnt.byte;
	}
	else
	{
		num_sects = 1;
	}

#if APRIDEBUG > 2
	printf("Core0: wd_write: drive %d: multiple = %d, num_sects = %d\n", cur_drive, multiple, num_sects);
#endif
	deassert_int2();
	
	wd_reg_status.status.drq = 0;

	wd_internal_seek();

	wd_reg_status.status.drq = 1;
	wd_reg_status.status.sc = 1;

#if APRIDEBUG > 3
	printf("Core0: wd_write: Waiting for data after seek.\n");
#endif

	while(brdy == 0){}

#if APRIDEBUG > 3
	printf("Core0: wd_write: continuing.\n");
#endif

	wd_reg_status.status.drq = 0;
	wd_reg_status.status.sc = 1;

	for (; num_sects > 0; num_sects--)
	{
		buf_read_sector(sector);
		fresult = f_write(DRV_FP(cur_drive - 1), sector, SECTORSIZ, &byteswritten);

		if (fresult != FR_OK)
		{
#if APRIDEBUG > 3
			printf("Core0: wd_write: f_write failed: %s\n", FRESULT_str(fresult));
#endif
			wd_reg_status.status.err = 1;
			wd_reg_errflg.byte = 0x10;
			hd_conf->numdrvs = 0;
			
			assert_int2();
			return;
		}
			
		wd_reg_sect_cnt.byte = num_sects - 1;

#if APRIDEBUG > 3
		printf("Core0: wd_write: Sector count = %d\n", wd_reg_sect_cnt.byte);
#endif

		if (num_sects > 1)
		{
			wd_reg_status.status.drq = 1;
			wd_reg_status.status.sc = 1;
			
#if APRIDEBUG > 3
			printf("Core0: wd_write: multiple: Waiting for data after seek.\n");
#endif
			while(brdy == 0){}

#if APRIDEBUG > 2
			printf("Core0: wd_write: multiple: continuing.\n");
#endif

			wd_reg_status.status.drq = 0;
			wd_reg_status.status.sc = 1;
		}
	}
	
	wd_reg_status.status.bsy = 0;
	wd_reg_status.status.cip = 0;
	wd_reg_status.status.sc = 1;
	wd_reg_status.status.drq = 1;
	assert_int2();
}

void wd_scanid()
{
	wd_reg_status.status.sc = 1;
	wd_reg_status.status.bsy = 0;
	wd_reg_status.status.cip = 0;
	assert_int2();
}

void wd_format()
{
	int num_sects, i;
	unsigned char buffer[MAXSECTORSIZ];
	unsigned int byteswritten;
	
	unsigned int drv, cyl, head, sector;
	FRESULT retval;

	cyl = (host_reg_cyl_high.byte << 8) + host_reg_cyl_low.byte;
	head = host_reg_sdh.byte & 0x07;
	sector = host_reg_sect_num.byte;

#if APRIDEBUG > 3
	printf("Core0: wd_format: Reset INT/ERR. Set CIP/BSY/BDRQ\n");
#endif

	deassert_int2();

	wd_reg_status.status.err = 0;
	wd_reg_status.status.bsy = 1;
	wd_reg_status.status.cip = 1;
	wd_reg_status.status.drq = 1;

#if APRIDEBUG > 3
	printf("Core0: wd_format: Wait for BRDY to be enabled.\n");
#endif

	while(brdy == 0){}

#if APRIDEBUG > 2
	printf("Core0: wd_format: Seek to desired CYL.\n");
#endif

	retval = hdimg_seek(cur_drive - 1, head, cyl, 0);

	if (retval != FR_OK)
	{
		wd_reg_status.status.err = 1;
		wd_reg_errflg.byte = 0x10;
		hd_conf->numdrvs = 0;
		return;
	}

#if APRIDEBUG > 3
	printf("Core0: wd_format: read sector data.\n");
#endif

	buf_read_sector(buffer);
	
	num_sects = host_reg_sect_cnt.byte;

	buf_read_sector(buffer);

#if APRIDEBUG > 3
	printf("Core0: wd_format: write %d sectors filled with 0xff\n", num_sects);
#endif

	for (i = 0; i < SECTORSIZ; i++)
		buffer[i] = 0xff;

	for (; num_sects > 0; num_sects--)
	{
		retval = f_write(DRV_FP(cur_drive - 1), buffer, SECTORSIZ, &byteswritten);

		if (retval != FR_OK)
		{
#if APRIDEBUG > 2
			printf("Core0: wd_format: f_write failed: %s\n", FRESULT_str(retval));
#endif
			wd_reg_status.status.err = 1;
			wd_reg_errflg.byte = 0x10;
			hd_conf->numdrvs = 0;
			
			assert_int2();
			return;
		}

		wd_reg_sect_cnt.byte = num_sects - 1;
	}

	wd_reg_status.status.sc = 1;
	wd_reg_status.status.bsy = 0;
	wd_reg_status.status.cip = 0;
	wd_reg_status.status.drq = 1;
	assert_int2();
}

void wd_seek()
{
	FRESULT retval;

	retval = wd_internal_seek();

	if (retval != FR_OK)
	{
#if APRIDEBUG > 2
		printf("Core0: wd_seek: wd_internal_seek failed: %s\n", FRESULT_str(retval));
#endif
		wd_reg_status.status.err = 1;
		wd_reg_errflg.byte = 0x10;
		hd_conf->numdrvs = 0;
		return;
	}

	spindown(cur_drive - 1);

	wd_reg_status.status.bsy = 0;
	wd_reg_status.status.cip = 0;
	wd_reg_status.status.drq = 1;
	wd_reg_status.status.sc = 1;
	
	assert_int2();
}

