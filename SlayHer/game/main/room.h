#ifndef ROOM_H
#define ROOM_H

#include <stdbool.h>

#include "npc.h"

#define MAX_ROOMS 7

typedef struct Room {
    int roomId;
    char name[50];
    char description[200];
    Item items[MAX_ITEMS];
    int itemCount;
    NPC npcs[MAX_NPC];
    int npcCount;
    Room *north;
    Room *south;
    Room *east;
    Room *west;
    int isInitialized;
} Room;

extern Room *rooms[MAX_ROOMS];

Room* createRoom(const char *name, const char *description);
void connectRooms(Room *room1, Room *room2, const char *direction);
void displayRoom(Room *room);
void addNpcToRoom(Room *room, const char *npcName, const char *npcDescription, int isHostile, bool isDead);
void freeRoom(Room *room);
void freeAllRooms();
void initializeRooms();
void gameLoop();

extern bool playing;

#endif