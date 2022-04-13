#include <stdio.h>
#include <SDL.h>
#include <windows.h>
#include "my_utils.h"
#include <string.h>
#include "sftn-top-lvl.h"
#include "sftn-tokenizer.cpp"
#include "sftn-char-to-index.cpp"
#include "sftn-txt-animation.cpp"
#include "sftn-rooms.cpp"
#include "sftn-items-inventory.cpp"
#include "sftn-savegame.cpp"
#include "sftn-load-assets.cpp"

const int FPS = 60;
const int frameDuration = 1000 / FPS;

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
    if (!strcmp(str, "EXAMINE")) return Look_command;
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

void handle_menu_mode_event(SDL_Event* event, SDL_Window* window, SDL_Surface* render_surface, GameMode* game_mode, Menu_RenderState* menu_state, Text_RenderState* animation_arrays, SaveData* save_data){
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
            save_data->res_index = menu_state->res_option_index;
            handle_savegame_io(_s_WRITE, save_data);
            AddTextAnimation(animation_arrays, save_animation, 240, NULL);
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

struct Item_State {
    PlayerInventory inv;
    ItemData items;
    EventData events;
};

void init_item_data(Item_State* s){

    for (int i = 0; i < MAX_ITEMS_IN_INVENTORY; i++){
        s->inv.inv_item[i] = {_i_None, -1};
        s->inv.occupied[i] = false;
    };
    s->inv.origin_index = -1;
    s->inv.tail_index = -1;
    s->inv.number_of_items_held = 0;

    for(int i = 0; i < MAX_ITEMS_IN_INVENTORY; i++){
        s->items.current_room_location[i] = _r_None;
    };


    char shrine_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "The shrine houses a burning candle,", "icons of Christ and⌈ the Saints.", 0, 0, 0
    };
    add_item_to_RoomItemData(&s->items, "SHRINE",
        shrine_description, false, (RoomEnumIndex)room_name_to_index("CHURCH"));

    char axe_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "The axe is stumpwise lodged.", 0, 0, 0, 0
    };
    add_item_to_RoomItemData(&s->items, "AXE", axe_description, 1, (RoomEnumIndex)room_name_to_index("CLEARING"));
    s->events.take_events[item_name_to_index("AXE")] = take_axe_event;
    char stump_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "The stump is axewise stuck.", 0, 0, 0, 0
    }; 
    add_item_to_RoomItemData(&s->items, "STUMP", stump_description, false, (RoomEnumIndex)room_name_to_index("CLEARING"));
    printf("Clearing to index -> %i \n", room_name_to_index("CLEARING"));
    PlaceEvent axe_in_stump_event;
    axe_in_stump_event.destination = (ItemEnumIndex)item_name_to_index("STUMP");
    axe_in_stump_event.item_to_place = (ItemEnumIndex)item_name_to_index("AXE");
    axe_in_stump_event.event = place_axe_in_stump_event;
    s->events.place_events[item_name_to_index("STUMP")] = axe_in_stump_event;
}

struct GlobalRenderStates {
    Text_RenderState txt;
    Inventory_RenderState inv;
    Menu_RenderState menu;
};



int main(int argc, char* args[]){
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 0;
    } 

    //vvv Initiate game data here vvv
    Item_State item_state;
    init_item_data(&item_state);
    //^^^ Initiate game data here ^^^

    //vvv load assets here vvv

    // SDL_Surface* quillArray[4] = {0};
    GlobalSurfaces global_surfaces;
    load_font_files(global_surfaces.font_surface_array);
    load_game_rooms(global_surfaces.room_backgrounds);
    load_quill(global_surfaces.quill_array);
    load_item_in_room_sprites(global_surfaces.item_in_room_sprites);
    //^^^ load assets here ^^^

    //vvv initialize global stuff here vvv
    GameMode game_mode = _Default;
    TextBufferData text_buffer;
    GlobalRenderStates global_render;
    global_render.menu.res_option_index = 0;
    global_render.menu.prev_res_option_index = 0;

    int current_room_index = 0;
    uint32_t church_palette[4] = {0xCBF1F5, 0x445975, 0x0E0F21, 0x050314};
    uint32_t clearing_palette[4] = {0xEBE08D, 0x8A7236, 0x3D2D17, 0x1A1006};
    uint32_t* room_palettes[NUMBER_OF_ROOMS] = {church_palette, clearing_palette};
    //^^^ initialize global stuff here ^^^


    //vvv apply savedata stuff here vvv
    FILE *saveFileHandle = fopen("save.dat", "rb");
    SaveData save_data;
    bool fresh_game = saveFileHandle == NULL;
    if (fresh_game){
        save_data.inventory_data = item_state.inv;
        save_data.item_data = item_state.items;
        save_data.res_index = 0;
        save_data.room_index = 0;
    }
    handle_savegame_io(saveFileHandle == NULL ? _s_WRITE : _s_READ, &save_data);
    if (!fresh_game){
        global_render.menu.res_option_index = save_data.res_index;
        global_render.menu.prev_res_option_index = save_data.res_index;
        current_room_index = save_data.room_index;
        item_state.items = save_data.item_data;
        item_state.inv = save_data.inventory_data;
    };

    if(save_data.room_index != 0){
        replace_all_palettes(&global_surfaces, room_palettes, 0, save_data.room_index);
    };
    //^^^ apply savedata stuff here ^^^

    //vvv setup window, render_surface, and working_surface handles here vvv
    SDL_Window* window = SDL_CreateWindow("Searching for the Name", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        NATIVE_WIDTH * (global_render.menu.res_option_index + 1), 
        NATIVE_HEIGHT * (global_render.menu.res_option_index + 1), 
        SDL_WINDOW_SHOWN );
    if( window == NULL ){
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return 0;
    };
    SDL_Surface* render_surface = SDL_GetWindowSurface( window );
    global_surfaces.working_surface = SDL_CreateRGBSurface(NULL, NATIVE_WIDTH, NATIVE_HEIGHT, 
        render_surface->format->BitsPerPixel, render_surface->format->Rmask, 
        render_surface->format->Gmask, render_surface->format->Bmask, render_surface->format->Amask);
    //^^^ setup window, render_surface, and working_surface handles here ^^^

    bool quit = false;
    while (quit != true){
        SDL_StartTextInput();
        Uint32 frameStart = SDL_GetTicks();
        SDL_Event event;
        while( SDL_PollEvent( &event ) != 0 ){
            if (event.type == SDL_QUIT){quit = true;break;};
            switch(game_mode){
                case _Default: handle_default_mode_event(&event, &text_buffer, &game_mode);break;
                case _Menu: handle_menu_mode_event(&event,window, render_surface, &game_mode, &global_render.menu, &global_render.txt, &save_data);break;
                case _Inventory: handle_inventory_mode_event(&event, &game_mode, &global_render.inv);break;
            };
        };
        
        ValidCommand command = parse_token(text_buffer.tokenBuffer[0]);
        switch(command){
            case Save_Command: {
                save_data.res_index = global_render.menu.res_option_index;
                save_data.room_index = current_room_index;
                memcpy(&save_data.item_data, &item_state.items, sizeof(item_state.items));
                memcpy(&save_data.inventory_data, &item_state.inv, sizeof(item_state.inv));
                handle_savegame_io(_s_WRITE, &save_data);
                AddTextAnimation(&global_render.txt, save_animation, 240, NULL);
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
                replace_all_palettes(&global_surfaces, room_palettes, previous_room_index, new_index);
            }; break;
            case Look_command:{

                if(strcmp(text_buffer.tokenBuffer[1], "") == 0){
                    AddTextAnimation(&global_render.txt, no_target_look_animation, 240, NULL);
                    break;
                };
                if(strcmp(text_buffer.tokenBuffer[1], "INVENTORY") == 0){
                    game_mode = _Inventory;
                    break;
                };
                int item_index = item_name_to_index(text_buffer.tokenBuffer[1]);
                printf("Looking for %s \n", text_buffer.tokenBuffer[1]);
                printf("Failed to find stump, item index: %i \n", item_index);
                if(item_index == -1 || item_state.items.current_room_location[item_index] != current_room_index){
                    char new_string[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
                        0, 0, 0, 0, 0
                    };
                    strcpy_s(new_string[0], text_buffer.tokenBuffer[1]);
                    printf("Failed to find stump, item index: %i \n", item_index);
                    printf("Room enum %i \n", item_state.items.current_room_location[item_index]);
                    printf("Current room location -> %i \n", item_state.items.current_room_location[item_index]);
                    printf("Current room enum %i \n", current_room_index);
                    AddTextAnimation(&global_render.txt, failed_item_find_animation, 240, new_string);
                    break;
                };
                char new_string[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
                        0, 0, 0, 0, 0
                    };
                for (int i = 0; i < 5; i++){
                    strcpy_s(new_string[i], item_state.items.item_description[item_index][i]);
                };
                
                AddTextAnimation(&global_render.txt, success_item_look_animation, 240, new_string);
            };break;
            case Take_command:{
                if(strcmp(text_buffer.tokenBuffer[1], "") == 0){
                    AddTextAnimation(&global_render.txt, no_target_get_animation, 240, NULL);
                    break;
                };
                int item_index = item_name_to_index(text_buffer.tokenBuffer[1]);
                if(item_index == -1 || item_state.items.current_room_location[item_index] != current_room_index){
                    char new_string[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
                        0, 0, 0, 0, 0
                    };

                    strcpy_s(new_string[0], text_buffer.tokenBuffer[1]);
                                    
                    AddTextAnimation(&global_render.txt, failed_item_find_animation, 240, new_string);
                    break;
                };
                item_state.items.is_item_taken[item_index] = true;
                item_state.items.current_room_location[item_index] = _r_None;
                item_state.items.number_of_items_in_room[current_room_index]--;
                item_state.items.index_within_inventory[item_index] = inventory_add_item(&item_state.inv, (ItemEnumIndex)item_index); // likely to be taken out
                

                if(item_state.events.take_events[item_index]){          
                    item_state.events.take_events[item_index](&item_state.items, item_index, -1);
                };
                char new_string[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {0};
                strcpy_s(new_string[0], text_buffer.tokenBuffer[1]);
                AddTextAnimation(&global_render.txt, success_item_take_animation, 240, new_string);
            };break;
              case Place_command:{
                if(strcmp(text_buffer.tokenBuffer[1], "") == 0){
                    AddTextAnimation(&global_render.txt, no_target_place_animation, 240, NULL);
                    break;
                };

                int item_enum = item_name_to_index(text_buffer.tokenBuffer[1]);
                int index_within_inventory = item_state.items.index_within_inventory[item_enum];

                if(item_enum == -1 || index_within_inventory == -1){
                    char new_string[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {0};
                    strcpy_s(new_string[0], text_buffer.tokenBuffer[1]);
                    AddTextAnimation(&global_render.txt, failed_place_item_find_animation, 240, new_string);
                    break;
                };

                if(strcmp(text_buffer.tokenBuffer[2], "") == 0){
                    AddTextAnimation(&global_render.txt, no_destination_place_animation, 240, NULL);
                    break;
                };
                int destination_index = item_name_to_index(text_buffer.tokenBuffer[2]);
                if(destination_index == -1 || item_state.items.current_room_location[destination_index] != current_room_index){
                    char new_string[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {0};
                    strcpy_s(new_string[0], text_buffer.tokenBuffer[2]);
                    AddTextAnimation(&global_render.txt, failed_item_find_animation, 240, new_string);
                    break;
                };
                if(item_state.events.place_events[destination_index].event == NULL){
                    AddTextAnimation(&global_render.txt, incompatible_place_animation, 240, NULL);
                    break;
                };

                if(item_state.events.place_events[destination_index].item_to_place == item_enum){
                    printf("firing place event\n");
                    item_state.events.place_events[destination_index].event(&item_state.items);
                };
                item_state.items.is_item_taken[item_enum] = false;
                item_state.items.current_room_location[item_enum] = (RoomEnumIndex)current_room_index;
                item_state.items.number_of_items_in_room[current_room_index]++;
                item_state.items.index_within_inventory[current_room_index] = -1;
                inventory_delete_item(&item_state.inv, index_within_inventory);

                char new_string[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {0};
                strcpy_s(new_string[0], text_buffer.tokenBuffer[1]);
                AddTextAnimation(&global_render.txt, success_place_take_animation, 240, new_string);
            };break;
            case Inventory_command:{
                game_mode = _Inventory;
            };break;
            case Invalid_Command:{};break;
        };
        ZeroOut(text_buffer.tokenBuffer);

        blit_background(global_surfaces.room_backgrounds[current_room_index], global_surfaces.working_surface);
        blit_items_in_room(current_room_index, &global_surfaces, &item_state.items);

        switch(game_mode){
            case _Inventory: render_inventory(&global_surfaces, &item_state.items, &item_state.inv, &global_render.inv);break;
            case _Menu: render_menu(&global_surfaces, &global_render.menu);break;
            case _Default: {
                DoTextAnimations(&global_render.txt, &global_surfaces);

                if (text_buffer.elems_in_charBuffer == 0){ break;};
                blit_tile(global_surfaces.font_surface_array[char_to_index(' ')],LENGTH_OF_CHAR_BUFFER_ARRAY + 4, global_surfaces.working_surface, (global_surfaces.font_surface_array[0]->clip_rect.h * -1) + NATIVE_HEIGHT, 0);
                blit_text(&global_surfaces, text_buffer.charBuffer, (global_surfaces.font_surface_array[0]->clip_rect.h * -1) + NATIVE_HEIGHT, 0);
                if(text_buffer.elems_in_charBuffer != 35){
                    animate_quill(&global_surfaces, text_buffer.elems_in_charBuffer);
                };               
            };break;
        }

        SDL_BlitScaled(global_surfaces.working_surface, NULL, render_surface, NULL);

        SDL_UpdateWindowSurface(window);
        int frameTime = SDL_GetTicks() - frameStart;
        if (frameDuration > frameTime){
            printf("ethtj ? \n");
            SDL_Delay(frameDuration - frameTime);
        };
    };

    //Cleanup
    SDL_Quit();

    return 0;
}