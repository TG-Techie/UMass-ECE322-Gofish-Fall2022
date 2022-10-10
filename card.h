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

#include "flavor.h"

typedef enum {
    ERROR = 0,
    SUCCESS = 1,
} err_t;

/**
 * @brief an enum representing the suits on a card
 *
 * @return typedef enum (one byte)
 */
typedef enum __attribute__((__packed__)) {
    SUIT_ERROR = ERROR,  // only used in function returns
    SUIT_HEARTS = 2,
    SUIT_CLUBS,
    SUIT_DIAMONDS,
    SUIT_SPADES,
} suit_t;

/**
 * @brief the values representing ranks on a card
 *s
 * @return typedef enum (one byte)
 */
typedef enum __attribute__((__packed__)) {
    RANK_ERROR = ERROR,  // only used in function returns
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

#define NULLCARD \
    (card_t) { .suit = SUIT_ERROR, .rank = RANK_ERROR }

// TODO docstring
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
    card_t card;
    nullable(struct hand_node*) next;  // optional
} * hand_node_t;

/**
 * @brief A linked list containing players'hands
 *
 * @interface provides a first member next node "interface"
 */
typedef struct {
    nullable(hand_node_t) head;
    size_t length;
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
 * @brief print the card using unicode card suits
 *
 * @param end what to print after the card
 */
void card_pretty_print(card_t, char* const end);

/**
 * @brief check if one more chards in the given have have the desired
 * rank
 *
 * @param hand
 * @param rank
 * @return true
 * @return false
 */
bool hand_has_rank(hand_t* hand, rank_t rank);

/**
 * @brief add a new node the a hand_t with the given card value
 *
 */
void hand_add_card(hand_t*, card_t);