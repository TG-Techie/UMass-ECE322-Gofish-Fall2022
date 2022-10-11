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

typedef enum {
    TURN_NEXT,
    TURN_EXTRA,
    TURN_WON,
    TURN_LOST,
} turn_result_t;

/**
 * @brief performs a turn
 *
 * --- turn structure ---
 * // TODO deal empty hands
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
 *     check if the new card makes a new book
 *
 * if extracted len == 4 (book length)
 *   then add_checK_book_did_win
 * else
 *   add the cards to the hand
 *
 * if won: break
 * elif extra turn: continue
 *
 * swap players
 */
turn_result_t play_turn(
    player_t* const playing,
    player_t* const other,
    deck_t* const   deck,
    player_t* const user_player,
    player_t* const compy_player  //
) {
    // -- preamble / info --
    printf("=== %s's Turn ===\n", playing->name);
    player_print_hand(user_player);
#ifdef GO_DEBUG
    player_print_hand(compy_player);
#endif
    player_print_books(user_player);
    player_print_books(compy_player);
    printf("\n");

    // -- commence turn --
    rank_t desired = playing->read_rank(playing);
    // TODO deal with empty deck

    card_t cards[7] = {0};  // (0 == CARD_NULL)
    int    other_count = 0;
    hand_search_remove_cards(&other->hand, desired, &cards[0], &other_count);
    int total = other_count;
    hand_search_remove_cards(
        &playing->hand, desired, &cards[other_count], &total);

    // Values to return
    turn_result_t result = TURN_NEXT;

    if (other_count > 0) {
        // print found cards
        char* a_cards_str;

        cards_asfmt(&a_cards_str, cards, 0, other_count);
        printf(
            "    %s had " ESC_GRN "%s" ESC_RST "\n",
            other->name,
            a_cards_str);
        free(a_cards_str);

        cards_asfmt(&a_cards_str, cards, other_count, total);
        printf(
            "    %s had " ESC_GRN "%s" ESC_RST "\n",
            playing->name,
            a_cards_str);
        free(a_cards_str);
    } else {
        printf(
            "    %s has no rank %s cards\n",
            other->name,
            rank_as_str(desired));

        // deal and print a
        card_t drawn = CARD_NULL;

        card_pretty_str_t buf;
        if (deck_deal(deck, &drawn)) {
            card_sfmt(drawn, &buf);
            printf(
                "    Go fish! %s draws a card " ESC_GRN "%s" ESC_RST "\n",
                playing->name,
                playing->reveal_cards ? buf.str : "");
        } else {
            printf("    Cannot go fish, the deck is empty\n");
        }

        // add the card to hand or book
        if (drawn.rank == desired) {
            result = TURN_EXTRA;
            cards[total++] = drawn;
            printf(
                "    %s drew the card they asked for " ESC_GRN "%s" ESC_RST
                "\n",
                playing->name,
                buf.str);
        } else if (  // this completes a book from what's in out hand
            drawn.rank != RANK_NULL &&
            3 == hand_has_rank(&playing->hand, drawn.rank)  //
        ) {
            card_t drawn_book[7];
            int    book_sanity_check = 0;
            hand_search_remove_cards(
                &playing->hand, drawn.rank, drawn_book, &book_sanity_check);
            if (book_sanity_check != 3)
                ohcrap("hand rank count mismatch, there're problems");
            if (player_add_book_did_win(playing, drawn.rank))
                result = TURN_WON;
            else {
                result = TURN_EXTRA;
                printf(
                    "    %s drew the %s (making a the book of the %s "
                    "cards)\n",
                    playing->name,
                    buf.str,
                    rank_as_str(drawn.rank));
            }
        } else if (drawn.rank != RANK_NULL) {
            // then this card is cool but can just go right in our hand
            hand_add_card(&playing->hand, drawn);
        } else  // the deck was empty
            ;   // TODO handle no cards in deck;
    }

    // exit early if possible
    if (result == TURN_WON) {
        printf("\n");
        return result;
    }

    // either add the desired book
    if (total == 4) {
        if (player_add_book_did_win(playing, desired)) result = TURN_WON;
        printf(
            "    %s made a book of the %s cards\n",
            playing->name,
            rank_as_str(desired));
        result = TURN_EXTRA;
    } else {
        for (range(idx, 0, total, 1))
            hand_add_card(&playing->hand, cards[idx]);
    }

    if (result == TURN_EXTRA)
        printf("    %s gets another turn\n", playing->name);

    printf("\n");
    return result;
    // card_pretty_str_t card_str;
    // card_pretty_print(card_str, )
}
// {
//     // -- genericise the turns ---

//     // new turn preamble / info
//     printf("\n--- %s's Turn ---\n", playing->name);
//     player_print_hand(&user);
// #ifdef GO_DEBUG
//     printf("---\n");
//     player_print_hand(&compy);
//     printf("---\n");
// #endif
//     player_print_books(&user);
//     player_print_books(&compy);

//     // -- start --
//     rank_t desired = playing->read_rank(playing);
//     card_t found[7] = {0};  // 0 being RANK_NULL and SUIT_NULL
//     int    count = 0;
//     hand_search_remove_cards(&other->hand, desired, found, &count);

//     // perform action based on cards found
//     bool   gets_an_extra_turn = false;
//     card_t card = CARD_NULL;
//     if (count > 0) {
//         // print the cards
//         printf("%s has ", other->name);
//         cards_print_arr(found, count, "no cards");

//         // see if cards form a book by removing them from the surrent
//         // player
//         int start = count;
//         hand_search_remove_cards(
//             &playing->hand, desired, &found[count], &count);

//         // show
//         printf("%s has ", playing->name);
//         cards_print_arr(&found[start], count - start, "no cards");

//         // add a book if present, else into the hand
//         if (count == 4) {
//             playing_just_won = player_add_book_did_win(playing,
//             desired); player_print_books(playing);
//         } else {
//             for (range(idx, 0, count, 1))
//                 hand_add_card(&playing->hand, found[idx]);
//         }
//     } else {
//         printf(
//             "Go fish! %s had no %s cards\n",
//             other->name,
//             rank_as_str(desired));
//         // deal a card to
//         if (deck_deal(&deck, &card)) {
//             hand_add_card(&playing->hand, card);
//             printf("%s draws a card ", playing->name);
//             if (playing->reveal_cards) card_pretty_print(card, "");
//             printf("\n");
//         } else {
//             printf("no cards left to deal\n");
//         }

//         // -- check for an extra turn --
//         gets_an_extra_turn = (card.rank == desired);
//     }

//     if (playing_just_won) {
//         break;  // handle winning out of the loop
//     }

//     // TODO check for a new book

//     if (gets_an_extra_turn) {
//         printf(
//             "Wait... %s drew the card they were looking for ",
//             playing->name);
//         card_pretty_print(
//             card, ", luck like that deserves an extra turn!\n");
//         continue;
//     }
// }

void play_game() {
    // --- setup---
    // obligatory feedback
    printf("\n\n=== [ New Game ] ===\nShuffling deck...\n\n");

    player_t user = player_init("Player 1", true, &pl_query_for_rank);
    player_t compy = player_init("Player 2", false, &pl_compy_turn);

    deck_t deck = {0};
    deck_init(&deck);
    // #ifndef GO_DEBUG
    deck_shuffle(&deck);
    // #endif

    player_deal_cards(&user, &deck, 7);
    player_deal_cards(&compy, &deck, 7);

    // --- play ---
    player_t* playing = &user;
    player_t* other = &compy;

    nullable(player_t*) player_that_won = NULL;
    while (player_that_won == NULL) {
        switch (play_turn(playing, other, &deck, &user, &compy)) {
            case TURN_WON:
                // TODO print stuff
                player_that_won = playing;
                break;
            case TURN_LOST:
                player_that_won = other;
                break;
            case TURN_EXTRA:
                // TODO print stuff
                continue;
            case TURN_NEXT: {
                player_t* temp = playing;
                playing = other;
                other = temp;
                break;
            }
        }
    };

    printf("Contgrats %s has won!\n", player_that_won->name);

    // --- cleanup ---
    player_cleanup(&user);
    player_cleanup(&user);
}

int main(int args, char* argv[]) {
    // player 1 is the user, player 2 is the computer
    do play_game();
    while (player_user_wants_to_play_again());
}
