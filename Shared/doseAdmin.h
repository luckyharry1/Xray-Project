#ifndef DOSEADMIN_H
#define DOSEADMIN_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_NAME	80
#define TABLE_SIZE  256

#define MAX_DOSE_MEASUREMENTS 10

/*typedef struct {
	uint8_t day;    // value in range [1, 31]
	uint8_t month;  // value in range [1, 12]
	uint16_t year;  // value in range [1900, 2500]
} date;*/

typedef struct{
    uint16_t dosage; //dose in mg
    char date[11];
} doseData;

typedef struct {
    char name[MAX_NAME];
    int age;
    doseData* doses[MAX_DOSE_MEASUREMENTS];    // dynamically allocated array of dose records
    int doseCount;
} patient;

extern patient* hashTable[TABLE_SIZE]; // NULL = empty slot
extern int selectedPatientIndex;      // -1 when no patient is selected

unsigned int hash(char *name);

/*
* Initialize the hash table (clear all entries)
*/
void initHashTable();

bool hashTableInsert(char *name, int age);

/*
* Print all patients in the hash table (for debugging)
*/
void printTable();

/**************************************************************************************
 * Changes characters to lowercase for searching
 * ************************************************************************************
 */
void toLowercase(char *string);

/***************************************************************************************
 * Removes the patient from the administration
 *
 * Returns -1 when the passed patientName is not present
 * Returns  0 when the patient data is successfully removed from the administration
 *
 * It is a precondition that patientName is not NULL and is \0 terminated
 */
int8_t removePatient(char *name);

/***************************************************************************************
 * Selects the patient as the active patient (selected patient)
 *
 * It is a precondition that patientName is not NULL and is \0 terminated
 */
void selectPatient();


/***************************************************************************************
 * Checks if the passed patientName is present in the administration
 *
 * Returns -1 when the passed patientName is not present
 * Returns the table index when the patientName is present
 *
 * It is a precondition that patientName is not NULL and is \0 terminated
 */
int16_t isPatientPresent(char * name);

/* Forward declaration to avoid circular include with menu.h */
void handlePatientSelection(int index);


/***************************************************************************************
 * Adds the dose a patient received during an examination at a particular date in
 * the administration. Dose is added to the selected patient.
 *
 * Returns -1 when no patient is selected or allocation failed
 * Returns  0 when the data is successfully added
 */
int8_t addPatientDose(uint16_t date, uint16_t dosage);


/***************************************************************************************
 * Returns the total dose a patient received in passed period.
 *
 * Returns -1 when the passed patientName is unknown
 * Returns  0 when the totalDose is updated successfully
 *
 * It is a precondition that patientName is not NULL and is \0 terminated
 * It is also a precondition that both dates and totalDose are not NULL
 */
//int8_t patientDoseInPeriod(char * patientName,
//                           date* startDate, date* endDate, uint32_t* totalDose);


/***************************************************************************************
 * Returns the number of dose measurements done for a given patient
 *
 * Returns -1 when the passed patientName is not present
 * Returns 0 otherwise, the number of measurements is put in nrOfMeasurements
 *
 * It is a precondition that patientName is not NULL and is \0 terminated
 */
//int8_t getNumberOfMeasurements(char * patientName, size_t * nrOfMeasurements);


/***************************************************************************************
 * Writes all patient data to a text file in the table
 *
 * Returns 0 on success
 * Returns -1 on failure
 */
int8_t writeToFile(char * filePath);


/***************************************************************************************
 * Reads all patient data from a text file, and put the data in an empty table.
 *
 * Returns 0 on success
 * Returns -1 on failure
 */
int8_t readFromFile(char * filePath);

#endif
