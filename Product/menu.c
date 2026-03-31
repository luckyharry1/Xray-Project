#include "menu.h"
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>

static int getInt();

static const char* MenuStrings[] = {
    "View Table Data",
    "Add Patient",
    "Manage Patient",
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

void handlePatientSelection(char *name) {
    system("clear");
    bool validInput = false;
    while (validInput == false){

        int nameFunctionInput;
        printf("Selected Patient: %s\nChoose Name Function:\n1. View Patient Details\n2. Remove User\n3. Exit\n", name);
        scanf("%i", &nameFunctionInput);
        switch(nameFunctionInput){
            case(1):
                printPatientData(name);
                break;
            case(2):
                if (removePatient(name) == 0){
                    printf("Remove Patient success\n");
                    return;
                }
                break;
            case(3):
                return;
        }

    }

}

void handleAddDosage(){
    int dose;
    int day; 
    int month; 
    int year;
    printf("How large of a dose would you like to add (in mg)?\n");
    scanf("%d\n", &dose);
    printf("Please fill in today's date (dd/mm/yyyy).\nType in the date and press enter.\n");
    scanf("%d",&day);
    printf("/");
    scanf("%d",&month);
    printf("/");
    scanf("%d",&year);
    printf("/");
}

void printDosage(int index){
    // TODO: implement when dosage field is added to Patient struct
    (void)index;
}