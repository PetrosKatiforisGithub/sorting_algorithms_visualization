#ifndef _ALGORITHMS_H
#define _ALGORITHMS_H

#include <stdlib.h>

#define MAX_ALGO_HISTORY 500

/*
 * Each sorting function will populate a list representing the history of the algorithm
 * That's what will allow users to time travel and inspect the process in a step-by-step fashion
 */

typedef enum
{
    ACTION_SET_VARIABLE,
    ACTION_SWAP_INDECES
} algorithm_action_e;

typedef struct
{
    algorithm_action_e action;

    // All possible action metadata will be stored in this union to save space
    // Only one action will be accessible during each read operation
    union
    {
        struct
        {
            int variable_table_index;
            int old_value, new_value;
        } set_variable_data;

        struct
        {
            // Indeces of the rectangles to be swapped
            int a, b;
        } swap_indeces_data;
    } data;
    
} algorithm_history_entry_t;

// This could have been a dynamic array,
// but the program won't exceed that limit anyway
extern algorithm_history_entry_t algo_history[MAX_ALGO_HISTORY];
extern size_t algo_history_length;

// A lookup table for an algorithm's variables
#define MAX_ALGORITHM_VARIABLES 5
typedef struct
{
    char *token;
    int value;

    // The position of the variable in the global lookup table
    // Access is much faster compared to searching for string matches
    int table_index;
} algorithm_variable_t;
extern algorithm_variable_t algo_variables[MAX_ALGORITHM_VARIABLES];
extern int algo_total_variables;

// Some helper functions to generate any algorithm's useful history
algorithm_variable_t* create_variable(char *token);
void update_variable(algorithm_variable_t *variable, int new_value);
void swap_indeces(int *data, int a, int b);



// Generic definition of a sorting algorithm's function
typedef void (*sorting_algorithm) (int *data, int lowwer, int upper);

// Declaring each algorithm's implementation
void selection_sort(int *data, int lower, int upper);
void insertion_sort(int *data, int lower, int upper);
void bubble_sort(int *data, int lower, int upper);

/*
 * Storing some algorithm-specific data!
 * Will be stored in the form of highly-accessible enum lists
 */
typedef enum
{
    ALGO_SELECTION_SORT,
    ALGO_INSERTION_SORT,
    ALGO_BUBBLE_SORT,
    ALGO_TOTAL
} algo_e;

extern char algorithm_titles[ALGO_TOTAL][60];
extern char algorithm_descriptions[ALGO_TOTAL][600];
extern sorting_algorithm algorithm_implementations[ALGO_TOTAL];

// Storing the algorithms' actual source code in a list
// They will later be displayed onto the screen for additional context
extern char *algorithm_source_codes[ALGO_TOTAL];
void load_algorithm_source_codes(void);

#endif
