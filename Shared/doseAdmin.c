#include "doseAdmin.h"
#include "doseAdmin_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Global hash table definition (extern declared in doseAdmin.h)
patient* hashTable[TABLE_SIZE];
 
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
        hashValue = hashValue * name[i];
    }
    // Use module one time at the end
    return hashValue % TABLE_SIZE;
}

void initHashTable(){

    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL; // Empty string
        //hashTable[i]->name[0]='\0';
        //hashTable[i]->age = 0;
    }

    hashTableInsert("JohnDoe", 1); // TODO: remove test data
    //empty table initialized
}

void printTable(){ //FOR DEBUGGING
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashTable[i]->name[0] != '\0') {
            printf("%i  %s\n",i, hashTable[i]->name); //print the string of the name and number
        }
    }
}

void toLowercase(char *string){
    size_t length = strlen(string);
    for (size_t i = 0; i < length && i< MAX_NAME -1; i++){
        string[i] = (char)tolower((unsigned char)string[i]); // changing any character to lowercase
    }
    string[length] = '\0';
}

bool hashTableInsert(char *name, int age){
    if (name == NULL || name[0] == '\0') { //checking if the input isnt nothing
        return false;
    }
    char nameLowercase[MAX_NAME];
    strncpy(nameLowercase, name, MAX_NAME -1); //make a copy, placing the patient in the lowercase hashed spot, with its original capitilization
    nameLowercase[MAX_NAME - 1] = '\0';
    toLowercase(nameLowercase);
    
    int index = hash(nameLowercase);

    if (hashTable[index]->name[0] != '\0') {
        printf("ERROR: PLACE IS TAKEN");
        return false;
    }
    strncpy(hashTable[index]->name, name, MAX_NAME - 1);
    hashTable[index]->name[MAX_NAME - 1] = '\0';
    hashTable[index]->age = age;
    //printf
    //addPatientDose(date, dose);
    return true;
}

bool isPatientPresent(char * name){
    char nameLowercase[MAX_NAME];
    strncpy(nameLowercase, name, MAX_NAME - 1);
    nameLowercase[MAX_NAME - 1] = '\0';
    toLowercase(nameLowercase); // copying the name for looking up, and changing it to lowercase

    int index = hash(nameLowercase);

    if (hashTable[index]->name[0] != '\0'){
        char storedNameLowercase[MAX_NAME];
        strncpy(storedNameLowercase, hashTable[index]->name, MAX_NAME - 1);
        storedNameLowercase[MAX_NAME - 1] = '\0';
        toLowercase(storedNameLowercase);

        if (strncmp(storedNameLowercase, nameLowercase, MAX_NAME) == 0){
            return index;
        } else {
            printf("ERROR: PLACE IS TAKEN\n");
            return -1;
        }
    } else {
        printf("ERROR, SPACE IS NULL\n");
        return -1;
    }

}

void selectPatient(){
    char name[MAX_NAME];
    printf("Enter name to select patient: ");
    if (scanf("%s", name) != 1){
        printf("ERROR: Invalid Input.");
        return;
    }

    int index = isPatientPresent(name);
    if (index == -1){
        return;
    }

    handlePatientSelection(index);
}

bool removePatient(char *name){
    int index = isPatientPresent(name);

    if (index == -1){
        return false;
    }

    if (strncmp(hashTable[index]->name, "JohnDoe", MAX_NAME) == 0){
        return false;
    }
    hashTable[index]->name[0] = '\0';
    hashTable[index]->age = 0;
    hashTable[index]->doseCount = 0;
    
    for (int i=0; i<MAX_DOSE_MEASUREMENTS; i++){
        hashTable[index]->doses[i]->dosage = 0;
        hashTable[index]->doses[i]->date[0] = '\0';
    }
    hashTable[index] = NULL;
    return true;
}

int8_t addPatientDose(uint8_t index, uint16_t date, uint16_t dosage){

    hashTable[index]->doseCount++;
	return -1;
}