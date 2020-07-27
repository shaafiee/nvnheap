#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm/io.h>
#include "include/typedef.c"
#include "include/data.c"
#include "include/mem.c"
#include "include/reserved.c"
#include "include/string.c"
#include "include/object.c"
#include "include/nfi.c"
#include "include/fs.c"
#include "apps/run.c"

//test another anoth

/*void *initMem() {
	for (int i=0; i<5; i++) {
	//	*addr[i] = ioremap_nocache(phys
	}
}

void *performRead() {
	
}*/

//another change again


int init_module(void) 
{
	char *test_string, *temp_string;
	char *another_string;
	struct string_splitStruct *split;
	struct nentryStruct *file;

	struct sampleStruct {
		int a;
		int b;
		char something[20];
	} *obj, *sbj;
	struct fs_objectPointer *toObject;

	printk(KERN_INFO "\n\n\nStarts nvnheap\n");
	printk("\n||| mem init |||||||||||||||||||||||||||||||||||||||||\n");
	if (mem_init() == 0) {
		return 0;
	}
	printk("fff\n");
	printk("\n||| nfi init |||||||||||||||||||||||||||||||||||||||||\n");
	nfi_init();
	printk("nvnheap nextAvailable: %d\n", nentryHeader->nextAvailable);

	printk("nentries %d", nentries);
	printk(KERN_INFO "testing %d\n", sizeof(struct string_splitStruct));

	//test_string = string_new(test_string, "something|was|wrong|with|him", 0);
	//printk("test_string %s\n", test_string);

	/*temp_string = string_new(temp_string, "");
	printk("temp_string %s\n", temp_string);
	string_copy(test_string, temp_string, 0);
	printk("temp_string %s\n", temp_string);*/

	/*
	another_string = string_new(another_string, "", 0);
	split = string_split(test_string, "|", 0);
	printk("VOILA %s\n", split->element);
	printk("%s\n", (split->next)->element);
	printk("temp_string %s\n", temp_string);
	*/

	//file = nfi_createFile("/somefile");


	/* =============================
	printk("\n||| file open |||||||||||||||||||||||||||||||||||||||||\n");

	file = fs_openFile("/somefile", 1);
	printk("file location: %d\n", file);

	printk("\n||| place object |||||||||||||||||||||||||||||||||||||||||\n");
	obj = fs_placeObject(file, sizeof(struct sampleStruct));
	obj->a = 1335;
	//&obj->something = string_new(&obj->something, "brouhahä", &obj->something);
	//obj->something = "brouhaha";

	printk("\n||| file open |||||||||||||||||||||||||||||||||||||||||\n");
	printk("previous file location: %d\n", file);
	file = fs_openFile("/somefile", 0);
	printk("file location now: %d\n", file);
	if (file > 0) {
		printk("file nentry found: %d\n", file);
		
	}
	toObject = fs_getObject(file, sizeof(struct sampleStruct), 1);
	printk("object gotten %d, %d\n", toObject, toObject->toObject);
	sbj = toObject->toObject;
	printk("sbj->a: %d\n", sbj->a);
	============================= */
	
	//if (file < 1) {
//		file = fs_newFile("/somefile");
//	}

	//printk("file %d", (unsigned long int)file);
/*
	void *m;
	m = ioremap_nocache(0x5dbeffff, 1);
	k = 50;
	iowrite8(k, m);
	s = ioread8(m);
	printk("voilpoop %d\n", (short)s);
	*m = 0x5dbeffff;
	m = m - 8;
	*(volatile u8 __force *)m = 30;
	//a = ioread8(m);
	//a = *(char *)m;
	//printk("voildoop %d\n", (short)a);
*/

	run_app();

	printk("\n||| mem release |||||||||||||||||||||||||||||||||||||||||\n");
	mem_release();

	return 0;
}

void cleanup_module(void) 
{
	iounmap(memRefStart);
	printk(KERN_INFO "Goodday 1.\n");
}
