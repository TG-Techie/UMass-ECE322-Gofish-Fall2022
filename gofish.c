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

#include "player.h"
#include "deck.h"
#include "flavor.h"

/** === [ Requirements ] ===
 * - This project may require the following flags
 *  -std=gnu11
 *  -Wno-missing-declarations
 */

#define GO_DEBUG

void play_game() {
    // --- setup---
    // obligatory feedback
    printf("\n\n=== [ New Game ] ===\nShuffling deck...\n");

    player_t user = player_init("Player 1", true, &pl_query_for_rank);
    player_t compy = player_init("Player 2", false, &pl_compy_turn);

    deck_t deck = {0};
    deck_init(&deck);
    deck_shuffle(&deck);

    player_deal_cards(&user, &deck, 7);
    player_deal_cards(&compy, &deck, 7);

    // --- play ---
    player_t* playing = &user;
    player_t* other = &compy;

    bool game_over = false;
    while (!game_over) {
        // -- genericise the turns ---

        // new turn preamble / info
        printf("\n--- %s's Turn ---\n", playing->name);
        if (playing->show_hand) player_print_hand(playing);
#ifdef GO_DEBUG
        printf("---\n");
        player_print_hand(&user);
        player_print_hand(&compy);
        printf("---\n");
#endif
        player_print_book(&user);
        player_print_book(&compy);

        // -- start --
        rank_t looking_for = playing->read_rank(playing);
        card_t found[4] = {0};  // 0 being RANK_ERROR and SUIT_ERROR
        hand_search_remove_cards(&other->hand, looking_for, found);

        for (range(idx, 0, 3, 1)) {
            if (found[idx].rank != RANK_ERROR)
                card_pretty_print(found[idx], " ");
        }
        printf("\n");

        // -- setup for the next turn ---
        // swap users
        player_t* temp = playing;
        playing = other;
        other = temp;

        // TODO DEBUG remove after debug
        printf("King (K) to exit – ");
        if (pl_query_for_rank(&user) == RANK_KING) exit(0);
    };

    // --- cleanup ---
    player_cleanup(&user);
    player_cleanup(&user);
}

int main(int args, char* argv[]) {
    // player 1 is the user, player 2 is the computer
    do play_game();
    while (player_user_wants_to_play_again());
}
