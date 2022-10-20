// Copyright 2022 Jonah 'Jay' Yolles-Murphy (TG-Techie)
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be
//  included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Terminal escape color codes
 */
#define ESC_BLK "\e[30m"
#define ESC_RED "\e[31m"
#define ESC_GRN "\e[32m"
#define ESC_YLW "\e[33m"
#define ESC_BLU "\e[34m"
#define ESC_MAG "\e[35m"
#define ESC_CYN "\e[36m"
#define ESC_WHT "\e[37m"
#define ESC_RST "\e[0m"

/**
 * @brief A sugar-only macro used to enforce for loop structure
 *
 * @param name the name of the loop variable
 * @param start <int> the first number to emit when iterating
 * @param upto <int> stop iterating when reached, excluded from emitted
 * values
 * @param step <int> the value to jump by between iterations
 */
#define range(name, start, upto, step)          \
    int name = start;                           \
    (step > 0) ? (name < upto) : (name > upto); \
    name += step

/**
 * @brief A cross-enum defined return type.
 *
 * @return typedef enum (one byte)
 */
typedef enum __attribute__((__packed__)) {
    ERROR = 0,
    SUCCESS = 1,
} err_t;

/**
 * @brief an enum representing the suits on a card
 *
 * @return typedef enum (one byte)
 */
typedef enum __attribute__((__packed__)) {
    SUIT_NULL = ERROR,  // only used in function returns
    SUIT_HEARTS = 2,
    SUIT_CLUBS,
    SUIT_DIAMONDS,
    SUIT_SPADES,
} suit_t;

/**
 * @brief the values representing ranks on a card
 *
 * @return typedef enum (one byte)
 */
typedef enum __attribute__((__packed__)) {
    RANK_NULL = ERROR,  // only used in function returns
    RANK_2 = 6,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    RANK_9,
    RANK_10,
    RANK_JACK,
    RANK_QUEEN,
    RANK_KING,
    RANK_ACE,
} rank_t;

/**
 * @brief A struct representing a specific card
 *
 */
typedef struct {
    suit_t suit;
    rank_t rank;
} card_t;

#define CARD_NULL ((card_t){.suit = SUIT_NULL, .rank = RANK_NULL})

/**
 * @brief a linked list node of a hand / group of cards
 * @ownership the hand this node is a member of
 *
 * ownership of a hand node cannot be transfered, inorder to transfer,
 * meaning: a hand node must be cleared of all values before it can be
 * removed from a hand. The calling cod emay then clean it up as
 * required. (buttom line, re-allocation is suggested)
 *
 */
typedef struct hand_node {
    card_t            card;
    struct hand_node* next;  // nullable
} * hand_node_t;
// TODO: determine if defining a struct pointer in a typedef is poor practice

/**
 * @brief A linked list containing players'hands
 *
 * @interface provides a first member next node "interface"
 */
typedef struct {
    hand_node_t head;  // nullable
    size_t      length;
} hand_t;

/**
 * @brief parse a char* as a typed suit
 *
 * the passed string should not have whitespace padding and my be one
 * or characters long
 *
 * @return rank_t
 */
rank_t rank_from_str(char* const);

/**
 * @brief return a pointer to a rank's short description
 *
 * @return rank_t
 */
const char* rank_as_str(rank_t);

/**
 * @brief type alias to make declaring card strings clearer
 *
 */
typedef struct {
    char str[8];  // more than needed just to be safe
} card_pretty_str_t;

/**
 * @brief print the card using unicode card suits
 *
 * @param end what to print after the card
 */
void card_sfmt(card_t, card_pretty_str_t*);

/**
 * @brief check if one more chards in the given have have the desired
 * rank
 *
 * @return the number of cards of that rank
 */
int hand_has_rank(const hand_t* const hand, rank_t rank);

/**
 * @brief add a new node the a hand_t with the given card value
 *
 */
void hand_add_card(hand_t*, card_t);

/**
 * @brief searches for all the cards of the given ranks and puts them
 * into the dest pointer
 *
 * The written array will be terminatored with a RANK_NULL and
 * SUIT_NULL card.
 *
 * @param hand the head to search in
 * @param rank the rank to search for
 * @param dest pointer to an array at least 4 card_t long
 * @param count increments this int by the number of cards found
 */
void hand_search_remove_cards(
    hand_t* const hand,
    rank_t        rank,
    card_t* const dest,
    int* const    count);

/**
 * @brief a new string formats the passed cards into it
 *
 * @param new_string
 * @param cards
 * @param from_idx must be less than upto_idx
 * @param upto_idx prints upto beu not including this index
 */
void cards_asfmt(
    char**              new_string,
    const card_t* const cards,
    size_t              from_idx,
    size_t              upto_idx);

/**
 * @brief print an error message and bail out of the program, this should
 * never need to be called
 *
 * @param msg the string to print as the error message
 */
void __attribute__((noreturn)) ohcrap(const char* const msg);
