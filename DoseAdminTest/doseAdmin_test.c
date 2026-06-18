#include <string.h>
#include "doseAdmin.h"
#include "doseAdmin_internal.h"
#include "unity.h"
#include <stdlib.h>

// I rather dislike keeping line numbers updated, so I made my own macro to ditch the line number
#define MY_RUN_TEST(func) RUN_TEST(func, 0)

void setUp(void)
{
    initPatientDoseAdmin();
}

void tearDown(void)
{
    resetPatientDoseAdmin();
}

void test_LeakTest(void)
{
	int *a = (int*) malloc(sizeof(int));
    TEST_ASSERT_EQUAL(1, 1); 
	*a = 666;
}

void readFromFile_WhenFileAvaliableWithData_ThenReadAndReturn0(void){
    addPatient("test");
    writeToFile("test.bin");
    resetPatientDoseAdmin();
    initPatientDoseAdmin();
    readFromFile("test.bin");
    TEST_ASSERT_NOT_NULL(isPatientPresent("test"));

}

void readFromFile_WhenFileUnavaliable_ThenReturnMinus1(void){
    TEST_ASSERT_EQUAL(-1, readFromFile("test1.bin"));
}


//-----------------------------------------

void selectPatient_WhenPatientAvaliable_ThenSelectAndReturn0(void){
    TEST_ASSERT_EQUAL(0, addPatient("test"));
    TEST_ASSERT_EQUAL(0, selectPatient("test"));
}

void selectPatient_WhenUnavaliablePatientSelected_ThenReturnMinus1(void)
{
    TEST_ASSERT_EQUAL(-1, selectPatient("test1"));
}


//-------------------------------------------


void addPatientDose_WhenDoseIsAddedToSelectedPatient_ThenReturn0(void)
{
    addPatient("test1");
    selectPatient("test1");
    TEST_ASSERT_EQUAL(0, addPatientDose(150));
}

void addPatientDose_WhenDoseIsAddedToDefaultPatient_ThenReturn0(void) //ADD PATIENT DOSE IN PERIOD
{
    TEST_ASSERT_EQUAL(0, addPatientDose(150));
}

//-------------------------------------------

void testIsPatientPresent_WhenPatientName80Chars_ThenReturn0(void)
{
    addPatient("testtesttetesttesttetesttesttetesttesttetesttesttetesttesttetesttesttetesttestte");
    TEST_ASSERT_NOT_NULL(isPatientPresent("testtesttetesttesttetesttesttetesttesttetesttesttetesttesttetesttesttetesttestte"));
}

void testIsPatientPresent_WhenPatientName81Chars_ThenReturnMinus1(void)
{
    TEST_ASSERT_EQUAL(-1, addPatient("testtesttetesttesttetesttesttetesttesttetesttesttetesttesttetesttesttetesttesttes"));
    TEST_ASSERT_NULL(isPatientPresent("testtesttetesttesttetesttesttetesttesttetesttesttetesttesttetesttesttetesttesttes"));
}

//-------------------------------------------

void testAddPatient_WhenPatientName80Chars_ThenReturn0(void)
{
    TEST_ASSERT_EQUAL(0, addPatient("testtesttetesttesttetesttesttetesttesttetesttesttetesttesttetesttesttetesttestte"));
}

void testAddPatient_WhenPatientName81Chars_ThenReturnMinus1(void)
{
    TEST_ASSERT_EQUAL(-1, addPatient("testtesttetesttesttetesttesttetesttesttetesttesttetesttesttetesttesttetesttesttes"));
}

void testAddPatient_WhenPatientAlreadyPresent_ThenReturnMinus1(void)
{
    addPatient("test1");
    TEST_ASSERT_EQUAL(-1, addPatient("test1"));
}

void testAddPatient_WhenNewPatientNameGiven_ThenReturn0AndPatientAdded(void)
{
    //ARRANGE
    char name[80] = "test1";
    int8_t expected_result = 0;

    //ACT
    int8_t actual_result = addPatient(name);

    //ASSERT
    TEST_ASSERT_EQUAL(actual_result, expected_result);
    TEST_ASSERT_NOT_NULL(isPatientPresent("test1"));
}

void testAddPatient_IfPatientIsAddedToLinkedList_CheckIfNextPointerIsNotNull(void)
{
    TEST_ASSERT_EQUAL(0, addPatient("test1"));
    TEST_ASSERT_EQUAL(0, addPatient("test2"));
    TEST_ASSERT_NOT_NULL(isPatientPresent("test2")->next);
    TEST_ASSERT_NULL(isPatientPresent("test2")->prev);
}

void testAddPatient_IfPatientIsAddedToLinkedList_CheckIfPrevPointerIsNotNull(void)
{
    TEST_ASSERT_EQUAL(0, addPatient("test1"));
    TEST_ASSERT_EQUAL(0, addPatient("test2"));
    TEST_ASSERT_NOT_NULL(isPatientPresent("test1")->prev);
    TEST_ASSERT_NULL(isPatientPresent("test1")->next);
}

//-------------------------------------------

void testRemovePatient_WhenRemovePatientIsJohnDoe_ThenReturnMinus1(void)
{
    TEST_ASSERT_EQUAL(-1, removePatient("johndoe"));
}

void testRemovePatient_WhenTableEntryIsEmpty_ThenRetrunMinus1(void)
{
    TEST_ASSERT_EQUAL(-1, removePatient("test1"));
}

void testRemovePatient_WhenTableEntryIsPatientToBeRemoved_ThenReturn0AndPatientIsRemoved(void)
{
    addPatient("test1");
    
    TEST_ASSERT_EQUAL_INT(0, removePatient("test1"));
    TEST_ASSERT_NULL(isPatientPresent("test1"));
}

void testRemovePatient_WhenPatientToBeRemovedIsAtHead_ThenRetrun0AndPatientIsRemoved(void)
{
    addPatient("test1");
    addPatient("test2");
    addPatient("test3");

    TEST_ASSERT_EQUAL_INT8(0, removePatient("test3"));
    TEST_ASSERT_NULL(isPatientPresent("test3"));

    TEST_ASSERT_NOT_NULL(isPatientPresent("test1"));
    TEST_ASSERT_NOT_NULL(isPatientPresent("test2"));
}

void testRemovePatient_WhenPatientToBeRemovedIsInMiddle_ThenReturn0AndPatientIsRemoved(void)
{
    addPatient("test1");
    addPatient("test2");
    addPatient("test3");

    TEST_ASSERT_EQUAL_INT8(0, removePatient("test2"));
    TEST_ASSERT_NULL(isPatientPresent("test2"));

    TEST_ASSERT_NOT_NULL(isPatientPresent("test1"));
    TEST_ASSERT_NOT_NULL(isPatientPresent("test3"));
}

void testRemovePatient_WhenPatientToBeRemovedIsAtTail_ThenReturn0AndPatientIsRemoved(void)
{
    addPatient("test1");
    addPatient("test2");
    addPatient("test3");

    TEST_ASSERT_EQUAL_INT8(0, removePatient("test1"));
    TEST_ASSERT_NULL(isPatientPresent("test1"));

    TEST_ASSERT_NOT_NULL(isPatientPresent("test2"));
    TEST_ASSERT_NOT_NULL(isPatientPresent("test3"));
}

//-------------------------------------------

void testResetPatientDoseAdmin_WhenTableIsResetPatientsAreNoLongerPresent(void)
{
    addPatient("test1");
    addPatient("test2");
    addPatient("test3");
    resetPatientDoseAdmin();
    TEST_ASSERT_NULL(isPatientPresent("test1"));
    TEST_ASSERT_NULL(isPatientPresent("test2"));
    TEST_ASSERT_NULL(isPatientPresent("test3"));
}

//-------------------------------------------

int main()
{
    UnityBegin();

    MY_RUN_TEST(test_LeakTest);

    MY_RUN_TEST(readFromFile_WhenFileAvaliableWithData_ThenReadAndReturn0);
    MY_RUN_TEST(readFromFile_WhenFileUnavaliable_ThenReturnMinus1);

    MY_RUN_TEST(selectPatient_WhenPatientAvaliable_ThenSelectAndReturn0);
    MY_RUN_TEST(selectPatient_WhenUnavaliablePatientSelected_ThenReturnMinus1);

    MY_RUN_TEST(addPatientDose_WhenDoseIsAddedToSelectedPatient_ThenReturn0);
    MY_RUN_TEST(addPatientDose_WhenDoseIsAddedToDefaultPatient_ThenReturn0);

    MY_RUN_TEST(testIsPatientPresent_WhenPatientName80Chars_ThenReturn0);
    MY_RUN_TEST(testIsPatientPresent_WhenPatientName81Chars_ThenReturnMinus1);

    MY_RUN_TEST(testAddPatient_WhenPatientName80Chars_ThenReturn0);
    MY_RUN_TEST(testAddPatient_WhenPatientName81Chars_ThenReturnMinus1);
    MY_RUN_TEST(testAddPatient_WhenPatientAlreadyPresent_ThenReturnMinus1);
    MY_RUN_TEST(testAddPatient_IfPatientIsAddedToLinkedList_CheckIfNextPointerIsNotNull);
    MY_RUN_TEST(testAddPatient_IfPatientIsAddedToLinkedList_CheckIfPrevPointerIsNotNull);

    MY_RUN_TEST(testRemovePatient_WhenRemovePatientIsJohnDoe_ThenReturnMinus1);
    MY_RUN_TEST(testRemovePatient_WhenTableEntryIsEmpty_ThenRetrunMinus1);
    MY_RUN_TEST(testRemovePatient_WhenTableEntryIsPatientToBeRemoved_ThenReturn0AndPatientIsRemoved);
    MY_RUN_TEST(testRemovePatient_WhenPatientToBeRemovedIsAtHead_ThenRetrun0AndPatientIsRemoved);
    MY_RUN_TEST(testRemovePatient_WhenPatientToBeRemovedIsInMiddle_ThenReturn0AndPatientIsRemoved);
    MY_RUN_TEST(testRemovePatient_WhenPatientToBeRemovedIsAtTail_ThenReturn0AndPatientIsRemoved);
    
    MY_RUN_TEST(testResetPatientDoseAdmin_WhenTableIsResetPatientsAreNoLongerPresent);

    UnityEnd();
}
