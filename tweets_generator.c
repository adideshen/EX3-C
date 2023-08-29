#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "markov_chain.h"

/***************************/
/*         DEFINE          */
/***************************/

#define BASE 10
#define FOUR_ARGC 4
#define FIVE_ARGC 5
#define NUM_OF_ARGC_ERROR_TWEETS "Usage: The program receives 3 or 4 arguments\
 only.\n"
#define MAX_WORDS_IN_TWEET 20
#define PRINT_TWEET "Tweet"
#define LINE_BREAK "\n"
#define PATH_ERROR "Error: The given file is invalid.\n"
#define MAX_LINE_LEN 1001
#define MAX_WORD_LEN 101
#define DELIM " \n\t\r"
#define DOT '.'

/***************************/

/**
 * This functions print the data of a string type object.
 * @param data pointer to string type object.
 */
static void s_print_func (void *data)
{
  char *data_to_print = data;
  printf (" %s", data_to_print);
}

/**
 * This function compare 2 strings.
 * @param data_1 pointer to the first string.
 * @param data_2 pointer to the second string.
 * @return 0 if the strings are equal, 1 if the first non-matching character in
 * the first string is greater (in ASCII) than that of the second string, -1
 * otherwise.
 */
static int s_comp_func (const void *data_1, const void *data_2)
{
  const char *str_1 = data_1;
  const char *str_2 = data_2;
  return strcmp(str_1, str_2);
}

/**
 * This function free a the data of a string.
 * @param data pointer to char type object.
 */
static void s_free_data (void *data)
{
  if (data == NULL)
  {
    return;
  }
  free (data);
}

/**
 * This function allocate new char type object copies the data of the given
 * pointer pointer to the new char type object.
 * @param data pointer to a char type object.
 * @return pointer to the copied char type object.
 */
static void* s_copy_func (void const *data)
{
  char const *str_data = data;
  unsigned int str_len = strlen (str_data);
  char *copy_str = malloc ((str_len+1) * (sizeof (char)));
  return strcpy (copy_str, str_data);
}

/**
 * This function checks is a string ends with '.'.
 * @param word
 * @return true if it ends with, false otherwise.
 */
static bool s_is_last (void *data)
{
  char const *str_data = data;
  unsigned long str_len = strlen (str_data);
  return (str_data[str_len - 1] == DOT);
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
 * This functions checks if the num of arguments and input file are valid.
 * @param argc number of arguments the user entered.
 * @param argv the arguments the user entered.
 * @return EXIT_SUCCESS if the user's input is valid, EXIT_FAILURE otherwise.
 */
static bool check_args_validity (int argc, const char *path)
{
  if ((argc != FOUR_ARGC) & (argc != FIVE_ARGC))
  {
    printf ("%s", NUM_OF_ARGC_ERROR_TWEETS);
    return false;
  }
  FILE *in_file = fopen (path, "r");
  if (in_file == NULL)
  {
    printf ("%s", PATH_ERROR);
    return false;
  }
  return true;
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
    return NULL;
  }
  markov_chain->database->first = NULL;
  markov_chain->database->last = NULL;
  markov_chain->database->size = 0;
  markov_chain->print_func = s_print_func;
  markov_chain->comp_func = s_comp_func;
  markov_chain->free_data = s_free_data;
  markov_chain->copy_func = s_copy_func;
  markov_chain->is_last = s_is_last;
  return markov_chain;
}

/**
 * This function parses a single line and adds new nodes and updates counter
 * lists if needed.
 * @param words_to_read num of words that will be read.
 * @param markov_chain a pointer to the markov chain.
 * @param
 * en_1 the word that appears before the current word.
 * @param token_2 the current word that will be added to the markov chain if
 * its new.
 * @param words_limit_flag a flag that if its equal 1 it means the user
 * limited the number of words that will be read and if its equal to 0 it
 * means the all file should be read.
 * @param beginning_of_line a flag that says whether the current word appears
 * at the beginning of a line or not
 * @return EXIT_FAILURE in case of memory allocation failure, EXIT_SUCCESS
 * otherwise.
 */
static int parse_line (long int *words_to_read, MarkovChain
*markov_chain, char *token_1, char *token_2, int words_limit_flag,
                int beginning_of_line)
{
  while ((token_2 != NULL) & (0 < *words_to_read))
  {
    Node *second_node = get_node_from_database (markov_chain, token_2);
    if (second_node == NULL)
    {
      second_node = add_to_database (markov_chain, token_2);
    }
    if (beginning_of_line == 0)
    {
      Node *first_node = get_node_from_database (markov_chain,token_1);
      if (s_is_last (token_1) == false)
      {
        if (add_node_to_counter_list (first_node->data,
                                      second_node->data, markov_chain)
            == false)
        {
          return EXIT_FAILURE;
        }
      }
    }
    if (words_limit_flag)
    {
      (*words_to_read)--;
    }
    beginning_of_line = 0;
    strcpy (token_1, token_2);
    token_2 = strtok (NULL, DELIM);
  }
  return EXIT_SUCCESS;
}

/**
 * This function fills all the wanted data to a markov chain.
 * @param fp a pinter to the file that includes all the words.
 * @param words_to_read If the number of words to be read is limited then the
 * number of the words itself, and if not then 0.
 * @param markov_chain a pointer to the markov chain.
 * @return
 */
static int fill_database (FILE *fp, long int words_to_read, MarkovChain
*markov_chain)
{
  int words_limit_flag = 1;
  if (words_to_read == 0)
  {
    words_to_read = 1;
    words_limit_flag = 0;
  }
  char new_line[MAX_LINE_LEN];
  while (fgets (new_line, MAX_LINE_LEN, fp) != NULL)
  {
    char token_1[MAX_WORD_LEN];
    char *token_2;
    token_2 = strtok (new_line, DELIM);
    if (parse_line (&words_to_read, markov_chain, token_1, token_2,
                    words_limit_flag, 1) == EXIT_FAILURE){
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

/**
 * This function generates random sequences.
 * @param markov_chain
 * @param tweet_to_create num of tweets to generates.
 */
void generate_sequences (MarkovChain *markov_chain, long int
tweet_to_create)
{
  for (int i = 0; i < tweet_to_create; i++)
  {
    printf ("%s %d:", PRINT_TWEET, i+1);
    MarkovNode *first_node = get_first_random_node (markov_chain);
    generate_random_sequence (markov_chain, first_node, MAX_WORDS_IN_TWEET);
    printf ("%s", LINE_BREAK);
  }
}

int main (int argc, char *argv[])
{
  long int seed = convert_char_to_int (argv[1]);
  long int num_of_tweets = convert_char_to_int (argv[2]);
  const char *path = argv[3];
  long int words_to_read = 0;
  if (!check_args_validity (argc, path))
  {
    return EXIT_FAILURE;
  }
  if (argc == FIVE_ARGC)
  {
    words_to_read = convert_char_to_int (argv[4]);
  }
  FILE *in_file = fopen (path, "r");
  if (in_file == NULL)
  {
    printf ("%s", PATH_ERROR);
    return EXIT_FAILURE;
  }
  MarkovChain *markov_chain = create_markov_chain ();
  if (markov_chain == NULL)
  {
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  if (fill_database (in_file, words_to_read, markov_chain) == 1)
  {
    fclose (in_file);
    free_markov_chain (&markov_chain);
    return EXIT_FAILURE;
  }
  srand (seed);
  generate_sequences (markov_chain, num_of_tweets);
  fclose (in_file);
  free_markov_chain (&markov_chain);
  return EXIT_SUCCESS;
}