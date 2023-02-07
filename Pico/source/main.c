#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/rtc.h"
#include "hardware/adc.h"

#include "rtc.h"

#include "sd_stdio.h"
#include "apricot-hd.h"

sd_card_t *SD;

volatile unsigned char brdy, cmdrdy, dsk_errored;

volatile LATCH latch;
volatile unsigned char cur_drive, wd_cur_drive;

volatile WDREG host_reg_precomp, host_reg_sect_cnt, host_reg_sect_num;
volatile WDREG host_reg_cyl_low, host_reg_cyl_high, host_reg_sdh;
volatile WDREG wd_reg_errflg, wd_reg_sect_cnt, wd_reg_sect_num;
volatile WDREG wd_reg_cyl_low, wd_reg_cyl_high, wd_reg_sdh;
volatile WDCMD host_cmd;
volatile WDSTAT wd_reg_status;

CONFIG *hd_conf;

int flashcodes[6][5] = {
	{ 2, 1, 0, 0, 0},
	{ 3, 1, 0, 0, 0 },
	{ 4, 1, 0, 0, 0 },
	{ 4, 1, 1, 0, 0 },
	{ 4, 1, 1, 1, 0 },
	{ 4, 1, 1, 1, 1 }
};

#ifdef APRIDEBUG
void print_fileinfo(char *fname)
{
	FRESULT fr;
	FILINFO fno;

	fr = f_stat(fname, &fno);

	printf("Filename: %s\n", fname);

	switch (fr)
	{
		case FR_OK:
			printf("Size: %lu\n", fno.fsize);
			printf("Timestamp: %u-%02u-%02u, %02u:%02u\n",
			       (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,
			       fno.ftime >> 11, fno.ftime >> 5 & 63);
			printf("Attributes: %c%c%c%c%c\n",
			       (fno.fattrib & AM_DIR) ? 'D' : '-',
			       (fno.fattrib & AM_RDO) ? 'R' : '-',
			       (fno.fattrib & AM_HID) ? 'H' : '-',
			       (fno.fattrib & AM_SYS) ? 'S' : '-',
			       (fno.fattrib & AM_ARC) ? 'A' : '-');
			break;
		case FR_NO_FILE:
			printf("\"%s\" does not exist.\n", fname);
			break;
		default:
			printf("An error occured. (%d)\n", fr);
	}		
}
#endif

int main()
{
	int i, j, create, written;
	size_t size;
	char buffer[1024], *ptr;
	char *filename;
	FRESULT retval;
	FILINFO fno;
	FIL *fp;

	stdio_init_all();
	time_init();
	adc_init();

	sleep_ms(2000);

	init_bus();

	SD = sd_get_by_num(0);

	retval = f_mount(&SD->fatfs, SD->pcName, 1);

	if (retval != FR_OK)
	{
		printf("f_mount error: %s (%d)\n", FRESULT_str(retval), retval);
		my_error(FERR_MOUNT);
	}

	retval = f_getcwd(buffer, 1024);

	if (retval != FR_OK)
	{
		printf("f_getcwd error: %s (%d)\n", FRESULT_str(retval), retval);
		my_error(FERR_GCWD);
	}
#ifdef APRIDEBUG
	printf("Current directory is: %s\n", buffer);
#endif
	hd_conf = read_config();

	if (hd_conf == NULL)
	{
		printf("Config: syntax error.\n");
		my_error(FERR_SYNTAX);
		hd_conf->numdrvs = 0;
	}

	for (i = 0; i < hd_conf->numdrvs; i++)
	{
		if (DRV_EXISTS(i))
		{
			filename = hd_conf->drvparams[i].filename;
#ifdef APRIDEBUG
			print_fileinfo(filename);
#endif

			create = 0;

			retval = f_stat(filename, &fno);

			if (retval != FR_OK)
			{
				create = 1;

				fp = (FIL *) malloc(sizeof(FIL));
				retval = f_open(fp, filename, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);

				ptr = create_drvimage(hd_conf->drvparams[i].type, &size);

				f_write(fp, ptr, 512, &written);

				for (j = 0; j < 512; j++)
					buffer[i] = 0xff;

				for (j = 512; j < size; j += 512)
					f_write(fp, buffer, 512, &written);

				f_close(fp);
			}
		}
	}

#ifdef APRIDEBUG
	printf("About to launch main loops.\n");
#endif

	multicore_launch_core1(host_interface);

	wd_command_interpreter();
}

void my_panic(int errorcode)
{
	int i, j;

	for (i = 0; i < 3; i++)
	{
		blink_led_short();

		sleep_ms(100);
	}

	sleep_ms(1000);

	j = flashcodes[errorcode][0];
	j++;
	
	while(1)
	{
		for (i = 1; i < j; i++)
		{
			if (flashcodes[errorcode][i] == 0)
				blink_led_short();
			else
				blink_led_long();

			sleep_ms(500);
		}

		sleep_ms(1000);
	}
}

void my_error(int errorcode)
{
	int i, j, k;

	for (i = 0; i < 3; i++)
	{
		blink_led_short();

		sleep_ms(100);
	}

	sleep_ms(1000);

	j = flashcodes[errorcode][0];
	j++;
	
	for (k = 0; k < 3; k++)
	{
		for (i = 1; i < j; i++)
		{
			if (flashcodes[errorcode][i] == 0)
				blink_led_short();
			else
				blink_led_long();

			sleep_ms(500);
		}

		sleep_ms(1000);
	}
}
