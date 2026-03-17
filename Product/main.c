#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "menu.h"
#include "CentralAcquisitionProxy.h"
#include "../Shared/doseAdmin.h"

typedef enum {
	NOT_CONNECTED_WITH_CENTRAL_ACQUISITION, 
	CONNECTED_WITH_CENTRAL_ACQUISITION
} CENTRAL_ACQUISITION_CONNECTION_STATE;

/*---------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	static CENTRAL_ACQUISITION_CONNECTION_STATE centralAcqConnectionState = NOT_CONNECTED_WITH_CENTRAL_ACQUISITION;
		
	initHashTable();
	
	if (connectWithCentralAcquisition()) {	
		centralAcqConnectionState = CONNECTED_WITH_CENTRAL_ACQUISITION;
	}
	else {
		printf("\n\nConnecting with CentralAcquisition Failed. No problem, you can continue with \n");
		printf("the functionality that does not depend on that connection!\n");
	}
	
	displayMenu();	
	while (true) {  
        MenuOptions choice = getMenuChoice();
		if (choice == -1) {
			if (centralAcqConnectionState == CONNECTED_WITH_CENTRAL_ACQUISITION) {
				uint32_t doseData;
				if (getDoseDataFromCentralAcquisition(&doseData)) {
					printf("Received dose: %d\n", doseData); // instead of this print call here the function that handles the received dose datat
				}
			}
		}
		else {
			switch (choice)
			{
				case MO_VIEW_TABLE:{
					printf("----- DATABASE DEBUGGING INFOMATION -----\n");
					printTable();
					break;
				}

				case MO_ADD_PATIENT:{
					char name[MAX_NAME];
					int age;
					int dose;
					
					printf("Enter Patient Name: ");					
					if (scanf("%s", name) != 1){
						printf("ERROR: Invalid Input.");
						break;
					}
					
					printf("Enter Patient Age: ");
					if (scanf("%d", &age) != 1){
						printf("ERROR: Invalid Input.");
						break;
					}

					printf("Enter Patient Dose: ");
					if (scanf("%d", &dose) != 1){
						printf("ERROR: Invalid Input.");
						break;
					}
					
					if (hashTableInsert(name, age) && addPatientDose(dose) == 1) {
						printf("Patient %s added successfully.\n", name);
					} else {
						printf("ERROR: Could not add patient.\n");
					}
					break;
				}

				case MO_SELECT_PATIENT:{
					// add here your select patient code
					selectPatient();

					break;
				}

				case MO_DELETE_PATIENT:{
					char name[MAX_NAME];
					printf("Enter name to remove: ");
					if (scanf("%s", name) != 1) {
						printf("ERROR: Invalid Input.");
						break;
					}

					if (removePatient(name) == -1){
						printf("ERROR: Remove Patient Failed");
						break;
					} else {
						printf("Removing Patient (%s) Success", name);
					}
					break;
				}
				
				case MO_SELECT_EXAMINATION_TYPE:{
					if (centralAcqConnectionState == CONNECTED_WITH_CENTRAL_ACQUISITION) {	
						// add here your select examination code
					}
					else {
						printf("This option is only valid when connected with CentralAcquisition\n");
					}
					break;
				}
				case MO_QUIT:{
					if (centralAcqConnectionState == CONNECTED_WITH_CENTRAL_ACQUISITION) {
						disconnectFromCentralAcquisition();
					}
					centralAcqConnectionState = NOT_CONNECTED_WITH_CENTRAL_ACQUISITION;
					return 0;
					break;
				}
				default:{
					printf("Please, enter a valid number! %d\n", choice);
					break;
				}
			}
			displayMenu();
		}
    }
	return 0;
}
