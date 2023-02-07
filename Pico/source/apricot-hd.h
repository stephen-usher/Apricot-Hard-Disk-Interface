struct hd_config {
	FIL	*fp;
	char	filename[512];
	int	type;
	int	spunup;
	int	exists;
};

typedef struct hd_full_conf {
	unsigned char numdrvs;
	struct hd_config drvparams[2];
} CONFIG;

#define DRV_FP(x)	hd_conf->drvparams[x].fp
#define DRV_FILE(x)	hd_conf->drvparams[x].filename
#define DRV_EXISTS(x)	hd_conf->drvparams[x].exists
#define DRV_SPUNUP(x)	hd_conf->drvparams[x].spunup
#define DRV_TYPE(x)	hd_conf->drvparams[x].type

typedef union wd_register {
	struct {
		unsigned int b0:1;
		unsigned int b1:1;
		unsigned int b2:1;
		unsigned int b3:1;
		unsigned int b4:1;
		unsigned int b5:1;
		unsigned int b6:1;
		unsigned int b7:1;
	} bits;
	unsigned char byte;
} WDREG;

typedef union wd_status {
	struct {
		unsigned int err:1;
		unsigned int cip:1;
		unsigned int res:1;
		unsigned int drq:1;
		unsigned int sc:1;
		unsigned int wf:1;
		unsigned int rdy:1;
		unsigned int bsy:1;
	} status;
	unsigned char byte;
} WDSTAT;

typedef union wd_cmd {
	struct {
		unsigned int opts:4;
		unsigned int cmd:3;
		unsigned int res:1;
	} command;
	unsigned char byte;
} WDCMD;

#define WDCMD_RESTORE	1
#define WDCMD_READ	2
#define WDCMD_WRITE	3
#define WDCMD_SCANID	4
#define WDCMD_FORMAT	5
#define WDCMD_SEEK	7

typedef union latch {
	struct {
		union heads {
			struct bits {
				unsigned int zero:1;
				unsigned int one:1;
				unsigned int two:1;
			} bits;
			unsigned int head:3;
		} heads;
		unsigned int drvsel1:1;
		unsigned int data_rdy:1;
		unsigned int buf_reset:1;
		unsigned int buf_master:1;
		unsigned int drvsel2:1;
	} bits;
	unsigned char byte;
} LATCH;

struct bus_addr {
	unsigned char highbits;
	unsigned char lowbits;
};

/*
 * Buffer operation functions
 */

void buf_reset();
int buf_write_byte(unsigned char *byte);
int buf_write_sector(unsigned char *sector);
int buf_read_byte(unsigned char *byte);
int buf_read_sector(unsigned char *sector);

/*
 * GPIO access functions
 */

void init_bus();
void assert_int2();
void deassert_int2();
void bus_mode_write();
void bus_mode_read();
unsigned char read_bus();
void write_bus(unsigned char byte);
unsigned int bus_is_read();
struct bus_addr *accessed_address();
unsigned int is_selected();
void assert_iowait();
void deassert_iowait();

/*
 * Config functions.
 */

CONFIG *read_config();
unsigned char *create_drvimage(int type, size_t *size);

/*
 * Host interface function call.
 */

void host_interface();

/*
 * WD1010-05 command interpreter function call.
 */

void wd_command_interpreter();

/*
 * Other functions.
 */

void blink_led_short();
void blink_led_long();
void my_panic(int errcode);
void my_error(int errcode);

#ifdef APRIDEBUG
void debug_print_status(WDSTAT reg);
void debug_print_sdh(WDREG reg);
void debug_print_error(WDREG reg);
#endif

#define FERR_MOUNT	0
#define FERR_GCWD	1
#define FERR_SYNTAX	2
#define FERR_OPEN	3

