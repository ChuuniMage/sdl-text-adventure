#pragma once

#include "sftn-rooms.cpp"
#include "sftn-items-inventory.cpp"
#include <stdio.h>

enum SAVEGAME_IO {
    _s_READ,
    _s_WRITE
};

struct SaveData {// Initialised with new game state
    int res_index = 0;
    int room_index = 0;
    ItemData item_data = {0};
    PlayerInventory inventory_data = {{_i_None, 0}};
};


inline size_t io_read(void* buffer, size_t size, size_t count, FILE* f){return fread(buffer, size, count, f);}
inline size_t io_write(void* buffer, size_t size, size_t count, FILE* f){return fwrite(buffer, size, count, f);}


void handle_savegame_io(SAVEGAME_IO mode, SaveData* save_data){
    size_t (*io_func)(void*, size_t, size_t, FILE*) = mode == _s_READ ? &io_read : &io_write;
    char* fopen_mode = mode == _s_READ ? "rb" : "wb";

    FILE* saveFileHandle = fopen("save.dat", fopen_mode);
    int items_handled = 0;
    items_handled = io_func(&save_data->res_index, sizeof(int), 1, saveFileHandle);
    items_handled = io_func(&save_data->room_index, sizeof(int), 1, saveFileHandle);
   
    //Room item data
    items_handled = io_func(&save_data->item_data.index_within_inventory, sizeof(save_data->item_data.index_within_inventory), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.is_item_taken, sizeof(save_data->item_data.is_item_taken), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.is_takeable_item, sizeof(save_data->item_data.is_takeable_item), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.item_description, sizeof(save_data->item_data.item_description), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.number_of_items_in_room, sizeof(save_data->item_data.number_of_items_in_room), 1, saveFileHandle);
    items_handled = io_func(&save_data->item_data.current_room_location, sizeof(save_data->item_data.current_room_location), 1, saveFileHandle);

    //Inventory Data
    items_handled = io_func(&save_data->inventory_data.inv_item, sizeof(save_data->inventory_data.inv_item), 1, saveFileHandle);
    items_handled = io_func(&save_data->inventory_data.number_of_items_held, sizeof(save_data->inventory_data.number_of_items_held), 1, saveFileHandle);
    items_handled = io_func(&save_data->inventory_data.occupied, sizeof(save_data->inventory_data.occupied), 1, saveFileHandle);
    items_handled = io_func(&save_data->inventory_data.origin_index, sizeof(save_data->inventory_data.origin_index), 1, saveFileHandle);
    items_handled = io_func(&save_data->inventory_data.tail_index, sizeof(save_data->inventory_data.tail_index), 1, saveFileHandle);

    fclose(saveFileHandle);
};
