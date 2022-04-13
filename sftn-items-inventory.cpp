#pragma once
#include "sftn-top-lvl.h"
#include "my_utils.h"
#include "sftn-rooms.cpp"
#include "sftn-char-to-index.cpp"

#define ITEM_ENUMS \
  X(SHRINE) \
  X(AXE) \
  X(STUMP)

enum ItemEnumIndex {
    _i_None = -1,
#define X(x) _i_##x,
    ITEM_ENUMS
#undef X
};

int item_name_to_index(char str[16]){
#define X(x) if (!strcmp(str, #x)) return _i_##x;
    ITEM_ENUMS
#undef X
    return -1;
};

//Note: strings 0-term by default so deal with it accordingly
char* item_index_to_name(int i){
    switch((ItemEnumIndex)i){
#define X(x) case _i_##x: return #x;
    ITEM_ENUMS
#undef X
        default: return "0";
    };
}

struct ItemData {
    char item_description[MAX_NUMBER_OF_ITEMS][5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {0};
    bool is_takeable_item[MAX_NUMBER_OF_ITEMS] = {0};
    bool is_item_taken[MAX_NUMBER_OF_ITEMS] = {0};
    RoomEnumIndex current_room_location[MAX_NUMBER_OF_ITEMS] = {_r_None};
    int number_of_items_in_room[NUMBER_OF_ROOMS] = {0};
    int index_within_inventory[MAX_NUMBER_OF_ITEMS] = {-1};
};

struct PlaceEvent {
    ItemEnumIndex destination;
    ItemEnumIndex item_to_place;
    void (*event) (ItemData*);
};

struct EventData {
    //These will very likely need to be made multi-dimensional.
    void (*take_events[MAX_NUMBER_OF_ITEMS])(ItemData*, int, int); //Rejiggy take command as a take event?
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
        if(inv->occupied[i] == true){continue;};
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


void add_item_to_RoomItemData(ItemData* room_items, 
    char name[MAX_NAME_LENGTH], 
    char description[5][LENGTH_OF_CHAR_BUFFER_ARRAY],
    bool is_takeable,
    RoomEnumIndex room_enum
    ){
    static int current_number_of_items = 0;
    CRASH_IF_FALSE(current_number_of_items < MAX_NUMBER_OF_ITEMS, "Added too many items.");
    CRASH_IF_FALSE(item_name_to_index(name) != -1, "Invalid item name.");

    int i = item_name_to_index(name);
    memcpy(room_items->item_description[i],description, sizeof(room_items->item_description[i]));
    room_items->is_takeable_item[i] = is_takeable;
    room_items->current_room_location[i] = room_enum;
    room_items->number_of_items_in_room[room_enum]++;
    room_items->index_within_inventory[i] = -1;
    current_number_of_items++;
};

void alter_item_description(ItemData* room_items, int item_index, char new_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY]){
    memcpy(room_items->item_description[item_index],new_description, sizeof(room_items->item_description[item_index]));
};

void take_axe_event(ItemData* room_items, int axe_index, int _){
    char axe_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "It's a well balanced axe. Much use.", 0, 0, 0,0
    };
    alter_item_description(room_items, item_name_to_index("AXE"), axe_description);

    char stump_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "Stump left behind from an old tree.", "Memories of childhood return.", 0, 0, 0
    };

    alter_item_description(room_items, item_name_to_index("STUMP"), stump_description);
};

void place_axe_in_stump_event(ItemData* room_items){
    char axe_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "The axe is stumpwise lodged.", 0, 0, 0, 0
    };
    char stump_description[5][LENGTH_OF_CHAR_BUFFER_ARRAY] = {
        "The stump is axewise stuck.", 0, 0, 0, 0
    }; 
    alter_item_description(room_items, item_name_to_index("AXE"), axe_description);
    alter_item_description(room_items, item_name_to_index("STUMP"), stump_description);
};

