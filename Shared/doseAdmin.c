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

unsigned int hash(char *name){ // unsigned means it can store only positive whole number, doubling the positive range
    int length = strlen(name); //count amount of char
    if (length < 3){
        return 0;
    }
    unsigned int hashValue = 0;
    for (int i=0; i < 3; i++){ 
        hashValue += name[i];
        hashValue = hashValue * name[i]; // brice: check the behavior with getHashPerformance (sprint4)
    }
    // Use module one time at the end
    return hashValue % TABLE_SIZE;
}

void initPatientDoseAdmin(){
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL; // Empty string
        //hashTable[i].name[0]='\0';
    }

    addPatient("JohnDoe");
    selectPatient("JohnDoe");
}

void resetPatientDoseAdmin(){
    Patient* current = NULL;
    Patient* next = NULL;
    selectedPatient = NULL;

    for (int i = 0; i < TABLE_SIZE; i++) {
        current = hashTable[i];
        while(current != NULL){
            next = current->next;
            free(current);
            current = next;
        }
        hashTable[i] = NULL;
    }
}



void printTable(){ //FOR DEBUGGING
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashTable[i] != NULL) {
            printf("%i  %s\n",i, hashTable[i]->name); //print the string of the name and number

            if (hashTable[i]->next != NULL){ //check linkedlist
                int listPos = 1; //reset counter
                Patient* current = hashTable[i]->next; //set to 1st person after head

                while (current != NULL){
                    printf("%i - %i %s", i, listPos, current->name); //print name

                    current = current->next;
                    listPos++;
                }
            }
        }
    }
}


void toLowercase(char *nameLowercase, const char *name){
    size_t i;
    for (i = 0; i < MAX_NAME && name[i] != '\0'; i++){
        nameLowercase[i] = (char)tolower(name[i]);
    }
    nameLowercase[i] = '\0';
}


// you need to read the header file (return values)
int8_t addPatient(char *name){
    if (sizeof(name[80]) > sizeof(char)){
        return -1;
    }
    char nameLowercase[MAX_NAME];
    toLowercase(nameLowercase, name);

    int index = hash(nameLowercase);

    if (isPatientPresent(name) != NULL){
        return -1;
    }

    Patient* tempPat = calloc(1, sizeof(Patient)); // free function for freeing memory, otherwise memory will stay allocated

    if (tempPat == NULL){
        return -3; // HEAP FULL
    }
    
    strncpy(tempPat->name, nameLowercase, MAX_NAME);

    if (hashTable[index] == NULL) {
        hashTable[index] = tempPat;
    } else {
        hashTable[index]->prev = tempPat;
        tempPat->next = hashTable[index];
        hashTable[index] = tempPat;
    }

    return 0;
}


Patient* isPatientPresent(char * name){
    char inputNameLowercase[MAX_NAME];
    toLowercase(inputNameLowercase, name);

    int index = hash(inputNameLowercase);

    if (hashTable[index] == NULL){
        return NULL;
    }

    Patient* current = hashTable[index];
    while (current != NULL){
        if (strncmp(current->name, inputNameLowercase, MAX_NAME) == 0){
            return current;
        }
        current = current->next;
    }

    return NULL;
}


int8_t selectPatient(char* name){
    Patient* ptr = isPatientPresent(name);
    if (ptr == NULL){
        return -1; // patient not found
    }
    selectedPatient = ptr;
    return 0;
}



void managePatient(){
    char patientName[MAX_NAME];
    strncpy(patientName, selectedPatient->name, MAX_NAME);

//    handlePatientSelection(patientName);
    return;
}



int8_t removePatient(char *name){

    Patient *ptr = isPatientPresent(name);
    if(ptr == NULL){
        return -1;
    }

    char inputNameLowercase[MAX_NAME];
    toLowercase(inputNameLowercase, name);
    int index = hash(inputNameLowercase);

    if (strncmp("johndoe", inputNameLowercase, MAX_NAME) == 0){
        return -1;
    }
    if (strncmp(ptr->name, inputNameLowercase, MAX_NAME) != 0){
        return -1;
    }
    

    if (ptr->prev == NULL){ //head of chain
        hashTable[index] = ptr->next;
        free(ptr);
        return 0;
    }

    if (ptr->next == NULL){ //Tail of chain
        ptr->prev->next = NULL;
        free(ptr);
        return 0;
    }
        
    ptr->next->prev = ptr->prev;
    ptr->prev->next = ptr->next;
    free(ptr);
    return 0;

    /*Patient *current = hashTable[index]; // start at head 
    Patient* prev = NULL;
    while(current != NULL){
        if(strncmp(current->name, inputNameLowercase, MAX_NAME) == 0){
            if (current->next != NULL){
                prev->next = current->next;
            }
            free(current);
            return 0;
        }
        prev = current;
        current = current->next;
    }*/
}



int8_t addPatientDose(uint16_t dosage){

    if(selectedPatient == NULL){
        return -1;
    }

    time_t t = time(NULL);
    struct tm* timePtr;
    timePtr = localtime(&t);

    int doseCnt = selectedPatient->doseCount;
    selectedPatient->doseData[doseCnt].date.day   = timePtr->tm_mday;
    selectedPatient->doseData[doseCnt].date.month = timePtr->tm_mon;
    selectedPatient->doseData[doseCnt].date.year  = timePtr->tm_year;
    selectedPatient->doseData[doseCnt].dosage     = dosage;
    selectedPatient->doseCount++;
	return 0;
}

int8_t writeToFile(char * filePath){

    FILE *record = fopen(filePath, "wb"); //write (binary mode)

    if (record == NULL){
        return -1; // memory FULL
    }

    Patient* current = NULL;  //RESETPATDOSEADMIN CODE STOLEN
    Patient* next = NULL;
    selectedPatient = NULL;

    for (int i = 0; i < TABLE_SIZE; i++) {
        current = hashTable[i];
        while(current != NULL){
            next = current->next; //save next so we can move on at the end

            current->prev = NULL;
            current->next = NULL;

            if (fwrite(current, sizeof(Patient), 1, record) != 1){  // write the entire patient structure data
                fclose(record);
                return -1; // memory FULL
            }
            free(current);
            current = next; //move on, nothing left from previous current
        }
        hashTable[i] = NULL; //reset table
    }

    fclose(record);
    return 0;
}

int8_t readFromFile(char * filePath){

    FILE *record = fopen(filePath, "rb"); //read (binary mode)

    if(record == NULL){
        return -1; // NO FILE EXISTS
    }

    size_t sizeOfPatient = sizeof(Patient);

    fseek(record, 0, SEEK_END); //set pos to end of file
    int patientCount = ftell(record) / sizeOfPatient;  //ftell finds amount of bytes since start

    fseek(record, 0, SEEK_SET);
    Patient* pat = calloc(1, sizeof(Patient));
    Patient* new = NULL;

    for(int i = 0; i< patientCount; i++){
        fread(pat, sizeof(Patient), 1, record); //store entrie struct in pat

        if (addPatient(pat->name) != 0){ //add name, allocate memory, handle pointers
            return -2 ; //ADDPATIENT ERROR
        }
        new = isPatientPresent(pat->name); //pointer now to variable new for adding data

        new->doseCount = pat->doseCount;
        for (int i = 0; i < pat->doseCount; i++){ //copy dosecount and dosedata depending on size
            new->doseData[i] = pat->doseData[i];
        }
    }

    fclose(record);
    free(pat);
    return 0;
}