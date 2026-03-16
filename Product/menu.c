#include "menu.h"
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>


static int getInt();

static const char* MenuStrings[] = {
    "View Table Data",
    "Add Patient",
//    "View Patient Data",
    "Select Patient",
    "Delete Patient",
    "Select Examination Type", 
    "Quit"
};

void handle_patient_selection(int8_t index);

MenuOptions getMenuChoice(void)
{ 
   return (MenuOptions) getInt();
}

void displayMenu()
{
	size_t NrMenuStrings = sizeof(MenuStrings) / sizeof(MenuStrings[0]);
	
	printf("\nMENU\n");
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


void handle_patient_selection(int8_t index) {
    char name[MAX_NAME];

    bool valid_input = false;
    while (valid_input == false){

        int name_function_input;
        printf("Selected Patient: %s\nChoose Name Function:\n1. View Patient Details\n2. Remove User\n3. Exit\n", hash_table[index].name);
        scanf("%i", &name_function_input);
        switch(name_function_input){
            case(1):
                printf("\n  --- Patient Details ---\n");
                printf("\tName: %s\n", hash_table[index].name);
                printf("\tAge: %d\n", hash_table[index].age);
                printf("\tDosage: %d\n\n", hash_table[index].dosage);
                break;
            case(2):
                if (RemovePatient(name) == 0){
                    printf("Remove patient success\n");
                    return;
                }
                break;
            case(3):
                return;
        }

    }

}
