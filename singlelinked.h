// Copyright 2022 Jonah "Jay" Yolles-Murphy (@TG-Techie)
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
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

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief A generic "interface" nodes of other types should be cast to
 * before use with this library
 *
 */
typedef struct _sl_node {
    struct _sl_node* next;
} * sl_node_t;

/**
 * @brief A generic "interface" for list heads other types should be
 * cast to before use
 *
 */
typedef struct _sl_list {
    sl_node_t first;
    size_t    length;
} * sl_list_t;

/**
 * @brief A utility type to cast from a external node pointer type to
 * sl_node_t
 *
 */
union sl_other_node {
    void*     other;
    sl_node_t to_sl_node;
};

/**
 * @brief A utility type to cast from a external head pointer type to
 * sl_node_t
 *
 */
union sl_other_list {
    void*     other;
    sl_list_t to_sl_list;
};

/**
 * @brief find and remove a node from the list that fulfills the given
 * match function
 *
 * @param list the list to search in
 * @param matches a funtion pointer that takes a node to check against
 * and the provided 'look_for' pointer
 * @param look_for the second argument passed to the 'matches'
 * callback
 * @return the removed node or null
 */
sl_node_t sl_remove_if(
    sl_list_t list,
    bool (*matches)(sl_node_t, void*),
    void* look_for);
