#define NENTRYHEADERSIZE 4096
#define NENTRYFILENAMEOFFSET 128
#define SYSPAGESIZE 1024
#define PAGEHEADER 8
#define PAGESIZE 1016 // should be 8 bytes less (4 for NFI back-reference and rest for padding)
#define SYSNENTRYSIZE 160
#define NVNVER 101
#define NFIPADDING 160

struct nentryStruct {
	struct nentryStruct *previous;
	struct nentryStruct *next;
	unsigned long int address; // this is always block start address + PAGEHEADER

	struct nentryStruct *parent;
	struct nentryStruct *owner;
	struct nentryStruct *firstChild;
	char isFirstChild;

	struct nentryStruct *seqPrevious; // previous, next in the sequence of data (for a single file)
	struct nentryStruct *seqNext;

	struct nentryStruct *peerPrevious; // the next item in this level (folder or file)
	struct nentryStruct *peerNext;

	struct nentryStruct *tailOffset; // this is the file tail offset block 
	int offset; // offset within this block

	unsigned long int PID;
	char rootPID; // root node for the PID store chain
	char type; // type of file (0 is unused, 1 is regular, 2 is directory, 3 is PID, 255 is root node)
	char fileRoot; // root node of a regular file
	//unsigned long int filename;
	char *filename; // must be less than or equal to 12 characters

	// NENTRY relations
	// next/previous: the sequential linked-list pointers from first nentry to last
	// peerNext/peerPrevious: pointers to other objects in the current directory level
	// seqNext/seqPrevious: pointers to pages of the current object (a file or blocks for a PID)
};

struct nentryHeaderStruct {
	unsigned long int check;
	unsigned long int firstNentryOffset;
	unsigned long int lastNentry;
	unsigned long int usableSpaceAvailable;
	unsigned long int usableSpaceEnd;
	unsigned long int nentries;
	unsigned long int NFIStart;
	unsigned long int lastAllocated;
	unsigned long int nextAvailable;
};

struct fileHeaderStruct {
	unsigned long int fileNentry;
};

unsigned long int NFIStart;
struct nentryHeaderStruct *nentryHeader;
struct nentryStruct *nentry;
unsigned long int nentries; // total number of nentries needed for the memory available
unsigned long int numberOfBlocks;

char nfi_generate(void) {
	unsigned long int i;
	unsigned long int *blockLocation;
	struct fileHeaderStruct *fileHeader;
	char *tempString;
	char *anotherShit;
	int somelength;
	unsigned long int filenameLocation;
	//printk("generator entered %d\n", nentries);
	for (i = 0; i < nentries; i++) {
		//printk("precycle%d %ul\n", i, (unsigned long int)NFIStart);
		nentry = NFIStart + (SYSNENTRYSIZE * i);

		//printk("cycle%ul\n", (unsigned long int)NFIStart);
		nentry->address = (unsigned long int)memRefStart + (SYSPAGESIZE * i) + PAGEHEADER;
		fileHeader = (unsigned long int)nentry->address - PAGEHEADER;
		fileHeader->fileNentry = (unsigned long int)nentry;
		//blockLocation = nentry->address - 4;
		//*blockLocation = (unsigned long int)nentry;

		//tempString = (unsigned long int)nentry + NENTRYFILENAMEOFFSET;
		//nentry->filename = (unsigned long int)tempString;
		filenameLocation = (unsigned long int)nentry + NENTRYFILENAMEOFFSET;
		nentry->filename = string_new(nentry->filename, 0, filenameLocation);
		/*if (i > 0 && i <5) {
			printk("nentry, filename: %d, %d\n", nentry, nentry->filename);
		}*/

		//printk("%d done\n", i);
		if (i == 0) {
			nentry->type = 255;
			nentry->previous = 0;
		} else {
			nentry->previous = ((unsigned long int)nentry - SYSNENTRYSIZE);
		}
		if (i == nentries - 1) {
			nentry->next = 0;
		} else {
			nentry->next = ((unsigned long int)nentry + SYSNENTRYSIZE);
		}
	}
	/*nentry = NFIStart + SYSNENTRYSIZE;
	printk("nentry, nentry filename, value: %d, %d, %s\n", nentry, nentry->filename, nentry->filename);
	printk("nentry, nentry filename, value: %d, %d, %s\n", nentry, nentry->filename, nentry->filename);
	tempString = string_new(tempString, "thisthisthis", 0);
	filenameLocation = nentry->filename;
	printk("tempString, filenameLocation %s, %d\n", tempString, filenameLocation);
	somelength = string_copy(tempString, filenameLocation, 0);
	printk("somethinglength %d\n", somelength);
	printk("nentry, nentry->filename, filename: %d, %d, %s, %d\n", nentry, nentry->filename, nentry->filename);
	printk("filenameLocation, content of: %d, %s\n", filenameLocation, filenameLocation);*/
}

char nfi_init(void) {
	unsigned long int usableMemory;
	//nentryHeader = ((unsigned long int)memRefEnd - NENTRYHEADERSIZE);
	//nentryHeader = ((unsigned long int)memRefStart + (MEDIASIZE) - NENTRYHEADERSIZE;
	//printk("nentryheader: %d\n", nentryHeader);
	usableMemory = ((unsigned long int)memRefEnd - (NENTRYHEADERSIZE + NFIPADDING)) - (unsigned long int)memRefStart;
	nentryHeader = (unsigned long int)memRefEnd - (NENTRYHEADERSIZE + 1);
	//nentryHeader = (unsigned long int)memRefStart;
	//usableMemory = ((unsigned long int)memRefEnd - (unsigned long int)memRefStart) - (NENTRYHEADERSIZE + NFIPADDING);
	if (nentryHeader->check == usableMemory / SYSPAGESIZE) {
	//if (0) {
		printk("nnh %d\n", (unsigned long int)nentryHeader->NFIStart);
		NFIStart = nentryHeader->NFIStart;
		nentry = NFIStart;
		usableSpaceAvailable = nentryHeader->usableSpaceAvailable;
		usableSpaceEnd = nentryHeader->usableSpaceEnd;
		nentries = nentryHeader->nentries;
		printk("nnh nextAvailable, nentry, NFIStart: %d, %d, %d\n", nentryHeader->nextAvailable, nentry, NFIStart);

		return 1;
	} else {
		// if entire storage space was used - actual number is stored in variable 'nentries'
		unsigned long int totalNentriesRequired, spaceRequiredForNFI;
		//totalNentriesRequired = (usableMemory / SYSPAGESIZE) - (usableMemory % SYSPAGESIZE);
		totalNentriesRequired = (usableMemory / SYSPAGESIZE);
		spaceRequiredForNFI = totalNentriesRequired * SYSNENTRYSIZE;

		usableSpaceAvailable = (usableMemory - spaceRequiredForNFI) - NFIPADDING;
		usableSpaceEnd = (unsigned long int)memRefStart + usableSpaceAvailable;
		nentries = (usableSpaceAvailable / SYSPAGESIZE);// - (usableSpaceAvailable % SYSPAGESIZE);
		//NFIStart = ((nentryHeader) - (nentries * SYSNENTRYSIZE));
		NFIStart = usableSpaceEnd + NFIPADDING;
		//printk("boothing: %d, %d, %d\n", usableMemory, totalNentriesRequired, nentries);

		/*printk("something: %o\n", (unsigned long int)NFIStart);
		unsigned long int *propota;
		propota = memRefStart + 10;
		*propota = 1010;
		iowrite8(1210, memRefStart);
		printk("onething: %o\n", (unsigned long int)NFIStart);
		iowrite8(1010, propota);
		printk("boothing: %d\n", *propota);*/

		nentryHeader->NFIStart = NFIStart;
		nentryHeader->usableSpaceAvailable = usableSpaceAvailable;
		nentryHeader->usableSpaceEnd = usableSpaceEnd;
		nentryHeader->nentries = nentries;
		nentryHeader->firstNentryOffset = NFIStart;
		nentry = NFIStart;
		nfi_generate();
		nentryHeader->lastNentry = (unsigned long int)nentry;
		nentryHeader->check = usableMemory / SYSPAGESIZE;
		nentryHeader->nextAvailable = (unsigned long int)(NFIStart + SYSNENTRYSIZE);
		
	}
	printk("nentryheader, memRefEnd, NFIStart: %d, %d, %d, %d\n", nentryHeader, (unsigned long int)memRefEnd, NFIStart);

	return 1;
}

struct nentryStruct *nfi_unused(struct nentryStruct *nentryToCheck) {
	char *nentryFilename;
	if (	NFIStart != (unsigned long int)nentryToCheck 
		&& nentryToCheck->PID < 1 
		&& string_length(nentryToCheck->filename) < 1
		) {
		//printk("boo! %d\n", nentryToCheck);
		return nentryToCheck;
	} else {
		return 0;
	}
}

unsigned long int nfi_nextUnused(struct nentryStruct *pivot) {
	struct nentryStruct *pivotStart;
	struct nentryStruct *found;
	pivotStart = pivot;
	found = 0;
	while (pivot->next > 0) {
		found = nfi_unused(pivot);
		if (found > 0) {
			printk("nnu found %d\n", pivot);
			pivot = found;
			break;
		}
		pivot = pivot->next;
	}
	if (found == 0) {
		pivot = pivotStart;
		while (found == 0 && pivot->previous > 0) {
			found = nfi_unused(pivot);
			if (found > 0) {
				printk("nnu found %d\n", pivot);
				pivot = found;
				break;
			}
			pivot = pivot->previous;
		}
	}
	if (found == 0) {
		return 0;
	} else {
		return (unsigned long int)pivot;
	}
}

unsigned long int nfi_nextAvailable() {
	struct nentryStruct *tempNentry;
	tempNentry = nentryHeader->nextAvailable;
	//printk("nfin 1.0 %d, %d\n", tempNentry, nentryHeader->nextAvailable);
	if (nfi_unused(tempNentry) > 0) {
		//printk("nfin 1.2 %d\n", nentryHeader->nextAvailable);
		return nentryHeader->nextAvailable;
	} else {
		//printk("nfin 2.0 \n");
		tempNentry = nfi_nextUnused(tempNentry);
		nentryHeader->nextAvailable = tempNentry;
		//printk("nfin 2.1 nextUnused: %d\n", (unsigned long int)tempNentry);
		return (unsigned long int)tempNentry;
	}
}

char hasChildren(struct nentryStruct *nentryToCheck) { // returns pointer to lastChild or 0
	int count, i;
	struct nentryStruct *tempNentry;
	unsigned long int nentryFound;
	for (i = 0; i < nentries; i++) {
		tempNentry = NFIStart + (SYSNENTRYSIZE * i);
		if (tempNentry->parent == (unsigned long int)nentryToCheck) {
			nentryFound = (unsigned long int)tempNentry;
			count++;
		}
	}
	if (count > 0) {
		return nentryFound;
	}
	return 0;
}

unsigned long int nfi_block(struct nentryStruct *nentryToBlock, unsigned long int count, char *filename, unsigned long int PID) {
	struct nentryStruct *firstNentry;
	unsigned long int counter;
	char *nentryFilename;
	if (*filename < 1 && PID < 1) {
		return 0;
	}
	if (nfi_unused(nentryToBlock) > 0) {
		if (string_length(filename) > 0) {
			string_copy(filename, nentryToBlock->filename, 0);
		} else {
			nentryToBlock->PID = PID;
		}
		printk("nbl 1.0: %s\n", filename);
		printk("nbl 1.1: %s\n", nentryToBlock->filename);
		nentryHeader->nextAvailable = nfi_nextUnused(nentryToBlock);
		printk("nbl 1.2: %s\n", nentryToBlock->filename);
		if (count <= 1) {
			printk("nbl 1.3: %s\n", nentryToBlock->filename);
			return (unsigned long int)nentryToBlock;
		} else {
			printk("nbl 1.4: %s\n", nentryToBlock->filename);
			firstNentry = nentryToBlock;
			for (counter = 1; counter < count; counter++) {
				nentryToBlock = nfi_nextUnused(nentryToBlock);
				if (nentryToBlock < 1) {
					break;
				}
				if (filename > 0) {
					string_copy(filename, nentryFilename, 0);
				} else {
					nentryToBlock->PID = PID;
				}
			}
			nentryHeader->nextAvailable = nfi_nextUnused(nentryToBlock);
			if (filename > 0) {
				return (unsigned long int)firstNentry;
			} else {
				return (unsigned long int)nentryToBlock;
			}
		}
	} else {
		return 0;
	}
}

unsigned long int nfi_forPID(unsigned long int PID) { // returns an address for PID (or the existing one)
	int counter;
	struct nentryStruct *tempNentry;
	struct nentryStruct *foundNentry;
	char found;
	found = 0;
	for (counter = 1; counter < nentryHeader->nentries; counter++) {
		tempNentry = (unsigned long int)NFIStart + (nentries * SYSNENTRYSIZE);
		if (tempNentry->PID == PID && tempNentry->rootPID > 0) {
			foundNentry = tempNentry;
			found = 2;
			break;
		}
	}
	if (found > 0) {
		return (unsigned long int)tempNentry;
	} else {
		tempNentry = nfi_nextAvailable();
		nfi_block(tempNentry, 1, 0, PID);
		tempNentry->rootPID = 1;
		tempNentry->type = 3;
	}
	return 0;
}

unsigned long int nfi_PIDTail(unsigned long int PID) {
	int counter;
	struct nentryStruct *tempNentry;
	for (counter = (nentries - 1); counter >= 0; counter--) {
		if (tempNentry->PID == PID && tempNentry->seqNext > 0) {
			return (unsigned long int)tempNentry;
		}
	}
}

unsigned long int nfi_addToPID(unsigned long int PID) {
	struct nentryStruct *tempNentry;
	struct nentryStruct *newNentry;
	tempNentry = nfi_PIDTail(PID);
	newNentry = nfi_nextAvailable();
	newNentry = nfi_block(newNentry, 1, 0, PID);
	newNentry->seqPrevious = tempNentry;
	tempNentry->seqNext = newNentry;
}

unsigned long int nfi_pathNextLeg(struct nentryStruct *parent, char *filename) {
	//struct nentryStruct *currentNentry;
	struct nentryStruct *pivot;
	char matched;
	char *nentryFilename;
	if (parent < 1 && filename < 1) {
		return NFIStart;
	}
	if (parent < 1) {
		parent = NFIStart;
	}
	if (parent->firstChild > 0) {
		pivot = parent->firstChild;
	} else {
		return 0;
	}
	if (pivot->peerPrevious < 1 && pivot->peerNext < 1) {
		return 0;
	}
	while (1) {
		if (pivot->peerPrevious < 1) {break;}
		pivot = pivot->peerPrevious;
	}
	while (1) {
		if (string_equal(pivot->filename, filename) > 0 && pivot->type == 2) {
			return (unsigned long int)pivot;
		}
		if (pivot->peerNext < 1) {break;}
		pivot = pivot->peerNext;
	}

	return 0;
}

unsigned long int nfi_forPath(char *path, struct string_splitStruct *preSplit) {
	struct string_splitStruct *currentPath;
	struct nentryStruct *currentNentry;
	struct nentryStruct *previousNentry;

	if (path < 1 && preSplit < 1) {
		return NFIStart;
	}

	if (preSplit > 0) {
		currentPath = preSplit;
	} else {
		currentPath = string_split(path, "/", 0);
	}
	if (currentPath->element > 0) {
	} else {
		return NFIStart;
	}
	while (1) {
		currentNentry = nfi_pathNextLeg(previousNentry, currentPath->element);
		if (currentNentry < 1) {
			if (preSplit < 1) {
				mem_updateSysPointer(currentSysPointer - string_splitLength(currentPath));
			}
			return 0;
		}
		
		if (currentPath->next < 1) {break;}
		previousNentry = currentNentry;
		currentPath = currentPath->next;
	}
	if (preSplit < 1) {
		mem_updateSysPointer(currentSysPointer - string_splitLength(currentPath));
	}
	return (unsigned long int)currentNentry;
}

unsigned long int nfi_createPath(char *sourcePath) {
	struct string_splitStruct *currentPath;
	struct nentryStruct *currentNentry;
	struct nentryStruct *newNentry;
	struct nentryStruct *lastChild;
	currentPath = string_split(sourcePath, "/", 0);
	if (currentPath < 1) {
		mem_updateSysPointer(currentSysPointer - string_splitLength(currentPath));
		return 0;
	}
	while (currentPath->next > 0) {
		currentNentry = nfi_pathNextLeg(currentNentry, currentPath->element);
		if (currentNentry < 1) {
			mem_updateSysPointer(currentSysPointer - string_splitLength(currentPath));
			return 0;
		}
	}
	if (nfi_pathNextLeg(currentNentry, currentPath->element) > 0 || string_length(currentPath->element) > 12) {
		mem_updateSysPointer(currentSysPointer - string_splitLength(currentPath));
		return 0;
	}
	newNentry = nfi_nextAvailable();

	if (nfi_block(newNentry, 1, currentPath->element, 0) > 0) {
		//string_copy(currentPath->element, newNentry->filename, 0);
		newNentry->type = 2;
		lastChild = hasChildren(currentNentry);
		if (lastChild > 0) {
			newNentry->parent = (unsigned long int)currentNentry;
			newNentry->peerPrevious = lastChild;
			lastChild->peerNext = newNentry;
		} else {
			currentNentry->firstChild = (unsigned long int)newNentry;
			newNentry->parent = (unsigned long int)currentNentry;
			newNentry->isFirstChild = 1;
		}
		//currentNentry->peerNext = newNentry;
		//newNentry->peerPrevious = currentNentry;
		mem_updateSysPointer(currentSysPointer - string_splitLength(currentPath));
		return (unsigned long int)newNentry;
	} else {
		mem_updateSysPointer(currentSysPointer - string_splitLength(currentPath));
		return 0;
	}
}

unsigned long int nfi_rootPID(unsigned long int PID) {
	int counter;
	struct nentryStruct *tempNentry;
	struct nentryStruct *rootNentry;
	for (counter = 0; counter < nentries; counter++) {
		tempNentry = NFIStart + (counter * SYSNENTRYSIZE);
		if (tempNentry->PID == PID && tempNentry->rootPID > 0) {
			return (unsigned long int)tempNentry;
		}
	}
}

unsigned long int nfi_createPID(unsigned long int PID) {
	struct nentryStruct *currentNentry;
	if (nfi_forPID(PID) < 1) {
		currentNentry = nfi_nextAvailable();
		currentNentry = nfi_block(currentNentry, 1, 0, PID);
		currentNentry->type = 3;
		currentNentry->rootPID = 1;
		return currentNentry;
	} else {
		return 0;
	}
}

unsigned long int nfi_extractFilename(struct string_splitStruct *pathToTreat) {
	char *filename;
	unsigned long int start;
	struct string_splitStruct *lastElement;
	start = (unsigned long int)pathToTreat;
	while (pathToTreat->next > 0) {
		pathToTreat = pathToTreat->next;
	}
	lastElement = pathToTreat;
	pathToTreat = pathToTreat->previous;
	pathToTreat->next = 0;

	printk("efn %d - %d\n", pathToTreat->previous, pathToTreat->next);
	printk("efn %s - %d - %d\n", lastElement->element, lastElement->previous, lastElement->next);
	pathToTreat = start;
	return lastElement;
}

unsigned long int nfi_firstFileNentry(struct string_splitStruct *path, char *filename) {
	struct nentryStruct *targetPath;
	struct nentryStruct *tempNentry;
	char *nentryFilename;
	//printk("nffn 1 %s\n", filename);
	if (path->element < 1) {
		targetPath = NFIStart;
	} else {
		targetPath = nfi_forPath(0, path);
	}
	if (targetPath < 1) {
		return 0;
	}
	int counter;
	//printk("nffn 2 %d\n", (unsigned long int)targetPath);
	for (counter = 0; counter < nentries; counter++) {
		//printk("nffn 3 - %d\n", counter);
		tempNentry = NFIStart + (counter * SYSNENTRYSIZE);
		//printk("nffn 3.1 - %d\n", (unsigned long int)tempNentry);
		//printk("nffn 3.2 - %s\n", tempNentry->filename);
		//nentryFilename = tempNentry->filename;
		//printk("nffn 2.1 assignment done %s\n", tempNentry->filename);
		if (	string_equal(tempNentry->filename, filename) > 0 
			&& tempNentry->parent == targetPath 
			&& tempNentry->type == 1) {
			return tempNentry;
		}
		//printk("nffn 3.2\n");
	}
	return 0;
}

unsigned long int nfi_forFile(char *filenameWithPath) {
	struct string_splitStruct *currentPath;
	struct string_splitStruct *filename;
	struct nentryStruct *tempNentry;
	int tempLength;
	if (filenameWithPath < 1) {
		return 0;
	}
	currentPath = string_split(filenameWithPath, "/", 0);
	printk("nff 3 %s - %d\n", currentPath->element, currentPath->next);
	filename = nfi_extractFilename(currentPath);
	printk("nff 3.1 %s - %d\n", currentPath->element, currentPath->next);
	printk("nff 3.2 %d - %d\n", currentPath, currentPath->next);

	tempNentry = nfi_firstFileNentry(currentPath, filename->element);
	printk("nff 2\n");
	if (tempNentry < 1) {
		printk("nff 4\n");

		printk("nff 4.0.1 %d - %d\n", currentPath, currentPath->next);
		printk("nff 4.0.1 <filename> %d - %d\n", filename, filename->next);
		printk("nff 4.0.1.1 %d - %d\n", currentPath, currentPath->next);
		tempLength = string_splitLength(filename);
		tempLength += string_splitLength(currentPath);
		mem_updateSysPointer(currentSysPointer - tempLength);
		printk("nff 4.0.2\n");
		return 0;
	} else {
		if (tempNentry->fileRoot > 0) {
			tempLength = string_splitLength(filename);
			tempLength += string_splitLength(currentPath);
			mem_updateSysPointer(currentSysPointer - tempLength);
			return (unsigned long int)tempNentry;
		}
	}

	printk("nff 4.1\n");
	/*if (tempNentry->fileRoot > 0) {
		tempLength = string_splitLength(filename);
		tempLength += string_splitLength(currentPath);
		mem_updateSysPointer(currentSysPointer - tempLength);
		return (unsigned long int)tempNentry;
	}
	printk("nff 5\n");*/
	while (tempNentry->seqNext > 0) {
		/*if (tempNentry->seqNext > 0) {
			tempNentry = tempNentry->seqNext;
			printk("nff 6");
		} else {
			break;
		}*/
		if (tempNentry->fileRoot > 0) {
			tempLength = string_splitLength(filename);
			tempLength += string_splitLength(currentPath);
			mem_updateSysPointer(currentSysPointer - tempLength);
			return (unsigned long int)tempNentry;
		}
		tempNentry = tempNentry->seqNext;
		/*printk("nff 7");
		if (tempNentry->seqNext < 1) {
			break;
		}
		printk("nff 8");*/
	}
	tempLength = string_splitLength(filename);
	tempLength += string_splitLength(currentPath);
	mem_updateSysPointer(currentSysPointer - tempLength);
	return 0;
}
/*
unsigned long int nfi_fileRoot(struct nentryStruct *pivot) {
	struct nentryStruct *tempNentry;
	tempNentry = pivot;
	if (pivot->fileRoot > 0) {
		return (unsigned long int)pivot;
	}
	if (pivot->seqNext > 0) {
		while (1) {
			pivot = pivot->seqNext;
			if (pivot->fileRoot > 0) {
				return (unsigned long int)pivot;
			}
			if (pivot->seqNext < 1) {
				break;
			}
		}
	}
	pivot = tempNentry;
	if (pivot->seqPrevious > 0) {
		while (1) {
			pivot = pivot->seqPrevious;
			if (pivot->fileRoot > 0) {
				return (unsigned long int)pivot;
			}
			if (pivot->seqPrevious < 1) {
				break;
			}
		}
		
	}
}
*/

unsigned long int nfi_fileTail(struct nentryStruct *start) {
	while (1) {
		if (start->seqNext < 1) {
			return (unsigned long int)start;
		}
		start = start->seqNext;
	}
}

unsigned long int nfi_createFile(char *filenameWithPath) {
	struct string_splitStruct *currentPath;
	unsigned long int tempPath;
	struct nentryStruct *tempNentry;
	struct nentryStruct *newNentry;
	struct nentryStruct *pathNentry;
	struct string_splitStruct *filename;
	int tempLength;
	struct nentryStruct *lastChild;
	//printk("ncf 1 %lu : %s\n", (unsigned long int)filenameWithPath, filenameWithPath);
	currentPath = string_split(filenameWithPath, "/", 0);
	printk("ncf 1.5 %d\n", currentPath);
	if (currentPath < 1) {
		return 0; // all file names should have a root
		/*if  (string_length(filenameWithPath) < 1) {
			return 0;
		} else {
			string_copy(filenameWithPath, filename, 0);
			filename = currentSysPointer;
			string_copy(filenameWithPath, filename->element, 0);
			currentSysPointer += sizeof(struct string_splitStruct) + string_length(filenameWithPath);
		}*/
	} else {
		//printk("ncf 4\n");
		/* tempPath = (unsigned long int)currentPath;
		printk("ncf 4.1 %d\n", (unsigned long int)currentPath);
		while (1) {
			printk("ncf 4.2 %d\n", (unsigned long int)currentPath);
			printk("ncf 4.2.1 %s\n", currentPath->element);
			if (currentPath->next < 1) {
				printk("ncf 4.3\n");
				filename = string_new(filename, 0, 0);
				printk("ncf 4.4\n");
				string_copy(currentPath->element, filename, 0);
				printk("ncf 4.5\n");
				break;
			}
			printk("ncf 4.2.2\n");
			currentPath = currentPath->next;
		}
		currentPath = currentPath->previous;
		currentPath->next = 0;
		currentPath = tempPath; */
		filename = nfi_extractFilename(currentPath);
		//printk("ncf 5\n");
	}

	//tempNentry = nfi_firstFileNentry(currentPath, filename->element);
	/*if (tempNentry < 1 || string_length(filename->element) > 12) {
		tempLength = string_splitLength(filename);
		tempLength += string_splitLength(currentPath);
		mem_updateSysPointer(currentSysPointer - tempLength);
		return 0;
	}*/

	printk("ncf 1.6 %d\n", currentPath);
	pathNentry = nfi_forPath(0, currentPath);
	printk("ncf 1.7 pathNentry: %d\n", pathNentry);
	lastChild = hasChildren(pathNentry);

	//if (tempNentry->seqNext > 1) {
	//	tempNentry = nfi_fileTail(tempNentry);
	//}

	printk("ncf 1.8 %d\n", nentryHeader->nextAvailable);
	newNentry = nfi_nextAvailable();
	if ((unsigned long int)newNentry < 1) {
		printk("Failed to find an empty nentry\n");
		return 0;
	}
	printk("ncf 1.8.1 %d\n", nentryHeader->nextAvailable);
	newNentry = nfi_block(newNentry, 1, filename->element, 0);
	printk("ncf 1.8.2 %d\n", nentryHeader->nextAvailable);
	newNentry->type = 1;
	newNentry->fileRoot = 1;
	newNentry->parent = pathNentry;
	if (lastChild > 0) {
		newNentry->peerPrevious = lastChild;
		lastChild->peerNext= newNentry;
	} else {
		newNentry->peerPrevious = 0;
		newNentry->isFirstChild = 1;
	}
	newNentry->peerNext = 0;
	printk("ncf 1.9 newNentry: %d\n", newNentry);
	
	tempLength = string_splitLength(filename);
	printk("ncf 1.9.1 %d\n", nentryHeader->nextAvailable);
	tempLength += string_splitLength(currentPath);
	printk("ncf 1.9.2 %d\n", nentryHeader->nextAvailable);
	mem_updateSysPointer(currentSysPointer - tempLength);
	printk("ncf 1.9.3 %d\n", nentryHeader->nextAvailable);

	return (unsigned long int)newNentry;
}

unsigned long int nfi_addToFile(char *filenameWithPath, struct nentryStruct *fileNentry) {
	struct string_splitStruct *currentPath;
	struct nentryStruct *tempNentry;
	struct nentryStruct *newNentry;
	struct nentryStruct *pathNentry;
	struct string_splitStruct *filename;
	char *nentryFilename;
	if (fileNentry > 0) {
		//printk("naf 1.0 starting file nentry is correct\n");
		tempNentry = fileNentry;
	} else {
		if (filenameWithPath > 0) {
			currentPath = string_split(filenameWithPath, "/", 0);
			filename = nfi_extractFilename(currentPath);

			tempNentry = nfi_firstFileNentry(currentPath, filename->element);
			if (tempNentry < 1) {
				mem_updateSysPointer(currentSysPointer - string_splitLength(filename));
				mem_updateSysPointer(currentSysPointer - string_splitLength(currentPath));
				return 0;
			}
			pathNentry = nfi_forPath(0, currentPath);
			tempNentry = nfi_fileTail(tempNentry);
		} else {
			//printk("Failed (nfi_addToFile): no node point info supplied\n");
		}
	}

	newNentry = nfi_nextAvailable();
	//printk("naf 1.1 newNentry %d\n", newNentry);
	string_copy(filename->element, nentry->filename, (newNentry + NENTRYFILENAMEOFFSET));
	//printk("naf 1.2 filename copied to new nentry\n");
	newNentry->parent = tempNentry->parent;
	newNentry->type = 1;
	tempNentry->seqNext = newNentry;
	newNentry->seqPrevious = tempNentry;

	if (fileNentry < 1 && filenameWithPath > 0) {
		mem_updateSysPointer(currentSysPointer - string_splitLength(filename));
		mem_updateSysPointer(currentSysPointer - string_splitLength(currentPath));
	}
	return (unsigned long int)tempNentry;
}


char nfi_release(struct nentryStruct *targetNentry) {
	// when a firstChild is released, make sure the parent nentry is updated with firstChild->nextPeer
	// update nentryHeader->nextAvailable to the released nentry if it precedes the current nextAvilable
	printk("release entered\n");
	if (targetNentry->firstChild > 0) { //what if this is a parent
		return 0; // do nothing and return 0
	}
	struct nentryStruct *nextNentry;
	struct nentryStruct *previousNentry;
	struct nentryStruct *parent;
	parent = 0;
	nextNentry = 0;
	previousNentry = 0;
	if (targetNentry->parent > 0) {
		parent = targetNentry->parent;
	}
	if (targetNentry->peerNext > 0) {
		nextNentry = targetNentry->peerNext;
	}
	if (targetNentry->peerPrevious > 0) {
		previousNentry = targetNentry->peerPrevious;
	}

	printk("release 1.0\n");
	if (parent > 0) { // has a parent?
		printk("release 2.0\n");
		if (targetNentry->isFirstChild > 0) {
			printk("release 2.1\n");
			if (nextNentry > 0) {
				nextNentry->isFirstChild = 1;
				parent->firstChild = nextNentry;
				nextNentry->peerPrevious = 0;
			} else {
				parent->firstChild = 0;
			}
		} else {
			printk("release 2.2\n");
			if (nextNentry > 0) {
				printk("release 2.2.1\n");
				if (previousNentry > 0) {
					nextNentry->peerPrevious = previousNentry;
					previousNentry->peerNext = nextNentry;
				} else {
					nextNentry->peerPrevious = 0;
				}
			} else {
				printk("release 2.2.2\n");
				if (previousNentry > 0) {
					previousNentry->peerNext = 0;
				}
			}
		}
		printk("release 2.3\n");
	} else {
		printk("release 3.0\n");
		if (targetNentry->PID > 0) {
			printk("release 3.1\n");
			if (targetNentry->rootPID > 1) {
				targetNentry->rootPID = 0;
				if (previousNentry > 0) {
					previousNentry->rootPID = 1;
				} else {
					if (nextNentry > 0) {
						nextNentry->rootPID = 1;
					}
				}
			}
			if (nextNentry > 0) {
				printk("release 3.2\n");
				if (previousNentry > 0) {
					nextNentry->peerPrevious = previousNentry;
					previousNentry->peerNext = nextNentry;
				} else {
					nextNentry->peerPrevious = 0;
				}
			} else {
				printk("release 3.3\n");
				if (previousNentry > 0) {
					previousNentry->peerNext = 0;
				}
			}
			printk("release 3.4\n");
		} else {
			printk("release 3.5\n");
			if (targetNentry->fileRoot > 0) {
				if (targetNentry->peerNext > 0 || targetNentry->peerPrevious > 0) {
					return 0;
				}
			}
			if (nextNentry > 0) {
				printk("release 3.7\n");
				if (previousNentry > 0) {
					nextNentry->peerPrevious = previousNentry;
					previousNentry->peerNext = nextNentry;
				} else {
					nextNentry->peerPrevious = 0;
				}
			} else {
				printk("release 3.8\n");
				if (previousNentry > 0) {
					previousNentry->peerNext = 0;
				}
			}
		}
		printk("release 3.9\n");

	}
	printk("release done\n");
	targetNentry->parent = 0;
	targetNentry->PID = 0;
	targetNentry->firstChild = 0;
	targetNentry->isFirstChild = 0;
	targetNentry->type = 0;
	targetNentry->peerNext = 0;
	targetNentry->peerPrevious = 0;
	targetNentry->seqNext = 0;
	targetNentry->seqPrevious = 0;
	return 1;
}

void nfi_blankNentry(struct nentryStruct *targetNentry) {
	targetNentry->parent = 0;
	targetNentry->PID = 0;
	targetNentry->firstChild = 0;
	targetNentry->isFirstChild = 0;
	targetNentry->type = 0;
	targetNentry->peerNext = 0;
	targetNentry->peerPrevious = 0;
	targetNentry->seqNext = 0;
	targetNentry->seqPrevious = 0;
}

char nfi_deleteFileNodes(struct nentryStruct *fileNentry) {
	struct nentryStruct *tempNentry;

	tempNentry = fileNentry;
	while (1) {
		if (tempNentry->seqNext < 1) {
			break;
		}
		tempNentry = tempNentry->seqNext;
		nfi_blankNentry(tempNentry);
	}
	while (1) {
		if (tempNentry->seqPrevious < 1) {
			break;
		}
		tempNentry = tempNentry->seqPrevious;
		nfi_blankNentry(tempNentry);
	}
}


char nfi_releaseByPath(char *sourcePath) {
	struct string_splitStruct *currentPath;
	struct nentryStruct *currentNentry;
	currentPath = string_split(sourcePath, "/", 0);
	if (currentPath < 1) {
		mem_updateSysPointer(currentSysPointer - string_splitLength(currentPath));
		return 0;
	}
	while (1) {
		currentNentry = nfi_pathNextLeg(currentNentry, currentPath->element);
		if (currentNentry < 1) {
			mem_updateSysPointer(currentSysPointer - string_splitLength(currentPath));
			return 0;
		}
		if (currentPath->next < 1) {
			break;
		}
	}
	mem_updateSysPointer(currentSysPointer - string_splitLength(currentPath));
	return nfi_release(currentNentry);
}

char nfi_releaseByPID(unsigned long int PID) {
 	int counter;
	struct nentryStruct *tempNentry, *previousNentry;
	for (counter = 0; counter < nentries; counter++) {
		tempNentry = NFIStart + (counter * SYSNENTRYSIZE);
		if (tempNentry->PID == PID) {
			break;
		}
	}
	while (1) {
		tempNentry = previousNentry->peerNext;
		previousNentry->peerNext = 0;
		previousNentry->peerPrevious = 0;
		tempNentry->PID = 0;
		tempNentry->rootPID = 0;
		tempNentry->type = 0;
		if (tempNentry->next < 1) {
			break;
		}
		previousNentry = tempNentry;
	}
	
}


