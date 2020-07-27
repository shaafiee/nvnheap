
//#define MEMEND 0x5dbeffff
//#define MEMEND 0xdffeffff
//#define MEMEND 0xffffffff

//#define MEMEND 0x11ffffff0

// optane
#define MEMBEGIN 0xf1000000

//#define MEMEND 0xbb7ffff1
//#define MEMEND 0xf0000000
//#define MEMBEGIN 0xbb800001
//#define MEMEND 0x7ffffffff
#define MEDIASIZE 10000000 //in bytes
#define RESERVED 20000
//#define MEMSIZE 0x1dcd6500
//#define MEMSTART 0x3ff19aff


struct resource *memRegion;
unsigned long int MEMSTART;
void *reservedStart;
void *reservedEnd;
void *memRefStart;
void *memRefEnd;
void *usableSpaceStart;
unsigned long int usableSpaceAvailable;
void *usableSpaceEnd;
void *currentSysPointer;
unsigned long int sysPointerState;
unsigned long int originalMemRefStart;

char mem_init(void) {
	//MEMSTART = MEMEND - MEDIASIZE;
	MEMSTART = MEMBEGIN;
	/*memRegion = request_mem_region(MEMSTART, MEDIASIZE, "reserved");
	if (memRegion <= 0) {
		printk("ERROR: Memory reservation failed!\n");
		return 0;
	}*/
	memRefStart = ioremap_nocache(MEMSTART, MEDIASIZE);
	if (memRefStart <= 0) {
		printk("ERROR: Memory allocation failed!\n");
		return 0;
	}
	//memRefStart = MEMSTART;
	memRefEnd = ((unsigned long int)memRefStart + MEDIASIZE);
	usableSpaceStart = memRefStart;
	reservedStart = memRefStart;
	reservedEnd = ((unsigned long int)memRefStart + RESERVED) - 1;
	memRefStart = (unsigned long int)memRefStart + RESERVED;
	currentSysPointer = reservedStart + 4;
	sysPointerState = reservedStart;
	sysPointerState = (unsigned long int)currentSysPointer;
	return 1;
}

char mem_updateSysPointer(unsigned long int newLocation) {
	currentSysPointer = newLocation;
	sysPointerState = (unsigned long int)currentSysPointer;
}

char mem_release() {
	//release_mem_region(MEMSTART, MEDIASIZE);
	return 1;
}

