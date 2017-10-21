// ----------------------------------------------------------------------
// File: randadd1.c
// ----------------------------------------------------------------------
// Description: This file implements a program that will pick a random
//     number of random numbers and add them together. It will display
//     the numbers selected and their total.
//
//     Internally, the list of numbers are generated first, and are
//     stored in a linked list. Each object of the list holds
//     a random number. During a second pass through the list the
//     program creates the subtotal to that point and stores it in each
//     object of the list. The last object in the linked list will
//     therefore have the grand total.
//
// Created: 2016-04-19 (P. Clark)
//
// Modified:
// 2016-10-19 (P. Clark)
//     Changed the program to limit the size of the random numbers, 
//     such that overflows won't be a problem (which has not been
//     covered yet). Also changed the max size of the list to a smaller
//     number.
// ----------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define MAX_OBJECTS  19
#define MAX_RANDOM   50

// error codes
#define SUCCESS 0
#define GENERIC_ERROR -1
#define BAD_POINTER -3
#define INVALID_NUMBER -4


// The structure that is used in the linked list

typedef struct value value_t;
struct value {
        long int val;      // the random number
        value_t *next;     // link to the next object in the list
};



// ---------------------------------------------------------------------
// Function: 
//     print_list
// Inputs:
//     start:
//         A pointer to the start of a linked list.
// Outputs:
//     function result:
//         An indicator of success or failure. A success is a zero, 
//         while a failure is any other value. 
// Description:
//     This function walks through the linked list, printing out each
//     value in the list.
// ---------------------------------------------------------------------

int print_list(value_t *start)
{
        int result = SUCCESS;
        int number = 1;
        
        if (start == NULL){
                result = BAD_POINTER;
        }else{
                do {    
                        printf("Number %2d = %2ld\n", number, start->val);
                        start = start->next;
                        number++;

                } while (start != NULL);
        }
        return result;
} 

// ---------------------------------------------------------------------
// Function: 
//     calc_total
// Inputs:
//     start
//         A pointer to the start of a linked list.
// Outputs:
//     total:
//         The total of all "val"s in the input linked list.
//     function result:
//         An indicator of success or failure. A success is a zero, 
//         while a failure is any other value. 
// Description:
//     Given the linked list passed as input, this function will calculate 
//     the total of each 'val' within the list. 
// ---------------------------------------------------------------------

int calc_total(value_t *start, long int *total)
{
        int result = SUCCESS;

        if (start == NULL){
                result = BAD_POINTER;
        }else{
                do {        
                        *total = *total + start->val;
                        start = start->next;

                } while (start != NULL);
        }
        return result;
} 



// ---------------------------------------------------------------------
// Function:
//     build_list
// Inputs:
//     num
//         A number indicating how many objects need to be created 
//         in a linked list.
// Outputs:
//     start
//         A pointer to the start of the constructed linked list.
//         If there is a failure, this is set to NULL on a return.
//     function result:
//         An indicator of success or failure. A success is a zero, 
//         while a failure is any other value.
// Description:
//     This function creates a linked list of value_t objects. The
//     number of objects to create is given as an input 'num', which
//     must be between 1 and MAX_OBJECTS (inclusive). In addition,
//     this function initializes the 'val' member of each object to
//     a random integer in the range provided by MAX_RANDOM.
// ---------------------------------------------------------------------
int build_list(const unsigned int num, value_t **start)
{
        int result = SUCCESS;
        unsigned int i;
        value_t *tmp1 = NULL;
        value_t *tmp2 = NULL;

        if (start == NULL) {
                printf("Error: unexpected NULL pointer in build_list\n");
                result = BAD_POINTER;
        } else if (num > MAX_OBJECTS) {
                printf("Error: invalid number of objects in build_list\n");
                result = INVALID_NUMBER;
        } else {
                // get the first structure and initialize it
                errno = 0;
                *start = malloc(sizeof(value_t)); 
                if ((start == NULL) && (errno == SUCCESS)) {
                        result = GENERIC_ERROR;
                        printf("Error: " 
                               "unable to allocate memory at start\n");
                } else if ((start == NULL) && (errno != SUCCESS)) {
                        result = errno;
                        perror("malloc error at start");
                }
        }

        if (result == SUCCESS) {
                // Success: Initialize the initial structure
                (*start)->val = (random() % MAX_RANDOM) + 1;
                (*start)->next = NULL;

                // get additional structures and link them together
                for (i=1, tmp1=(*start); i < num; ++i) {
                        errno = SUCCESS;
                        tmp2 = malloc(sizeof(value_t));
                        if ((tmp2 == NULL) || (errno != SUCCESS)) {
                                if (errno == SUCCESS) {
                                        result = GENERIC_ERROR;
                                } else {
                                        result = errno;
                                }
                                printf("Error: unable to allocate "
                                       "memory at structure #%d\n", num);
                                break;
                        }
                        tmp2->val = (random() % MAX_RANDOM) + 1;
                        tmp2->next = NULL;
                        tmp1->next = tmp2;
                        tmp1 = tmp2;
                }                                
        }

        return result;
} // build_list



// **********************************************************************
// **************************  M  A  I  N  ******************************
// **********************************************************************
int main(void)
{
        unsigned int num;
        int result = SUCCESS;
        value_t *start = NULL;
        value_t *tmp1 = NULL;
        value_t *tmp2 = NULL;
        long int total = 0;

        srandom(time(NULL));   // seed the random number generator
        num = (rand() % MAX_OBJECTS) + 1; // get a random number
        printf("\nLength of list = %d\n\n", num);

        // Build the linked list
        result = build_list(num, &start);
        if (result != SUCCESS) {
                printf("Error: problem building list\n");
        } else {
                // Calculate the sum of the linked list
                result = calc_total(start, &total);
                if (result != SUCCESS) {
                        printf("Error calculating total\n");
                }
        }

        if (result == SUCCESS) {
                // Print out list
                result = print_list(start);
                if (result != SUCCESS) {
                        printf("Error printing the list\n");
                }
        }

        if (result == SUCCESS) {
                // Print the total
                printf("\nTotal = %ld\n\n", total);

                // Free up the linked list
                if (result == SUCCESS) {
                        
                        tmp1 = start;
                         
                        while (1){
                                if (tmp1->next != NULL){
                                       tmp2 = tmp1->next;
                                       free(tmp1);
                                       tmp1 = tmp2;
                                }else{
                                        free(tmp1);
                                        break; 
                                }
                        }         
                        
                        start = NULL;
                }
        }

        return result;
} // main()

// End of randadd1.c
