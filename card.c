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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "card.h"

rank_t rank_from_str(char* const str) {
    if (strcmp("10", str) == 0) return RANK_10;
    if (strlen(str) != 1) return RANK_NULL;

    char c = str[0];

    // return a numbered rank
    if (('2' <= c) && (c <= '9')) {
        return c - '2' + RANK_2;
    }

    // otherwise it must be a king, queen, jack, or ace
    // if not, it's not a rank...
    switch (toupper(c)) {
        case 'J':
            return RANK_JACK;
        case 'Q':
            return RANK_QUEEN;
        case 'K':
            return RANK_KING;
        case 'A':
            return RANK_ACE;
        default:
            return RANK_NULL;
    }
}

static const char* _ranks_as_strs[] =
    {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};

const char* rank_as_str(rank_t r) { return _ranks_as_strs[r - RANK_2]; }

void card_sfmt(card_t c, card_pretty_str_t* str) {
    char* suit = NULL;
    switch (c.suit) {
        case SUIT_HEARTS:
            suit = "♥";
            break;
        case SUIT_CLUBS:
            suit = "♣";
            break;
        case SUIT_DIAMONDS:
            suit = "♦";
            break;
        case SUIT_SPADES:
            suit = "♠";
            break;
        default:
            suit = "?";
            break;
    }

    sprintf(str->str, "[%-2s%s]", rank_as_str(c.rank), suit);
}

static inline hand_node_t new_hand_node(card_t card) {
    hand_node_t node = malloc(sizeof(struct hand_node));
    *node = (struct hand_node){.card = card, .next = NULL};
    return node;
}

void hand_add_card(hand_t* hand, card_t card) {
    hand_node_t new_node = new_hand_node(card);  // nullable
    new_node->next = NULL;

    // TODO make the hand_add_card insert in rank order

    if (hand->head == NULL) {
        if (hand->length != 0)
            ohcrap("mis-matched hand size (>0 with NULL)");
        // else {
        //     printf("(*0: %s) ", x.str);
        // }
        hand->head = new_node;
        hand->length++;
    } else {
        // iterate over the nodes and find the last one
        hand_node_t last = hand->head;
        int         card_count = 1;
        while (last->next != NULL) {
            last = last->next;
            card_count++;
        }

        // Add the node
        last->next = new_node;
        card_count++;
        hand->length++;

        // sanity check
        if (card_count != hand->length) {
            char* errstr;
            asprintf(
                &errstr,
                "mis-matched hand size (count mismatch %i != %i)",
                card_count,
                (int)hand->length);
            ohcrap(errstr);
        }
    }
}

void hand_search_remove_cards(
    hand_t* const hand,
    rank_t        rank,
    card_t* const dest,
    int* const    count  //
) {
    int pos = 0;

    hand_node_t node = hand->head;
    // while the head contains the card, pop and write it out
    while (hand->head != NULL && hand->head->card.rank == rank) {
        node = hand->head;
        dest[pos++] = node->card;

        hand->head = node->next;
        free(node);
        hand->length--;
    }

    // iterate over the rest of the nodes
    while (node != NULL && node->next != NULL) {
        if (node->next->card.rank != rank) {
            // proceed forward
            node = node->next;
        } else {
            // pop the node and emit the card
            hand_node_t popped = node->next;
            node->next = popped->next;
            // write out the card
            dest[pos++] = popped->card;
            // release the hand node
            free(popped);
            hand->length--;
        }
    }

    *count += pos;
}

int hand_has_rank(const hand_t* const hand, rank_t rank) {
    hand_node_t node = hand->head;
    int         rank_count = 0;
    while (node != NULL) {
        if (node->card.rank == rank) rank_count++;
        node = node->next;
    }
    return rank_count;
}

void cards_asfmt(
    char**              new_string,
    const card_t* const cards,
    size_t              from_idx,
    size_t              upto_idx  //
) {
    char* str = calloc((upto_idx - from_idx) + 3, sizeof(card_pretty_str_t));

    for (range(idx, from_idx, upto_idx, 1)) {
        card_pretty_str_t buf;
        card_sfmt(cards[idx], &buf);
        sprintf(&str[strlen(str)], "%s ", buf.str);
    }

    str[strlen(str)] = '\0';

    *new_string = str;
}