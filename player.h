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

#include "stddef.h"

// #include "card.h"
#include "deck.h"

// /* === [ start template compat ] === */
// int add_card(struct player* target, struct card* new_card);
// int remove_card(struct player* target, struct card* old_card);
// char check_add_book(struct player* target);
// int search(struct player* target, char rank);
// /* === [ end template compat ] === */

/**
 * @brief represents a player
 *
 * This strict was desinged so it is generic over a user player vs a
 * computer player. It does this with a customizable name / callback
 * interface
 */
typedef struct _player {
    /* --- configuration members --- */
    // string pointer to what to print for the player's name
    const char* const name;
    // callback to ask the player for a rank, passed a pointer to the
    // owning player when called
    rank_t (*const read_rank)(struct _player*);
    // whether to print the player's hand
    const bool reveal_cards;
    /* --- mutated --- */
    // the player's hand
    hand_t hand;
    // the ranks that player has collected, null terminated
    rank_t books[7];
    // overflow / canary padding
    rank_t _canary;
} player_t;

/**
 * @brief sets
 *
 * @param name see struct definition
 * @param read_rank see struct definition
 * @return player_t
 */
player_t player_init(
    const char* const name,
    bool              reveal_cards,
    rank_t (*read_rank)(struct _player*));

// TODO docstring
void player_cleanup(player_t* const);

// TODO docstring
void player_print_hand(const player_t* const);

// TODO docstring
void player_print_books(const player_t* const);

/**
 * @brief searches a hand for cards of a specific rank, put the cards
 * into an array if present
 *
 * @param hand the hand to search
 * @param for_rank the rank to search for
 * @param put_into NULL if the cards should be kept, else a pounter to
 * an `card_t array[4]`
 *
 * @return err_t: SUCCESS if found / copied if provided, ERROR
 * elsewise
 */
err_t pl_search(hand_t* hand, rank_t for_rank, card_t* put_into);

/**
 * @brief asks the player if they'd like ot continue
 *
 * @return true
 * @return false
 */
bool player_user_wants_to_play_again();

/**
 * @brief
 * @exception exits if unable to deal cards
 *
 * @param count
 */
void player_deal_cards(
    player_t* const player,
    deck_t* const   deck,
    uint8_t         count  //
);

// TODO docstring
rank_t player_query_for_rank(player_t* player);

// TODO docstring
rank_t play_compy_turn(player_t* player);

/**
 * @brief adds a rank to a player's book
 *
 * do not pass a full player with a full book.
 *
 * @return true if the player won, else false
 */
bool player_add_book_did_win(player_t* const, rank_t);