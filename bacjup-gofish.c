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

    bool playing_just_won = false;
    while (!playing_just_won) {
        // -- genericise the turns ---

        // new turn preamble / info
        printf("\n--- %s's Turn ---\n", playing->name);
        player_print_hand(&user);
#ifdef GO_DEBUG
        printf("---\n");
        player_print_hand(&compy);
        printf("---\n");
#endif
        player_print_books(&user);
        player_print_books(&compy);

        // -- start --
        rank_t desired = playing->read_rank(playing);
        card_t found[7] = {0};  // 0 being RANK_NULL and SUIT_NULL
        int    count = 0;
        hand_search_remove_cards(&other->hand, desired, found, &count);

        // TODO === !!!!! Always check for stuff in the current hand !!!!!!
        // ===
        PICKUP_HERE(
            "always check for both, that way if you draw the final card for "
            "a book it;s acocunted for");
        /** --- turn structure ---
         * ask for desired rank
         * check other's hand (save count)
         * check your hand (save total)
         * if count > 0:
         *   then show related info
         * else: go fish
         *   - if the desired rank
         *     then add to the extracted cards
         *     then mark turn for repeat
         *   - else
         *     then add to hand
         *
         * if extracted len == 4 (book length)
         *   then add_checK_book_did_win
         * else
         *   add the cards to the hand
         *
         * if won: break
         * elif extra turn: continue
         *
         *
         *
         * swap players
         */

        // perform action based on cards found
        bool   gets_an_extra_turn = false;
        card_t card = CARD_NULL;
        if (count > 0) {
            // print the cards
            printf("%s has ", other->name);
            cards_print_arr(found, count, "no cards");

            // see if cards form a book by removing them from the surrent
            // player
            int start = count;
            hand_search_remove_cards(
                &playing->hand, desired, &found[count], &count);

            // show
            printf("%s has ", playing->name);
            cards_print_arr(&found[start], count - start, "no cards");

            // add a book if present, else into the hand
            if (count == 4) {
                playing_just_won = player_add_book_did_win(playing, desired);
                player_print_books(playing);
            } else {
                for (range(idx, 0, count, 1))
                    hand_add_card(&playing->hand, found[idx]);
            }
        } else {
            printf(
                "Go fish! %s had no %s cards\n",
                other->name,
                rank_as_str(desired));
            // deal a card to
            if (deck_deal(&deck, &card)) {
                hand_add_card(&playing->hand, card);
                printf("%s draws a card ", playing->name);
                if (playing->reveal_cards) card_sfmt(card, "");
                printf("\n");
            } else {
                printf("no cards left to deal\n");
            }

            // -- check for an extra turn --
            gets_an_extra_turn = (card.rank == desired);
        }

        if (playing_just_won) {
            break;  // handle winning out of the loop
        }

        // TODO check for a new book

        if (gets_an_extra_turn) {
            printf(
                "Wait... %s drew the card they were looking for ",
                playing->name);
            card_sfmt(card, ", luck like that deserves an extra turn!\n");
            continue;
        }

        // -- setup for the next turn ---
        // swap players
        player_t* temp = playing;
        playing = other;
        other = temp;

        // // TODO DEBUG remove after debug
        // printf("King (K) to exit – ");
        // if (pl_query_for_rank(&user) == RANK_KING) exit(0);
    };

    printf("Contgrats %s has won!\n", playing->name);

    // --- cleanup ---
    player_cleanup(&user);
    player_cleanup(&user);
}

int main(int args, char* argv[]) {
    // player 1 is the user, player 2 is the computer
    do play_game();
    while (player_user_wants_to_play_again());
}
