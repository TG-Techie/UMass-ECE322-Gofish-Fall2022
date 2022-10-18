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

/** === [ Notes ] ===
 * This project may require the following flags:
 *  -std=gnu11
 *  -Wno-missing-declarations
 *  -Wshadow
 *  -Wno-format (for macOS & Linux cross compatibility)
 *
 * Undefined behavior in the assingment:
 *  Where the rules given miss an edge case I used softgame's rules
 *  http://www.softgame.net/cards/go-fish-rules.htm
 */

#include <stdio.h>
#include <stdlib.h>

#include "player.h"
#include "deck.h"

// #define GO_DEBUG

/**
 * @brief Information required to continue the game after a single turn.
 * Effectively the edge on the graph of a state machine for this game.
 *
 */
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
    /* === [ Commence Turn ] === */
    printf("=== %s's Turn ===\n", playing->name);

    /* --- [ empty hand ] --- */
    // if the player's hand is empty, draw a card if able
    card_t draw_up = CARD_NULL;
    if (playing->hand.length == 0) {
        // the deck has cards
        if (deck_deal(deck, &draw_up)) {
            printf("%s has no cards, drawing...\n", playing->name);
            hand_add_card(&playing->hand, draw_up);
        }
        // if they could not draw a card and have no cards pass the turn
        else {
            printf(
                "%s has no cards and cannot draw a card from an empty deck, "
                "passing the turn...\n",
                playing->name);
            return TURN_NEXT;
        }
    }

    // -- preamble / info --

    player_print_hand(user_player);
#ifdef GO_DEBUG
    player_print_hand(compy_player);
#endif
    player_print_books(user_player);
    player_print_books(compy_player);
    printf("\n");

    /* === [ Choose and Request a Rank ] === */
    turn_result_t result = TURN_NEXT;
    rank_t        desired = playing->read_rank(playing);
    card_t        cards[8] = {0};  // 8 just in case

    // extract and count the other player's card matching ther desired rank
    int other_count = 0;
    hand_search_remove_cards(&other->hand, desired, &cards[0], &other_count);

    // extract the cards from this player's hand, this makes
    // counting/checking for a book easy
    int total = other_count;
    hand_search_remove_cards(
        &playing->hand, desired, &cards[other_count], &total);

    // if the other player had cards
    if (other_count > 0) {
        char* a_cards_str;

        // print the other player's cards
        cards_asfmt(&a_cards_str, cards, 0, other_count);
        printf(
            "    %s had " ESC_GRN "%s" ESC_RST "\n",
            other->name,
            a_cards_str);
        free(a_cards_str);

        // print the current player's cards
        cards_asfmt(&a_cards_str, cards, other_count, total);
        printf(
            "    %s had " ESC_GRN "%s" ESC_RST "\n",
            playing->name,
            a_cards_str);
        free(a_cards_str);

    }
    // if the other player had none
    else {
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
                "    %s drew the card they asked for %s%s%s\n",
                playing->name,
                ESC_GRN,
                buf.str,
                ESC_RST);
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
            if (player_add_book_did_win(playing, drawn.rank)) {
                return TURN_WON;
            } else {
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
        if (player_add_book_did_win(playing, desired)) {
            return TURN_WON;
        }
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

void play_game() {
    // --- setup---
    // obligatory feedback
    printf("\n\n=== [ New Game ] ===\nShuffling deck...\n\n");

    player_t user = player_init("Player 1", true, &player_query_for_rank);
    player_t compy = player_init("Player 2", false, &play_compy_turn);

    deck_t deck = {0};
    deck_init(&deck);
    deck_shuffle(&deck);

    player_deal_cards(&user, &deck, 7);
    player_deal_cards(&compy, &deck, 7);

    // --- play ---
    player_t* playing = &user;
    player_t* other = &compy;

    player_t* player_that_won = NULL;  // nullable
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

    // --- cleanup ---
    player_cleanup(&user);
    player_cleanup(&user);
}

int main(int args, char* argv[]) {
    // player 1 is the user, player 2 is the computer
    do play_game();
    while (player_user_wants_to_play_again());
}
