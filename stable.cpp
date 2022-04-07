#include <stdio.h>
#include <SDL.h>
#include <windows.h>
#include "my_utils.cpp"
#include <string.h>

#define RedOf(hexRGB888) ((hexRGB888 >> 16) & 255)
#define GreenOf(hexRGB888) ((hexRGB888 >> 8) & 255)
#define BlueOf(hexRGB888) (hexRGB888 & 255)
#define u8RGB_To_u32_RGB888(R,G,B) (R << 16) | (G << 8) | B

#define CRASH_IF_FALSE(Expression, msg) if(Expression == false) {printf(msg);*(int *)0 = 0;}

#define LENGTH_OF_TOKEN_ARRAY 18
#define LENGTH_OF_CHAR_BUFFER_ARRAY 36
#define LENGTH_OF_FONT_SURFACE_ARRAY 96
#define MAXIMUM_TOKEN_SIZE 16
#define NUMBER_OF_ROOMS 2

const int NATIVE_WIDTH = 320;
const int NATIVE_HEIGHT = 200;
const int FPS = 60;
const int frameDuration = 1000 / FPS;

inline int char_to_index(char input){
    switch(input){
        case 'A': return 0;
        case 'B': return 1;
        case 'C': return 2;
        case 'D': return 3;
        case 'E': return 4;
        case 'F': return 5;
        case 'G': return 6;
        case 'H': return 7;
        case 'I': return 8;
        case 'J': return 9;
        case 'K': return 10;
        case 'L': return 11;
        case 'M': return 12;
        case 'N': return 13;
        case 'O': return 14;
        case 'P': return 15;
        case 'Q': return 16;
        case 'R': return 17;
        case 'S': return 18;
        case 'T': return 19;
        case 'U': return 20;
        case 'V': return 21;
        case 'W': return 22;
        case 'X': return 23;
        case 'Y': return 24;
        case 'Z': return 25;
        case ' ': return 26;
        case '0': return 27;
        case '1': return 28;
        case '2': return 29;
        case '3': return 30;
        case '4': return 31;
        case '5': return 32;
        case '6': return 33;
        case '7': return 34;
        case '8': return 35;
        case '9': return 36;
        case '+': return 37;
        case '?': return 38;
        case '!': return 39;
        case ',': return 40;
        case '.': return 41;
        case '\'': return 42;
        case 'a': return 43;
        case 'b': return 44;
        case 'c': return 45;
        case 'd': return 46;
        case 'e': return 47;
        case 'f': return 48;
        case 'g': return 49;
        case 'h': return 50;
        case 'i': return 51;
        case 'j': return 52;
        case 'k': return 53;
        case 'l': return 54;
        case 'm': return 55;
        case 'n': return 56;
        case 'o': return 57;
        case 'p': return 58;
        case 'q': return 59;
        case 'r': return 60;
        case 's': return 61;
        case 't': return 62;
        case 'u': return 63;
        case 'v': return 64;
        case 'w': return 65;
        case 'x': return 66;
        case 'y': return 67;
        case 'z': return 68;
        default: return -1;
    };
};

struct TextBufferData {
    char charBuffer[LENGTH_OF_CHAR_BUFFER_ARRAY] = {0};
    char tokenBuffer[LENGTH_OF_TOKEN_ARRAY][MAXIMUM_TOKEN_SIZE] = {0};
    uint8_t elems_in_charBuffer = 0;
};

void tokenize( TextBufferData* t) {
    int word_start_index, new_token_index = 0; 
    int str_len = strlen(t->charBuffer);
    for (int i = 0; i < str_len + 1; i++) {
        char current_char = t->charBuffer[i];
        if (current_char == ' ' || current_char == 0) {
            if (i == 0 || !t->charBuffer[i - 1]) {
                word_start_index = word_start_index + 1;
                continue;
            }
            char detected_string[36] = { 0 };
            int new_word_start_index = word_start_index;
            for (int j = 0; j < i - word_start_index; j++) {
                detected_string[j] = t->charBuffer[j + word_start_index];
                new_word_start_index = new_word_start_index + 1;
            }
            memcpy(t->tokenBuffer[new_token_index], detected_string, sizeof(t->tokenBuffer[new_token_index]));
            new_token_index = new_token_index + 1;
            word_start_index = new_word_start_index + 1;
        };
    };
};

void load_quill(SDL_Surface* quillArray[4]){
    char current_char[2] = "1";
    for (int i = 0; i < 4; i++, current_char[0]++){
        char temp_directory[64] = {0};
        strcat(strcat(strcat(temp_directory, "assets/quill/quill_"), current_char), ".bmp");
        quillArray[i] = SDL_LoadBMP(temp_directory);
    };
};

void load_font_files(SDL_Surface* array[LENGTH_OF_FONT_SURFACE_ARRAY]){
    char current_char[2] = "A";
    
    for(int i = 0; i <= 25; i++, current_char[0]++){
        char temp_directory[64] = {0};
        strcat(strcat(strcat(temp_directory, "assets/font/cap_"), current_char), ".bmp");
        array[char_to_index(current_char[0])] = SDL_LoadBMP(temp_directory);
    };
    current_char[0] = 'a';
    for(int i = 0; i <= 25; i++, current_char[0]++){
        char temp_directory[64] = {0};
        strcat(strcat(strcat(temp_directory, "assets/font/low_"), current_char), ".bmp");
        array[char_to_index(current_char[0])] = SDL_LoadBMP(temp_directory);
    };
    current_char[0] = '0';
    for(int i = 0; i <= 9; i++, current_char[0]++){
        char temp_directory[64] = {0};
        strcat(strcat(strcat(temp_directory, "assets/font/"), current_char), ".bmp");
        array[char_to_index(current_char[0])] = SDL_LoadBMP(temp_directory);
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
    char* temp_string; // must alloc this
    char temp_multiline[5][35];
    int temp_int;
};


void blit_text(TextAnimationGlobals* pl, char* string_to_blit, int YPosition, int XPosition){
    int len = strlen(string_to_blit);
    int* indices = (int*) malloc(sizeof(int)*len);
    for (int i = 0; i < len ; i++){
        indices[i] = char_to_index(string_to_blit[i]);
    };
    SDL_Rect space_rect = {};
    space_rect.h = pl->font_surface_array[0]->clip_rect.h; space_rect.w = pl->font_surface_array[0]->clip_rect.w;
    space_rect.y = YPosition;
    for (int i = 0; i < len; i++){
        space_rect.x = i * 9 + XPosition;
        SDL_BlitSurface(pl->font_surface_array[indices[i]], NULL, pl->working_surface, &space_rect);
    };
};

struct menu_state {
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
    Look_command
};

enum GameMode {
    _Default,
    _Menu
};

ValidCommand parse_token(char* str) {
    if (!strcmp(str, "QUIT")) return Quit_Command;
    if (!strcmp(str, "MENU")) return Menu_Command;
    if (!strcmp(str, "SAVE")) return Save_Command;
    if (!strcmp(str, "GO")) return Go_command;
    if (!strcmp(str, "LOOK")) return Look_command;
    return Invalid_Command;
};

void handle_default_mode_event(SDL_Event* event, TextBufferData* t, GameMode* game_mode){
    if(event->type == SDL_KEYDOWN){
        if(event->key.keysym.sym == SDLK_BACKSPACE){
            if (t->elems_in_charBuffer == 0){
                return;
            };
            t->charBuffer[t->elems_in_charBuffer - 1] = '\0';
            t->elems_in_charBuffer--;
            printf("Chars in buffer:[%s] Length %i\n", t->charBuffer, t->elems_in_charBuffer);
        };
        if(event->key.keysym.sym == SDLK_RETURN){
            tokenize(t);
            for(int i = 0; i < LENGTH_OF_TOKEN_ARRAY; i++){                
                if(t->tokenBuffer[i][0] != '\0'){
                    for(int j = 0; j < 18; j++){
                        if (islower(t->tokenBuffer[i][j])){
                            t->tokenBuffer[i][j] = toupper(t->tokenBuffer[i][j]);
                        }; 
                    };
                    printf("tokenbuffer %i -> %s, ", i, t->tokenBuffer[i]);
                };
            };
            printf("\n");
            memset(t->charBuffer, 0, LENGTH_OF_CHAR_BUFFER_ARRAY); t->elems_in_charBuffer = 0;
            return;
        };
    };

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
};


void handle_menu_mode_event(SDL_Event* event, SDL_Window* window, SDL_Surface* render_surface, GameMode* game_mode, menu_state* MenuState, bool* trigger_save_animation){
    if(event->type != SDL_KEYDOWN){return;};

    if(event->key.keysym.sym == SDLK_UP){
        if (MenuState->selected_option_index == 0){return;}
        MenuState->selected_option_index--;
    };
    if(event->key.keysym.sym == SDLK_DOWN){
        if (MenuState->selected_option_index == 2){return;}
        MenuState->selected_option_index++;
    };

    if(event->key.keysym.sym == SDLK_RETURN){
        if(MenuState->selected_option_index == 2){
            *game_mode = _Default;
            MenuState->res_option_index = MenuState->prev_res_option_index;
        };
        if(MenuState->selected_option_index == 1){
            FILE* saveFileHandle = fopen("save.dat", "wb");
            fwrite(&MenuState->res_option_index, sizeof(int), 1, saveFileHandle);
            fclose(saveFileHandle);
            *trigger_save_animation = true;
            *game_mode = _Default;
        };
    };

    if(MenuState->selected_option_index == 0){
        if(event->key.keysym.sym == SDLK_LEFT){
            if (MenuState->res_option_index == 0){return;}
            MenuState->res_option_index--;
        };
        if(event->key.keysym.sym == SDLK_RIGHT){
            if (MenuState->res_option_index == 4){return;}
            MenuState->res_option_index++;
        };
        if(event->key.keysym.sym == SDLK_RETURN){
            if (MenuState->prev_res_option_index == MenuState->res_option_index){return;}
            MenuState->prev_res_option_index = MenuState->res_option_index;
            SDL_SetWindowSize(window, NATIVE_WIDTH * (MenuState->res_option_index + 1), NATIVE_HEIGHT * (MenuState->res_option_index + 1));
            render_surface = SDL_GetWindowSurface(window);
        };
    };
};

void render_menu(TextAnimationGlobals* pl, menu_state* MenuState ){
    for(int Y = 0; Y < 10; Y++){
        blit_tile(pl->font_surface_array[26],24,pl->working_surface, pl->font_surface_array[26]->clip_rect.h* Y, 0);
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

struct SaveData {// Initialised with new game state
    int res_index = 0;
    int room_index = 0;
};

struct RoomLookData {
    char item_names[16][16];
    char item_descriptions[16][5][LENGTH_OF_CHAR_BUFFER_ARRAY];
    int description_line_count[16];
};

enum RoomEnumIndex {
    _r_Church = 0,
    _r_Clearing
};

int room_name_to_index(char* maybe_name){
    if (!strcmp(maybe_name, "CHURCH")) return _r_Church;
    if (!strcmp(maybe_name, "CLEARING")) return _r_Clearing;
    return -1;
};

void load_game_rooms(char* room_names[2], SDL_Surface* room_backgrounds[2]) {
    for(int i = 0; i < 2; i++){
        char temp_directory[64] = {0};
        strcat(strcat(strcat(temp_directory, "assets/"), room_names[i]), ".bmp");
        char* current_name = room_names[i];
        int index = room_name_to_index(room_names[i]);
        printf("Current room name -> %s, index is %i\n", current_name, index);

        CRASH_IF_FALSE(index >= 0, "Invalid room name");
        room_backgrounds[index]= SDL_LoadBMP(temp_directory);
        //TODO: Save data will be relevant here
        //For storing inventory and axe state;
    };
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
    io_func(&save_data->res_index, sizeof(int), 1, saveFileHandle);
    io_func(&save_data->room_index, sizeof(int), 1, saveFileHandle);
    fclose(saveFileHandle);
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
            // if(red == RedOf(replace_color) && green == GreenOf(replace_color) && blue == BlueOf(replace_color)){
                uint32_t new_color = new_palette[j];
                *pixel_ptr = SDL_MapRGB(target->format,RedOf(new_color),GreenOf(new_color),BlueOf(new_color));
            };
        };
    };
};

struct AnimationArrays {
    int (*anim[32]) (int, TextAnimationGlobals*, void*) = {0};
    int duration_counts[32] = {0};
    bool occupied[32] = {0};
    void* temp_data[32] = {0};
};

void DoTextAnimations(AnimationArrays* arr, TextAnimationGlobals* tap){
    for(int i = 0; i < 32; i ++){
        if(arr->occupied[i] == false){continue;};
        int new_count = arr->anim[i](arr->duration_counts[i], tap, arr->temp_data[i]);
        if(new_count == -1){
            ZeroOut(arr->anim[i]);
            ZeroOut(arr->duration_counts[i]);
            arr->occupied[i] = false;
            if(arr->temp_data[i] != NULL){free(arr->temp_data[i]);};
        };
        arr->duration_counts[i] = new_count;
    };
};

void AddTextAnimation(AnimationArrays*arr, int (*anim) (int, TextAnimationGlobals*, void*), int duration, void* temp_data){
    for(int i = 0; i < 32; i++){
        if(arr->occupied[i] == FALSE){
            arr->anim[i] = anim;
            arr->occupied[i] = true;
            arr->duration_counts[i] = duration;
            arr->temp_data[i] = temp_data;
            return;
        };
    };
};

int success_item_look_animation(int duration, TextAnimationGlobals* pl, void* temp_data){
    // printf("trying to print: -> %s, %s, %s \n", pl->temp_multiline[0],pl->temp_multiline[1],pl->temp_multiline[2]);
    int line_count = *(int*)temp_data;
    char (*string_data) = (char*)temp_data;
    string_data = string_data + sizeof(int);
    
    char* temp_string_ptr = string_data;
    for(int i = 0; i < line_count; i++){
        char line_to_blit[LENGTH_OF_CHAR_BUFFER_ARRAY - 1] = {0};
        int YPosition = (NATIVE_HEIGHT) - (pl->font_surface_array[26]->clip_rect.h * (line_count - i + 1 ));
        memcpy(line_to_blit, temp_string_ptr, (LENGTH_OF_CHAR_BUFFER_ARRAY - 1) * sizeof(char));
        blit_tile(pl->font_surface_array[26],LENGTH_OF_CHAR_BUFFER_ARRAY + 4, pl->working_surface, YPosition, 0);
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

int no_target_look_animation(int duration, TextAnimationGlobals* payload, void* _){
    blit_tile(payload->font_surface_array[26],34,payload->working_surface, (NATIVE_HEIGHT) - payload->font_surface_array[26]->clip_rect.h* 2, 0);
    blit_text(payload, "WHAT WOULD YOU LIKE TO LOOK AT?", (NATIVE_HEIGHT) - payload->font_surface_array[26]->clip_rect.h* 2, 0);
    duration--;
    if(duration == 0){
        return -1;
    };
    return duration;
}

int failed_item_find_animation(int duration, TextAnimationGlobals* pl, void* temp_string){
    char error_string[36] = "";
    strcat(strcat(strcat(error_string, "CANNOT FIND THE '"), (char*)temp_string), "'.");
    static int error_thing = 0;
    if (error_thing != 3){
        printf("tempstring is %s \n", temp_string);
        printf("Current error string at number %i -> %s\n", error_thing, error_string);
        error_thing++;
    };
    blit_tile(pl->font_surface_array[26],34,pl->working_surface, (NATIVE_HEIGHT) - pl->font_surface_array[26]->clip_rect.h* 2, 0);
    blit_text(pl, error_string, (NATIVE_HEIGHT) - pl->font_surface_array[26]->clip_rect.h* 2, 0);
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

int main(int argc, char* args[]){
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 0;
    } 
    //vvv load assets here vvv
    SDL_Surface* fontSurfaceArray[LENGTH_OF_FONT_SURFACE_ARRAY] = {0};
    SDL_Surface* roomBackgrounds[2] = {0};
    SDL_Surface* quillArray[4] = {0};
    char* gameRoomNames[2] = {"CHURCH", "CLEARING"};
    load_font_files(fontSurfaceArray);
    load_game_rooms(gameRoomNames, roomBackgrounds);
    load_quill(quillArray);
    //^^^ load assets here ^^^



    //vvv initialize global stuff here vvv
    FILE *saveFileHandle = fopen("save.dat", "rb");
    SaveData save_data;
    handle_savegame_io(saveFileHandle == NULL ? _s_WRITE : _s_READ, &save_data);

    GameMode game_mode = _Default;
    //Put into struct?
    TextBufferData text_buffer;

    menu_state MenuState;
    int current_room_index = -1;
    uint32_t church_palette[4] = {0xCBF1F5, 0x445975, 0x0E0F21, 0x050314};
    uint32_t clearing_palette[4] = {0xEBE08D, 0x8A7236, 0x3D2D17, 0x1A1006};
    uint32_t* room_palettes[2] = {church_palette, clearing_palette};
    RoomLookData room_items[NUMBER_OF_ROOMS] = {0};
    RoomLookData church_items;
    memcpy(church_items.item_names[0], "SHRINE",16);
    church_items.description_line_count[0] = 2;
    char shrine_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {0};
    memcpy(shrine_description[0], "THE SHRINE HOUSES A BURNING CANDLE ", LENGTH_OF_CHAR_BUFFER_ARRAY*sizeof(char));
    memcpy(shrine_description[1], "AND ICONS OF CHRIST AND THE SAINTS.", LENGTH_OF_CHAR_BUFFER_ARRAY*sizeof(char));
    memcpy(church_items.item_descriptions[0][0],shrine_description[0], LENGTH_OF_CHAR_BUFFER_ARRAY*sizeof(char));
    memcpy(church_items.item_descriptions[0][1],shrine_description[1], LENGTH_OF_CHAR_BUFFER_ARRAY*sizeof(char));
    room_items[_r_Church] = church_items;

    //^^^ initialize global stuff here ^^^



    //vvv apply savedata stuff here vvv
    MenuState.res_option_index = save_data.res_index;
    MenuState.prev_res_option_index = save_data.res_index;
    current_room_index = save_data.room_index;
    if(save_data.room_index != 0){
        replace_all_palettes(fontSurfaceArray,quillArray, room_palettes, 0, save_data.room_index);
    };
    //^^^ apply savedata stuff here ^^^

    SDL_Window* window = SDL_CreateWindow("Searching for the Name", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        NATIVE_WIDTH * (MenuState.res_option_index + 1), 
        NATIVE_HEIGHT * (MenuState.res_option_index + 1), 
        SDL_WINDOW_SHOWN );
    if( window == NULL ){
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return 0;
    };
    SDL_Surface* render_surface = SDL_GetWindowSurface( window );
    SDL_Surface* working_surface = SDL_CreateRGBSurface(NULL, NATIVE_WIDTH, NATIVE_HEIGHT, 
        render_surface->format->BitsPerPixel, render_surface->format->Rmask, 
        render_surface->format->Gmask, render_surface->format->Bmask, render_surface->format->Amask);

    //vvv init animations here
    TextAnimationGlobals text_animation_payload;
    text_animation_payload.working_surface = working_surface;
    memcpy(text_animation_payload.font_surface_array, fontSurfaceArray, sizeof(text_animation_payload.font_surface_array));

    AnimationArrays animation_arrays;
    ///^^^ init animations here

    bool quit = false; bool trigger_save_animation = false; bool trigger_failed_look_command = false;
    while (quit != true){
        SDL_StartTextInput();
        Uint32 frameStart = SDL_GetTicks();
        SDL_Event event;
        while( SDL_PollEvent( &event ) != 0 ){
            if (event.type == SDL_QUIT){quit = true;};
            switch(game_mode){
                case _Default: handle_default_mode_event(&event, &text_buffer, &game_mode);break;
                case _Menu: handle_menu_mode_event(&event,window, render_surface, &game_mode, &MenuState, &trigger_save_animation);break;
            };
        };
        
        ValidCommand command = parse_token(text_buffer.tokenBuffer[0]);
        switch(command){
            case Save_Command: {
                save_data.res_index = MenuState.res_option_index;
                save_data.room_index = current_room_index;
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
                int previous_room_index = current_room_index;
                int new_index = room_name_to_index(text_buffer.tokenBuffer[1]);
                if (new_index == -1){break;}
                current_room_index = new_index;
                replace_all_palettes(fontSurfaceArray,quillArray, room_palettes, previous_room_index, new_index);
            }; break;
            case Look_command:{
                if(!strcmp(text_buffer.tokenBuffer[1], "")){
                    AddTextAnimation(&animation_arrays, no_target_look_animation, 240, NULL);
                    break;
                };
                bool item_found = false;
                void* temp_description_data;
                for (int i = 0; i < 16; i++){
                    if (strcmp(text_buffer.tokenBuffer[1], room_items[current_room_index].item_names[i]) != 0){
                        continue;
                    };
                    int description_size = sizeof(room_items[current_room_index].item_descriptions[0]);

                    temp_description_data = malloc(sizeof(int) + description_size);
                    memcpy(temp_description_data, &room_items[current_room_index].description_line_count[i], sizeof(int));
                    memcpy((char*)temp_description_data + sizeof(int), room_items[current_room_index].item_descriptions[i], description_size);
                    AddTextAnimation(&animation_arrays, success_item_look_animation, 240, temp_description_data);
                    item_found = true;
                    break;
                };
                if(!item_found){
                    void* temp_string = malloc(sizeof(text_buffer.tokenBuffer[1]));
                    printf("tokenBuffer[1] being passed: %s \n", text_buffer.tokenBuffer[1]);
                    memcpy(temp_string, text_buffer.tokenBuffer[1], sizeof(text_buffer.tokenBuffer[1]));
                    AddTextAnimation(&animation_arrays, failed_item_find_animation, 240, temp_string);
                };
            };break;
            case Invalid_Command:{};break;
        };
        ZeroOut(text_buffer.tokenBuffer);
        blit_background(roomBackgrounds[current_room_index], working_surface);

        if (game_mode == _Menu){
            render_menu(&text_animation_payload, &MenuState);
        };

        if (game_mode == _Default){
            if (text_buffer.elems_in_charBuffer > 0){
                blit_tile(fontSurfaceArray[26],LENGTH_OF_CHAR_BUFFER_ARRAY + 4, working_surface, (fontSurfaceArray[0]->clip_rect.h * -1) + NATIVE_HEIGHT, 0);
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
            SDL_Delay(frameDuration - frameTime);
        };
    };

    //Cleanup
    SDL_Quit();

    return 0;
}