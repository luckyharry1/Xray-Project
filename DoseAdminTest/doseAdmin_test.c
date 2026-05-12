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
    // This is run after EACH test
}

void test_FailTest(void)
{
    TEST_ASSERT_EQUAL(1, 0); 
}

void test_LeakTest(void)
{
	int *a = (int*) malloc(sizeof(int));
    TEST_ASSERT_EQUAL(1, 1); 
	*a = 666;
}

void test_OutOfRangeTest(void)
{
	int array[10];
	
	array[10] = 666;
	(void)array;
}

// add here all your dose admin testcases, and call them in main!! 
// Remove the given testcases, they were only added to check if everything is up and running

int main()
{
    UnityBegin();

    MY_RUN_TEST(test_FailTest);
    MY_RUN_TEST(test_LeakTest);
    MY_RUN_TEST(test_OutOfRangeTest);
    
    MY_RUN_TEST(testRemovePatient_WhenTableEntryIsEmpty_ThenRetrunMinus1);
    MY_RUN_TEST(testRemovePatient_WhenTableEntryIsPatientToBeRemoved_ThenRetrun0AndPatientIsRemoved);
    MY_RUN_TEST(testRemovePatient_WhenPatientToBeRemovedIsAtTail_ThenRetrun0AndPatientIsRemoved);
    MY_RUN_TEST(testRemovePatient_WhenPatientToBeRemovedIsInMiddle_ThenRetrun0AndPatientIsRemoved);

    UnityEnd();
}


void testRemovePatient_WhenTableEntryIsEmpty_ThenRetrunMinus1(void)
{
    
    TEST_ASSER_EQUAL(-1, removePatient("aaaaa"));
}
void testRemovePatient_WhenTableEntryIsPatientToBeRemoved_ThenRetrun0AndPatientIsRemoved(void)
{
    
    addPatient("aaaaa");
    
    TEST_ASSERT_EQUAL(0, removePatient("aaaaa"));
    TEST_ASSERT_EQUAL(-1, isPatientPresent("aaaaa"));
}

testRemovePatient_WhenPatientToBeRemovedIsAtTail_ThenRetrun0AndPatientIsRemoved(void)
{
    
}
testRemovePatient_WhenPatientToBeRemovedIsInMiddle_ThenRetrun0AndPatientIsRemoved(void)
{
    
}