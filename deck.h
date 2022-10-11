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

#include "card.h"

/**
 * @brief represents a deck and it's contents
 *
 * The "top" card is considered the card with the highest index while
 * still in the number of cards reamaining (always at index =
 * .remaining-1 ).
 *
 * Operations performed on a deck, excluding inialization, will
 * involve altering the value of card. That is to say, there should
 * always be a complete deck's set of cards in the array; removing a
 * card constitutes decrementing the `.remaining` and swapping or
 * rotating such cards so the removed card is not in the pool of
 * remaining cards. Pointers to card nodes can only be returned when
 * not withing the number of remaining cards.
 */
typedef struct {
    size_t remaining;
    card_t cards[52];
    card_t _canary;  // overflow / canary padding
} deck_t;

/**
 * @brief initializes a deck with 52 cards (order undefined)
 *
 * @return
 */
void deck_init(deck_t* const);

/**
 * @brief deals one card off the top of the deck
 *
 * The value of into* remains untouched unless a card is dealt
 *
 * @param deck deal a card from this deck
 * @param into deal the card into this pointer
 * @return err_t
 */
err_t deck_deal(deck_t* const deck, card_t* const into);

/**
 * @brief shuffle the remaining cards in the deck
 *
 */
void deck_shuffle(deck_t* const);

/**
 * @brief returns the number of cards remaining in the deck (always
 * 0-52)
 *
 * @param deck
 * @return size_t
 */
size_t deck_size(deck_t* const deck);
