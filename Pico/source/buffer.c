#include <stdio.h>

unsigned char buffer[8193];

unsigned int bufpos = 0;

void buf_reset()
{
	bufpos = 0;
}

int buf_write_byte(unsigned char *byte)
{
	if (bufpos >= 8192)
	{
		return -1;
	}

	buffer[bufpos++] = *byte;
	return 0;
}

int buf_write_sector(unsigned char *sector)
{
	int endpos, cpycnt;
	int i;

	if (bufpos >= 8192)
		return -1;

	endpos = bufpos + 512;

	if (endpos > 8192)
		cpycnt = 8192 - bufpos;
	else
		cpycnt = 512;

#if APRIDEBUG > 3
	printf("buffer: write_sector: bufpos = %d, cpycnt = %d, endpos = %d\n", bufpos, cpycnt, endpos);
#endif

	for (i = 0; i < cpycnt; i++)
		buffer[i + bufpos] = sector[i];

	bufpos = bufpos + cpycnt;

	return cpycnt;
}

int buf_read_byte(unsigned char *byte)
{

#if APRIDEBUG > 3
	printf("buf_read_byte: returning %08b\n", *byte);
#endif
	if (bufpos >= 8192)
	{
		return -1;
	}
	*byte = buffer[bufpos++];

	return 0;
}

int buf_read_sector(unsigned char *sector)
{
	int endpos, cpycnt;
	int i;

	if (bufpos >= 8192)
		return -1;
		
	endpos = bufpos + 512;

	if (endpos > 8192)
		cpycnt = 8192 - bufpos;
	else
		cpycnt = 512;

#if APRIDEBUG > 3
	printf("buffer: read_sector: bufpos = %d, cpycnt = %d, endpos = %d\n", bufpos, cpycnt, endpos);
#endif

	for (i = 0; i < cpycnt; i++)
		sector[i] = buffer[i + bufpos];

	bufpos = bufpos + cpycnt;

	return cpycnt;
}
