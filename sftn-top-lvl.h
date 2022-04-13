#pragma once

#define LENGTH_OF_TOKEN_ARRAY 18
#define LENGTH_OF_CHAR_BUFFER_ARRAY 36
#define LENGTH_OF_FONT_SURFACE_ARRAY 96
#define MAXIMUM_TOKEN_SIZE 16
#define NUMBER_OF_ROOMS 2
#define MAX_NUMBER_OF_ITEMS_IN_ROOM 16
#define MAX_NAME_LENGTH 16
#define MAX_NUMBER_OF_ITEMS 16
#define MAX_ITEMS_IN_INVENTORY 12

#define NATIVE_WIDTH 320
#define NATIVE_HEIGHT 200

#define CRASH_IF_FALSE(Expression, msg) if(Expression == false) {printf(msg);*(int *)0 = 0;}

#define TempDebugOutput(...) char __COUNTER__##debugStringBuffer[256];  \
    wsprintfA(debugStringBuffer, __VA_ARGS__);  \
    printf(debugStringBUffer);                  \
    OutputDebugStringA(debugStringBuffer);      \