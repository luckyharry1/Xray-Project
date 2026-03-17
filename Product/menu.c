#include "menu.h"
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>

void printDosage();

void handlePatientSelection(int8_t index);

static int getInt();

static const char* MenuStrings[] = {
    "View Table Data",
    "Add Patient",
    "Select Patient",
    "Delete Patient",
    "Select Examination Type", 
    "Quit"
};

MenuOptions getMenuChoice(void)
{ 
   return (MenuOptions) getInt();
}

void displayMenu()
{
	size_t NrMenuStrings = sizeof(MenuStrings) / sizeof(MenuStrings[0]);
	
	printf("\n\t------Menu-----\t\n");
	for (int i = 0; i < NrMenuStrings; i++) printf("  [%d] %s\n", i, MenuStrings[i]);
	printf("choice: ");
}

static int getInt()
{
    char line[MAX_NAME];
    char* result = NULL;
    int value = -1;

    result = fgets(line, sizeof(line), stdin);
    if (result != NULL) {
        sscanf(result, "%d", &value);
    }
    return value;
}

void handlePatientSelection(int8_t index) {
    bool validInput = false;
    while (validInput == false){

        int nameFunctionInput;
        printf("Selected Patient: %s\nChoose Name Function:\n1. View Patient Details\n2. Remove User\n3. Exit\n", hashTable[index].name);
        scanf("%i", &nameFunctionInput);
        switch(nameFunctionInput){
            case(1):
                printf("\n  --- Patient Details ---\n");
                printf("\tName: %s\n", hashTable[index].name);
                printf("\tAge: %d\n", hashTable[index].age);
                printDosage(index);
                break;
            case(2):
                if (removePatient(hashTable[index].name) == 0){
                    printf("Remove patient success\n");
                    return;
                }
                break;
            case(3):
                return;
        }

    }

}

void printDosage(index){
    printf(" --------------------------------------\n");
    printf("|\t\t\t\t\t|\n");
    printf("|\t\t\t\t\t|\n");
    printf("|\t\tDosage: %d\t\t|\n", hashTable[index].dosage);
    printf("|\t\t\t\t\t|\n");
    printf("|\t\t\t\t\t|\n");
    printf(" --------------------------------------\n");
    }