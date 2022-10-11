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

// terminal escape codes
#define ESC_BLK "\e[30m"
#define ESC_RED "\e[31m"
#define ESC_GRN "\e[32m"
#define ESC_YLW "\e[33m"
#define ESC_BLU "\e[34m"
#define ESC_MAG "\e[35m"
#define ESC_CYN "\e[36m"
#define ESC_WHT "\e[37m"
#define ESC_RST "\e[0m"

#define nullable(ptr_t) ptr_t

#define ever() \
    ;          \
    ;

// usage: for (range(i, start, upto, step)) { ... }
// setup != 0
#define range(name, start, upto, step)          \
    int name = start;                           \
    (step > 0) ? (name < upto) : (name > upto); \
    name += step

void __attribute__((noreturn)) ohcrap(const char* const msg);