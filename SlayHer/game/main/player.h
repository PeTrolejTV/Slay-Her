#ifndef PLAYER_H
#define PLAYER_H

#define MAX_ITEMS 11
#include "item.h"

typedef struct Player{
    char name[50];
    Item inventory[MAX_ITEMS];
    int inventoryCount;
    int currentRoomId;
    int hasCrown;
    int treasureRoomChecked;
} Player;

extern Player player;

void initPlayer(Player *player, const char *name);
void displayInventory(const Player *player);
void giveItemToPlayer(Player *player, int itemId);
void setPlayerRoom(const Player *player);
void restorePlayerState();

#endif