#ifndef NPC_H
#define NPC_H

#include <stdio.h>
#include "player.h"

#define MAX_NAME_LENGTH 50
#define MAX_DESC_LENGTH 200
#define MAX_NPC 5

typedef struct NPC {
    int npcId;
    char name[MAX_NAME_LENGTH];
    char description[MAX_DESC_LENGTH];
    int isHostile;
    bool isDead;
} NPC;

void displayNpc(Room *room);
void interactWithNpcById(Room *room, const Item *inventory, const int *inventoryCount, const char *playerName);
void saveNpcState(FILE *file, Room *room);
void loadNpcState(FILE *file, Room *room);

#endif