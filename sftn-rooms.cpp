#pragma once
// could possibly just run this through a macro

#define ROOM_ENUMS \
  X(CHURCH) \
  X(CLEARING)

enum RoomEnumIndex {
    _r_None = -1,
#define X(x) _r_##x,
    ROOM_ENUMS
#undef X
};

int room_name_to_index(char* str){
#define X(x) if (!strcmp(str, #x)) return _r_##x;
    ROOM_ENUMS
#undef X
    return -1;
};

char* room_index_to_name(int i){
    switch((RoomEnumIndex)i){
#define X(x) case _r_##x: return #x;
    ROOM_ENUMS
#undef X
        default: return "0";
    };
}
