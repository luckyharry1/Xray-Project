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
int selectedPatient = 254;


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


unsigned int hash(char *name);

void initHashTable();

void printTable();

void printPatientData(char *name);

void toLowercase(char *string);

int8_t addPatient(char *name);

int16_t isPatientPresent(char * name);

int8_t selectPatient(char *name);

void managePatient();

bool removePatient();

int8_t addPatientDose(uint16_t dosage);



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
}



void printTable(){ //FOR DEBUGGING
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashTable[i] != NULL && hashTable[i]->name[0] != '\0') {
            printf("%i  %s\n",i, hashTable[i]->name); //print the string of the name and number
        }
    }
}


void printPatientData(char *name){ // NO PRINTF's, return data from empty pointers initalized in menu.c
    int index = selectedPatient;

    printf("\n  --- Patient Details ---\n");
    printf("\tName: %s\n", hashTable[index]->name);
    //printDosage(index);
    return;
}



void toLowercase(char *string){
    size_t length = strlen(string);
    for (size_t i = 0; i < length && i< MAX_NAME -1; i++){
        string[i] = (char)tolower((unsigned char)string[i]); // changing any character to lowercase
    }
    string[length] = '\0';
}


// you need to read the header file (return values)
int8_t addPatient(char *name){
    char nameLowercase[MAX_NAME];
    strncpy(nameLowercase, name, MAX_NAME - 1);
    nameLowercase[MAX_NAME - 1] = '\0';
    toLowercase(nameLowercase); // copying the name for looking up, and changing it to lowercase
    
    int index = hash(nameLowercase);

    if (hashTable[index] != NULL) {
        return -2;
    }
    
    hashTable[index] = malloc(sizeof(Patient)); // free function for freeing memory, otherwise memory will stay allocated
    for(int i=0; i<MAX_DOSE_MEASUREMENTS; i++){
        hashTable[index]->doseData[i] = malloc(sizeof(doseData));
    }

    if (hashTable[index] == NULL){
        return -3; // HEAP FULL
    }

    strncpy(hashTable[index]->name, name, MAX_NAME - 1);
    hashTable[index]->name[MAX_NAME - 1] = '\0';
    hashTable[index]->doseCount = 0;
    
    return 0;
}



int16_t isPatientPresent(char * name){
    char nameLowercase[MAX_NAME];
    strncpy(nameLowercase, name, MAX_NAME - 1);
    nameLowercase[MAX_NAME - 1] = '\0';
    toLowercase(nameLowercase); // copying the name for looking up, and changing it to lowercase

    int index = hash(nameLowercase);

    if (hashTable[index] != NULL && hashTable[index]->name[0] != '\0'){
        char storedNameLowercase[MAX_NAME];
        strncpy(storedNameLowercase, hashTable[index]->name, MAX_NAME - 1);
        storedNameLowercase[MAX_NAME - 1] = '\0';
        toLowercase(storedNameLowercase);

        if (strncmp(storedNameLowercase, nameLowercase, MAX_NAME) == 0){
            return index;
        } else {
            //printf("ERROR: PATIENT ALREADY ADDED\n");
            return -1;
        }
    } else {
        //printf("ERROR, SPACE IS TAKEN\n");
        return -2;
    }

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
    int index = selectedPatient;

    char patientName[MAX_NAME];
    strncpy(patientName, hashTable[index]->name, MAX_NAME); 
    
    handlePatientSelection(patientName);
    return;
}



bool removePatient(){
    int index = selectedPatient;

    if (strncmp(hashTable[index]->name, "JohnDoe", MAX_NAME) == 0){
        return false;
    }

    free(hashTable[index]);
    hashTable[index] = NULL;
    
    /*for (int i=0; i<MAX_DOSE_MEASUREMENTS; i++){
        hashTable[index]->doseData[i]->dosage = 0;
        hashTable[index]->doseData[i]->date.day = 0;
        hashTable[index]->doseData[i]->date.month = 0;
        hashTable[index]->doseData[i]->date.year = 0;
    }*/
    return true;
}



int8_t addPatientDose(uint16_t dosage){
    int index = selectedPatient;
    
    time_t t = time(NULL);
    struct tm* timePtr;
    timePtr = localtime(&t);


    int doseCnt = hashTable[index]->doseCount;
    hashTable[index]->doseData[doseCnt]->date.day = timePtr-> tm_mday;
    hashTable[index]->doseData[doseCnt]->date.month = timePtr-> tm_mon;
    hashTable[index]->doseData[doseCnt]->date.year = timePtr-> tm_year;

    hashTable[index]->doseData[doseCnt]->dosage = dosage;

    hashTable[index]->doseCount++;
	return 0;
}