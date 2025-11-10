#ifndef ITEM_H
#define ITEM_H

#include <stdio.h>

typedef struct {
    int itemId;
    char *name;
    char *description;
    int pickedUp;
} Item;

extern int numItems;
extern Item items[];

typedef struct Room Room;

void addItemToRoom(Room *room, const char *itemName);
void displayItems(const Room *room);
int pickUpItemById(Room *room, Item *inventory, int *inventoryCount);
void saveItemState(const Room *room, FILE *file);
void loadItemState(Room *room, FILE *file);

#endif