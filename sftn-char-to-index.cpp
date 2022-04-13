#pragma once
#include "sftn-rooms.cpp"
#include "sftn-char-to-index.cpp"

inline int char_to_index(char input){
    if(input >= 'A' && input <= 'Z'){
        return input - 'A';
    };
    if(input >= '0' && input <= '9'){
        return 26 + input - '0';
    };
    if(input >= 'a' && input <= 'z'){
        return 36 + input - 'a';
    };
    switch(input){
        case ' ':return 62;
        case '+': return 63;
        case '?': return 64;
        case '!': return 65;
        case ',': return 66;
        case '.': return 67;
        case '\'': return 68;
        default: return -1;
    };
};
