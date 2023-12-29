#include "algorithms.h"
#include "config.h"
#include <stdio.h>
#include <string.h>

// Initializing the globals
algorithm_history_entry_t algo_history[MAX_ALGO_HISTORY];
algorithm_variable_t algo_variables[MAX_ALGORITHM_VARIABLES];
int algo_total_variables = 0;
size_t algo_history_length = 0;

char algorithm_titles[ALGO_TOTAL][60] = {
    "Selection Sort",
    "Insertion Sort",
    "Bubble Sort"
};

char algorithm_descriptions[ALGO_TOTAL][600] = {
    "Ο συγκεκριμένος αλγόριθμος περνάει από κάθε κουτάκι ξεκινώντας από τα αριστερά "
    "και τοποθετεί σε αυτό το μικρότερο στοιχείο της λίστας που δεν έχει ακόμα ταξινομηθεί",

    "Μπορούμε να φανταστούμε τον Insertion Sort να κρατάει μια ταξινομημένη τράπουλα στο χέρι του. "
    "Τα στοιχεία στα δεξιά του τοποθετούνται ένα ένα στην ταξινομημένη \"τράπουλα\" με βάση το ύψος τους.",

    "Ο αλγόρθιμος Bubble Sort αρχίζει ξανά το i από τα αριστερά και σε κάθε iteration τοποθετεί το μικρότερο "
    "στοιχείο της υπόλοιπης λίστας (από i μέχρι τέρμα), στην σωστή θέση του ταξινομημένου υποπίνακα (από 0 μέχρι i - 1).\n"
    "Μπορείτε να φανταστείτε τις μικρές μπάρες να ανεβαίνουν στην επιφάνεια σαν φυσαλίδες",
};

sorting_algorithm algorithm_implementations[ALGO_TOTAL] = {
    selection_sort,
    insertion_sort,
    bubble_sort
};

char *algorithm_source_codes[ALGO_TOTAL];

void load_algorithm_source_codes(void)
{
    // The source codes shall be accessed throughout all of the program's lifetime
    // So I'm just allocating them on the heap. They will be freed automatically by the operating system
    for (int i = 0; i < ALGO_TOTAL; i++)
    {
        char file_path[256];
        sprintf(file_path, "res/algorithms_source_code/%d.c", i);
        FILE *algo_file = fopen(file_path, "r");

        if (!algo_file)
        {
            fprintf(stderr, "failed to read res/algorithms_source/code/, does the folder exist?\n");
            exit(EXIT_FAILURE);
        }
    
        // Collecting the total size of the file in bytes and allocating enough space on the heap
        fseek(algo_file, 0, SEEK_END);
        size_t length = ftell(algo_file);
        fseek(algo_file, 0, SEEK_SET);

        char *buffer = malloc((length + 1) * sizeof(char));
        fread(buffer, sizeof(char), length, algo_file);
        // Terminating the string with a null byte
        buffer[length] = 0;

        algorithm_source_codes[i] = buffer;
        fclose(algo_file);
    }
}

// Create variable and initialize to the invalid value -1
algorithm_variable_t* create_variable(char *token)
{
    // Find an empty spot in the variables table
    int variable_index;
    for (variable_index = 0; variable_index < MAX_ALGORITHM_VARIABLES; variable_index++)
    {
        if (algo_variables[variable_index].token == NULL)
        {
            algo_variables[variable_index].token = strdup(token);
            algo_variables[variable_index].value = -1;
            algo_variables[variable_index].table_index = variable_index;

            algo_total_variables++;
            break;
        }
    }
    
    return &algo_variables[variable_index];
}

void update_variable(algorithm_variable_t *variable, int new_value)
{
    if (new_value < LIST_LENGTH)
    {
        // History entry side-effects
        algo_history[algo_history_length].action = ACTION_SET_VARIABLE;
        algo_history[algo_history_length].data.set_variable_data.variable_table_index = variable->table_index;
        algo_history[algo_history_length].data.set_variable_data.old_value = variable->value;
        algo_history[algo_history_length].data.set_variable_data.new_value = new_value;

        algo_history_length++;
    }
    
    variable->value = new_value;
}

void swap_indeces(int *data, int a, int b)
{
    // Executing the swap operation on the ordinary list of integers
    int temp = data[a];
    data[a] = data[b];
    data[b] = temp;
    
    algo_history[algo_history_length].action = ACTION_SWAP_INDECES;
    algo_history[algo_history_length].data.swap_indeces_data.a = a;
    algo_history[algo_history_length].data.swap_indeces_data.b = b;

    algo_history_length++;
}

/*
 * Implementing the lecture's sorting algorithms with history recording in mind
 * They will all end up slower and considerably more verbose
 */
void selection_sort(int *data, int lower, int upper)
{
    algorithm_variable_t *i = create_variable("i");
    algorithm_variable_t *j = create_variable("j");
    algorithm_variable_t *min = create_variable("min");

    for (update_variable(i, 1); i->value <= upper; update_variable(i, i->value + 1))
    {
        update_variable(min, i->value - 1);
    
        for (update_variable(j, i->value); j->value <= upper; update_variable(j, j->value + 1))
        {
            if (data[j->value] < data[min->value])
                update_variable(min, j->value);
        }
    
        swap_indeces(data, i->value - 1, min->value);
    }
}

void insertion_sort(int *data, int lower, int upper)
{
    algorithm_variable_t *i = create_variable("i");
    algorithm_variable_t *j = create_variable("j");

    for (update_variable(i, 1); i->value <= upper; update_variable(i, i->value + 1))
    {
        update_variable(j, i->value - 1);
    
        while (j->value >= 0 && data[j->value] > data[j->value + 1])
        {
            swap_indeces(data, j->value, j->value + 1);
            update_variable(j, j->value - 1);
        }
    }
}

void bubble_sort(int *data, int lower, int upper)
{
    algorithm_variable_t *i = create_variable("i");
    algorithm_variable_t *j = create_variable("j");

    for (update_variable(i, 1); i->value <= upper; update_variable(i, i->value + 1))
    {
        for (update_variable(j, upper); j->value >= i->value; update_variable(j, j->value - 1))
        {
            if (data[j->value - 1] > data[j->value])
                swap_indeces(data, j->value - 1, j->value);
        }
    }
}
