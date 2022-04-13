#include "sftn-top-lvl.h"
#include <string.h>
#include <windows.h>
#include <stdint.h>

struct TextBufferData {
    uint8_t elems_in_charBuffer = 0;
    char charBuffer[LENGTH_OF_CHAR_BUFFER_ARRAY] = {0};
    char tokenBuffer[LENGTH_OF_TOKEN_ARRAY][MAXIMUM_TOKEN_SIZE] = {0};
};

void tokenize( TextBufferData* t) {
    int word_start_index, new_token_index = 0; 
    int str_len = strlen(t->charBuffer);
    for (int i = 0; i < str_len + 1; i++) {
        char current_char = t->charBuffer[i];
        if (current_char != ' ' && current_char != 0) {continue;}
        if (i == 0 || !t->charBuffer[i - 1]) {
            word_start_index = word_start_index + 1;
            continue;
        }
        char detected_string[36] = {0};
        int new_word_start_index = word_start_index;
        for (int j = 0; j < i - word_start_index; j++) {
            detected_string[j] = t->charBuffer[j + word_start_index];
            new_word_start_index = new_word_start_index + 1;
        }
        strcpy_s(t->tokenBuffer[new_token_index], detected_string);
        new_token_index = new_token_index + 1;
        word_start_index = new_word_start_index + 1;
    };
};