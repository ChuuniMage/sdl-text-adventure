#pragma once

#include <SDL.h>
#include "sftn-top-lvl.h"
#include "sftn-char-to-index.cpp"
#include <string.h>
#include "my_utils.h"
#include "sftn-rooms.cpp"
#include "sftn-items-inventory.cpp"


struct SpriteInRoomPoint {
    int x;
    int y;
};

struct GlobalSurfaces {
    SDL_Surface* working_surface;
    SDL_Surface* font_surface_array[LENGTH_OF_FONT_SURFACE_ARRAY] = {0};
    SDL_Surface* room_backgrounds[NUMBER_OF_ROOMS] = {0};
    SDL_Surface* item_in_room_sprites[MAX_NUMBER_OF_ITEMS][NUMBER_OF_ROOMS] = {0};
    SpriteInRoomPoint item_points_in_room[NUMBER_OF_ROOMS][MAX_NUMBER_OF_ITEMS] = {0,0};
    SDL_Surface* quill_array[4] = {0};
};

struct Inventory_RenderState{
    char* quit_string = "  EXIT INVENTORY";
    bool left_column_selected = true;
    int row_selected = 0;
};

void blit_text(GlobalSurfaces* gs, char* string_to_blit, int YPosition, int XPosition){
    int len = strlen(string_to_blit);
    SDL_Rect space_rect = {};
    space_rect.h = gs->font_surface_array[0]->clip_rect.h; space_rect.w = gs->font_surface_array[0]->clip_rect.w;
    space_rect.y = YPosition;
    for (int i = 0; i < len; i++){
        space_rect.x = i * 9 + XPosition;
        SDL_BlitSurface(gs->font_surface_array[char_to_index(string_to_blit[i])], NULL, gs->working_surface, &space_rect);
    };
};

void blit_tile(SDL_Surface* space, int blit_length, SDL_Surface* working_surface, int YPosition, int XPosition){
    SDL_Rect space_rect = {};
    space_rect.h = space->clip_rect.h; space_rect.w = space->clip_rect.w;
    space_rect.y = YPosition;
    for (int i = 0; i < blit_length; i++){
        space_rect.x = i * 8 + XPosition;
        SDL_BlitSurface(space, NULL, working_surface, &space_rect);
    };
};


int save_animation(int duration, GlobalSurfaces* gs, char _[5][LENGTH_OF_CHAR_BUFFER_ARRAY]){
    blit_text(gs, "+GAME SAVED+", 0, (gs->font_surface_array[0]->clip_rect.w * 13) );
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int success_item_look_animation(int duration, GlobalSurfaces* gs, char temp_data[5][LENGTH_OF_CHAR_BUFFER_ARRAY]){
    int line_count = 5;
    
    for(int i = 0; i < 5; i++){
        if(temp_data[i][0] == 0){
            line_count = i;
            break;
        };
    };

    for(int i = 0; i < line_count; i++){
        int YPosition = (NATIVE_HEIGHT) - (gs->font_surface_array[char_to_index(' ')]->clip_rect.h * (line_count - i + 1 ));
        blit_tile(gs->font_surface_array[char_to_index(' ')],LENGTH_OF_CHAR_BUFFER_ARRAY + 4, gs->working_surface, YPosition, 0);
        blit_text(gs, temp_data[i], YPosition, 0);
    };
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int no_target_place_animation(int duration, GlobalSurfaces* payload, char _[5][LENGTH_OF_CHAR_BUFFER_ARRAY]){
    blit_tile(payload->font_surface_array[char_to_index(' ')],34,payload->working_surface, (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(payload, "What would you like to place?", (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int no_destination_place_animation(int duration, GlobalSurfaces* payload, char _[5][LENGTH_OF_CHAR_BUFFER_ARRAY]){
    blit_tile(payload->font_surface_array[char_to_index(' ')],36,payload->working_surface, (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(payload, "Where would you like to place it?", (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int no_target_get_animation(int duration, GlobalSurfaces* payload, char _[5][LENGTH_OF_CHAR_BUFFER_ARRAY]){
    blit_tile(payload->font_surface_array[char_to_index(' ')],34,payload->working_surface, (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(payload, "What would you like to take?", (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int no_target_look_animation(int duration, GlobalSurfaces* payload, char _[5][LENGTH_OF_CHAR_BUFFER_ARRAY]){
    blit_tile(payload->font_surface_array[char_to_index(' ')],34,payload->working_surface, (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(payload, "What would you like to look at?", (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int incompatible_place_animation(int duration, GlobalSurfaces* payload, char _[5][LENGTH_OF_CHAR_BUFFER_ARRAY]){
    blit_tile(payload->font_surface_array[char_to_index(' ')],34,payload->working_surface, (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(payload, "You can't place that placed there.", (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int failed_item_take_animation(int duration, GlobalSurfaces* gs, void* temp_string){
    char error_string[36] = "";
    strcat(strcat(strcat(error_string, "Cannot take the'"), (char*)temp_string), "'.");
    blit_tile(gs->font_surface_array[char_to_index(' ')],34,gs->working_surface, (NATIVE_HEIGHT) - gs->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(gs, error_string, (NATIVE_HEIGHT) - gs->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int failed_item_find_animation(int duration, GlobalSurfaces* gs, char temp_string[5][LENGTH_OF_CHAR_BUFFER_ARRAY]){
    char error_string[36] = "";
    strcat(strcat(strcat(error_string, "Cannot find the'"), (char*)temp_string[0]), "'.");
    blit_tile(gs->font_surface_array[char_to_index(' ')],34,gs->working_surface, (NATIVE_HEIGHT) - gs->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(gs, error_string, (NATIVE_HEIGHT) - gs->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int failed_place_item_find_animation(int duration, GlobalSurfaces* gs, char temp_string[5][LENGTH_OF_CHAR_BUFFER_ARRAY]){
    char error_string[36] = "";
    strcat(strcat(strcat(error_string, "You have no '"), (char*)temp_string[0]), "'.");
    blit_tile(gs->font_surface_array[char_to_index(' ')],34,gs->working_surface, (NATIVE_HEIGHT) - gs->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(gs, error_string, (NATIVE_HEIGHT) - gs->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int success_item_take_animation(int duration, GlobalSurfaces* gs, char temp_string[5][LENGTH_OF_CHAR_BUFFER_ARRAY]){
    char error_string[36] = "";
    strcat(strcat(strcat(error_string, "You take the '"), (char*)temp_string[0]), "'.");
    blit_tile(gs->font_surface_array[char_to_index(' ')],34,gs->working_surface, (NATIVE_HEIGHT) - gs->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(gs, error_string, (NATIVE_HEIGHT) - gs->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int success_place_take_animation(int duration, GlobalSurfaces* gs, char temp_string[5][LENGTH_OF_CHAR_BUFFER_ARRAY]){
    char error_string[36] = "";
    strcat(strcat(strcat(error_string, "The '"), (char*)temp_string[0]), "' is placed.");
    blit_tile(gs->font_surface_array[char_to_index(' ')],34,gs->working_surface, (NATIVE_HEIGHT) - gs->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(gs, error_string, (NATIVE_HEIGHT) - gs->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

void blit_background(SDL_Surface* background, SDL_Surface* working_surface){
    SDL_BlitSurface(background, NULL, working_surface, NULL);
};

void animate_quill(GlobalSurfaces* gs, int elems_in_charBuffer){
    static int animation_frame = 0;
    static int counter = 0;
    counter++;
    if (counter == 30) {
        animation_frame = animation_frame == 3 ? 0 : animation_frame + 1;
        counter = 0;
    };
    blit_tile(gs->quill_array[animation_frame],1, gs->working_surface, (gs->quill_array[0]->clip_rect.h * -1) + NATIVE_HEIGHT, (gs->quill_array[0]->clip_rect.w + 1) * elems_in_charBuffer);
};

struct Text_RenderState {
    int (*current_txt_anim) (int, GlobalSurfaces*, char[5][LENGTH_OF_CHAR_BUFFER_ARRAY]) = {0};
    int duration_count = 0;
    char temp_data[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {0};
};

void DoTextAnimations(Text_RenderState* arr, GlobalSurfaces* tap){
    if(arr->current_txt_anim == NULL){
        return;
    }
    int new_count = arr->current_txt_anim(arr->duration_count, tap, arr->temp_data);
    if(new_count == -1){
        ZeroOut(arr->current_txt_anim);
        ZeroOut(arr->duration_count);
        ZeroOut(arr->temp_data);
    };
    arr->duration_count = new_count;
};

void AddTextAnimation(Text_RenderState*arr, int (*anim) (int, GlobalSurfaces*, char[5][LENGTH_OF_CHAR_BUFFER_ARRAY]), int duration, char temp_data[5][LENGTH_OF_CHAR_BUFFER_ARRAY]){
    arr->current_txt_anim = anim;
    arr->duration_count = duration;
    if(temp_data == NULL){return;}
    for(int i = 0; i < 5; i ++){
        for(int j = 0; j < LENGTH_OF_CHAR_BUFFER_ARRAY; j++){
            arr->temp_data[i][j] = temp_data[i][j];
        }
    }
};

void render_inventory(GlobalSurfaces* gs, ItemData* room_items, PlayerInventory* player_inventory, Inventory_RenderState* inv_state){
    for(int Y = 0; Y < 8; Y++){
        blit_tile(gs->font_surface_array[char_to_index(' ')],36,gs->working_surface, gs->font_surface_array[char_to_index(' ')]->clip_rect.h* Y, 0);
    };

    if (player_inventory->number_of_items_held > 0){
        for (int i = 0; i < (MAX_ITEMS_IN_INVENTORY / 2); i++){
            if(player_inventory->occupied[i] == FALSE){continue;};
            char* item_name = item_index_to_name(player_inventory->inv_item[i].item_enum);
            blit_text(gs, item_name, gs->font_surface_array[0]->clip_rect.h * i, gs->font_surface_array[0]->clip_rect.w * 3);
        };
        for (int i = 0; i < (MAX_ITEMS_IN_INVENTORY / 2); i++){
            if(player_inventory->occupied[i+(MAX_ITEMS_IN_INVENTORY / 2)] == false){continue;};
            char* item_name = item_index_to_name(player_inventory->inv_item[i + (MAX_ITEMS_IN_INVENTORY / 2)].item_enum);
            blit_text(gs, item_name, gs->font_surface_array[0]->clip_rect.h * i, gs->font_surface_array[0]->clip_rect.w * 20);
        };
    };

    int selected_item_in_inventory_index = inv_state->row_selected == (MAX_ITEMS_IN_INVENTORY / 2) 
        ? -1 
        : inv_state->left_column_selected 
        ? inv_state->row_selected 
        : inv_state->row_selected + (MAX_ITEMS_IN_INVENTORY / 2);

    int selected_item_enum = inventory_get_nth_item(player_inventory, selected_item_in_inventory_index);

    if(selected_item_enum != -1 && strcmp(item_index_to_name(selected_item_enum) , "") != 0){
        int line_count = 5;
        
        for(int i = 0; i < 5; i++){
            if(room_items->item_description[selected_item_enum][i][0] == 0){
                line_count = i;
                break;
            };
        };
        printf("Line count for item display -> %i \n", line_count);
        for(int i = 0; i < line_count; i++){
            char line_to_blit[LENGTH_OF_CHAR_BUFFER_ARRAY] = {0};
            int YPosition = (NATIVE_HEIGHT) - (gs->font_surface_array[char_to_index(' ')]->clip_rect.h * (line_count - i + 4 ));
            strcpy_s(line_to_blit, room_items->item_description[selected_item_enum][i]);
            blit_tile(gs->font_surface_array[char_to_index(' ')],LENGTH_OF_CHAR_BUFFER_ARRAY + 4, gs->working_surface, YPosition, 0);
            blit_text(gs, line_to_blit, YPosition, 0);
        };
    };
    blit_text(gs, inv_state->quit_string, gs->font_surface_array[0]->clip_rect.h * (MAX_ITEMS_IN_INVENTORY / 2), 0);
    int plus_x_pos = gs->font_surface_array[0]->clip_rect.h * inv_state->row_selected ;
    int plus_y_pos = inv_state->left_column_selected 
        ? gs->font_surface_array[0]->clip_rect.w 
        : gs->font_surface_array[0]->clip_rect.w * 17; 
    blit_text(gs, "+", plus_x_pos, plus_y_pos);
};

void blit_items_in_room(
    int room_index, 
    GlobalSurfaces* gs,
    ItemData* room_item_data){
    for(int i = 0; i < MAX_NUMBER_OF_ITEMS; i++){
        if(room_item_data->is_item_taken[i]){continue;}
        SDL_Surface* item_sprite = gs->item_in_room_sprites[i][room_index];
        if(item_sprite == NULL){continue;};
        // int sprite_loc_index = room_item_data->sprite_within_room_index[i].item_slot;
        SDL_BlitSurface(item_sprite, NULL, gs->working_surface, &item_sprite->clip_rect);
    };
};

#define RedOf(hexRGB888) ((hexRGB888 >> 16) & 255)
#define GreenOf(hexRGB888) ((hexRGB888 >> 8) & 255)
#define BlueOf(hexRGB888) (hexRGB888 & 255)
#define u8RGB_To_u32_RGB888(R,G,B) (R << 16) | (G << 8) | B

void replace_palette(SDL_Surface* target, uint32_t current_palette[4], uint32_t new_palette[4]){
    uint32_t* pixel_ptr = (uint32_t*)target->pixels;
    for (int i = 0; i <target->h * target->w; i++, pixel_ptr++){
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        SDL_GetRGB(*pixel_ptr,target->format,&red,&green,&blue);
        uint32_t detected_colour = u8RGB_To_u32_RGB888(red,green,blue);
        for (int j = 0; j < 4; j++){
            uint32_t replace_color = current_palette[j];
            if(detected_colour == replace_color){
                uint32_t new_color = new_palette[j];
                *pixel_ptr = SDL_MapRGB(target->format,RedOf(new_color),GreenOf(new_color),BlueOf(new_color));
            };
        };
    };
};

void replace_all_palettes(GlobalSurfaces* gs, uint32_t* room_palettes[2], int prev_index, int new_index){
    for(int i = 0; i < LENGTH_OF_FONT_SURFACE_ARRAY; i++){
        SDL_Surface* char_sprite = gs->font_surface_array[i];
        if(char_sprite == NULL){continue;};
        replace_palette(char_sprite, room_palettes[prev_index], room_palettes[new_index]);
    };
    for(int i = 0; i < 4; i++){
        replace_palette(gs->quill_array[i], room_palettes[prev_index], room_palettes[new_index]);
    };
};

struct Menu_RenderState {
    char* res_option_str[5] = {"1\0", "2\0", "3\0", "4\0", "5\0"};
    int res_option_index = 0; int prev_res_option_index = 0;
    char* option_string[3] = {"  WINDOW SCALE X", "  SAVE", "  QUIT"};
    int selected_option_index = 0;
};

void render_menu(GlobalSurfaces* gs, Menu_RenderState* menu_state ){
    for(int Y = 0; Y < 10; Y++){
        blit_tile(gs->font_surface_array[char_to_index(' ')],24,gs->working_surface, gs->font_surface_array[char_to_index(' ')]->clip_rect.h* Y, 0);
    };
    char resolutionMessage[24] = {0};
    strcat(resolutionMessage, menu_state->option_string[0]);
    strcat(resolutionMessage,  menu_state->res_option_str[menu_state->res_option_index]);
    blit_text(gs, resolutionMessage, gs->font_surface_array[0]->clip_rect.h * 1, 0);
    blit_text(gs, menu_state->option_string[1], gs->font_surface_array[0]->clip_rect.h * 3, 0);
    blit_text(gs, menu_state->option_string[2], gs->font_surface_array[0]->clip_rect.h * 9, 0);
    blit_text(gs, "+",
        gs->font_surface_array[0]->clip_rect.h * (menu_state->selected_option_index == 2 ? 9 :( menu_state->selected_option_index * 2 + 1)), 
        gs->font_surface_array[0]->clip_rect.w);
};