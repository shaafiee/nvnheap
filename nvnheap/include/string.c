#define STRINGMAX 1048576

struct string_splitStruct {
	struct string_splitStruct *next;
	struct string_splitStruct *previous;
	int length;
	char *element;
};

char string_equal(char *originString, char *targetString) {
	unsigned long int tempStr1, tempStr2;
	tempStr1 = (unsigned long int)originString;
	tempStr2 = (unsigned long int)targetString;
	char matched = 1;
	//printk("seq 1\n");
	while (*originString != (char)0) {
		//printk("seq 2.1\n");
		if (*targetString != (char)*originString) {
			matched = 0;
			break;
		}
		//printk("seq 2.2\n");
		targetString++;
		originString++;
	}
	//printk("seq 3\n");
	originString = tempStr1;
	targetString = tempStr2;
	return matched;
}

char string_concat(char *originString, char *targetString) {
	unsigned long int tempStr1, tempStr2;
	tempStr1 = (unsigned long int)originString;
	tempStr2 = (unsigned long int)targetString;
	char matched = 1;
	while (*targetString != (char)0) {
		targetString++;
	}
	//printk("seq 1\n");
	while (*originString != (char)0) {
		//printk("seq 2.1\n");
		if (*targetString != (char)*originString) {
			matched = 0;
			break;
		}
		//printk("seq 2.2\n");
		targetString++;
		originString++;
	}
	//printk("seq 3\n");
	originString = tempStr1;
	targetString = tempStr2;
	return matched;
}

int string_copy(char *originString, char *targetString, int length) {
	int i;
	int tempLength;
	tempLength = 0;
	if (length > STRINGMAX) {return 0;} // a string cannot store more than 1 MB
	if (length == 0) {
		while (*originString != 0 && *originString !='\0') {
			*targetString = (char)*originString;
			//printk("---> targetString, originString, length: %d, %d, %d\n", targetString, originString, tempLength);
			originString++;
			targetString++;
			tempLength++;
			if (tempLength > STRINGMAX) {return 0;} // a string cannot store more than 1 MB
		}
	} else {
		for (i = 0; i < length; i++) {
			if (*originString == 0 || *originString == '\0') {
				break;
			}
			*targetString = (char)*originString;
			originString++;
			targetString++;
			tempLength++;
			if (tempLength > STRINGMAX) {return 0;} // a string cannot store more than 1 MB
		}
	}
	*targetString = (char)0;
	return tempLength;
}

/*
int string_copy(char *originString, char *targetString, int length) {
	int i;
	char includesNull = 0;
	int tempLength = 0;
	//printk("origin string: %s\n", originString);
	if (length == 0) {
		while (*originString != (char)0) {
			*targetString = (char)*originString;
			targetString++;
			originString++;
			tempLength++;
			if (tempLength > STRINGMAX) {return 0;} // a string cannot store more than 1 MB
		}
	} else {
		if (length > STRINGMAX) {return 0;} // a string cannot store more than 1 MB
		for (i = 0; i < length; i++) {
			if (*originString == (char)0) {
				break;
			}
			*targetString = (char)*originString;
			targetString++;
			originString++;
			tempLength++;
		}
	}
	*targetString = '\0';
	//*targetString = (char)0;
	if (tempLength < 1) {
		return 0;
	}
	//originString = tempOriginPointer;
	//targetString = tempTargetPointer;
	//return (unsigned long int)targetString;
	return tempLength;
}
*/

unsigned long int string_new(char *newString, char *source, unsigned long int customAddress) {
	int length;
	if (customAddress > 0) {
		newString = customAddress;
	} else {
		newString = (unsigned long int)currentSysPointer;
	}
	//printk("source new: %s\n", source);
	if (source > 0) {
		length = string_copy(source, newString, 0);
		if (length < 1) {
			return 0;
		}
	} else {
		*newString = 0;
	}
	if (customAddress > 0) {
		//customAddress += (length + 4);
	} else {
		//currentSysPointer += (length + 4);
		//mem_updateSysPointer(currentSysPointer + (length + 4));
	}
	return (unsigned long int)newString;
}

unsigned long int string_split(char *stringToSplit, char *splitChar, void *tempSysPointer) {
	struct string_splitStruct *currentSlice;
	struct string_splitStruct *previousSlice;
	struct string_splitStruct *firstSlice;
	char customPointer;
	if ((unsigned long int)tempSysPointer < 1) {
		tempSysPointer = (unsigned long int)currentSysPointer;
	} else {
		customPointer = 1;
	}
	int length = 0;
	int scLength = 0; // length in split chars (sc)
	char noMatch = 1;
	unsigned long int matches;
	unsigned long int originalStringStart;
	//currentSlice = (struct string_splitStruct *)malloc(sizeof(struct string_splitStruct)); //user space
	char splicesCreated = 0;
	int numberOfElements = 0;
	int totalSCLength = 0;
	int firstSPinc = 0;
	int rootFound = 0;
	originalStringStart = (unsigned long int)stringToSplit;
	currentSlice = tempSysPointer;
	tempSysPointer += (unsigned int)sizeof(struct string_splitStruct);
	firstSlice = currentSlice;
	//printk("ssp pre1.1 %d\n", originalStringStart);
	while (*stringToSplit != (char)0) {
		scLength = 0;
		//while (*stringToSplit != (char)0) {
		while (1) {
			if (*stringToSplit == (char)*splitChar) {
				//printk("ssp pre1.2\n");
				if (rootFound == 0) {
					rootFound = 1;
					noMatch = 3;
				} else {
					noMatch = 0;
				}
				break;
			}
			scLength++;
			stringToSplit++;
			if (*stringToSplit == (char)0) {
				//printk("encountered\n");
				noMatch = 2;
				break;
			}
		}
		if (noMatch) {
			if (noMatch == 2) {
				if (numberOfElements < 1) {
					tempSysPointer -= (unsigned int)sizeof(struct string_splitStruct);
					return 0;
				} else {
				}

				stringToSplit -= (scLength);
				//printk("ssp 1.4 %s\n", stringToSplit);
				currentSlice->element = string_new(currentSlice->element, 0, tempSysPointer);
				//printk("ssp 1 %d\n", (unsigned long int)stringToSplit);
				string_copy(stringToSplit, currentSlice->element, scLength);
				//printk("ssp 2\n");
				currentSlice->length = scLength;
				matches++;
				previousSlice->next = currentSlice;
				currentSlice->previous = previousSlice;
				//printk("ssp 2.1 %d\n", (unsigned long int)currentSlice);
				printk("ssp 2.2 %s - %d\n", currentSlice->element, (unsigned long int)currentSlice->previous);
				tempSysPointer += scLength;
				currentSlice->next = 0;
				//currentSlice = tempSysPointer;
				//tempSysPointer += sizeof(struct string_splitStruct);
				break;
			}
			if (noMatch == 3) {
				//stringToSplit -= (scLength);
				printk("ssp 1.5\n");
				currentSlice->element = 0;
				//currentSlice->element = string_new(currentSlice->element, 0, tempSysPointer);
				printk("ssp 1.1 root found\n");
				//string_copy(stringToSplit, currentSlice->element, scLength);
				currentSlice->element = 0;
				//printk("ssp 2.1\n");
				currentSlice->length = 0;
				matches++;
				//stringToSplit++;
				//totalSCLength += scLength;
				//tempSysPointer += scLength;
				previousSlice = currentSlice;
				currentSlice->previous = 0;
				if (firstSPinc == 0) {firstSPinc = 1;}
				numberOfElements++;
				noMatch = 1;
				currentSlice = tempSysPointer;
				tempSysPointer += (unsigned int)sizeof(struct string_splitStruct);
			}
		} else {

			if (firstSPinc == 0) {
				tempSysPointer += (unsigned int)sizeof(struct string_splitStruct);
				currentSlice->previous = 0;
			}

			stringToSplit -= (scLength + 1);
			//currentSlice->element = currentSysPointer;
			currentSlice->element = string_new(currentSlice->element, "", tempSysPointer);
			string_copy(stringToSplit, currentSlice->element, scLength);
			currentSlice->length = scLength;
			matches++;
			stringToSplit += scLength;
			totalSCLength += scLength;
			tempSysPointer += scLength;
			if (previousSlice > 0) {
				previousSlice->next = currentSlice;
				currentSlice->previous = previousSlice;
			} else {
				currentSlice->previous = 0;
			}
			previousSlice = currentSlice;
			printk("ssp 2.9 %s - %d\n", currentSlice->element, (unsigned long int)currentSlice->previous);
			if (firstSPinc == 0) {firstSPinc = 1;}
			noMatch = 1;
			numberOfElements++;
			currentSlice = tempSysPointer;
			tempSysPointer += (unsigned int)sizeof(struct string_splitStruct);
			//printk("element: %s\n", previousSlice->element);
		}
		length++;
		stringToSplit++;
	}
	if (customPointer > 0) {
	} else {
		mem_updateSysPointer(tempSysPointer);
	}
	return firstSlice;
}

int string_length(char *originalString) {
	int length = 0;
	//printf("within: %s\n", originalString);
	while (*originalString != (char)0) {
		length++;
		//printf("while: %s\n", originalString);
		//printf("pro: %c\n", (char)*originalString);
		originalString++;
	}
	return length;
}

unsigned long int string_splitStart(struct string_splitStruct *source) {
	struct string_splitStruct *tempString;
	tempString = source;
	while (source->previous > 0) {
		tempString = tempString->previous;
	}
	return (unsigned long int)tempString;
}

int string_splitLength(struct string_splitStruct *source) {
	int length;
	unsigned long int sourceStart;
	length = 0;
	printk("spl 1.0 %d - %d\n", source, source->next);
	if (source < 1) {
		return 0;
	}
	printk("spl 1.1 \n");
	sourceStart = (unsigned long int)source;
	printk("spl 1.2 \n");
	while (1) {
		printk("spl 1.3 \n");
		length = length + (sizeof(struct string_splitStruct) + source->length);
		printk("spl 1.4 %s - %d -- %d\n", source->element, source->next, sizeof(struct string_splitStruct));
		if (source->next < 1) {
			break;
		}
		printk("spl 1.5 \n");
		source = source->next;
	}
	source = sourceStart;
	return length;
}


