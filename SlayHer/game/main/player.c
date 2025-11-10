#include <stdio.h>
#include <string.h>
#include "player.h"

#include "room.h"
#include "utils.h"

void initPlayer(Player *player, const char *name) {
    strncpy(player->name, name, sizeof(player->name) - 1);
    player->name[sizeof(player->name) - 1] = '\0';
    player->inventoryCount = 0;
    player->hasCrown = 0;
    player->treasureRoomChecked = 0;

    char logMessage[100];
    snprintf(logMessage, sizeof(logMessage), "Initializing player: %s", player->name);
    logToFile(logMessage);
}

void displayInventory(const Player *player) {
    char countLog[100];
    snprintf(countLog, sizeof(countLog), "Displaying inventory. Current count: %d", player->inventoryCount);
    logToFile(countLog);

    if (player->inventoryCount == 0) {
        typeWriterEffectLine("\n-----------------------------\n");
        typeWriterEffect("Your inventory is empty.\n");
        typeWriterEffectLine("-----------------------------\n");
        typeWriterEffectLine("ACTION TO TAKE: ");
        logToFile("Player's inventory is empty.");
    } else {
        typeWriterEffectLine("\n-----------------------------\n");
        typeWriterEffect("Your inventory contains:\n");
        logToFile("Displaying player's inventory.");

        for (int i = 0; i < player->inventoryCount; i++) {
            if (i >= MAX_ITEMS) {
                logToFile("Error: Inventory index out of bounds.");
                break;
            }

            if (player->inventory[i].name != NULL && player->inventory[i].description != NULL) {
                if (strlen(player->inventory[i].name) > 0 && strlen(player->inventory[i].description) > 0) {
                    char logMessage[200];
                    snprintf(logMessage, sizeof(logMessage), "- %s: %s", player->inventory[i].name, player->inventory[i].description);
                    logToFile(logMessage);
                    printf("- %s: %s\n", player->inventory[i].name, player->inventory[i].description);
                } else {
                    logToFile("Error: Item has invalid name or description length.");
                    printf("Error: Invalid item at index %d\n", i);
                }
            } else {
                char errorLog[100];
                snprintf(errorLog, sizeof(errorLog), "Invalid item at index %d: Name or description is NULL.", i);
                logToFile(errorLog);
                printf("Error: Invalid item at index %d\n", i);
            }
        }
        typeWriterEffectLine("\n-----------------------------\n");
        typeWriterEffectLine("ACTION TO TAKE: ");
    }
}

void giveItemToPlayer(Player *player, const int itemId) {
    const Item *itemToGive = NULL;

    for (int i = 0; i < numItems; i++) {
        if (items[i].itemId == itemId) {
            itemToGive = &items[i];
            break;
        }
    }

    char message[256];

    if (itemToGive == NULL) {
        snprintf(message, sizeof(message), "The item with ID %d does not exist.\n", itemId);
        typeWriterEffectLine(message);
        logToFile("Attempted to give a nonexistent item to the player.");
        return;
    }

    for (int i = 0; i < player->inventoryCount; i++) {
        if (player->inventory[i].itemId == itemToGive->itemId) {
            typeWriterEffectLine("-----------------------------\n");
            snprintf(message, sizeof(message), "You already have the item: %s.\n", itemToGive->name);
            typeWriterEffectLine(message);
            typeWriterEffectLine("-----------------------------\n");
            typeWriterEffectLine("ACTION TO TAKE: ");
            logToFile("Player already has the item in inventory.");
            return;
        }
    }

    if (player->inventoryCount >= MAX_ITEMS) {
        typeWriterEffectLine("Your inventory is full. Cannot add more items.\n");
        logToFile("Player's inventory is full.");
        return;
    }

    player->inventory[player->inventoryCount] = *itemToGive;
    player->inventoryCount++;

    snprintf(message, sizeof(message), "Item added to inventory. Current inventory count: %d", player->inventoryCount);
    logToFile(message);

    typeWriterEffectLine("-----------------------------\n");
    snprintf(message, sizeof(message), "You have received the item: %s.", itemToGive->name);
    typeWriterEffect(message);
    typeWriterEffectLine("\n-----------------------------\n");

    if (!player->hasCrown) typeWriterEffectLine("ACTION TO TAKE: ");

    snprintf(message, sizeof(message), "Player received the item: %s. Inventory count: %d.",
             itemToGive->name, player->inventoryCount);
    logToFile(message);
}

void setPlayerRoom(const Player *player) {
    if (player->currentRoomId >= 0 && player->currentRoomId < MAX_ROOMS) {
        Room *room = rooms[player->currentRoomId];
        if (room == NULL) {
            logToFile("Error: Room pointer is NULL in setPlayerRoom.");
            typeWriterEffect("The room you were last in no longer exists...\nStarting from the beginning...\n");
            displayRoom(rooms[0]);
            return;
        }

        typeWriterEffect("You are back in the room where you last saved..\n");

        char logMessage[100];
        snprintf(logMessage, sizeof(logMessage), "Player is in the room with ID: %d, Room name: %s", player->currentRoomId, room->name);
        logToFile(logMessage);
    } else {
        typeWriterEffect("The room you were last in no longer exists...\nStarting from the beginning...\n");
        displayRoom(rooms[0]);

        logToFile("Player tried to enter an invalid room ID. Starting from the beginning.");
    }
}

void restorePlayerState() {
    char saveFile[150];
    snprintf(saveFile, sizeof(saveFile), "%s_save.dat", player.name);

    printf("Attempting to load save file: %s\n", saveFile);

    FILE *file = fopen(saveFile, "rb");
    if (!file) {
        perror("Failed to load player state");
        typeWriterEffectLine("No saved game found. Starting a new game.\n");

        char logMessage[256];
        snprintf(logMessage, sizeof(logMessage),
                 "Failed to load player state for player '%s': %s\n",
                 player.name, strerror(errno));
        logToFile(logMessage);
        return;
    }

    if (fread(&player, sizeof(Player), 1, file) != 1 ||
        fread(&player.inventoryCount, sizeof(int), 1, file) != 1) {
        logToFile("Error reading player state or inventory count.\n");
        fclose(file);
        return;
        }

    for (int i = 0; i < player.inventoryCount; i++) {
        if (fread(&player.inventory[i], sizeof(Item), 1, file) != 1) {
            logToFile("Error reading inventory item during restore.\n");
            fclose(file);
            return;
        }
    }

    fclose(file);

    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage),
             "Player state restored successfully for player '%s'. Player is now in room %d with %d items in inventory.\n",
             player.name, player.currentRoomId, player.inventoryCount);
    logToFile(logMessage);

    setPlayerRoom(&player);
}