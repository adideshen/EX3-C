#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"


/***************************/
/*         DEFINE          */
/***************************/

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))
#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60
#define BASE 10
#define LINE_BREAK "\n"
#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20
#define NUM_OF_ARGC_ERROR_SNL "Usage: The program receives 2 arguments only.\n"
#define RANDOM_WALK "Random Walk"
#define ARROW "->"
#define LADDER_TO "-ladder to"
#define SNAKE_TO "-snake to"
#define THREE_ARGS 3

/***************************/

/***************************/
/*        STRUCTS          */
/***************************/

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the ladder in case
    // there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in case
    // there is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/***************************/

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * This functions print the data of a cell type object.
 * @param data pointer to cell type object.
 */
static void cell_print_func (void *data)
{
  Cell *cell = (Cell*) data;
  printf ("[%d]", cell->number);
  if (cell->number == BOARD_SIZE)
  {
    return;
  }
  if (cell->ladder_to > EMPTY)
  {
    printf ("%s %d %s ", LADDER_TO, cell->ladder_to, ARROW);
  }
  else if (cell->snake_to > EMPTY)
  {
    printf ("%s %d %s ", SNAKE_TO, cell->snake_to, ARROW);
  }
  else
  {
    printf (" %s ", ARROW);
  }
}

/**
 * This function compare the data 2 two cell type objects.
 * @param data_1 pointer to the first cell type object.
 * @param data_2 pointer to the second cell type object.
 * @return 1 if the the data of the first cell is bigger, -1 if the the data
 * of the second cell is bigger, 0 otherwise.
 */
static int cell_comp_func (const void *data_1, const void *data_2)
{
  Cell *cell_1 = (Cell*) data_1;
  Cell *cell_2 = (Cell*) data_2;
  if (cell_1->number > cell_2->number)
  {
    return 1;
  }
  else if (cell_1->number < cell_2->number)
  {
    return -1;
  }
  return 0;
}

/**
 * This function free a the data of cell type object.
 * @param data pointer to cell type object.
 */
static void cell_free_data (void *data)
{
  if (data == NULL)
  {
    return;
  }
  free (data);
}

/**
 * This function allocate new cell type object copies the data of the
 * given pointer pointer to the new cell type object.
 * @param data
 * @return pointer to the copied cell type object.
 */
static void* cell_copy_func (void const *data)
{
  Cell *copy_cell = malloc (sizeof (Cell));
  if (copy_cell == NULL)
  {
    return NULL;
  }
  Cell *org_cell = (Cell*) data;
  copy_cell->number = org_cell->number;
  copy_cell->snake_to = org_cell->snake_to;
  copy_cell->ladder_to = org_cell->ladder_to;
  return copy_cell;
}

/**
 * This function checks if the given cell is the last one on the board.
 * @param data
 * @return true if it does, false otherwise.
 */
static bool cell_is_last (void *data)
{
  Cell *cur_cell = (Cell*) data;
  return (cur_cell->number == BOARD_SIZE);
}

/**
 * This function converts a string to int.
 * @param num_in_char string that represents a number.
 * @return long int.
 */
static long int convert_char_to_int (char *num_in_char)
{
  char *remaining;
  long int num_in_int = (long int) strtol (num_in_char, &remaining, BASE);
  return num_in_int;
}

/**
 * This function creates new markov chain.
 * @return pointer to MarkovChain, NULL in case of memory allocation failure.
 */
static MarkovChain *create_markov_chain ()
{
  MarkovChain *markov_chain = malloc (sizeof (MarkovChain));
  if (markov_chain == NULL)
  {
    return NULL;
  }
  markov_chain->database = malloc (sizeof (LinkedList));
  if (markov_chain->database == NULL)
  {
    free (markov_chain);
    return NULL;
  }
  markov_chain->database->first = NULL;
  markov_chain->database->last = NULL;
  markov_chain->database->size = 0;
  markov_chain->print_func = cell_print_func;
  markov_chain->comp_func = cell_comp_func;
  markov_chain->free_data = cell_free_data;
  markov_chain->copy_func = cell_copy_func;
  markov_chain->is_last = cell_is_last;
  return markov_chain;
}

/** Error handler **/
static int handle_error (char *error_msg, MarkovChain **database)
{
    printf ("%s", error_msg);
    if (database != NULL)
    {
        free_markov_chain(database);
    }
    return EXIT_FAILURE;
}


static int create_board (Cell *cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc (sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++) {
                free (cells[j]);
            }
            handle_error(ALLOCATION_ERROR_MASSAGE,NULL);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to)
        {
            cells[from - 1]->ladder_to = to;
        }
        else
        {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database (MarkovChain *markov_chain)
{
  Cell* cells[BOARD_SIZE];
  if (create_board (cells) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  MarkovNode *from_node = NULL, *to_node = NULL;
  size_t index_to;
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    add_to_database (markov_chain, cells[i]);
  }

  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    from_node = get_node_from_database (markov_chain,cells[i])->data;

    if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
    {
      index_to = MAX (cells[i]->snake_to,cells[i]->ladder_to) - 1;
      to_node = get_node_from_database (markov_chain, cells[index_to])
          ->data;
      add_node_to_counter_list (from_node, to_node, markov_chain);
    }
    else
    {
      for (int j = 1; j <= DICE_MAX; j++)
      {
        index_to = ((Cell*) (from_node->data))->number + j - 1;
        if (index_to >= BOARD_SIZE)
        {
          break;
        }
        to_node = get_node_from_database (markov_chain, cells[index_to])
            ->data;
        add_node_to_counter_list (from_node, to_node, markov_chain);
      }
    }
  }
  // free temp arr
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    free (cells[i]);
  }
  return EXIT_SUCCESS;
}

/**
 * This function generates random sequences.
 * @param markov_chain
 * @param sequences_to_create num of sequences to generates.
 */
static void generate_sequences (MarkovChain *markov_chain, long int
sequences_to_create)
{
  for (int i = 0; i < sequences_to_create; i++)
  {
    printf ("%s %d: ", RANDOM_WALK, i+1);
    MarkovNode *first_node = markov_chain->database->first->data;
    generate_random_sequence (markov_chain, first_node, MAX_GENERATION_LENGTH);
    printf ("%s", LINE_BREAK);
  }
}


/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
  long int seed = convert_char_to_int (argv[1]);
  long int num_of_route = convert_char_to_int (argv[2]);
  if (argc != THREE_ARGS)
  {
    printf ("%s\n", NUM_OF_ARGC_ERROR_SNL);
    return EXIT_FAILURE;
  }
  MarkovChain *markov_chain = create_markov_chain();
  if (markov_chain == NULL)
  {
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  if (fill_database (markov_chain) == 1)
  {
    free_markov_chain (&markov_chain);
    return EXIT_FAILURE;
  }
  srand (seed);
  generate_sequences (markov_chain, num_of_route);
  free_markov_chain (&markov_chain);
  return EXIT_SUCCESS;
}
