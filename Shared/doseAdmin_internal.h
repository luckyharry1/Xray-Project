#ifndef DOSEADMIN_INTERNAL_H
#define DOSEADMIN_INTERNAL_H


typedef struct{
    uint16_t dosage; //dose in mg
    Date date;
} doseData;


typedef struct patient {
    char name[MAX_NAME];
    doseData doseData[MAX_DOSE_MEASUREMENTS];    // dynamically allocated array of dose records
    int doseCount;
    struct patient* next;
    struct patient* prev;
} Patient;


void initPatientDoseAdmin();


//adds a patient into the system
//returns 0 if success, -1 if 
int8_t addPatient(char *name);


//provides a UI for viewing patient data
void managePatient();


/*
* Print all patients in the hash table (for debugging)
*/
void printTable();


void printPatientData(char *name);


/**************************************************************************************
 * Changes characters to lowercase for searching
 * ************************************************************************************
 */
void toLowercase(char *nameLowercase, const char *name);


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
 * -1 not present
 * 0 if pass
 * It is a precondition that patientName is not NULL and is \0 terminated
 */
int8_t selectPatient(char *name);


/***************************************************************************************
 * Checks if the passed patientName is present in the administration
 *
 * Returns NULL when the passed patientName is not present
 *
 * It is a precondition that patientName is not NULL and is \0 terminated
 */
Patient* isPatientPresent(char *name);


/***************************************************************************************
 * Returns a number in the range [0, HASHTABLE_SIZE), i.e. the entry in the hash table
 * 
 * It is a precondition that patientName is not NULL, is \0 terminated, and length
 * does not exceed MAX_PATIENTNAME_SIZE
 */
unsigned int hash(char *name);


/***************************************************************************************
 * Returns the entry to the hash table
 *
 * Returns now a void pointer, but feel free to change it to for instance a pointer to a 
 * Patient struct.
 */
Patient* isPatientPresent(char * name);


/***************************************************************************************
 * Returns the total number of patients in the table, the average number of patients in 
 * a table entry and standard deviation of an table entry. 
 * This function is used to check if the hash function is good enough (i.e. distributes 
 * the patients equally over the entries (i.e. small standard deviation)).
 * 
 */
void GetHashPerformance(size_t *totalNumberOfPatients, double *averageNumberOfPatients,
                        double *standardDeviation);

#endif
