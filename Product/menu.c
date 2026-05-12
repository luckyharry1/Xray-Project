#include "menu.h"
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdbool.h>

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

void displayMenu() {
	size_t NrMenuStrings = sizeof(MenuStrings) / sizeof(MenuStrings[0]);
	
	printf("\n\t------Menu-----\t\n");
	for (int i = 0; i < NrMenuStrings; i++) printf("  [%d] %s\n", i, MenuStrings[i]);
	printf("choice: ");
}

static int getInt() {
    char line[MAX_NAME];
    char* result = NULL;
    int value = -1;

    result = fgets(line, sizeof(line), stdin);
    if (result != NULL) {
        sscanf(result, "%d", &value);
    }
    return value;
}

void handlePatientSelection(char *name) {
    system("clear");
    bool validInput = false;
    while (validInput == false){

        int nameFunctionInput;
        printf("Selected Patient: %s\nChoose Name Function:\n1. View Patient Details\n2. Exit\n", name);
        scanf("%i", &nameFunctionInput);
        switch(nameFunctionInput){
            case(1):
                //printPatientData(name);
                break;
            case(2):
                return;
        }

    }

}