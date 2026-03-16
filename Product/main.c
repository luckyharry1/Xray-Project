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
		
	init_hash_table();

	hash_table_insert("JohnDoe", 0, 0);
	
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
					print_table();
					break;
				}

				case MO_ADD_PATIENT:{
					char name[MAX_NAME];
					int age;
					int dose;
					
					printf("Enter Patient Name: ");					
					if (scanf("%79s", name) != 1){
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

					if (hash_table_insert(name, age, dose) == 1) {
						printf("Patient %s added successfully.\n", name);
					} else {
						printf("ERROR: Could not add patient.\n");
					}
					break;
				}

				case MO_SELECT_PATIENT:{
					// add here your select patient code
					char name[MAX_NAME];
					printf("Enter name to select patient:\t");
					if (scanf("%79s",name) != 1){
						printf("ERROR: Invalid Input.");
						break;
					}

					SelectPatient(name);

					break;
				}

				case MO_DELETE_PATIENT:{
					char name[MAX_NAME];
					printf("Enter name to remove: ");
					if (scanf("%79s", name) != 1) {
						printf("ERROR: Invalid Input.");
						break;
					}

					if (RemovePatient(name) == -1){
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
