#include "rgen.c"

void qsort_init() {
	struct nentryStruct *sourceFile;
	//struct nentryStruct *sourceFile2;
	unsigned int quantity;
	struct data_long *rnum;
	struct fs_objectPointer *obj;

	quantity = 100000;
	sourceFile = rgen_init(quantity);
	sourceFile = fs_openFile("/randomNumbers", 0);

	//sourceFile2 = fs_openFile("/randomNumbers", 0);
	//printk("file check %d, %d\n", sourceFile, sourceFile2);

	obj = fs_getObject(sourceFile, sizeof(struct data_long), 0);
	rnum = obj->toObject;
	printk("selection 8: %d, %d\n", obj->toObject, rnum->value);
	obj = fs_getObject(sourceFile, sizeof(struct data_long), 10015);
	if (obj < 1) {
		printk("Entry not found\n");
		return 0;
	}
	rnum = obj->toObject;
	printk("selection 10015: %d, %d\n", rnum, rnum->value);
}

unsigned long int sorter() {
	
}

