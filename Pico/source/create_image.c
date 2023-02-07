#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int disksiz[] = { 5, 10, 20 };
int heads[] = { 2, 4, 4};
int clustersiz[] = { 4, 8, 16 };
int cyls[] = { 306, 306, 612 };
int sectors[] = { 9792, 19584, 39168 };
int media_id[] = { 0xf8, 0xf9, 0xf9 };
int disk_type[] = { 3, 4, 8 };

struct bpb {
	unsigned short bpb_sector_size;
	unsigned char  bpb_cluster_size;
	unsigned short bpb_reserved_sectors;
	unsigned char  bpb_num_fats;
	unsigned short bpb_dirents;
	unsigned short bpb_numsectors;
	unsigned char  bpb_media_id;
	unsigned short bpb_sect_per_fat;
	unsigned char  bpb_disk_type;
} defbpb;

struct dtab {
	unsigned char  dtab_boot_disk_type;
	unsigned char  dtab_media_type;
	unsigned short dtab_parking_cylinder;
	unsigned short dtab_sectors_per_track;
	unsigned short dtab_tracks_per_side;
	unsigned short dtab_write_precomp_cyl;
	unsigned char  dtab_num_heads;
	unsigned char  dtab_interleave;
	unsigned short dtab_phys_sect_size;
} defdtab;

struct drvhead {
	char name[8];
	unsigned char os;
} defhead;

char *start_string = "wfe  1.0";

unsigned char *create_drvimage(int type, size_t *size)
{
	int i;
	size_t drvsiz;
	unsigned char *drv_image;
	unsigned char *bpb_start, *dtab_start, *odat_start;

	drvsiz = sectors[type] * 512;

	*size = drvsiz;

	drv_image = (unsigned char *) malloc((size_t) 512);

	for (i = 0; i < 512; i++)
		drv_image[i] = 0x00;

	bpb_start = drv_image + 0x50;
	dtab_start = drv_image + 0x0c;

	defbpb.bpb_sector_size = 512;
	defbpb.bpb_cluster_size = clustersiz[type];
	defbpb.bpb_reserved_sectors = 16;
	defbpb.bpb_num_fats = 2;
	defbpb.bpb_dirents = 256;
	defbpb.bpb_numsectors = sectors[type];
	defbpb.bpb_media_id = media_id[type];
	defbpb.bpb_sect_per_fat = 8;
	defbpb.bpb_disk_type = disk_type[type];

	defdtab.dtab_boot_disk_type = 0;
	defdtab.dtab_media_type = 1;
	defdtab.dtab_parking_cylinder = 305;
	defdtab.dtab_sectors_per_track = 16;
	defdtab.dtab_tracks_per_side = cyls[type];
	defdtab.dtab_write_precomp_cyl = 0;
	defdtab.dtab_num_heads = heads[type];
	defdtab.dtab_interleave = 1;
	defdtab.dtab_phys_sect_size = 512;

	memcpy(drv_image, &defhead, sizeof(struct drvhead));
	memcpy(bpb_start, &defbpb, sizeof(struct bpb));
	memcpy(dtab_start, &defdtab, sizeof(struct dtab));

	drv_image[0x100] = 0xff;
	drv_image[0x101] = 0xff;

	return drv_image;
}
