#ifndef DOSEADMIN_H
#define DOSEADMIN_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // for line 15 and 41
#include <stdbool.h>
#include <ctype.h> // for disabling cap sensitivity

#define MAX_NAME	80
#define TABLE_SIZE  255

typedef struct{ //struct is a group of variables, typedef is for initializing a variable (type)
    char name[MAX_NAME];
    int age;
    int dosage;
    // can add more later
} patient; 

typedef struct {
	uint8_t   day;    // value in range [1, 31]
	uint8_t   month;  // value in range [1, 12]
	uint16_t  year;   // value in range [1900, 2500]
} Date;

extern patient hash_table[TABLE_SIZE]; // Use : patient hash_table[TABLE_SIZE]; Extern so all files get the same copy of the table

unsigned int hash(char *name);

/*
* Initialize the hash table (clear all entries)
*/
void init_hash_table();



bool hash_table_insert(char *name, int age, int dosage);

/*
* Print all patients in the hash table (for debugging)
*/
void print_table();

/**************************************************************************************
 * Changes characters to lowercase for searching
 * ************************************************************************************
 */
void to_lowercase(char *string);

/***************************************************************************************
 * Removes the patient from the administration
 * 
 * Returns -1 when the passed patientName is not present 
 * Returns -2 when string length of patientName exceeds MAX_PATIENTNAME_SIZE
 * Returns  0 when the patient data is successfully removed from the administration
 * 
 * It is a precondition that patientName is not NULL and is \0 terminated
 */
int8_t RemovePatient(char *name);

/***************************************************************************************
 * Selects the patient as the active patient (selected patient)
 *
 * Returns -1 when the passed patientName is not present 
 * Returns -2 when string length of patientName exceeds MAX_PATIENTNAME_SIZE
 * Returns  0 when the patient is successfully selected
 * 
 * It is a precondition that patientName is not NULL and is \0 terminated
 */
int16_t IsPatientPresent(char * name);

/***************************************************************************************
 * Checks if the passed patientName is present in the administration
 * 
 * Returns -1 when the passed patientName is not present 
 * Returns -2 when string length of patientName exceeds MAX_PATIENTNAME_SIZE
 * Returns 0 when the patientName is present
 * 
 * It is a precondition that patientName is not NULL and is \0 terminated
 */


void display_patient_details(int16_t index);


/***************************************************************************************
 * Selects the patient as the active patient (selected patient)
 *
 * Returns -1 when the passed patientName is not present 
 * Returns -2 when string length of patientName exceeds MAX_PATIENTNAME_SIZE
 * Returns  0 when the patient is successfully selected
 * 
 * It is a precondition that patientName is not NULL and is \0 terminated
 */
void SelectPatient(char *name);


					   
//void RemoveAllDataFromPatientDoseAdmin();




/***************************************************************************************
 * Adds the dose a patient received during an examination at a particular date in 
 * the administration. Dose is added to the selected patient.
 * 
 * Returns -1 when allocation of memory failed
 * Returns -2 when string length of patientName exceeds MAX_PATIENTNAME_SIZE
 * Returns  0 when the data is successfully copied into the administration
 * 
 * It is also a precondition that date is not NULL
 * It is also a precondition that dose is not 0
 */
//int8_t AddPatientDose(Date* date, uint16_t dose);


/***************************************************************************************
 * Returns the total dose a patient received in passed period.
 * 
 * Returns -1 when the passed patientName is unknown
 * Returns -2 when string length of patientName exceeds MAX_PATIENTNAME_SIZE
 * Returns  0 when the totalDose is  updated successfully
 * 
 * It is a precondition that patientName is not NULL and is \0 terminated
 * It is also a precondition that both dates and totalDose are not NULL
 */
//int8_t PatientDoseInPeriod(char * patientName, 
//                           Date* startDate, Date* endDate, uint32_t* totalDose);


/***************************************************************************************
 * Returns the number of dose measurements done for a given patient
 *
 * Returns -1 when the passed patientName is not present 
 * Returns -2 when string length of patientName exceeds MAX_PATIENTNAME_SIZE
 * Returns 0 otherwise, the number of measurements is put in nrOfMeasurements
 * 
 * It is a precondition that patientName is not NULL and is \0 terminated
 */
//int8_t GetNumberOfMeasurements(char * patientName, 
//                               size_t * nrOfMeasurements);
								

/***************************************************************************************
 * Writes all patient data to a text file in the table
 * 
 * Returns 0 on success
 * Returns -1 on faillure
 */
//int8_t WriteToFile(char * filePath);



/***************************************************************************************
 * Reads all patient data from a text file, and put the data in an empty table.
 * 
 * Returns 0 on success
 * Returns -1 on faillure
 */
//int8_t ReadFromFile(char * filePath);

#endif
