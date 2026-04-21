#include "doseAdmin.h"
#include "doseAdmin_internal.h"
#include "../Product/menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>


// Brice: use Patient
Patient* hashTable[TABLE_SIZE]; // NULL = empty slot
// no extern keyword => hashTable only visible in doseAdmin.c
// implement pritn function here and call it from main.c

Patient* selectedPatient = NULL;

Patient* johnDoePtr = NULL;

//int8_t patientDoseInPeriod(char * patientName,
//                           date* startDate, date* endDate, uint32_t* totalDose){
//	 return -1;
//}

int8_t getNumberOfMeasurements(char * patientName, size_t * nrOfMeasurements){
	 return -1;
}

void getHashPerformance(size_t *totalNumberOfPatients, double *averageNumberOfPatients,
                        double *standardDeviation){
	 return;
}
				
int8_t writeToFile(char * filePath){
	 return -1;
}

int8_t readFromFile(char * filePath){
	 return -1;
}


unsigned int hash(char *name){ // unsigned means it can store only positive whole number, doubling the positive range
    int length = strlen(name); //count amount of char
    unsigned int hashValue = 0;
    for (int i=0; i < length; i++){ 
        hashValue += name[i];
        hashValue = hashValue * name[i]; // brice: check the behavior with getHashPerformance (sprint4)
    }
    // Use module one time at the end
    return hashValue % TABLE_SIZE;
}

void initHashTable(){

    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL; // Empty string
        //hashTable[i].name[0]='\0';
    }

    addPatient("JohnDoe");
    //johnDoePtr = isPatientPresent("JohnDoe");
    //selectPatient("JohnDoe");
}



void printTable(){ //FOR DEBUGGING
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashTable[i] != NULL && hashTable[i]->name[0] != '\0') {
            printf("%i  %s\n",i, hashTable[i]->name); //print the string of the name and number
        }
    }
}


void printPatientData(char *name){ // NO PRINTF's, return data from empty pointers initalized in menu.c
    printf("\n  --- Patient Details ---\n");
    printf("\tName: %s\n", selectedPatient->name);
    //printDosage(index);
    return;
}



void toLowercase(char *nameLowercase, const char *name){
    size_t i;
    for (i = 0; i < MAX_NAME - 1 && name[i] != '\0'; i++){
        nameLowercase[i] = (char)tolower(name[i]);
    }
    nameLowercase[i] = '\0';
}


// you need to read the header file (return values)
int8_t addPatient(char *name){
    char nameLowercase[MAX_NAME];
    toLowercase(nameLowercase, name);

    int index = hash(nameLowercase);

    if (hashTable[index] != NULL) {
        return -2;
    }
    
    hashTable[index] = malloc(sizeof(Patient)); // free function for freeing memory, otherwise memory will stay allocated

    if (hashTable[index] == NULL){
        return -3; // HEAP FULL
    }

    strncpy(hashTable[index]->name, name, MAX_NAME - 1);
    hashTable[index]->name[MAX_NAME - 1] = '\0';
    hashTable[index]->doseCount = 0;
    
    return 0;
}



Patient* isPatientPresent(char * name){
    char inputNameLowercase[MAX_NAME];
    toLowercase(inputNameLowercase, name);

    int index = hash(inputNameLowercase);


    if (hashTable[index] == NULL){
        return NULL;
    }


    char *storedNameLowercase[MAX_NAME];
    toLowercase(storedNameLowercase[MAX_NAME], hashTable[index]->name);


    if (strncmp(storedNameLowercase, inputNameLowercase, MAX_NAME) == 0){
        return hashTable[index];
    }
     
    Patient* current = hashTable[index]; 
    while (current != NULL){
        toLowercase(storedNameLowercase, current->name);
        if (strncmp(storedNameLowercase, inputNameLowercase, MAX_NAME) == 0){
            return current;
        }
        current = current->next;
    }

    return NULL;
}


int8_t selectPatient(char* name){
    int index = isPatientPresent(name);
    if(index == -2){
        return -2; // patient not found
    }
    if(index == -1){
        return -1; // hash collision, different patient at slot
    }

    selectedPatient = index;
    strncpy(hashTable[index]->name, name, MAX_NAME - 1);
    return 0;
}



void managePatient(){
    char patientName[MAX_NAME];
    strncpy(patientName, selectedPatient->name, MAX_NAME);

    handlePatientSelection(patientName);
    return;
}



bool removePatient(char name){
    Patient *patPtr = isPatientPresent(name);
    if(patPtr == NULL){
        return false;
    }

    char inputNameLowercase[MAX_NAME];
    toLowercase(inputNameLowercase, name);
    int index = hash(inputNameLowercase);

    char tempName[MAX_NAME];
    toLowercase(tempName, hashTable[index]->name);

    if (strncmp(tempName, inputNameLowercase, MAX_NAME) == 0){ // patient is at the head of the chain
        if (hashTable[index]->next != NULL){
            hashTable[index] = hashTable[index]->next;
            hashTable[index]->prev = NULL;
        } else {
            hashTable[index] = NULL;
        }
        free(patPtr);
        return true;
    }

        Patient *current = hashTable[index]; // start at head 
        while(current != NULL){
            toLowercase(tempName, current->name);
            if(strncmp(tempName, inputNameLowercase, MAX_NAME) == 0){
                if (current->next != NULL){
                    current->prev->next = current->next;
                    current->next->prev = current->prev;
                } else {
                    current->prev->next = NULL;
                }
                free(current);
                return true;
            }
            current = current->next;
        }

    return false;
}



int8_t addPatientDose(uint16_t dosage){
    int index = selectedPatient;
    
    time_t t = time(NULL);
    struct tm* timePtr;
    timePtr = localtime(&t);


    int doseCnt = hashTable[index]->doseCount;
    hashTable[index]->doseData[doseCnt].date.day = timePtr-> tm_mday;
    hashTable[index]->doseData[doseCnt].date.month = timePtr-> tm_mon;
    hashTable[index]->doseData[doseCnt].date.year = timePtr-> tm_year;

    hashTable[index]->doseData[doseCnt].dosage = dosage;

    hashTable[index]->doseCount++;
	return 0;
}