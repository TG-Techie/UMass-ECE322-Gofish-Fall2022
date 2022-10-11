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
#include <stdbool.h>

#include "deck.h"
#include "flavor.h"

bool test_deck_shuffle_okay() {
    // deck_t ordered = {0};
    // deck_init(&ordered);

    deck_t shuffled = {0};
    deck_init(&shuffled);
    deck_shuffle(&shuffled);

    typedef struct {
        card_t card;
        int    count;
    } entry_t;

    entry_t entries[52] = {0};

    // check there are 23 of each suit and 4 of each numbers
    for (range(i, 51, -1, -1)) {
        // printf("%i\n", i);
        card_t card = shuffled.cards[i];
        // then increment the corresponding
        int idx = (card.suit - SUIT_HEARTS) * 13 + card.rank - RANK_2;
        entries[idx].count++;
        entries[idx].card = card;
    }

    bool passed = true;
    for (range(i, shuffled.remaining - 1, -1, -1)) {
        if (entries[i].count != 1) {
            passed = false;
            card_sfmt(entries[i].card, "");
            printf(" found %i times\n", entries[i].count);
        }
    }
    return passed;
}

#define testcase(name) \
    { printf("%s %s\n", name() ? "[✅] passed" : "[❌] failed", #name); }

int main() { testcase(test_deck_shuffle_okay); }