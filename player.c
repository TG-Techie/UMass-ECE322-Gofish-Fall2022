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

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>

#include "player.h"

void __attribute__((noreturn)) ohcrap(const char *const msg) {
    fprintf(stderr, "\nError: %s\n", msg);
    exit(-1);
}

player_t player_init(
    const char *const name,
    bool              reveal_cards,
    rank_t (*read_rank)(struct _player *)  //
) {
    // base setup
    player_t p = {
        .hand = (hand_t){.head = NULL, .length = 0},
        .name = name,
        .reveal_cards = reveal_cards,
        .read_rank = read_rank,
        .books =
            {RANK_NULL,
             RANK_NULL,
             RANK_NULL,
             RANK_NULL,
             RANK_NULL,
             RANK_NULL}  //
    };

    // set the overflow member to 0 as an always present null
    // terminator / canary
    p._canary = 0;

    return p;
}

void player_cleanup(player_t *player) {
    // free each hand node
    hand_node_t node = player->hand.head;
    int         node_count = 0;
    while (node != NULL) {
        hand_node_t prev = node;
        node = prev->next;
        free(prev);
        node_count++;
    };

    // check for leak or ~~daemons eating your socks due to undefined
    // behavior~~... i mean double-free
    if (node_count != player->hand.length) {
        char *errmsg;
        asprintf(
            &errmsg,
            "node mismatch while cleaning up '%s', (%i != %u)",
            player->name,
            node_count,
            player->hand.length);
        ohcrap(errmsg);
    }

    // then clear out the hand
    player->hand = (hand_t){.head = NULL, .length = 0};
}

void player_print_hand(const player_t *const player) {
    printf("%s's hand – ", player->name);
    // printf(" {%u} ", player->hand.length);

    hand_node_t node = player->hand.head;
    while (node != NULL) {
        card_pretty_str_t buf;
        card_sfmt(node->card, &buf);
        printf("%s ", buf.str);
        node = node->next;
    }
    printf("\n");
}

void player_print_books(const player_t *const player) {
    printf("%s's books – ", player->name);

    bool prev_was_blank = false;
    for (range(idx, 0, 7, 1)) {
        // check: if the book was previously terminated the rest
        // should be RANK_NULLS too
        if (prev_was_blank && player->books[idx] != RANK_NULL)
            ohcrap("invalid book, incorrectly terminated");

        // otherwise, print
        rank_t book = player->books[idx];
        if (book != RANK_NULL) printf("% 2s ", rank_as_str(book));
    }
    printf("\n");
}

bool player_user_wants_to_play_again() {
    for (;;) {
        // ask
        printf("Do you want to play again [Y/N]: " ESC_RED);
        char str[7];
        if (fgets(str, 4, stdin) == NULL)
            ohcrap(ESC_RST " fgets failed, something's serisouly wrong");
        char rank_input = '\0';
        for (range(i, 0, strlen(str), 1)) {
            if (str[i] != ' ') {
                rank_input = str[i];
                break;
            }
        }
        printf(ESC_RST);  // always turn red off

        // parse
        switch (toupper(rank_input)) {
            case 'Y':
                return true;
            case 'N':
                return false;
            default:
                printf("Invalid input\n");
        };
    };
}

rank_t player_query_for_rank(player_t *player) {
    for (;;) {
        printf("What are you looking for? enter a Rank: " ESC_RED);

        // query for the rank
        char str[7] = {0};
        if (fgets(str, 5, stdin) == NULL)
            ohcrap(ESC_RST " fgets failed, something's serisouly wrong");

        printf(ESC_RST);

        // to pass this to rank_from_str, write a null terminator
        // in any '\n' char
        for (range(i, 0, 3, 1)) str[i] = str[i] == '\n' ? '\0' : str[i];

        rank_t r = rank_from_str(str);

        // TODO make sure the rank is one the user has

        // maybe done
        if (hand_has_rank(&player->hand, r) && r != RANK_NULL) {
            return r;
        }

        // otherwise warn and repeat
        printf("Invalid choice '%s', enter a valid rank you have\n", str);
    }
}

rank_t play_compy_turn(player_t *player) {
    // randomly select a card's index and we'll return it's rank
    srand(time(NULL));
    int idx = rand() % player->hand.length;

    // if the hand is empty, error and return
    if (player->hand.length == 0) return RANK_NULL;

    // go borrow the card, the hand still maintains ownership
    hand_node_t node = player->hand.head;
    for (range(_, 0, idx, 1)) {
        if (node->next == NULL)
            ohcrap(
                "cannot select a card past the end of the  copmy's "
                "hand ().");
        node = node->next;
    }

    rank_t rank = node->card.rank;

    printf(
        "%s is looking for Rank: " ESC_CYN "%s" ESC_RST "\n",
        player->name,
        rank_as_str(rank));

    // then return the rank
    return rank;
}

void player_deal_cards(
    player_t *const player,
    deck_t *const   deck,
    uint8_t         count  //
) {
    for (range(_, 0, count, 1)) {
        card_t card = CARD_NULL;

        if (deck_deal(deck, &card))
            hand_add_card(&player->hand, card);
        else
            ohcrap("cannot deal from an empty deck");
    };
}

bool player_add_book_did_win(player_t *const player, rank_t rank) {
    rank_t *books = player->books;
    for (range(idx, 0, 7, 1))
        if (books[idx] == RANK_NULL) {
            books[idx] = rank;
            bool did_win = idx == 6;
            if (did_win) {  // true when this last the last indexs
                player_print_books(player);
                printf(
                    "\n\nHear ye! Hear ye! %s has won!\n\n", player->name);
                return true;
            }
            return did_win;
        }
    return false;
}