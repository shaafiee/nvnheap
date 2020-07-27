
unsigned long int rgen_generateNumbers(unsigned int quantity) {
	//time_t t;
	struct nentryStruct *randomNumbersFile;
	struct data_long *rnum;
	int counter;
	long buffer;
	struct fs_objectPointer *some;

	//srand((unsigned)time(&t));

	fs_deleteFile("/randomNumbers");
	randomNumbersFile = fs_openFile("/randomNumbers", 1);

	for (counter = 0; counter < quantity; counter++) {
		rnum = fs_placeObject(randomNumbersFile, sizeof(struct data_long));
		get_random_bytes(&buffer, sizeof(long));
		rnum->value = buffer;
		printk("rgs ");
		printk("rgen.g %d ", rnum);
		printk("rgen.gn %d, %d, %d\n", counter, buffer, rnum->value);
	}
	return randomNumbersFile;
}

unsigned long int rgen_init(unsigned int quantity) {
	return rgen_generateNumbers(quantity);
}

