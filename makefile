CC = gcc
CCFLAGS = -Wall -Wextra -Wvla -std=c99

snake: markov_chain.h markov_chain.c snakes_and_ladders.c linked_list.c
	$(CC) $(CCFLAGS) $^ -o snakes_and_ladders

tweets: markov_chain.h markov_chain.c tweets_generator.c linked_list.c
	$(CC) $(CCFLAGS) $^ -o tweets_generator


