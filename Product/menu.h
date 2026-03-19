#ifndef MENU_H
#define MENU_H
#include "../Shared/doseAdmin.h"

typedef enum {
	MO_VIEW_TABLE,
	MO_ADD_PATIENT,
	MO_SELECT_PATIENT,
	MO_DELETE_PATIENT,
	MO_SELECT_EXAMINATION_TYPE,
    MO_QUIT
} MenuOptions;

void printDosage(int index);

void handlePatientSelection(int index);

MenuOptions getMenuChoice(void);
void displayMenu();


#endif