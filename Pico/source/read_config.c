#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/rtc.h"
#include "hardware/adc.h"

#include "rtc.h"

#include "sd_stdio.h"
#include "apricot-hd.h"

int drvsiz[] = { 5, 10, 20 };

int d1 = 0;
int d2 = 0;

void clear_config(CONFIG *hd_conf)
{
	int i;

	for (i = 0; i < 2; i++)
	{
		DRV_FP(i) = (FIL *) malloc(sizeof(FIL));
		DRV_EXISTS(i) = 0;
		DRV_SPUNUP(i) = 0;
		DRV_TYPE(i) = 0;
	}
}

void strip_ws(char *string)
{
	char buffer[512];
	int i, j;

	i = j = 0;

	while((string[i] != '\0') && (j < 512))
	{
		if (((string[i] != ' ') && (string[i] != '\t')) && ((string[i] != 0x0a) && (string[i] != 0x0d)))
		{
#if APRIDEBUG > 4
			printf("Got character %02x: Not whitespace.\n", string[i]);
#endif
			buffer[j++] = string[i++];
		}
		else
		{
#if APRIDEBUG > 4
			printf("Got character %02x: is whitespace.\n", string[i]);
#endif
			i++;
		}
	}

	buffer[i] = '\0';

	i = j = 0;

	while(buffer[i] != '\0')
		string[j++] = buffer[i++];

	string[j] = '\0';

	return;
}

CONFIG *read_config()
{
	int i;
	static CONFIG config;
	FIL fp;
	FRESULT fr;
	FILINFO fno;
	char *filename = "config.txt";
	char *ptr, *ptr2;
	char buffer[512];
	int value, type;
	int numdrvs = 0;

	clear_config(&config);
	
#if APRIDEBUG > 1
	printf("read_config: opening config file.");
#endif

	fr = f_open(&fp, filename, FA_READ);

	if (fr != FR_OK)
		return NULL;

#if APRIDEBUG > 1
	printf("read_config: Starting to read file.");
#endif

	while (f_gets(buffer, 512, &fp) != NULL)
	{
#if APRIDEBUG > 1
		printf("read_config: Got the following line:\n'%s'\n", buffer);
#endif

		if (buffer[0] == '#')
#if APRIDEBUG > 1
		{
			printf("read_config: It's a comment.\n");
#endif
			continue;
#if APRIDEBUG > 1
		}
#endif

		strip_ws(buffer);

#if APRIDEBUG > 1
		printf("read_config: Stripped whitespace:\n'%s'\n", buffer);
#endif

		if ((ptr = strchr(buffer, '=')) == NULL)
#if APRIDEBUG > 1
		{
			printf("read_config: No equals, so syntax error.\n");
#endif
			return NULL;
#if APRIDEBUG > 1
		}
#endif

		*ptr++ = '\0';

#if APRIDEBUG > 1
		printf("read_config: token = '%s', value = %s\n", buffer, ptr);
#endif

		if (!strcmp("drive1", buffer))
		{
#if APRIDEBUG > 1
			printf("read_config: drive1\n");
#endif

			ptr2 = strchr(ptr, ',');

			if (ptr2 == NULL)
#if APRIDEBUG > 1
			{
				printf("read_config: comma, so syntax error.\n");
#endif
				return NULL;
#if APRIDEBUG > 1
			}
#endif

			*ptr2++ = '\0';

			type = -1;

#if APRIDEBUG > 1
			printf("read_config: searching for drive type.\n");
#endif
			value = atoi(ptr2);
			
			for (i = 0; i < 3; i++)
			{
				if (value == drvsiz[i])
				{
					type = i;
				}
#if APRIDEBUG > 1
				else
				{
					printf("read_config: %d is not %d\n", value, drvsiz[i]);
				}
#endif

			}

			if (type == -1)
			{
#if APRIDEBUG > 1
				printf("read_config: didn't find one: error.\n");
#endif
				return NULL;
			}

			config.drvparams[0].type = type;
			strcpy(config.drvparams[0].filename, ptr);
			config.drvparams[0].exists = 1;
			d1 = 1;
		}


		if (!strcmp("drive2", buffer))
		{
#if APRIDEBUG > 1
			printf("read_config: drive1\n");
#endif

			ptr2 = strchr(ptr, ',');

			if (ptr2 == NULL)
#if APRIDEBUG > 1
			{
				printf("read_config: comma, so syntax error.\n");
#endif
				return NULL;
#if APRIDEBUG > 1
			}
#endif

			*ptr2++ = '\0';

			type = -1;

#if APRIDEBUG > 1
			printf("read_config: searching for drive type.\n");
#endif

			value = atoi(ptr2);

			for (i = 0; i < 3; i++)
			{
				if (value == drvsiz[i])
				{
					type = i;
				}
#if APRIDEBUG > 1
				else
				{
					printf("read_config: %d is not %d\n", value, drvsiz[i]);
				}
#endif
			}
			
			if (type == -1)
			{
#if APRIDEBUG > 1
				printf("read_config: didn't find one: error.\n");
#endif
				return NULL;
			}

			config.drvparams[1].type = type;
			strcpy(config.drvparams[1].filename, ptr);
			config.drvparams[1].exists = 1;
			d2 = 1;
		}
	}

	f_close(&fp);

	numdrvs = d1 + d2;

#if APRIDEBUG > 1
				printf("read_config: Number of drives found %d.\n", numdrvs);
#endif

	config.numdrvs = numdrvs;

	return &config;
}
