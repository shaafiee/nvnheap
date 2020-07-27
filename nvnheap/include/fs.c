struct fs_objectPointer {
	unsigned long int *toNentry;
	unsigned long int *toObject;
};

unsigned long int fs_newFile(char *filenameWithPath) { // pointer to a nentryStruct
	return nfi_createFile(filenameWithPath);
}

unsigned long int fs_openFile(char *filenameWithPath, char createNew /* if one does not exist */) {
	unsigned long int fileHandle;
	printk("fso 1.0 nextAvailable %d\n", nentryHeader->nextAvailable);
	fileHandle = nfi_forFile(filenameWithPath);
	printk("fso 1.1 nextAvailable, filehandle: %d, %d\n", nentryHeader->nextAvailable, fileHandle);
	if (fileHandle < 1 && createNew > 0) {
		fileHandle = nfi_createFile(filenameWithPath);
	}
	printk("fso 1.2 nextAvailable %d\n", nentryHeader->nextAvailable);
	return fileHandle;
	//return nfi_forFile(filenameWithPath);
}

/*char fs_writeObject(struct nentryStruct *fileNentry, void *object, unsigned long int offset) {
	unsigned long int location;
	location = fileNentry->address + (sizeof(object) * offset);
}*/

unsigned long int fs_placeObject(struct nentryStruct *fileNentry, unsigned int objectSize) { // object is a struct of numericals and pointers (including char *), but not the content of pointers
	//printk("fsp 1.0 fileNentry %d\n", fileNentry);
	unsigned long int start;
	start = (unsigned long int)fileNentry;
	unsigned long int pointer;
	unsigned long int object;
	struct nentryStruct *oldFileNentry;
	//printk("fsp 1.1\n");
	if (fileNentry->tailOffset > 0) {
		//pointer = fileNentry->address + fileNentry->offset;
	} else {
		//printk("fsp 2.0\n");
		fileNentry->tailOffset = nfi_fileTail(fileNentry);
		//printk("fsp 2.1\n");
		//pointer = fileNentry->address + fileNentry->offset;
	}
	fileNentry = fileNentry->tailOffset;

	//printk ("fsp 2.2 tailOffset, objectSize, address, pagesize : %d, %d, %d, %d\n", fileNentry->tailOffset, objectSize, fileNentry->address, PAGESIZE);
	//printk("fsp 2.2 offset, objectSize, address, pagesize - %d, %d, %d, %d\n", fileNentry->offset, objectSize, fileNentry->address, PAGESIZE);
	if ((fileNentry->offset + objectSize) > PAGESIZE) {
		fileNentry->offset = 0;

		oldFileNentry = fileNentry;

		//printk("fsp 3.0 %d\n", fileNentry);
		fileNentry = nfi_addToFile(0, fileNentry);
		fileNentry->seqPrevious = oldFileNentry;
		oldFileNentry->seqNext = fileNentry;
		fileNentry->offset = 0;
		//printk("fsp 3.1 %d\n", fileNentry);
		fileNentry->tailOffset = fileNentry;
		object = fileNentry->address;
		fileNentry->offset = (unsigned int)sizeof(object);
	} else {
		object = fileNentry->address + fileNentry->offset;
		fileNentry->offset = fileNentry->offset + sizeof(object);
	}

	fileNentry = start;
	//printk("fsp 4.0 object: %d\n", object);
	return object;
}

unsigned long int fs_deleteFile(char *filenameWithPath) {
	struct nentryStruct *fileNentry;
	fileNentry = nfi_forFile(filenameWithPath);
	printk("deleting 1.0 %d\n", fileNentry);
	if (fileNentry > 0) {
		nfi_deleteFileNodes(fileNentry);
		return nfi_release(fileNentry);
	} else {
		return 0;
	}
}

unsigned long int fs_getObjectFunc(struct nentryStruct *fileNentry,
			unsigned int objectSize, 
			unsigned long int offset, /*in terms of number of objects and starting from fileNentry*/
			char reverse) {
	unsigned int objectsPerPage;
	unsigned int pagesTillTarget;
	int balance;
	unsigned int counter;
	struct nentryStruct *currentNentry;
	struct fs_objectPointer *tempPointer;

	objectsPerPage = PAGESIZE / objectSize;

	//balance = 0;
	//printk("fsgo 1.0 fileNentry, %d\n", fileNentry);	
	tempPointer = object_new(sizeof(struct fs_objectPointer));
	//printk("fsgo 1.1\n");	
	if (offset > objectsPerPage) {
		pagesTillTarget = offset / objectsPerPage;
		balance = offset % objectsPerPage;
		if (balance > 0) {
			pagesTillTarget++;
		}
	} else {
		pagesTillTarget = 0;
	}
	//printk("fsgo 1.2\n");	
	currentNentry = fileNentry;
	//printk("fsgo 1.3 currentNentry, pagesTillTarget: %d, %d\n", currentNentry, pagesTillTarget);
	if (pagesTillTarget > 0) {
		if (reverse == 1) {
			for (counter = 1; counter <= pagesTillTarget; counter++) {
				if (currentNentry->seqPrevious < 1) {
					return 0;
				}
				currentNentry = currentNentry->seqPrevious;
			}
		} else {
			for (counter = 1; counter <= pagesTillTarget; counter++) {
				if (currentNentry->seqNext < 1) {
					//printk("fsgo 1.3.1 counter %d\n", counter);
					return 0;
				}
				currentNentry = currentNentry->seqNext;
			}
		}
	}
	//printk("fsgo 1.4\n");	
	tempPointer->toNentry = currentNentry;
	tempPointer->toObject = currentNentry->address + ((offset - (pagesTillTarget * objectsPerPage)) * objectSize);
	//printk("offset = %d, pagesTillTarget = %d, objectsPerPage = %d, objectSize = %d\n", offset, pagesTillTarget, objectsPerPage, objectSize);
	/*if (balance == 0) {
		printk("fsgo 1.4,1\n");	
		tempPointer->toObject = currentNentry->address;
	} else {
		tempPointer->toObject = currentNentry->address + ((offset - (objectsPerPage * (pagesTillTarget - 1))) * objectSize);
	}*/
	//printk("fsgo 1.5 tempPointer, currentNentry->address, tempPointer->toNentry, tempPointer->toObject: %d, %d, %d, %d\n", tempPointer, currentNentry->address, tempPointer->toNentry, tempPointer->toObject);
	return tempPointer;
	
}

unsigned long int fs_getObject(	struct nentryStruct *fileNentry,
			unsigned int objectSize, 
			unsigned long int offset /*in terms of number of objects and starting from fileNentry*/) {
	return fs_getObjectFunc(fileNentry, objectSize, offset, 0);
}

unsigned long int fs_getObjectReverse(	struct nentryStruct *fileNentry,
			unsigned int objectSize, 
			unsigned long int offset /*in terms of number of objects and starting from fileNentry*/) {
	return fs_getObjectFunc(fileNentry, objectSize, offset, 1);
}

