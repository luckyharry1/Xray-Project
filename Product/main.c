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

char selectedPatientName[MAX_NAME];

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
				case MO_VIEW_TABLE:
				{
					system("clear");
					printf("----- DATABASE DEBUGGING INFOMATION -----\n");
					printTable();
					break;
				}

				case MO_ADD_PATIENT:
				{
					system("clear");
					char name[MAX_NAME];
					
					printf("Enter Patient Name: (0 to cancel) ");					
					if (scanf("%s", name) != 1){
						printf("ERROR: Invalid Input.");
						break;
					}


					int strLen = sizeof(name);

					for(int i=0; i < sizeof(name); i++) // if any char == 0, cancel
					{ 
						if ((int)name[i] == 48){
							printf("Cancelling.");
							break;
						}
					}
					switch (addPatient(name)) 
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
				}		
				break;
				

				case MO_MANAGE_PATIENT:{
					system("clear");

					managePatient();
					break;
				}

				case MO_SELECT_PATIENT:{
					system("clear");
					char name[MAX_NAME];

					printf("Enter name to select patient: (0 to cancel) ");
					if (scanf("%s", name) != 1){
						printf("ERROR: Invalid Input.");
						return 0;
					}

					for(int i=0; i < sizeof(name); i++){ // if any char == 0, cancel
						if ((int)name[i] == 48){
							printf("Cancelling.");
							break;
						}
					}

					int* ptr = NULL;
					ptr = &selectedPatientName;

					selectPatient(ptr);
					if(selectedPatientName[0] == "\0") //-------------- add more error codes -------------
					{   
						printf("ERROR, PERSON NOT FOUND");
					}

					break;
				}

				case MO_DELETE_PATIENT:{
					system("clear");

					char input;
					printf("Are you sure you would like to remove ""%s""?\n Y/n\n", selectedPatientName);
					if (scanf("%s", &input) != 1) {
						printf("ERROR: Invalid Input.");
						break;
					} else if(input != "Y" || input != "y") {
						break;
					}
					
					if (removePatient() == false) {
						printf("ERROR: Remove Patient Failed");
						break;
					} else {
						printf("Removing Patient (%s) Success", selectedPatientName);
					}
					break;
				}
				
				case MO_SELECT_EXAMINATION_TYPE:{
					system("clear");
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
	return;
}