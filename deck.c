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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "deck.h"
#include "flavor.h"

void deck_init(deck_t* const d) {
    for (range(i, 0, 52, 1)) {
        d->cards[i] = (card_t){
            .suit = (i / 13) + SUIT_HEARTS, .rank = (i % 13) + RANK_2};
    };
    d->_canary = (card_t){0, 0};
    d->remaining = 52;
}

void deck_shuffle(deck_t* const deck) {
    // iterate from the top card of the deck (remaining-1) to the
    // bottom(0) and swap it with a random card below it.

    srand(time(NULL));
    // NOTE: this uses rand() with mod operator (%) which is know to
    // be a poor source of randomness when combined with  but we
    // aren't performing anything near what needs to be secure so...
    // ah well

    card_t* cards = deck->cards;
    int     rand_pos;
    for (int src_pos = deck->remaining - 1; src_pos > 0; src_pos--) {
        rand_pos = rand() % src_pos;
        // swap the cards
        card_t src_card = cards[src_pos];
        cards[src_pos] = cards[rand_pos];
        cards[rand_pos] = src_card;
    }
}

err_t deck_deal(deck_t* const deck, card_t* const into) {
    *into = CARD_NULL;
    if (deck->remaining == 0) return ERROR;
    *into = deck->cards[--deck->remaining];
    return SUCCESS;
}

size_t deck_size(deck_t* const deck);
