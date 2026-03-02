#include "doseAdmin.h"
#include "doseAdmin_internal.h"

unsigned int hash(char *name){ // unsigned means it can store only positive whole number, doubling the positive range
    int length = strnlen(name, MAX_NAME); //count amount of char
    unsigned int hash_value = 0;
    for (int i=0; i < length; i++){ //run for amount of char, adding the total
        hash_value += name[i];
        hash_value = hash_value * name[i] % TABLE_SIZE; // for spreading out the outcome, to increase accuracy, but there are many ways to do this
    }
    return hash_value;
}

void init_hash_table()  {   //TEST, if this resets the table? it should
    for (int i = 0; i < TABLE_SIZE; i++) {
        hash_table[i] = NULL;
    }
    //empty table initialized
}

void * getHashTable() 
{
	// add here the code that returns the entry to your hash table
	return NULL;
}

void print_table()  { //FOR DEBUGGING
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hash_table [i] == NULL) {
            printf("%i  NULL\n", i);
        }
        else{
            printf("%i  %s\n",i, hash_table[i]->name); //print the string of the name and number
        }
    }
}

void to_lowercase(char *string){
    for (size_t i = 0; i < strlen(string); i++){
        string[i] = (char)tolower((unsigned char)string[i]); // changing any character to lowercase
    }
}

void CreatePatientDoseAdmin()
{
	 return;
}
 
void RemoveAllDataFromPatientDoseAdmin()
{
	 return;
}

bool hash_table_insert(person *p){
    if (p == NULL) { //checking if the input isnt nothing
        return false;
    }
    char name_lowercase[MAX_NAME];
    strncpy(name_lowercase, p->name, MAX_NAME); //make a copy, placing the person in the lowercase hased spot, with its original capitilization
    to_lowercase(name_lowercase);
    
    int index = hash(name_lowercase); //index for placing the person
    if (hash_table[index] != NULL) {
        return false;
    }
    hash_table[index] = p;
    return true;
}

int8_t AddPatient(char * patientName)
{
	 return -1;
}

// find a person in the table by their name (number)
person *hash_table_lookup (char *name){
    char name_lowercase[MAX_NAME];
    strncpy(name_lowercase, name, MAX_NAME);
    to_lowercase(name_lowercase); // copying the name for looking up, and changing it to lowercase

    int index = hash(name_lowercase);
    if (hash_table[index] != NULL &&
        strncmp(hash_table[index]->name, name, TABLE_SIZE)==0){ //comparing if the lookup == the name in the space
            return hash_table[index];
    }
    else if (hash_table[index] == NULL){
        printf("ERROR, SPACE IS NULL\n");
        return 0;
    } 
    else{
        printf("ERROR, SPACE IS MISMATCH\n");
        return 0;
    }
}

int8_t SelectPatient(char * patientName)
{
	 return -1;
}

 
int8_t AddPatientDose(Date* date, uint16_t dose)
{
	 return -1;
}
 
int8_t PatientDoseInPeriod(char * patientName, 
                           Date* startDate, Date* endDate, uint32_t* totalDose)
{
	 return -1;
}
 
int8_t RemovePatient(char * patientName)
{
	 return -1;
}
 
int8_t IsPatientPresent(char * patientName)
{
	 return -1;
}

int8_t GetNumberOfMeasurements(char * patientName, 
                               size_t * nrOfMeasurements)
{
	 return -1;
}

void GetHashPerformance(size_t *totalNumberOfPatients, double *averageNumberOfPatients,
                        double *standardDeviation)
{
	 return;
}
				
int8_t WriteToFile(char * filePath)
{
	 return -1;
}

int8_t ReadFromFile(char * filePath)
{
	 return -1;
}

int main(){

    init_hash_table();

    printf("Choose function\n1. Print ");
    
    person sjors = {.name="Sjors", .age = 100, .dosage = 0};
    person vasco = {.name = "Vasco", .age = 18, .dosage = 3};
    person natalie = {.name = "Natalie", .age = 20, .dosage = 3000};

    hash_table_insert(&sjors); //& finds the memory address of the varaible, sjors
    hash_table_insert(&vasco);
    hash_table_insert(&natalie);
    //print_table();



    person *temp = hash_table_lookup("Sjors");

    printf("%s\n", temp->name);

    /*printf("Jacob -> %u\n", hash("Jacob")); // %u is for unsigned
    printf("Natalie -> %u\n", hash("Natalie"));
    printf("Vasco -> %u\n", hash("Vasco"));
    printf("Sjors -> %u\n", hash("Sjors"));*/
    return 0;
}