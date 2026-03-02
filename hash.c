#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for line 15 and 41
#include <stdbool.h>
#include <ctype.h> // for disabling cap sensitivity

#define MAX_NAME 80
#define TABLE_SIZE 256

typedef struct{ //struct is a group of variables, typedef is for initializing a variable (type)
    char name[MAX_NAME];
    int age;
    int dosage;
    // can add more later
} person;

person * hash_table[TABLE_SIZE];


unsigned int hash(char *name){ // unsigned means it can store only positive whole number, doubling the positive range
    int length = strnlen(name, MAX_NAME); //count amount of char
    unsigned int hash_value = 0;
    for (int i=0; i < length; i++){ //run for amount of char, adding the total
        hash_value += name[i];
        hash_value = hash_value * name[i] % TABLE_SIZE; // for spreading out the outcome, to increase accuracy, but there are many ways to do this
    }
    return hash_value;
}

void init_hash_table()  {
    for (int i = 0; i < TABLE_SIZE; i++) {
        hash_table[i] = NULL;
    }
    //empty table initialized
}

void print_table()  {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hash_table [i] == NULL) {
            printf("%i  NULL\n", i);
        }
        else{
            printf("%i  %s\n",i, hash_table[i]->name); //print the string of the name and number
        }
    }
}

void to_lowercase(char *string){
    for (size_t i = 0; i < strlen(string); i++){
        string[i] = (char)tolower((unsigned char)string[i]); // changing any character to lowercase
    }
}

bool hash_table_insert(person *p){
    if (p == NULL) { //checking if the input isnt nothing
        return false;
    }
    char name_lowercase[MAX_NAME];
    strncpy(name_lowercase, p->name, MAX_NAME); //make a copy, placing the person in the lowercase hased spot, with its original capitilization
    to_lowercase(name_lowercase);
    
    int index = hash(name_lowercase); //index for placing the person
    if (hash_table[index] != NULL) {
        return false;
    }
    hash_table[index] = p;
    return true;
}

// find a person in the table by their name (number)
person *hash_table_lookup (char *name){
    char name_lowercase[MAX_NAME];
    strncpy(name_lowercase, name, MAX_NAME);
    to_lowercase(name_lowercase); // copying the name for looking up, and changing it to lowercase

    int index = hash(name_lowercase);
    if (hash_table[index] != NULL &&
        strncmp(hash_table[index]->name, name, TABLE_SIZE)==0){ //comparing if the lookup == the name in the space
            return hash_table[index];
    }
    else if (hash_table[index] == NULL){
        printf("ERROR, SPACE IS NULL\n");
        return 0;
    } 
    else{
        printf("ERROR, SPACE IS MISMATCH\n");
        return 0;
    }
}

int main(){

    init_hash_table();

    printf("Choose function\n1. Print ");
    
    person sjors = {.name="Sjors", .age = 100, .dosage = 0};
    person vasco = {.name = "Vasco", .age = 18, .dosage = 3};
    person natalie = {.name = "Natalie", .age = 20, .dosage = 3000};

    hash_table_insert(&sjors); //& finds the memory address of the varaible, sjors
    hash_table_insert(&vasco);
    hash_table_insert(&natalie);
    //print_table();



    person *temp = hash_table_lookup("Sjors");

    printf("%s\n", temp->name);

    /*printf("Jacob -> %u\n", hash("Jacob")); // %u is for unsigned
    printf("Natalie -> %u\n", hash("Natalie"));
    printf("Vasco -> %u\n", hash("Vasco"));
    printf("Sjors -> %u\n", hash("Sjors"));*/
    return 0;
}

