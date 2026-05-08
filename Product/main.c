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

char selectedPatientName[MAX_NAME] = "johndoe";

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
					if (name[0] == 0){
						break;
					}

					int strLen = sizeof(name);
					for(int i=0; i < strLen; i++) // if any char == 0, cancel
					{ 
						bool status = true;
						if ((int)name[i] == 48){
							printf("Cancelling.");
							status = false;
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
				}


				case MO_SELECT_PATIENT:{
					system("clear");
					char nameInput[MAX_NAME];

					printf("Enter name to select patient: (0 to cancel) ");
					if (scanf("%s", nameInput) != 1){
						printf("ERROR: Invalid Input.");
						return 0;
					}
					if(nameInput[0] == 0){
						break;
					}

					for(int i=0; i < sizeof(nameInput); i++){ // if any char == 0, cancel
						if ((int)nameInput[i] == 48){
							printf("Cancelling.");
							break;
						}
					}

					int8_t selectResult = selectPatient(nameInput);
					if(selectResult == -2){
						printf("ERROR: Patient '%s' not found.\n", nameInput);
						break;
					} else if(selectResult == -1){
						printf("ERROR: Hash collision — a different patient occupies this slot.\n");
						break;
					}
					strncpy(selectedPatientName, nameInput, MAX_NAME);

					managePatient();
					break;
				}

				case MO_DELETE_PATIENT:{
					system("clear");

					char input;
					printf("Are you sure you would like to remove ""%s""?\nY/n\n", selectedPatientName);
					if (scanf(" %c", &input) != 1) {
						printf("ERROR: Invalid Input.");
						break;
					} else if(input != 'Y' && input != 'y') {
						break;
					}
					
					if (removePatient(input) == false) {
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
						uint32_t doseData;
						uint8_t examType;
						
						printf("Exam Type Menu\n[0]\tNO EXAM TYPE\n[1]\tSINGLE SHOT EXAM\n[2]\tSERIES EXAM\n[3]\tSERIES WITH MOTION\n[4]\tFLOURO EXAM\n");
						printf("Enter Exam Type (0-4) ");					
						if (scanf("%i", &examType) != 1){
							printf("ERROR: Invalid Input.");
							break;
						}
						
						switch(examType){
							case EXAM_TYPE_NONE: 
								selectExaminationType(EXAM_TYPE_NONE);
								break;
							case EXAM_TYPE_SINGLE_SHOT: 
								selectExaminationType(EXAM_TYPE_SINGLE_SHOT);
								break;
							case EXAM_TYPE_SERIES:
								selectExaminationType(EXAM_TYPE_SERIES);
								break;
							case EXAM_TYPE_SERIES_WITH_MOTION:
								selectExaminationType(EXAM_TYPE_SERIES_WITH_MOTION);
								break;
							case EXAM_TYPE_FLUORO:
								selectExaminationType(EXAM_TYPE_FLUORO);
								break;
						}

						//if (getDoseDataFromCentralAcquisition(&doseData)) {
						//	printf("Received dose: %d\n", doseData); // TODO: call the function that handles the received dose data
						//}

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