#include "markov_chain.h"
#include <string.h>
#include <stdlib.h>

#define LINE_BREAK "\n"

/**
 * This function adds the a new node to a markov chain.
 * @param markov_chain
 * @param new_node
 * @param data_ptr a string of the new word that will be added to the markov
 * chain
 * @return NULL if the allocation of the new node failed, a pointer to the
 * MarkovNode of the new node.
 */
void* add_node_to_markov_chain (MarkovChain *markov_chain, void *data_ptr)
{
  MarkovNode *markov_node = malloc (sizeof (MarkovNode));
  if (markov_node == NULL)
  {
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  void *node_data = markov_chain->copy_func (data_ptr);
  markov_node->data = node_data;
  markov_node->counter_list = NULL;
  markov_node->num_of_next_nodes = 0;
  if (add (markov_chain->database, markov_node) == 1)
  {
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  return markov_node;
}

Node* get_node_from_database (MarkovChain *markov_chain, void *data_ptr)
{
  if (markov_chain->database->size == 0)
  {
    return NULL;
  }
  Node *cur_node = markov_chain->database->first;
  while (cur_node != NULL)
  {
    if (markov_chain->comp_func (cur_node->data->data, data_ptr) == 0)
    {
      return cur_node;
    }
    cur_node = cur_node->next;
  }
  return NULL;
}

bool add_node_to_counter_list (MarkovNode *first_node, MarkovNode *second_node,
                              MarkovChain *markov_chain)
{
  first_node->counter_list = realloc (first_node->counter_list,
                                      (first_node->num_of_next_nodes + 1)
                                      *sizeof
                                      (NextNodeCounter));
  if (first_node->counter_list == NULL)
  {
    printf ("%s", ALLOCATION_ERROR_MASSAGE);
    return false;
  }
  for (long int i = 0; i < first_node->num_of_next_nodes; i++)
  {
    if (markov_chain->comp_func (first_node->counter_list[i].markov_node->data,
                                 second_node->data) == 0)
    {
      first_node->counter_list[i].frequency++;
      return true;
    }
  }
  NextNodeCounter next_node_counter;
  next_node_counter.markov_node = second_node;
  next_node_counter.frequency = 1;
  first_node->counter_list[(first_node->num_of_next_nodes)] =
      next_node_counter;
  first_node->num_of_next_nodes++;
  return true;
}

Node* add_to_database(MarkovChain *markov_chain, void *data_ptr)
{
  Node *cur_node = get_node_from_database (markov_chain, data_ptr);
  if (cur_node == NULL)
  {
    if (add_node_to_markov_chain (markov_chain, data_ptr) == NULL)
    {
      return NULL;
    }
  }
  return markov_chain->database->last;
}

/**
* Get random number between 0 and max_number [0, max_number).
* @param max_number maximal number to return (not including)
* @return Random number
*/
int get_random_number(int max_number)
{
  return rand() % max_number;
}

MarkovNode* get_first_random_node(MarkovChain *markov_chain)
{
  int random_num = get_random_number (markov_chain->database->size);
  Node *random_node = markov_chain->database->first;
  for (int cur_index = 0; cur_index < random_num; cur_index++)
  {
    random_node = random_node->next;
  }
  if (markov_chain->is_last (random_node->data->data) == true)
  {
    return get_first_random_node (markov_chain);
  }
    return random_node->data;
}

MarkovNode* get_next_random_node (MarkovNode *state_struct_ptr)
{
  int counter = 0;
  for (int i = 0; i < state_struct_ptr->num_of_next_nodes; i++)
  {
    counter += state_struct_ptr->counter_list[i].frequency;
  }
  int random_num = get_random_number (counter);
  MarkovNode *random_markov_node;
  long int cur_iter = 0;
  for (int j = 0; j < state_struct_ptr->num_of_next_nodes; j++)
  {
    cur_iter += state_struct_ptr->counter_list[j].frequency;
    if (cur_iter > random_num)
    {
      random_markov_node = state_struct_ptr->counter_list[j].markov_node;
      return random_markov_node;
    }
  }
  return NULL;
}

void generate_random_sequence (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length)
{
  if (first_node == NULL)
  {
    first_node = get_first_random_node (markov_chain);
  }
  markov_chain->print_func (first_node->data);
  MarkovNode *next_node = get_next_random_node (first_node);
  markov_chain->print_func (next_node->data);
  int num_of_words = 2;
  while ((num_of_words < max_length) &
         (markov_chain->is_last (next_node->data) == false) &
         (next_node->num_of_next_nodes > 0))
  {
    next_node = get_next_random_node (next_node);
    markov_chain->print_func (next_node->data);
    num_of_words++;
  }
}

void free_markov_chain(MarkovChain ** ptr_chain)
{
  Node *cur_node = (*ptr_chain)->database->first;
  Node *temp;
  while (cur_node != NULL)
  {
    free (cur_node->data->data);
    free (cur_node->data->counter_list);
    (*ptr_chain)->free_data (cur_node->data);
    temp = cur_node->next;
    free (cur_node);
    cur_node = temp;
  }
  free ((*ptr_chain)->database);
  free (*ptr_chain);
}