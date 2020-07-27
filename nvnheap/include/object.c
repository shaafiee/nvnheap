unsigned long int object_new(unsigned int objectSize) {
	unsigned long int objectAddress;
	objectAddress = (unsigned long int)currentSysPointer;
	currentSysPointer += objectSize;
	return objectAddress;
}


