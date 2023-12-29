#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <time.h>
#include "sdl_utils.h"
#include "algorithms.h"
#include "config.h"

// Window dimensions
#define WIDTH 800
#define HEIGHT 640

#define BAR_Y 450
#define BAR_WIDTH 20
#define BAR_SPACING 2
#define TITLE_X 30
#define TITLE_Y 30

int list_to_be_sorted[LIST_LENGTH];
SDL_Rect list_rectangles[LIST_LENGTH];
int max_list_height = 0;

// Some global variables
SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *main_font;
Mix_Chunk *swap_sfx;
utils_label_t title_label, description_label, instructions_label,
    source_code_label;

// Text that represents variables based on their table index
utils_label_t variable_labels[MAX_ALGORITHM_VARIABLES];

// Will be modified to read and interpret the algorithm's history
int current_history_index = 0;
int swapped_a = -1, swapped_b = -1;
algo_e current_algorithm;

SDL_Color white = {255, 255, 255, 255};
SDL_Color gray = {100, 100, 100, 255};

void exit_with_error(const char *message)
{
    fprintf(stderr, "{error}: %s\n", message);
    exit(EXIT_FAILURE);
}

// Making sure that all rectangles are properly aligned, despite their variable height
void align_rect_to_x_axis(int index)
{
    list_rectangles[index].y = BAR_Y - list_rectangles[index].h;
}

void randomize_list(void)
{
    // Populate the global list with some random values
    for (int i = 0; i < LIST_LENGTH; i++)
    {
        list_to_be_sorted[i] = 20 + rand() % 100;

        if (list_to_be_sorted[i] > max_list_height)
            max_list_height = list_to_be_sorted[i];
    
        // Modify the corresponding rectangles
        list_rectangles[i].h = list_to_be_sorted[i];
        align_rect_to_x_axis(i);
    }
}

void interpret_next_algorithm_history_entry(void)
{
    // Check if we've reached the end of the history
    if (current_history_index >= algo_history_length) return;
    
    algorithm_history_entry_t *entry = &algo_history[current_history_index];
    
    switch (entry->action)
    {
    case ACTION_SWAP_INDECES:
        swapped_a = entry->data.swap_indeces_data.a;
        swapped_b = entry->data.swap_indeces_data.b;
    
        // Swap the rectangles described by the entry using an intermediate copy
        int temp = list_rectangles[swapped_a].h;

        list_rectangles[swapped_a].h = list_rectangles[swapped_b].h;
        align_rect_to_x_axis(swapped_a);

        list_rectangles[swapped_b].h = temp;
        align_rect_to_x_axis(swapped_b);

        // Playing a swap sound effect
        Mix_PlayChannel(-1, swap_sfx, 0);
    
        break;

    case ACTION_SET_VARIABLE:
        swapped_a = swapped_b = -1;
    
        // If a variable has just been modified, then create some visual feedback
        // I just need to position the variable's label according to the index that it is refering to
        int table_index = algo_history[current_history_index].data.set_variable_data.variable_table_index;
        int new_value = algo_history[current_history_index].data.set_variable_data.new_value;

        variable_labels[table_index].background.x = (WIDTH - LIST_LENGTH * (BAR_WIDTH + BAR_SPACING)) / 2 + \
            new_value * (BAR_WIDTH + BAR_SPACING);
    
        break;
    }
    
    current_history_index++;
}

void interpret_previous_algorithm_history_entry(void)
{
    // Check if we've reached the start of the history
    if (current_history_index == 0) return;
    current_history_index--;
    
    algorithm_history_entry_t *entry = &algo_history[current_history_index];
    
    switch (entry->action)
    {
    case ACTION_SWAP_INDECES:
        // The exact same implementation, the swap operation is automatically reveresable
        swapped_a = entry->data.swap_indeces_data.a;
        swapped_b = entry->data.swap_indeces_data.b;
    
        int temp = list_rectangles[swapped_a].h;

        list_rectangles[swapped_a].h = list_rectangles[swapped_b].h;
        align_rect_to_x_axis(swapped_a);

        list_rectangles[swapped_b].h = temp;
        align_rect_to_x_axis(swapped_b);

        Mix_PlayChannel(-1, swap_sfx, 0);
    
        break;

    case ACTION_SET_VARIABLE:
        swapped_a = swapped_b = -1;

        // Do the reverse, just assign the old value back to the variable
        int table_index = algo_history[current_history_index].data.set_variable_data.variable_table_index;
        int old_value = algo_history[current_history_index].data.set_variable_data.old_value;

        variable_labels[table_index].background.x = (WIDTH - LIST_LENGTH * (BAR_WIDTH + BAR_SPACING)) / 2 + \
            old_value * (BAR_WIDTH + BAR_SPACING);
    
        break;
    }
}

// Executes the specified algorithm by populating the global algo_history list with the correct entries
// The user shall then let the program interpret the entires either forwards or backwards!
void execute_algorithm(algo_e algo_type)
{
    current_algorithm = algo_type;
    
    // Get rid of the old variables to avoid memory leaks
    for (int i = 0; i < algo_total_variables; i++)
    {
        utils_label_destroy(&variable_labels[i]);
        free(algo_variables[i].token);
    }

    // Cleaning up the previous data
    memset(algo_history, 0, sizeof(algo_history));
    memset(algo_variables, 0, sizeof(algo_variables));
    algo_history_length = 0;
    algo_total_variables = 0;
    current_history_index = 0;
    swapped_a = swapped_b = -1;

    // Updating label content
    utils_label_set_content(&title_label, renderer, algorithm_titles[algo_type]);
    title_label.background.x = TITLE_X;
    title_label.background.y = TITLE_Y;
    
    utils_label_set_content(&description_label, renderer, algorithm_descriptions[algo_type]);
    description_label.background.y = TITLE_Y + title_label.background.h + 10;
    description_label.background.x = TITLE_X;

    utils_label_set_content(&source_code_label, renderer, algorithm_source_codes[algo_type]);
    source_code_label.background.x = WIDTH - source_code_label.background.w - TITLE_X;
    source_code_label.background.y = TITLE_Y;

    randomize_list();
    algorithm_implementations[algo_type](list_to_be_sorted, 0, LIST_LENGTH - 1);

    // Initialize the new variable labels
    for (int i = 0; i < algo_total_variables; i++)
    {
        utils_label_create(&variable_labels[i], main_font, 40, &white, true);
        utils_label_set_content(&variable_labels[i], renderer, algo_variables[i].token);
        variable_labels[i].background.y = BAR_Y + 5 + 20 * i;
        variable_labels[i].background.x = 0;
    }
}

void initialize_globals(void)
{
    // Creating an SDL renderer and a window
    window = SDL_CreateWindow("Visualizing Sorting Algorithms", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
        exit_with_error("failed to create the main SDL window");
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    main_font = TTF_OpenFont("res/liberation_mono.ttf", 13);
    load_algorithm_source_codes();

    swap_sfx = Mix_LoadWAV("res/swap.wav");
    
    // Initializing some sprites and labels
    utils_label_create(&source_code_label, main_font, 350, &white, true);
    utils_label_create(&title_label, main_font, 200, &white, true);
    utils_label_create(&description_label, main_font, 200, &white, true);
    
    utils_label_create(&instructions_label, main_font, 500, &gray, false);
    utils_label_set_content(&instructions_label, renderer, "(Βήματα με βελάκια, SPACE για αλλαγή αλγορίθμου)");
    instructions_label.background.y = HEIGHT - 40;
    instructions_label.background.x = (WIDTH - instructions_label.background.w) / 2;
    
    // Positioning the list rectangles correctly
    for (int i = 0; i < LIST_LENGTH; i++)
    {
        list_rectangles[i].w = BAR_WIDTH;
        list_rectangles[i].x = (WIDTH - LIST_LENGTH * (BAR_WIDTH + BAR_SPACING)) / 2 + i * (BAR_WIDTH + BAR_SPACING);
    }

    execute_algorithm(ALGO_SELECTION_SORT);
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) exit_with_error("failed to initialize SDL, is it installed?");
    if (TTF_Init() < 0) exit_with_error("failed to initialize SDL2_ttf, is it installed?");
    // Trying to open an audio device with the default, recommended sampling frequency
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) exit_with_error("failed to open audio device, SDL_mixer installed?");

    // Making randomness more interesting with a UNIX-time based seed
    srand(time(NULL));
    initialize_globals();
    
    // Implementing the visualizer's rendering loop
    SDL_Event event;
    
    for (;;)
    {
        while (SDL_PollEvent(&event))
        {
            // Check if the user has just triggered a window destruction event
            if (event.type == SDL_QUIT) goto destroy_game;

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_RIGHT:
                    // If the user wants to step forward through the process of the algorithm,
                    // just interpret the next history instruction
                    interpret_next_algorithm_history_entry();
                    break;

                case SDLK_LEFT:
                    interpret_previous_algorithm_history_entry();
                    break;

                case SDLK_SPACE:
                    // When space is pressed, the next algorithm shall be enabled
                    execute_algorithm(current_algorithm < ALGO_TOTAL - 1 ? current_algorithm + 1 : 0);
                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
        SDL_RenderClear(renderer);

        // Rendering the list contents using rectangles
        for (int i = 0; i < LIST_LENGTH; i++)
        {
            // Highlight the items that were just swapped with a special color
            if (i == swapped_a || i == swapped_b)
            {
                SDL_SetRenderDrawColor(renderer, 200, 200, 255, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 90, 90, 255, 255);
            }
        
            SDL_RenderFillRect(renderer, &list_rectangles[i]);
        }

        // Rendering explanatory labels
        utils_label_render(&instructions_label, renderer);
        utils_label_render(&title_label, renderer);
        utils_label_render(&description_label, renderer);
        utils_label_render(&source_code_label, renderer);

        // Rendering the variables' labels
        for (int i = 0; i < algo_total_variables; i++)
        {
            if (variable_labels[i].background.x >= list_rectangles[0].x)
            {
                utils_label_render(&variable_labels[i], renderer);
            }
        }

        SDL_RenderPresent(renderer);
        // Running the program at 20 frames per second to improve performance
        SDL_Delay(1000 / 20);
    }

destroy_game:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
}
