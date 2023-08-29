cmake_minimum_required(VERSION 3.16)
project(ex3b_adideshen C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(ex3b_adideshen
        linked_list.c
        linked_list.h
        markov_chain.c
        markov_chain.h
#        snakes_and_ladders.c)
        tweets_generator.c)
