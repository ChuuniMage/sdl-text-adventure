#include <SDL.h>
#include <stdio.h>
#include "sftn-char-to-index.cpp"
#include "sftn-top-lvl.h"
#include "sftn-items-inventory.cpp"
#include "sftn-rooms.cpp"

void load_quill(SDL_Surface* quillArray[4]){
    for (int i = 0; i < 4; i++){
        char temp_directory[64] = {0};
        sprintf(temp_directory, "assets/quill/quill_%c.bmp", '1' + i);
        quillArray[i] = SDL_LoadBMP(temp_directory);
    };
};

void load_font_files(SDL_Surface* array[LENGTH_OF_FONT_SURFACE_ARRAY]){
    for(int i = 0; i <= 25; i++){
        char temp_directory[64] = {0};
        sprintf(temp_directory, "assets/font/cap_%c.bmp", 'A' + i);
        array[char_to_index('A' + i)] = SDL_LoadBMP(temp_directory);
    };
    for(int i = 0; i <= 25; i++){
        char temp_directory[64] = {0};
        sprintf(temp_directory, "assets/font/low_%c.bmp", 'a' + i);
        array[char_to_index('a' + i)] = SDL_LoadBMP(temp_directory);
    };
    for(int i = 0; i <= 9; i++){
        char temp_directory[64] = {0};
        sprintf(temp_directory, "assets/font/%c.bmp", '0' + i);
        array[char_to_index('0' + i)] = SDL_LoadBMP(temp_directory);
    };
    array[char_to_index(' ')] = SDL_LoadBMP("assets/font/space.bmp");
    array[char_to_index('+')] = SDL_LoadBMP("assets/font/+.bmp");
    array[char_to_index('?')] = SDL_LoadBMP("assets/font/question_mark.bmp");
    array[char_to_index('!')] = SDL_LoadBMP("assets/font/exclamation_mark.bmp");
    array[char_to_index(',')] = SDL_LoadBMP("assets/font/comma.bmp");
    array[char_to_index('.')] = SDL_LoadBMP("assets/font/period.bmp");
    array[char_to_index('\'')] = SDL_LoadBMP("assets/font/apostrophe.bmp");
};

void load_game_rooms(SDL_Surface* room_backgrounds[NUMBER_OF_ROOMS]) {
    for(int i = 0; i < 2; i++){
        char temp_directory[64] = {0};
        char* room_name = room_index_to_name(i);
        if(room_name[0] == '0'){continue;};
        sprintf(temp_directory, "assets/%s.bmp", room_name);
        room_backgrounds[i] = SDL_LoadBMP(temp_directory);
    };
};

void load_item_in_room_sprites(SDL_Surface* item_in_room_sprites[MAX_NUMBER_OF_ITEMS][NUMBER_OF_ROOMS]){
    for(int i = 0; i < MAX_NUMBER_OF_ITEMS; i++){
        for(int j = 0; j < NUMBER_OF_ROOMS; j++){
            char temp_directory[64] = {0};
            sprintf(temp_directory, "assets/%s_item_%s.bmp", room_index_to_name(j), item_index_to_name(i));
            //Limitation: Only one place for an item in any room.
            //Only address if relevant.
            item_in_room_sprites[i][j] = SDL_LoadBMP(temp_directory);
            if(item_in_room_sprites[i][j] == NULL){
                continue;
            }
            if(!strcmp(item_index_to_name(i), "AXE")){
                item_in_room_sprites[i][j]->clip_rect.x = 242;
                item_in_room_sprites[i][j]->clip_rect.y = 113;
            };
        }
    }
}