#include "doseAdmin.h"
#include "doseAdmin_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // for line 15 and 41
#include <stdbool.h>
#include <ctype.h> // for disabling cap sensitivity

//void * getHashTable();

//int8_t AddPatientDose(Date* date, uint16_t dose);

/*int8_t PatientDoseInPeriod(char * patientName,
                           Date* startDate, Date* endDate, uint32_t* totalDose);*/

//int8_t GetNumberOfMeasurements(char * patientName, size_t * nrOfMeasurements);

//void get_hash_performance(size_t *totalNumberOfPatients, double *averageNumberOfPatients, double *standardDeviation);

//int8_t WriteToFile(char * filePath);

//int8_t ReadFromFile(char * filePath);

unsigned int hash(char *name);

void init_hash_table();

void print_table();

void to_lowercase(char *string);

int8_t hash_table_insert(char *name, int age, int dosage);

int16_t IsPatientPresent(char * name);

int8_t RemovePatient(char *name);

void handle_patient_selection();

int main(){

    init_hash_table();

    hash_table_insert("Sjors", 100, 0);
    hash_table_insert("Vasco", 18, 3);
    hash_table_insert("Natalie", 10, 2);
    //print_table();
    
    bool userInput = false;
    while (userInput == false){
        int function_input;
        printf("Choose Function:\n1. Print Table\n2. Lookup Name\n3. Insert Name\n4. Delete all Table Data\n5. Calculate Table Performance\n6. Exit and Delete Data\n");
        
        scanf("%i", &function_input);

        switch(function_input) {
            case(1):
                print_table();
                break;
            case(2):
                handle_patient_selection();
                break;
            case(3):
                printf("output:3");
                break;
            case(4):
                init_hash_table();
                break;
            case(5):
                //get_hash_performance();
                break;
            case(6):
                return 0;
        }
            
    }
}

void * getHashTable() 
{
	// add here the code that returns the entry to your hash table
	return NULL;
}

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

void display_patient_details(int16_t index) {
    if (index < 0 || index > TABLE_SIZE) {
        printf("ERROR: Invalid patient index\n");
        return;
    }
    printf("\n  --- Patient Details ---\n");
    printf("\tName: %s\n", hash_table[index].name);
    printf("\tAge: %d\n", hash_table[index].age);
    printf("\tDosage: %d\n\n");
}



unsigned int hash(char *name){ // unsigned means it can store only positive whole number, doubling the positive range
    int length = strnlen(name, MAX_NAME); //count amount of char
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
int8_t hash_table_insert(char *name, int age, int dosage){
    if (name == NULL || name[0] == '\0') { //checking if the input isnt nothing
        return 0;
    }
    char name_lowercase[MAX_NAME];
    strncpy(name_lowercase, name, MAX_NAME -1 ); //make a copy, placing the patient in the lowercase hashed spot, with its original capitilization
    name_lowercase[MAX_NAME - 1] = '\0';
    to_lowercase(name_lowercase);
    
    int index = hash(name_lowercase); //index for placing the patient

    if (hash_table[index].name[0] != '\0') {
        printf("ERROR: PLACE IS TAKEN");
        return 0;
    }
    strncpy(hash_table[index].name, name, MAX_NAME - 1);
    hash_table[index].name[MAX_NAME - 1] = '\0';
    hash_table[index].age = age;
    hash_table[index].dosage = dosage;
    return 1;
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

};

// use int8_t RemovePatient(char * patientName)
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


void handle_patient_selection(){
    char patient_name[MAX_NAME];
    int index;

    printf("Enter name to search\n");
    scanf("%49s", patient_name);

    if (patient_name[0] == '\0'){
        printf("\nNAME ERROR PLEASE TRY AGAIN\n"); //remove
        return;
    } 

    index = IsPatientPresent(patient_name);
    if (index == -1){
        return;
    } 

    bool valid_input = false;
    while (valid_input == false){
    
        int name_function_input;
        printf("Selected Patient: %s\nChoose Name Function:\n1. View Patient Details\n2. Remove User\n3. Exit\n", hash_table[index].name);//edit to use ptr
        scanf("%i", &name_function_input);
        switch(name_function_input){
            case(1):
                printf("\nName: %s\nAge: %i\nDose: %i\n\n", 
                    hash_table[index].name, 
                    hash_table[index].age, 
                    hash_table[index].dosage);
                break;
            case(2):
                if (RemovePatient(patient_name) == 0){
                    printf("Remove patient success\n");
                    return;
                }
                break;
            case(3):
                return;
        }

    }
    
}