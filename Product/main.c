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
	
	//fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);   //non blocking standard input

	
	displayMenu();	
	while (true) {  
        MenuOptions choice = getMenuChoice();
		if (choice == -1) {
		//	if (centralAcqConnectionState == CONNECTED_WITH_CENTRAL_ACQUISITION) {
		//		uint32_t doseData;
		//		if (getDoseDataFromCentralAcquisition(&doseData)) {
		//			printf("Received dose: %d\n", doseData); // TODO: call the function that handles the received dose data
		//		}
		//	}
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
					
					printf("Enter Patient Name: (type 0 to cancel) ");					
					if (scanf("%s", name) != 1){
						printf("ERROR: Invalid Input.");
						break;
					}
					if (name[0] == 0){
						return 0;
					}
					switch (hashTableInsert(name))
					{
					case 0:
						printf("Patient %s added successfully.\n", name);
						break;
					case -1:
						break;
					case -2:
						printf("ERROR: PLACE IS TAKEN");
						break;
					case -3:
						printf("ERROR: MEMORY HEAP FULL");
						break;
					
					default:
						
						break;
					}

				}

				case MO_MANAGE_PATIENT:{
					char name[MAX_NAME];
					printf("Enter name to manage patient: ");
					if (scanf("%s", name) != 1){
						printf("ERROR: Invalid Input.");
						return;
					}
					managePatient(name);

					break;
				}

				case MO_SELECT_PATIENT:{
					// add here your select patient code
					
				}

				case MO_DELETE_PATIENT:{
					char name[MAX_NAME];
					printf("Enter name to remove: ");
					if (scanf("%s", name) != 1) {
						printf("ERROR: Invalid Input.");
						break;
					}

					if (removePatient(name) == false){
						printf("ERROR: Remove Patient Failed");
						break;
					} else {
						printf("Removing Patient (%s) Success", name);
					}
					break;
				}
				
				case MO_SELECT_EXAMINATION_TYPE:{
					if (centralAcqConnectionState == CONNECTED_WITH_CENTRAL_ACQUISITION) {	
						//void selectExaminationType(const EXAMINATION_TYPES examination);
						uint32_t doseData;
						if (getDoseDataFromCentralAcquisition(&doseData)) {
							printf("Received dose: %d\n", doseData); // TODO: call the function that handles the received dose data
						}
						selectExaminationType(EXAM_TYPE_SINGLE_SHOT);
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
				}
				default:{
					printf("Please enter a valid option (0-%d).\n", MO_QUIT);
					break;
				}
			}
			displayMenu();
		}
    }
	return 0;
}
