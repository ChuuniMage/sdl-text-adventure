#include <stdio.h>
#include <SDL.h>
#include <windows.h>
#include "my_utils.h"
#include <string.h>

#define RedOf(hexRGB888) ((hexRGB888 >> 16) & 255)
#define GreenOf(hexRGB888) ((hexRGB888 >> 8) & 255)
#define BlueOf(hexRGB888) (hexRGB888 & 255)
#define u8RGB_To_u32_RGB888(R,G,B) (R << 16) | (G << 8) | B

#define CRASH_IF_FALSE(Expression, msg) if(Expression == false) {printf(msg);*(int *)0 = 0;}

#define TempDebugOutput(...) char __COUNTER__##debugStringBuffer[256];  \
    wsprintfA(debugStringBuffer, __VA_ARGS__);  \
    printf(debugStringBUffer);                  \
    OutputDebugStringA(debugStringBuffer);      \

#define LENGTH_OF_TOKEN_ARRAY 18
#define LENGTH_OF_CHAR_BUFFER_ARRAY 36
#define LENGTH_OF_FONT_SURFACE_ARRAY 96
#define MAXIMUM_TOKEN_SIZE 16
#define NUMBER_OF_ROOMS 2
#define MAX_NUMBER_OF_ITEMS_IN_ROOM 16
#define MAX_NAME_LENGTH 16
#define MAX_NUMBER_OF_ITEMS 16
#define MAX_ITEMS_IN_INVENTORY 12

const int NATIVE_WIDTH = 320;
const int NATIVE_HEIGHT = 200;
const int FPS = 60;
const int frameDuration = 1000 / FPS;

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

void blit_background(SDL_Surface* background, SDL_Surface* working_surface){
    SDL_BlitSurface(background, NULL, working_surface, NULL);
};

struct TextAnimationGlobals {
    SDL_Surface* working_surface;
    SDL_Surface* font_surface_array[LENGTH_OF_FONT_SURFACE_ARRAY];
};

void blit_text(TextAnimationGlobals* pl, char* string_to_blit, int YPosition, int XPosition){
    int len = strlen(string_to_blit);
    SDL_Rect space_rect = {};
    space_rect.h = pl->font_surface_array[0]->clip_rect.h; space_rect.w = pl->font_surface_array[0]->clip_rect.w;
    space_rect.y = YPosition;
    for (int i = 0; i < len; i++){
        space_rect.x = i * 9 + XPosition;
        SDL_BlitSurface(pl->font_surface_array[char_to_index(string_to_blit[i])], NULL, pl->working_surface, &space_rect);
    };
};

struct Menu_RenderState {
    char* res_option_str[5] = {"1\0", "2\0", "3\0", "4\0", "5\0"};
    int res_option_index = 0; int prev_res_option_index = 0;
    char* option_string[3] = {"  WINDOW SCALE X", "  SAVE", "  QUIT"};
    int selected_option_index = 0;
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

void animate_quill(SDL_Surface* quillArray[4], SDL_Surface* working_surface, int elems_in_charBuffer){
    static int animation_frame = 0;
    static int counter = 0;
    counter++;
    if (counter == 30) {
        animation_frame = animation_frame == 3 ? 0 : animation_frame + 1;
        counter = 0;
    };
    blit_tile(quillArray[animation_frame],1, working_surface, (quillArray[0]->clip_rect.h * -1) + NATIVE_HEIGHT, (quillArray[0]->clip_rect.w + 1) * elems_in_charBuffer);
};


enum ValidCommand {
    Quit_Command,
    Menu_Command,
    Invalid_Command,
    Save_Command,
    Go_command,
    Look_command,
    Take_command,
    Place_command,
    Inventory_command
};

enum GameMode {
    _Default,
    _Menu,
    _Inventory
};

ValidCommand parse_token(char* str) {
    if (!strcmp(str, "QUIT")) return Quit_Command;
    if (!strcmp(str, "MENU")) return Menu_Command;
    if (!strcmp(str, "SAVE")) return Save_Command;
    if (!strcmp(str, "GO")) return Go_command;
    if (!strcmp(str, "LOOK")) return Look_command;
    if (!strcmp(str, "TAKE")) return Take_command;
    if (!strcmp(str, "INVENTORY")) return Inventory_command;
    if (!strcmp(str, "PLACE")) return Place_command;
    if (!strcmp(str, "PUT")) return Place_command;
    return Invalid_Command;
};

void handle_default_mode_event(SDL_Event* event, TextBufferData* t, GameMode* game_mode){
    if (event->type == SDL_TEXTINPUT){
        if (t->elems_in_charBuffer == LENGTH_OF_CHAR_BUFFER_ARRAY - 1){return;};
        char char_to_put = '\0'; char char_to_parse = event->text.text[0];
        if(char_to_index(char_to_parse) >= 0){
            char_to_put = char_to_parse;
        } else {return;};
        t->charBuffer[t->elems_in_charBuffer] = char_to_put;
        t->elems_in_charBuffer++;
        printf("Chars in buffer:[%s] Length %i\n", t->charBuffer, t->elems_in_charBuffer);
    };

    if(event->type != SDL_KEYDOWN){return;};

    if(event->key.keysym.sym == SDLK_BACKSPACE){
        if (t->elems_in_charBuffer == 0){
            return;
        };
        t->charBuffer[t->elems_in_charBuffer - 1] = '\0';
        t->elems_in_charBuffer--;
        printf("Chars in buffer:[%s] Length %i\n", t->charBuffer, t->elems_in_charBuffer);
    };
    if(event->key.keysym.sym == SDLK_RETURN){
        //Submit the buffer for command parsing
        tokenize(t);
        for(int i = 0; i < LENGTH_OF_TOKEN_ARRAY; i++){                
            if(t->tokenBuffer[i][0] == '\0'){ continue;}
            for(int j = 0; j < 18; j++){
                if (islower(t->tokenBuffer[i][j])){
                    t->tokenBuffer[i][j] = toupper(t->tokenBuffer[i][j]);
                }; 
            };
            printf("tokenbuffer %i -> %s, ", i, t->tokenBuffer[i]);
        };
        printf("\n");
        memset(t->charBuffer, 0, LENGTH_OF_CHAR_BUFFER_ARRAY); t->elems_in_charBuffer = 0;
        return;
    };

};

struct Inventory_RenderState{
    char* quit_string = "  EXIT INVENTORY";
    int left_column_item_indices[MAX_ITEMS_IN_INVENTORY / 2];
    int left_column_item_count = 0;
    int right_column_item_indices[MAX_ITEMS_IN_INVENTORY / 2];
    int right_column_item_count = 0;
    bool left_column_selected = true;
    int row_selected = 0;
};


enum RoomEnumIndex {
    _r_None = -1,
    _r_CHURCH ,
    _r_CLEARING
};
//Should probably just have a big ol' hash map of items for the inventory system

enum ItemEnumIndex {
    _i_None = -1,
    _i_Shrine,
    _i_Axe,
    _i_Stump
};

struct ItemRoomLocation {
    RoomEnumIndex room;
    int item_slot;
};

struct ItemData {
    int current_number_of_items = 0;
    char item_names[MAX_NUMBER_OF_ITEMS][MAX_NAME_LENGTH] = {0};
    char item_description[MAX_NUMBER_OF_ITEMS][5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {0};
    int description_line_count[MAX_NUMBER_OF_ITEMS] = {0};
    bool is_takeable_item[MAX_NUMBER_OF_ITEMS] = {0};
    bool is_item_taken[MAX_NUMBER_OF_ITEMS] = {0};
    ItemRoomLocation sprite_within_room_index[MAX_NUMBER_OF_ITEMS]; // if in room
    bool has_sprite[MAX_NUMBER_OF_ITEMS];
    int number_of_items_in_room[NUMBER_OF_ROOMS];
    int index_within_inventory[MAX_NUMBER_OF_ITEMS];
};

struct PlaceEvent {
    ItemEnumIndex destination;
    ItemEnumIndex item_to_place;
    void (*event) (ItemData*);
};

struct EventData {
    //These will very likely need to be made multi-dimensional.
    void (*take_events[MAX_NUMBER_OF_ITEMS])(ItemData*, int, int);
    PlaceEvent place_events[MAX_NUMBER_OF_ITEMS]; 
};

struct InventoryItem {
    ItemEnumIndex item_enum;
    int next_item_index; // -1 = tail
};

struct PlayerInventory {
    InventoryItem inv_item[MAX_ITEMS_IN_INVENTORY];
    bool occupied[MAX_ITEMS_IN_INVENTORY];
    int tail_index;
    int origin_index;
    int number_of_items_held;
};

int inventory_add_item(PlayerInventory* inv, ItemEnumIndex new_item) {
    InventoryItem new_inv_item = {};
    new_inv_item.item_enum = new_item;
    new_inv_item.next_item_index = -1;

    if(inv->tail_index == -1){
        inv->inv_item[0] = new_inv_item;
        inv->occupied[0] = true;
        inv->tail_index = 0;
        inv->origin_index = 0;
        inv->number_of_items_held++;
        return 0;
    };
    for(int i = 0; i < MAX_ITEMS_IN_INVENTORY; i++){
        if(inv->occupied[i] == TRUE){continue;};
        inv->inv_item[inv->tail_index].next_item_index = i;
        inv->tail_index = i;
        inv->occupied[i] = true;
        inv->inv_item[i] = new_inv_item;
        inv->number_of_items_held++;
        return i;
    };
    //All slots occupado
    return -1;
};

int inventory_delete_item(PlayerInventory* inv, int deletion_index) {
    if(deletion_index == inv->origin_index){
        int new_origin_index = inv->inv_item[inv->origin_index].next_item_index;
        ZeroOut(inv->inv_item[inv->origin_index]);
        inv->occupied[inv->origin_index] = false;
        inv->number_of_items_held--;
        return 0;
    };
    for(int i = 0; i < MAX_ITEMS_IN_INVENTORY; i++){
        if(inv->inv_item[i].next_item_index != deletion_index){continue;};
        inv->inv_item->next_item_index = inv->inv_item[deletion_index].next_item_index;
        inv->tail_index = i;
        ZeroOut(inv->inv_item[deletion_index]);
        inv->occupied[deletion_index] = false;
        inv->number_of_items_held--;
        return 0;
    };
    //All slots occupado
    return -1;
};

int inventory_get_nth_item(PlayerInventory* inv, int position_in_list){

    InventoryItem current_item = inv->inv_item[inv->origin_index];
    for(int i = 0; i < inv->number_of_items_held; i++){
        if(i == position_in_list){
            return current_item.item_enum;
        };
        current_item = inv->inv_item[current_item.next_item_index];
    };
    return -1;
};

enum ItemStatus {
    _i_InRoom,
    _i_InInventory
};

int room_name_to_index(char* maybe_name){
    if (!strcmp(maybe_name, "CHURCH")) return _r_CHURCH;
    if (!strcmp(maybe_name, "CLEARING")) return _r_CLEARING;
    return -1;
};

void load_game_rooms(
    char* room_names[2], SDL_Surface* room_backgrounds[NUMBER_OF_ROOMS], 
    ItemData room_item_data, SDL_Surface* room_item_surfaces[NUMBER_OF_ROOMS][16]) {
    for(int i = 0; i < 2; i++){
        char temp_directory[64] = {0};
        sprintf(temp_directory, "assets/%s.bmp", room_names[i]);
        int index = room_name_to_index(room_names[i]);
        CRASH_IF_FALSE(index >= 0, "Invalid room name");
        room_backgrounds[index] = SDL_LoadBMP(temp_directory);
        for(int j = 0; j < MAX_NUMBER_OF_ITEMS; j++){
            if (room_item_data.has_sprite[j] == false ){
                continue;
            };
            RoomEnumIndex room_index = room_item_data.sprite_within_room_index[j].room;
            int location_in_room = room_item_data.sprite_within_room_index[j].item_slot;
            char temp_directory[64] = {0};
            sprintf(temp_directory, "assets/%s_item_%s.bmp", room_names[room_index], room_item_data.item_names[j]);
            room_item_surfaces[room_index][location_in_room] = SDL_LoadBMP(temp_directory);
            if(!strcmp(room_item_data.item_names[j], "AXE")){
                room_item_surfaces[room_index][location_in_room]->clip_rect.x = 242;
                room_item_surfaces[room_index][location_in_room]->clip_rect.y = 113;
            };
        };
        
    };
};

void blit_items_in_room(
    int room_index, 
    SDL_Surface* room_item_surfaces[NUMBER_OF_ROOMS][MAX_NUMBER_OF_ITEMS_IN_ROOM], 
    ItemData* room_item_data, 
    SDL_Surface* working_surface){
    for(int i = 0; i < MAX_NUMBER_OF_ITEMS; i++){
        if(room_item_data->is_item_taken[i]){continue;}
        int item_room_index = room_item_data->sprite_within_room_index[i].room;
        if(item_room_index != room_index){continue;}
        int sprite_loc_index = room_item_data->sprite_within_room_index[i].item_slot;
        SDL_BlitSurface(room_item_surfaces[room_index][sprite_loc_index], NULL, working_surface, &room_item_surfaces[room_index][sprite_loc_index]->clip_rect);
    };
};


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

struct Text_RenderState {
    int (*anim[32]) (int, TextAnimationGlobals*, void*) = {0};
    int duration_counts[32] = {0};
    bool occupied[32] = {0};
    void* temp_data[32] = {0};
};

void DoTextAnimations(Text_RenderState* arr, TextAnimationGlobals* tap){
    for(int i = 0; i < 32; i ++){
        if(arr->occupied[i] == false){continue;};
        int new_count = arr->anim[i](arr->duration_counts[i], tap, arr->temp_data[i]);
        if(new_count == -1){
            ZeroOut(arr->anim[i]);
            ZeroOut(arr->duration_counts[i]);
            arr->occupied[i] = false;
            if(arr->temp_data[i] != NULL){
                free(arr->temp_data[i]);
                arr->temp_data[i] = {0};
            };
        };
        arr->duration_counts[i] = new_count;
    };
};

void AddTextAnimation(Text_RenderState*arr, int (*anim) (int, TextAnimationGlobals*, void*), int duration, void* temp_data){
    for(int i = 0; i < 32; i++){
        if(arr->occupied[i] == TRUE){continue;};
        arr->anim[i] = anim;
        arr->occupied[i] = true;
        arr->duration_counts[i] = duration;
        arr->temp_data[i] = temp_data;
        return;
    };
};

int success_item_look_animation(int duration, TextAnimationGlobals* pl, void* temp_data){
    int line_count = *(int*)temp_data;
    char (*string_data) = (char*)temp_data;
    string_data = string_data + sizeof(int);
    
    char* temp_string_ptr = string_data;
    for(int i = 0; i < line_count; i++){
        char line_to_blit[LENGTH_OF_CHAR_BUFFER_ARRAY] = {0};
        int YPosition = (NATIVE_HEIGHT) - (pl->font_surface_array[char_to_index(' ')]->clip_rect.h * (line_count - i + 1 ));
        strcpy_s(line_to_blit, temp_string_ptr);
        blit_tile(pl->font_surface_array[char_to_index(' ')],LENGTH_OF_CHAR_BUFFER_ARRAY + 4, pl->working_surface, YPosition, 0);
        blit_text(pl, line_to_blit, YPosition, 0);
        temp_string_ptr = temp_string_ptr + LENGTH_OF_CHAR_BUFFER_ARRAY;
    };
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int save_animation(int duration, TextAnimationGlobals* payload, void* _){
    blit_text(payload, "+GAME SAVED+", 0, (payload->font_surface_array[0]->clip_rect.w * 13) );
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int no_target_place_animation(int duration, TextAnimationGlobals* payload, void* _){
    blit_tile(payload->font_surface_array[char_to_index(' ')],34,payload->working_surface, (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(payload, "What would you like to place?", (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int no_destination_place_animation(int duration, TextAnimationGlobals* payload, void* _){
    blit_tile(payload->font_surface_array[char_to_index(' ')],36,payload->working_surface, (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(payload, "Where would you like to place it?", (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int no_target_get_animation(int duration, TextAnimationGlobals* payload, void* _){
    blit_tile(payload->font_surface_array[char_to_index(' ')],34,payload->working_surface, (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(payload, "What would you like to take?", (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int no_target_look_animation(int duration, TextAnimationGlobals* payload, void* _){
    blit_tile(payload->font_surface_array[char_to_index(' ')],34,payload->working_surface, (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(payload, "What would you like to look at?", (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int incompatible_place_animation(int duration, TextAnimationGlobals* payload, void* _){
    blit_tile(payload->font_surface_array[char_to_index(' ')],34,payload->working_surface, (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(payload, "You can't place that placed there.", (NATIVE_HEIGHT) - payload->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int failed_item_take_animation(int duration, TextAnimationGlobals* pl, void* temp_string){
    char error_string[36] = "";
    strcat(strcat(strcat(error_string, "Cannot take the'"), (char*)temp_string), "'.");
    blit_tile(pl->font_surface_array[char_to_index(' ')],34,pl->working_surface, (NATIVE_HEIGHT) - pl->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(pl, error_string, (NATIVE_HEIGHT) - pl->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int failed_item_find_animation(int duration, TextAnimationGlobals* pl, void* temp_string){
    char error_string[36] = "";
    strcat(strcat(strcat(error_string, "Cannot find the'"), (char*)temp_string), "'.");
    blit_tile(pl->font_surface_array[char_to_index(' ')],34,pl->working_surface, (NATIVE_HEIGHT) - pl->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(pl, error_string, (NATIVE_HEIGHT) - pl->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int failed_place_item_find_animation(int duration, TextAnimationGlobals* pl, void* temp_string){
    char error_string[36] = "";
    strcat(strcat(strcat(error_string, "You have no '"), (char*)temp_string), "'.");
    blit_tile(pl->font_surface_array[char_to_index(' ')],34,pl->working_surface, (NATIVE_HEIGHT) - pl->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(pl, error_string, (NATIVE_HEIGHT) - pl->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int success_item_take_animation(int duration, TextAnimationGlobals* pl, void* temp_string){
    char error_string[36] = "";
    strcat(strcat(strcat(error_string, "You take the '"), (char*)temp_string), "'.");
    blit_tile(pl->font_surface_array[char_to_index(' ')],34,pl->working_surface, (NATIVE_HEIGHT) - pl->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(pl, error_string, (NATIVE_HEIGHT) - pl->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

int success_place_take_animation(int duration, TextAnimationGlobals* pl, void* temp_string){
    char error_string[36] = "";
    strcat(strcat(strcat(error_string, "The '"), (char*)temp_string), "' is placed.");
    blit_tile(pl->font_surface_array[char_to_index(' ')],34,pl->working_surface, (NATIVE_HEIGHT) - pl->font_surface_array[0]->clip_rect.h* 2, 0);
    blit_text(pl, error_string, (NATIVE_HEIGHT) - pl->font_surface_array[0]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
};

void replace_all_palettes(SDL_Surface* fontSurfaceArray[LENGTH_OF_FONT_SURFACE_ARRAY], SDL_Surface* quillArray[4], uint32_t* room_palettes[2], int prev_index, int new_index){
    for(int i = 0; i < LENGTH_OF_FONT_SURFACE_ARRAY; i++){
        SDL_Surface* char_sprite = fontSurfaceArray[i];
        if(char_sprite == NULL){continue;};
        replace_palette(char_sprite, room_palettes[prev_index], room_palettes[new_index]);
    };
    for(int i = 0; i < 4; i++){
        replace_palette(quillArray[i], room_palettes[prev_index], room_palettes[new_index]);
    };
}

int item_name_to_index(char tested_string[16]){
    if(!strcmp(tested_string,"SHRINE")){return _i_Shrine;};
    if(!strcmp(tested_string,"AXE")){return _i_Axe;};
    if(!strcmp(tested_string,"STUMP")){return _i_Stump;};
    return -1;
};

void add_item_to_RoomItemData(ItemData* room_items, 
    char name[MAX_NAME_LENGTH], 
    char description[5][LENGTH_OF_CHAR_BUFFER_ARRAY], 
    int description_line_count, 
    bool is_takeable,
    RoomEnumIndex room_enum, bool has_sprite
    ){
    CRASH_IF_FALSE(room_items->current_number_of_items < MAX_NUMBER_OF_ITEMS, "Added too many items.");
    CRASH_IF_FALSE(item_name_to_index(name) != -1, "Invalid item name.");

    int i = item_name_to_index(name);
    strcpy_s(room_items->item_names[i], name);
    room_items->description_line_count[i] = description_line_count;
    memcpy(room_items->item_description[i],description, sizeof(room_items->item_description[i]));
    room_items->is_takeable_item[i] = is_takeable;

    int index_in_room = room_items->number_of_items_in_room[room_enum];
    room_items->sprite_within_room_index[i] = {room_enum, index_in_room};
    room_items->number_of_items_in_room[room_enum]++;
    room_items->has_sprite[i] = has_sprite;
    room_items->index_within_inventory[i] = -1;
    room_items->current_number_of_items++;
};

void alter_item_description(ItemData* room_items, int item_index, char new_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY], int new_line_count){
    memcpy(room_items->item_description[item_index],new_description, sizeof(room_items->item_description[item_index]));
    room_items->description_line_count[item_index] = new_line_count;
};

void take_axe_event(ItemData* room_items, int axe_index, int _){
    char axe_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "It's a well balanced axe. Much use.", 0, 0, 0,0
    };
    alter_item_description(room_items, axe_index, axe_description, 1);

    char stump_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "Stump left behind from an old tree.", "Memories of childhood return.", 0, 0, 0
    };
    int stump_index = 2;
    alter_item_description(room_items, 2, stump_description, 2);
};

void place_axe_in_stump_event(ItemData* room_items){
    char axe_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "The axe is stumpwise lodged.", 0, 0, 0, 0
    };
    char stump_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "The stump is axewise stuck.", 0, 0, 0, 0
    }; 
    alter_item_description(room_items, _i_Axe, axe_description, 1);
    alter_item_description(room_items, _i_Stump, stump_description, 1);
};


void handle_inventory_mode_event(SDL_Event* event, GameMode* game_mode, Inventory_RenderState* inv_state){
    if(event->type != SDL_KEYDOWN){return;};
    if(event->key.keysym.sym == SDLK_UP){
        if(inv_state->row_selected == 0){return;}
        inv_state->row_selected--;
    }
    if(event->key.keysym.sym == SDLK_DOWN){
        printf("Row selected before attempting increment -> %i", inv_state->row_selected);
    if(inv_state->row_selected == (MAX_ITEMS_IN_INVENTORY / 2)){return;}
        inv_state->row_selected++;
    if(inv_state->row_selected == (MAX_ITEMS_IN_INVENTORY / 2)){
            inv_state->left_column_selected = true;
        };
    };
    if(event->key.keysym.sym == SDLK_LEFT){
        inv_state->left_column_selected = true;
    };
    if(event->key.keysym.sym == SDLK_RIGHT){
        if(inv_state->row_selected == (MAX_ITEMS_IN_INVENTORY / 2)){
            return;
        };
        inv_state->left_column_selected = false;
    };
    if(event->key.keysym.sym == SDLK_RETURN){
        if(inv_state->row_selected == (MAX_ITEMS_IN_INVENTORY / 2)){
            *game_mode = _Default;
        };
    };
};


void render_inventory(TextAnimationGlobals* pl, ItemData* room_items, PlayerInventory* player_inventory, Inventory_RenderState* inv_state){
    for(int Y = 0; Y < 8; Y++){
        blit_tile(pl->font_surface_array[char_to_index(' ')],36,pl->working_surface, pl->font_surface_array[char_to_index(' ')]->clip_rect.h* Y, 0);
    };

    if (player_inventory->number_of_items_held > 0){
        for (int i = 0; i < (MAX_ITEMS_IN_INVENTORY / 2); i++){
            if(player_inventory->occupied[i] == FALSE){continue;};
            char* item_name = room_items->item_names[player_inventory->inv_item[i].item_enum];
            blit_text(pl, item_name, pl->font_surface_array[0]->clip_rect.h * i, pl->font_surface_array[0]->clip_rect.w * 3);
        };
        for (int i = 0; i < (MAX_ITEMS_IN_INVENTORY / 2); i++){
            if(player_inventory->occupied[i+(MAX_ITEMS_IN_INVENTORY / 2)] == false){continue;};
            char* item_name = room_items->item_names[player_inventory->inv_item[i].item_enum];
            blit_text(pl, item_name, pl->font_surface_array[0]->clip_rect.h * i, pl->font_surface_array[0]->clip_rect.w * 20);
        };
    };

    int selected_item_in_inventory_index = inv_state->row_selected == (MAX_ITEMS_IN_INVENTORY / 2) 
        ? -1 
        : inv_state->left_column_selected 
        ? inv_state->row_selected 
        : inv_state->row_selected + (MAX_ITEMS_IN_INVENTORY / 2);

    int selected_item_enum = inventory_get_nth_item(player_inventory, selected_item_in_inventory_index);

    if(selected_item_enum != -1 && strcmp(room_items->item_names[selected_item_enum] , "") != 0){
        int line_count = room_items->description_line_count[selected_item_enum];

        for(int i = 0; i < line_count; i++){
            char line_to_blit[LENGTH_OF_CHAR_BUFFER_ARRAY] = {0};
            int YPosition = (NATIVE_HEIGHT) - (pl->font_surface_array[char_to_index(' ')]->clip_rect.h * (line_count - i + 4 ));
            strcpy_s(line_to_blit, room_items->item_description[selected_item_enum][i]);
            blit_tile(pl->font_surface_array[char_to_index(' ')],LENGTH_OF_CHAR_BUFFER_ARRAY + 4, pl->working_surface, YPosition, 0);
            blit_text(pl, line_to_blit, YPosition, 0);
        };
    };
    blit_text(pl, inv_state->quit_string, pl->font_surface_array[0]->clip_rect.h * (MAX_ITEMS_IN_INVENTORY / 2), 0);
    int plus_x_pos = pl->font_surface_array[0]->clip_rect.h * inv_state->row_selected ;
    int plus_y_pos = inv_state->left_column_selected 
        ? pl->font_surface_array[0]->clip_rect.w 
        : pl->font_surface_array[0]->clip_rect.w * 17; 
    blit_text(pl, "+", plus_x_pos, plus_y_pos);
};

struct SaveData {// Initialised with new game state
    int res_index = 0;
    int room_index = 0;
    ItemData item_data = {0};
    PlayerInventory inventory_data = {{_i_None, 0}};
};

inline size_t io_read(void* buffer, size_t size, size_t count, FILE* f){return fread(buffer, size, count, f);}
inline size_t io_write(void* buffer, size_t size, size_t count, FILE* f){return fwrite(buffer, size, count, f);}

enum SAVEGAME_IO {
    _s_READ,
    _s_WRITE
};

void handle_savegame_io(SAVEGAME_IO mode, SaveData* save_data){
    size_t (*io_func)(void*, size_t, size_t, FILE*) = mode == _s_READ ? &io_read : &io_write;
    char* fopen_mode = mode == _s_READ ? "rb" : "wb";

    FILE* saveFileHandle = fopen("save.dat", fopen_mode);
    int items_handled = 0;
    items_handled = io_func(&save_data->res_index, sizeof(int), 1, saveFileHandle);
    items_handled = io_func(&save_data->room_index, sizeof(int), 1, saveFileHandle);
   
    //Room item data
    items_handled = io_func(&save_data->item_data.current_number_of_items, sizeof(int), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.description_line_count, sizeof(save_data->item_data.description_line_count), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.has_sprite, sizeof(save_data->item_data.has_sprite), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.index_within_inventory, sizeof(save_data->item_data.index_within_inventory), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.is_item_taken, sizeof(save_data->item_data.is_item_taken), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.is_takeable_item, sizeof(save_data->item_data.is_takeable_item), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.item_description, sizeof(save_data->item_data.item_description), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.item_names, sizeof(save_data->item_data.item_names), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.number_of_items_in_room, sizeof(save_data->item_data.number_of_items_in_room), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.sprite_within_room_index, sizeof(save_data->item_data.sprite_within_room_index), 1, saveFileHandle);

    //Inventory Data
    items_handled = io_func(&save_data->inventory_data.inv_item, sizeof(save_data->inventory_data.inv_item), 1, saveFileHandle);
    items_handled = io_func(&save_data->inventory_data.number_of_items_held, sizeof(save_data->inventory_data.number_of_items_held), 1, saveFileHandle);
    items_handled = io_func(&save_data->inventory_data.occupied, sizeof(save_data->inventory_data.occupied), 1, saveFileHandle);
    items_handled = io_func(&save_data->inventory_data.origin_index, sizeof(save_data->inventory_data.origin_index), 1, saveFileHandle);
    items_handled = io_func(&save_data->inventory_data.tail_index, sizeof(save_data->inventory_data.tail_index), 1, saveFileHandle);

    if(mode == _s_READ){
        printf("loaded axe desc -> %s \n", save_data->item_data.item_description[_i_Axe]);
    }

    fclose(saveFileHandle);
};

void update_save_data(SaveData* save_data, Menu_RenderState* menu_state, ItemData* room_items, PlayerInventory* inventory, int current_room_index){
    save_data->res_index = menu_state->res_option_index;
    save_data->room_index = current_room_index;
    memcpy(&save_data->item_data, &room_items, sizeof(room_items));
    memcpy(&save_data->inventory_data, &inventory, sizeof(inventory));
};


void handle_menu_mode_event(SDL_Event* event, 
    SDL_Window* window, 
    SDL_Surface* render_surface,
    GameMode* game_mode, 
    Menu_RenderState* menu_state, 
    SaveData* save_data,
    ItemData* room_items,
    PlayerInventory* player_inventory,
    int current_room_index,
    Text_RenderState * anim_arr
    ){
    if(event->type != SDL_KEYDOWN){return;};
    if(event->key.keysym.sym == SDLK_UP){
        if (menu_state->selected_option_index == 0){return;}
        menu_state->selected_option_index--;
    };
    if(event->key.keysym.sym == SDLK_DOWN){
        if (menu_state->selected_option_index == 2){return;}
        menu_state->selected_option_index++;
    };

    if(event->key.keysym.sym == SDLK_RETURN){
        if(menu_state->selected_option_index == 2){
            *game_mode = _Default;
            menu_state->res_option_index = menu_state->prev_res_option_index;
        };
        if(menu_state->selected_option_index == 1){
            update_save_data(save_data, menu_state, room_items, player_inventory, current_room_index);
            handle_savegame_io(_s_WRITE, save_data);
            AddTextAnimation(anim_arr, save_animation, 240, NULL);
            *game_mode = _Default;
        };
    };

    if(menu_state->selected_option_index == 0){
        if(event->key.keysym.sym == SDLK_LEFT){
            if (menu_state->res_option_index == 0){return;}
            menu_state->res_option_index--;
        };
        if(event->key.keysym.sym == SDLK_RIGHT){
            if (menu_state->res_option_index == 4){return;}
            menu_state->res_option_index++;
        };
        if(event->key.keysym.sym == SDLK_RETURN){
            if (menu_state->prev_res_option_index == menu_state->res_option_index){return;}
            menu_state->prev_res_option_index = menu_state->res_option_index;
            SDL_SetWindowSize(window, NATIVE_WIDTH * (menu_state->res_option_index + 1), NATIVE_HEIGHT * (menu_state->res_option_index + 1));
            render_surface = SDL_GetWindowSurface(window);
        };
    };
};

void render_menu(TextAnimationGlobals* pl, Menu_RenderState* MenuState ){
    for(int Y = 0; Y < 10; Y++){
        blit_tile(pl->font_surface_array[char_to_index(' ')],24,pl->working_surface, pl->font_surface_array[char_to_index(' ')]->clip_rect.h* Y, 0);
    };
    char resolutionMessage[24] = {0};
    strcat(resolutionMessage, MenuState->option_string[0]);
    strcat(resolutionMessage,  MenuState->res_option_str[MenuState->res_option_index]);
    blit_text(pl, resolutionMessage, pl->font_surface_array[0]->clip_rect.h * 1, 0);
    blit_text(pl, MenuState->option_string[1], pl->font_surface_array[0]->clip_rect.h * 3, 0);
    blit_text(pl, MenuState->option_string[2], pl->font_surface_array[0]->clip_rect.h * 9, 0);
    blit_text(pl, "+",
        pl->font_surface_array[0]->clip_rect.h * (MenuState->selected_option_index == 2 ? 9 :( MenuState->selected_option_index * 2 + 1)), 
        pl->font_surface_array[0]->clip_rect.w);
};

struct GlobalState {
    //TODO: put things here... because urgh
};


int main(int argc, char* args[]){

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 0;
    } 

    //vvv initialize global stuff here vvv


    GameMode game_mode = _Default;
    TextBufferData text_buffer;
    Menu_RenderState menu_state;
    menu_state.res_option_index = 0;
    menu_state.prev_res_option_index = 0;

    int current_room_index = 0;
    uint32_t church_palette[4] = {0xCBF1F5, 0x445975, 0x0E0F21, 0x050314};
    uint32_t clearing_palette[4] = {0xEBE08D, 0x8A7236, 0x3D2D17, 0x1A1006};
    uint32_t* room_palettes[NUMBER_OF_ROOMS] = {church_palette, clearing_palette};
    EventData events = {0};
    //^^^ initialize global stuff here ^^^

    //vvv Initiate game data here vvv
    PlayerInventory player_inventory;
    for (int i = 0; i < MAX_ITEMS_IN_INVENTORY; i++){
        player_inventory.inv_item[i] = {_i_None, -1};
        player_inventory.occupied[i] = false;
    };
    player_inventory.origin_index = -1;
    player_inventory.tail_index = -1;
    player_inventory.number_of_items_held = 0;
    Inventory_RenderState inventory_state;

    ItemData room_items = {0};
    char shrine_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "The shrine houses a burning candle,", "icons of Christ and the Saints.", 0, 0, 0
    };
    add_item_to_RoomItemData(&room_items, "SHRINE",
        shrine_description, 2, false, _r_CHURCH, false);

    char axe_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "The axe is stumpwise lodged.", 0, 0, 0, 0
    };
    add_item_to_RoomItemData(&room_items, "AXE", axe_description, 1, 1, _r_CLEARING, true);
    events.take_events[_i_Axe] = take_axe_event;
    char stump_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "The stump is axewise stuck.", 0, 0, 0, 0
    }; 
    add_item_to_RoomItemData(&room_items, "STUMP", stump_description, 1, false, _r_CLEARING, false);
    PlaceEvent axe_in_stump_event;
    axe_in_stump_event.destination = _i_Stump;
    axe_in_stump_event.item_to_place = _i_Axe;
    axe_in_stump_event.event = place_axe_in_stump_event;
    events.place_events[_i_Stump] = axe_in_stump_event;
    //^^^ Initiate game data here ^^^

    //vvv load assets here vvv
    SDL_Surface* fontSurfaceArray[LENGTH_OF_FONT_SURFACE_ARRAY] = {0};
    SDL_Surface* roomBackgrounds[NUMBER_OF_ROOMS] = {0};
    SDL_Surface* room_item_sprites[NUMBER_OF_ROOMS][MAX_NUMBER_OF_ITEMS_IN_ROOM] = {0};
    SDL_Surface* quillArray[4] = {0};
    char* gameRoomNames[NUMBER_OF_ROOMS] = {"CHURCH", "CLEARING"};
    load_font_files(fontSurfaceArray);
    load_game_rooms(gameRoomNames, roomBackgrounds, room_items, room_item_sprites);
    load_quill(quillArray);
    //^^^ load assets here ^^^
    
    FILE *saveFileHandle = fopen("save.dat", "rb");
    SaveData save_data;
    bool fresh_game = saveFileHandle == NULL;
    if(fresh_game){
        save_data.res_index = 0;
        save_data.room_index = 0;
        save_data.inventory_data = player_inventory;
        save_data.item_data = room_items;
    } else {
        printf("Not fresh game, loading from savedata");
        menu_state.res_option_index = save_data.res_index;
        menu_state.prev_res_option_index = save_data.res_index;
        current_room_index = save_data.room_index;
        room_items = save_data.item_data;
        player_inventory = save_data.inventory_data;
    };

    if(save_data.room_index != 0){
        replace_all_palettes(fontSurfaceArray,quillArray, room_palettes, 0, save_data.room_index);
    };
    //^^^ apply savedata stuff here ^^^

    //vvv setup window, render_surface, and working_surface handles here vvv
    SDL_Window* window = SDL_CreateWindow("Searching for the Name", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        NATIVE_WIDTH * (menu_state.res_option_index + 1), 
        NATIVE_HEIGHT * (menu_state.res_option_index + 1), 
        SDL_WINDOW_SHOWN );
    if( window == NULL ){
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return 0;
    };
    SDL_Surface* render_surface = SDL_GetWindowSurface( window );
    SDL_Surface* working_surface = SDL_CreateRGBSurface(NULL, NATIVE_WIDTH, NATIVE_HEIGHT, 
        render_surface->format->BitsPerPixel, render_surface->format->Rmask, 
        render_surface->format->Gmask, render_surface->format->Bmask, render_surface->format->Amask);
    //^^^ setup window, render_surface, and working_surface handles here ^^^

    //vvv init animation data here
    TextAnimationGlobals text_animation_payload;
    text_animation_payload.working_surface = working_surface;
    memcpy(text_animation_payload.font_surface_array, fontSurfaceArray, sizeof(text_animation_payload.font_surface_array));

    Text_RenderState animation_arrays;
    ///^^^ init animation data here

    bool quit = false;
    while (quit != true){
        SDL_StartTextInput();
        Uint32 frameStart = SDL_GetTicks();
        SDL_Event event;
        while( SDL_PollEvent( &event ) != 0 ){
            if (event.type == SDL_QUIT){quit = true;break;};
            switch(game_mode){
                case _Default: handle_default_mode_event(&event, &text_buffer, &game_mode);break;
                case _Menu: handle_menu_mode_event(&event,window, render_surface, &game_mode, &menu_state, &save_data, &room_items, &player_inventory, current_room_index, &animation_arrays);break;
                case _Inventory: handle_inventory_mode_event(&event, &game_mode, &inventory_state);break;
            };
        };
        
        ValidCommand command = parse_token(text_buffer.tokenBuffer[0]);
        switch(command){
            case Save_Command: {

                update_save_data(&save_data, &menu_state, &room_items, &player_inventory, current_room_index);
                handle_savegame_io(_s_WRITE, &save_data);
                AddTextAnimation(&animation_arrays, save_animation, 240, NULL);
            };break;
            case Menu_Command: { 
                game_mode = _Menu;
            }; break;
            case Quit_Command: {
                SDL_Event e = {};
                e.type = SDL_QUIT;
                SDL_PushEvent(&e);
            }; break;
            case Go_command: {
                int new_index = room_name_to_index(text_buffer.tokenBuffer[1]);
                if (new_index == -1){break;}
                int previous_room_index = current_room_index;
                current_room_index = new_index;
                replace_all_palettes(fontSurfaceArray,quillArray, room_palettes, previous_room_index, new_index);
            }; break;
            case Look_command:{
                if(strcmp(text_buffer.tokenBuffer[1], "") == 0){
                    AddTextAnimation(&animation_arrays, no_target_look_animation, 240, NULL);
                    break;
                };
                if(strcmp(text_buffer.tokenBuffer[1], "INVENTORY") == 0){
                    game_mode = _Inventory;
                    break;
                };
                int item_index = item_name_to_index(text_buffer.tokenBuffer[1]);
                if(item_index == -1 || room_items.sprite_within_room_index[item_index].room != current_room_index){
                    void* temp_string = (char*)malloc(sizeof(text_buffer.tokenBuffer[1]));
                    memcpy(temp_string, text_buffer.tokenBuffer[1], sizeof(text_buffer.tokenBuffer[1]));
                    AddTextAnimation(&animation_arrays, failed_item_find_animation, 240, temp_string);
                    break;
                };
                int description_size = sizeof(room_items.item_description[0]);

                void* temp_description_data;
                temp_description_data = malloc(sizeof(int) + description_size);
                memcpy(temp_description_data, &room_items.description_line_count[item_index], sizeof(int));
                memcpy((char*)temp_description_data + sizeof(int), room_items.item_description[item_index], description_size);
                AddTextAnimation(&animation_arrays, success_item_look_animation, 240, temp_description_data);
            };break;
            case Take_command:{
                if(strcmp(text_buffer.tokenBuffer[1], "") == 0){
                    AddTextAnimation(&animation_arrays, no_target_get_animation, 240, NULL);
                    break;
                };
                int item_index = item_name_to_index(text_buffer.tokenBuffer[1]);
                if(item_index == -1 || room_items.sprite_within_room_index[item_index].room != current_room_index){
                    void* temp_string = (char*)malloc(sizeof(text_buffer.tokenBuffer[1]));
                    memcpy(temp_string, text_buffer.tokenBuffer[1], sizeof(text_buffer.tokenBuffer[1]));
                    AddTextAnimation(&animation_arrays, failed_item_find_animation, 240, temp_string);
                    break;
                };
                room_items.is_item_taken[item_index] = true; // Redundant?
                room_items.sprite_within_room_index[item_index].room = _r_None;

                room_items.number_of_items_in_room[current_room_index]--;

                room_items.index_within_inventory[item_index] = inventory_add_item(&player_inventory, (ItemEnumIndex)item_index); // likely to be taken out
                

                if(events.take_events[item_index]){          
                    events.take_events[item_index](&room_items, item_index, -1);
                };
                void* temp_string = (char*)malloc(sizeof(text_buffer.tokenBuffer[1]));
                memcpy(temp_string, text_buffer.tokenBuffer[1], sizeof(text_buffer.tokenBuffer[1]));
                AddTextAnimation(&animation_arrays, success_item_take_animation, 240, temp_string);
            };break;
              case Place_command:{
                if(strcmp(text_buffer.tokenBuffer[1], "") == 0){
                    AddTextAnimation(&animation_arrays, no_target_place_animation, 240, NULL);
                    break;
                };

                int item_enum = item_name_to_index(text_buffer.tokenBuffer[1]);
                int index_within_inventory = room_items.index_within_inventory[item_enum];
                // printf("For item index %i, trying to place \n", item_index);
                // for (int i = 0; i < 16; i++){ player_has_item_held = player_inventory.item_index[i] == item_index; }
                if(item_enum == -1 || index_within_inventory == -1){
                    void* temp_string = (char*)malloc(sizeof(text_buffer.tokenBuffer[1]));
                    memcpy(temp_string, text_buffer.tokenBuffer[1], sizeof(text_buffer.tokenBuffer[1]));
                    AddTextAnimation(&animation_arrays, failed_place_item_find_animation, 240, temp_string);
                    break;
                };

                if(strcmp(text_buffer.tokenBuffer[2], "") == 0){
                    AddTextAnimation(&animation_arrays, no_destination_place_animation, 240, NULL);
                    break;
                };


                int destination_index = item_name_to_index(text_buffer.tokenBuffer[2]);


                if(destination_index == -1 || room_items.sprite_within_room_index[destination_index].room != current_room_index){
                    void* temp_string = (char*)malloc(sizeof(text_buffer.tokenBuffer[2]));
                    memcpy(temp_string, text_buffer.tokenBuffer[2], sizeof(text_buffer.tokenBuffer[2]));
                    AddTextAnimation(&animation_arrays, failed_item_find_animation, 240, temp_string);
                    break;
                };
                if(events.place_events[destination_index].event == NULL){
                    
                    AddTextAnimation(&animation_arrays, incompatible_place_animation, 240, NULL);
                    break;
                };

                if(events.place_events[destination_index].item_to_place == item_enum){
                    printf("firing place event\n");
                    events.place_events[destination_index].event(&room_items);
                };
                room_items.is_item_taken[item_enum] = false;
                room_items.sprite_within_room_index[item_enum].room = (RoomEnumIndex)current_room_index;
                room_items.number_of_items_in_room[current_room_index]++;
                room_items.index_within_inventory[current_room_index] = -1;
                inventory_delete_item(&player_inventory, index_within_inventory);

                void* temp_string = (char*)malloc(sizeof(text_buffer.tokenBuffer[1]));
                memcpy(temp_string, text_buffer.tokenBuffer[1], sizeof(text_buffer.tokenBuffer[1]));
                AddTextAnimation(&animation_arrays, success_place_take_animation, 240, temp_string);
            };break;
            case Inventory_command:{
                game_mode = _Inventory;
            };break;
            case Invalid_Command:{};break;
        };
        ZeroOut(text_buffer.tokenBuffer);

        blit_background(roomBackgrounds[current_room_index], working_surface);
        blit_items_in_room(current_room_index, room_item_sprites, &room_items, working_surface);

        if (game_mode == _Inventory){
            render_inventory(&text_animation_payload, &room_items, &player_inventory, &inventory_state);
        };

        if (game_mode == _Menu){
            render_menu(&text_animation_payload, &menu_state);
        };

        if (game_mode == _Default){
            if (text_buffer.elems_in_charBuffer > 0){
                blit_tile(fontSurfaceArray[char_to_index(' ')],LENGTH_OF_CHAR_BUFFER_ARRAY + 4, working_surface, (fontSurfaceArray[0]->clip_rect.h * -1) + NATIVE_HEIGHT, 0);
                blit_text(&text_animation_payload, text_buffer.charBuffer, (fontSurfaceArray[0]->clip_rect.h * -1) + NATIVE_HEIGHT, 0);
                if(text_buffer.elems_in_charBuffer != 35){
                    animate_quill(quillArray, working_surface, text_buffer.elems_in_charBuffer);
                };
            };
            DoTextAnimations(&animation_arrays, &text_animation_payload);
        };
        
        SDL_BlitScaled(working_surface, NULL, render_surface, NULL);

        SDL_UpdateWindowSurface(window);
        int frameTime = SDL_GetTicks() - frameStart;
        if (frameDuration > frameTime){
            // printf("newrefac \n");
            SDL_Delay(frameDuration - frameTime);
        };
    };

    //Cleanup
    SDL_Quit();

    return 0;
}