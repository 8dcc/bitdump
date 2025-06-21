/*
 * Copyright 2025 8dcc
 *
 * This file is part of bitdump.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h> /* strerror */
#include <limits.h> /* CHAR_BIT */

/*
 * Maximum number of bytes (not bits) that can be displayed in a single row of
 * the output.
 *
 * TODO: Read from command-line arguments.
 */
#define BYTES_PER_ROW 8

/*
 * Character that should be displayed when a bit is set.
 */
#define CHAR_SET '1'

/*
 * Character that should be displayed when a bit is not set.
 */
#define CHAR_UNSET '0'

/*
 * Character used to separate bytes in the output.
 */
#define CHAR_SEPARATOR ' '

/*----------------------------------------------------------------------------*/

#define ERR(...)                                                               \
    do {                                                                       \
        fprintf(stderr, "bitdump: ");                                          \
        fprintf(stderr, __VA_ARGS__);                                          \
        fputc('\n', stderr);                                                   \
    } while (0)

/*----------------------------------------------------------------------------*/

int main(int argc, char** argv) {
    if (argc > 2) {
        fprintf(stderr, "Usage: %s [FILE]", argv[0]);
        return 1;
    }

    FILE* input = (argc < 2) ? stdin : fopen(argv[1], "rb");
    if (input == NULL) {
        ERR("Failed to open input file: %s", strerror(errno));
        return 1;
    }

    /* Number of printed bytes */
    long num_printed = 0;

    int c;
    while ((c = fgetc(input)) != EOF) {
        const char byte = c;

        /*
         * Print the file offset, if this byte is the first one of its row.
         *
         * NOTE: We are currently displaying the number of printed bytes, which
         * matches the input offset since we read from the first byte.
         * TODO: If we add some '--input-offset' argument, we should display
         * that value (while still changing lines depending on 'num_printed').
         */
        if ((num_printed % BYTES_PER_ROW) == 0)
            printf("%08lX: ", num_printed);

        /*
         * Iterate from the highest bit (7) to the lowest one (0). Check if each
         * bit is set, and print a different character depending on that.
         */
        for (int j = CHAR_BIT - 1; j >= 0; j--)
            putchar(((byte & (1 << j)) != 0) ? CHAR_SET : CHAR_UNSET);
        num_printed++;

        /*
         * If the character is the last one of its row, move to the next
         * one. Otherwise, print the byte separator.
         *
         * TODO: Support command-line byte grouping (e.g. insert the separator
         * every N bytes).
         */
        if ((num_printed % BYTES_PER_ROW) == 0)
            putchar('\n');
        else
            putchar(CHAR_SEPARATOR);
    }

    /*
     * If the last character wasn't the last one of its row, print the
     * terminating newline.
     */
    if ((num_printed % BYTES_PER_ROW) != 0)
        putchar('\n');

    return 0;
}
