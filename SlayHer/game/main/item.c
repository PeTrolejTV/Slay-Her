#include <stdlib.h>
#include <string.h>
#include "room.h"
#include "utils.h"
#include "item.h"

Item items[] = {
    {1, "Duck", "A rubber ducky.", 0},
    {2, "Plushie", "A cute plushie.", 0},
    {3, "Book", "An interesting looking book.", 0},
    {4, "Frying pan", "An ordinary looking pan.", 0},
    {5, "Sack of Gold", "A sack full of gold.", 0},
    {6, "Golden Trophy", "A shiny looking trophy.", 0},
    {7, "Legendary Sword", "A sword of legendary power.", 0},
    {8, "Sword", "A sturdy weapon to protect yourself.", 0},
    {9, "Magical Ring", "A ring with mystical properties.", 0},
    {10, "Gift", "A token of friendship.", 0},
    {11, "Crown", "A crown imbued with dark magic.", 0}
};

int numItems = sizeof(items) / sizeof(items[0]);

void addItemToRoom(Room *room, const char *itemName) {
    if (room->itemCount >= MAX_ITEMS) {
        logToFile("Room item limit reached. Could not add item.");
        return;
    }

    Item *itemToAdd = NULL;
    for (int i = 0; i < numItems; i++) {
        if (strcmp(items[i].name, itemName) == 0) {
            itemToAdd = &items[i];
            break;
        }
    }

    if (itemToAdd) {
        if (itemToAdd->pickedUp == 1) {
            char logMessage[256];
            snprintf(logMessage, sizeof(logMessage), "Item '%s' already picked up. Not adding to the room.", itemName);
            logToFile(logMessage);
            return;
        }

        room->items[room->itemCount] = *itemToAdd;
        room->itemCount++;
        char logMessage[256];
        snprintf(logMessage, sizeof(logMessage), "Item '%s' added to room '%s'.", itemName, room->name);
        logToFile(logMessage);
    } else {
        char logMessage[256];
        snprintf(logMessage, sizeof(logMessage), "Item '%s' not found. Could not add to room '%s'.", itemName, room->name);
        logToFile(logMessage);
    }
}

void displayItems(const Room *room) {
    if (room == NULL) {
        typeWriterEffectLine("Error: Room is NULL.\n");
        logToFile("Error: Room is NULL.");
        return;
    }

    if (room->itemCount == 0) {
        typeWriterEffect("There are no items here.\n");
        logToFile("No items found in the room.");
        return;
    }

    typeWriterEffect("Items found in the Area:\n");
    logToFile("Displaying items in the room.");

    for (int i = 0; i < room->itemCount; i++) {
        if (room->items[i].pickedUp == 1) {
            continue;
        }

        char message[256];
        snprintf(message, sizeof(message), "Item: %s\nDescription: %s\n", room->items[i].name, room->items[i].description);
        typeWriterEffectLine(message);
        logToFile(message);
    }
}

int pickUpItemById(Room *room, Item *inventory, int *inventoryCount) {
    if (room->itemCount == 0) {
        typeWriterEffectLine("\n-----------------------------\n");
        typeWriterEffect("There are no items in this Area.\n");
        typeWriterEffectLine("-----------------------------\n");
        typeWriterEffectLine("ACTION TO TAKE: ");
        logToFile("Player attempted to pick up an item, but no items are available.");
        return 0;
    }

    char message[256];
    typeWriterEffectLine("\n-----------------------------\n");
    typeWriterEffect("Items in this Area:\n");
    logToFile("Displaying available items to pick up.");

    for (int i = 0; i < room->itemCount; i++) {
        if (room->items[i].pickedUp == 1) {
            continue;
        }

        snprintf(message, sizeof(message), "%d. %s - %s", i + 1, room->items[i].name, room->items[i].description);
        typeWriterEffectLine(message);
        logToFile(message);
    }

    int itemId;
    char input[10];
    typeWriterEffectLine("\n-----------------------------\n");
    typeWriterEffectLine("NUMBER OF THE ITEM TO PICK UP: ");
    while (1) {
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) != 0) {
            char *endPtr;
            itemId = strtol(input, &endPtr, 10);

            if (*endPtr != '\0') {
                typeWriterEffectLine("Invalid input. Please enter a valid item number.");
                continue;
            }

            if (itemId < 1 || itemId > room->itemCount) {
                typeWriterEffectLine("Invalid item ID. Please try again.");
                continue;
            }
            break;
        }
    }

    Item selectedItem = room->items[itemId - 1];
    inventory[*inventoryCount] = selectedItem;
    (*inventoryCount)++;

    typeWriterEffectLine("\n-----------------------------\n");
    snprintf(message, sizeof(message), "You picked up: %s.\n", selectedItem.name);
    typeWriterEffect(message);
    typeWriterEffectLine("-----------------------------\n");
    typeWriterEffectLine("ACTION TO TAKE: ");
    logToFile(message);

    room->items[itemId - 1].pickedUp = 1;
    for (int i = itemId - 1; i < room->itemCount - 1; i++) {
        room->items[i] = room->items[i + 1];
    }
    room->itemCount--;

    logToFile("Item picked up and removed from the room.");
    return 1;
}

void saveItemState(const Room *room, FILE *file) {
    char logMessage[256];

    snprintf(logMessage, sizeof(logMessage), "Saving %d items in room.\n", room->itemCount);
    logToFile(logMessage);

    fwrite(&room->itemCount, sizeof(int), 1, file);
    for (int i = 0; i < room->itemCount; i++) {
        if (room->items[i].pickedUp == 1) {
            continue;
        }

        snprintf(logMessage, sizeof(logMessage), "Saving item %d: %s.\n", i, room->items[i].name);
        logToFile(logMessage);

        fwrite(&room->items[i], sizeof(Item), 1, file);
    }
}

void loadItemState(Room *room, FILE *file) {
    char logMessage[256];

    if (fread(&room->itemCount, sizeof(int), 1, file) != 1) {
        logToFile("Error reading the item count for room.\n");
        return;
    }
    snprintf(logMessage, sizeof(logMessage), "Loading %d items in room.\n", room->itemCount);
    logToFile(logMessage);

    for (int i = 0; i < room->itemCount; i++) {
        if (fread(&room->items[i], sizeof(Item), 1, file) != 1) {
            snprintf(logMessage, sizeof(logMessage), "Error reading item %d.\n", i);
            logToFile(logMessage);
            return;
        }
        snprintf(logMessage, sizeof(logMessage), "Loaded item %d: %s.\n", i, room->items[i].name);
        logToFile(logMessage);
    }
}