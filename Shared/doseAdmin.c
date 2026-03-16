#include "doseAdmin.h"
#include "doseAdmin_internal.h"
#include "../Product/menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // for line 15 and 41
#include <stdbool.h>
#include <ctype.h> // for disabling cap sensitivity

 // Global hash table definition (inter-file memory allocation because of extern in .h)
patient hash_table[TABLE_SIZE];

//SelectPatient(char name[MAX_NAME]);

//int8_t AddPatientDose(Date* date, uint16_t dose);

/*int8_t PatientDoseInPeriod(char * patientName,
                           Date* startDate, Date* endDate, uint32_t* totalDose);*/

//int8_t GetNumberOfMeasurements(char * patientName, size_t * nrOfMeasurements);

//void get_hash_performance(size_t *totalNumberOfPatients, double *averageNumberOfPatients, double *standardDeviation);

//int8_t WriteToFile(char * filePath);

//int8_t ReadFromFile(char * filePath);

int8_t AddPatientDose(Date* date, uint16_t dose){
	return -1;
}
 
int8_t PatientDoseInPeriod(char * patientName,
                           Date* startDate, Date* endDate, uint32_t* totalDose){
	 return -1;
}

int8_t GetNumberOfMeasurements(char * patientName, size_t * nrOfMeasurements){
	 return -1;
}

void get_hash_performance(size_t *totalNumberOfPatients, double *averageNumberOfPatients,
                        double *standardDeviation){
	 return;
}
				
int8_t WriteToFile(char * filePath){
	 return -1;
}

int8_t ReadFromFile(char * filePath){
	 return -1;
}




unsigned int hash(char *name){ // unsigned means it can store only positive whole number, doubling the positive range
    int length = strlen(name); //count amount of char
    unsigned int hash_value = 0;
    for (int i=0; i < length; i++){ //run for amount of char, adding the total
        hash_value += name[i];
        hash_value = hash_value * name[i] % TABLE_SIZE; // for spreading out the outcome, to increase accuracy, but there are many ways to do this
    }
    // Use module one time at the end
    return hash_value;
}

void init_hash_table(){

    for (int i = 0; i < TABLE_SIZE; i++) {
        hash_table[i].name[0] = '\0'; // Empty string
        hash_table[i].age = 0;
        hash_table[i].dosage = 0;
    }
    //empty table initialized
}

void print_table(){ //FOR DEBUGGING
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hash_table[i].name[0] != '\0') {
            printf("%i  %s\n",i, hash_table[i].name); //print the string of the name and number
        }
    }
}

void to_lowercase(char *string){
    for (size_t i = 0; i < strlen(string) && i< MAX_NAME -1; i++){
        string[i] = (char)tolower((unsigned char)string[i]); // changing any character to lowercase
    }
}

// use addPatient instead and call it from the menu
bool hash_table_insert(char *name, int age, int dosage){
    if (name == NULL || name[0] == '\0') { //checking if the input isnt nothing
        return false;
    }
    char name_lowercase[MAX_NAME];
    strncpy(name_lowercase, name, MAX_NAME -1 ); //make a copy, placing the patient in the lowercase hashed spot, with its original capitilization
    name_lowercase[MAX_NAME - 1] = '\0';
    to_lowercase(name_lowercase);
    
    int index = hash(name_lowercase); //index for placing the patient

    if (hash_table[index].name[0] != '\0') {
        printf("ERROR: PLACE IS TAKEN");
        return false;
    }
    strncpy(hash_table[index].name, name, MAX_NAME - 1);
    hash_table[index].name[MAX_NAME - 1] = '\0';
    hash_table[index].age = age;
    hash_table[index].dosage = dosage;
    return true;
}

// use isPatientPrsent instead
// find a patient in the table by their name (number) 
int16_t IsPatientPresent(char * name){
    char name_lowercase[MAX_NAME];
    strncpy(name_lowercase, name, MAX_NAME - 1);
    name_lowercase[MAX_NAME - 1] = '\0';
    to_lowercase(name_lowercase); // copying the name for looking up, and changing it to lowercase

    int index = hash(name_lowercase);

    if (hash_table[index].name[0] != '\0'){
        char stored_name_lowercase[MAX_NAME];
        strncpy(stored_name_lowercase, hash_table[index].name, MAX_NAME - 1);
        stored_name_lowercase[MAX_NAME - 1] = '\0';
        to_lowercase(stored_name_lowercase);

        if (strncmp(stored_name_lowercase, name_lowercase, MAX_NAME) == 0){
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

void SelectPatient(char *name){
    int index = IsPatientPresent(name);

    if (index == -1){
        return;
    }

    handle_patient_selection(index);
}

int8_t RemovePatient(char *name){
    int index = IsPatientPresent(name);

    if (index == -1){
        return -1;
    }

    hash_table[index].name[0] = '\0';
    hash_table[index].age = 0;
    hash_table[index].dosage = 0;
    
    return 0;
}
