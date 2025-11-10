#include "game.h"
#include "player.h"
#include "room.h"
#include "item.h"
#include "npc.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int loadGameSuccess = 0;

// ---------------- Helper ----------------
static Item *findItemById(int id) {
    for (int i = 0; i < numItems; ++i) {
        if (items[i].itemId == id)
            return &items[i];
    }
    return NULL;
}

// ---------------- SAVE GAME ----------------
void saveGame(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening save file");
        logToFile("Error opening save file for saving game.\n");
        typeWriterEffectLine("Failed to save the game. Please try again.\n");
        return;
    }

    // --- Save player basic info ---
    fwrite(&player.currentRoomId, sizeof(int), 1, file);
    fwrite(&player.inventoryCount, sizeof(int), 1, file);

    // Save new player variables
    fwrite(&player.hasCrown, sizeof(int), 1, file);
    fwrite(&player.treasureRoomChecked, sizeof(int), 1, file);

    // Save only item IDs for inventory
    for (int i = 0; i < player.inventoryCount; ++i) {
        int id = player.inventory[i].itemId;
        fwrite(&id, sizeof(int), 1, file);
    }

    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage),
             "Saved player's current room ID: %d, inventory count: %d, hasCrown: %d, treasureChecked: %d.\n",
             player.currentRoomId, player.inventoryCount, player.hasCrown, player.treasureRoomChecked);
    logToFile(logMessage);

    // --- Save all rooms ---
    for (int i = 0; i < MAX_ROOMS; ++i) {
        int exists = (rooms[i] && rooms[i]->isInitialized) ? 1 : 0;
        fwrite(&exists, sizeof(int), 1, file);

        if (!exists) continue;

        Room *r = rooms[i];

        // Basic info
        fwrite(&r->roomId, sizeof(int), 1, file);
        fwrite(r->name, sizeof(r->name), 1, file);
        fwrite(r->description, sizeof(r->description), 1, file);

        // Items (save only itemId + pickedUp)
        fwrite(&r->itemCount, sizeof(int), 1, file);
        for (int j = 0; j < r->itemCount; ++j) {
            int id = r->items[j].itemId;
            int picked = r->items[j].pickedUp;
            fwrite(&id, sizeof(int), 1, file);
            fwrite(&picked, sizeof(int), 1, file);
        }

        // NPCs (safe to write directly)
        fwrite(&r->npcCount, sizeof(int), 1, file);
        fwrite(r->npcs, sizeof(NPC), r->npcCount, file);

        // Save neighbor roomIds (or -1)
        int northId = r->north ? r->north->roomId : -1;
        int southId = r->south ? r->south->roomId : -1;
        int eastId  = r->east  ? r->east->roomId  : -1;
        int westId  = r->west  ? r->west->roomId  : -1;

        fwrite(&northId, sizeof(int), 1, file);
        fwrite(&southId, sizeof(int), 1, file);
        fwrite(&eastId,  sizeof(int), 1, file);
        fwrite(&westId,  sizeof(int), 1, file);

        snprintf(logMessage, sizeof(logMessage),
                 "Saved room %d. Items: %d, NPCs: %d, Neighbors N:%d S:%d E:%d W:%d\n",
                 r->roomId, r->itemCount, r->npcCount,
                 northId, southId, eastId, westId);
        logToFile(logMessage);
    }

    fclose(file);
    typeWriterEffectLine("\n-----------------------------\n");
    typeWriterEffect("Game saved successfully!\n");
    typeWriterEffectLine("-----------------------------\n");
    typeWriterEffectLine("ACTION TO TAKE: ");
    logToFile("Game saved successfully.\n");
}

// ---------------- LOAD GAME ----------------
void loadGame(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening save file");
        logToFile("Error opening save file for loading game.\n");
        typeWriterEffectLine("Failed to load the game. No save file found.\n");
        return;
    }

    int neighborIds[MAX_ROOMS][4];
    memset(neighborIds, -1, sizeof(neighborIds));

    // --- Player basic info ---
    if (fread(&player.currentRoomId, sizeof(int), 1, file) != 1) {
        logToFile("Error reading player currentRoomId.\n");
        fclose(file);
        return;
    }

    if (fread(&player.inventoryCount, sizeof(int), 1, file) != 1) {
        logToFile("Error reading player inventoryCount.\n");
        fclose(file);
        return;
    }

    // Load new player variables
    if (fread(&player.hasCrown, sizeof(int), 1, file) != 1) {
        player.hasCrown = 0;
    }
    if (fread(&player.treasureRoomChecked, sizeof(int), 1, file) != 1) {
        player.treasureRoomChecked = 0;
    }

    for (int i = 0; i < player.inventoryCount; ++i) {
        int id;
        fread(&id, sizeof(int), 1, file);
        Item *it = findItemById(id);
        if (it)
            player.inventory[i] = *it;
        else
            memset(&player.inventory[i], 0, sizeof(Item));
    }

    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage),
             "Loaded player's current room ID: %d, inventory count: %d, hasCrown: %d, treasureChecked: %d.\n",
             player.currentRoomId, player.inventoryCount, player.hasCrown, player.treasureRoomChecked);
    logToFile(logMessage);

    // --- Load rooms ---
    for (int i = 0; i < MAX_ROOMS; ++i) {
        int exists;
        if (fread(&exists, sizeof(int), 1, file) != 1)
            break;

        if (!exists) {
            if (rooms[i]) {
                free(rooms[i]);
                rooms[i] = NULL;
            }
            continue;
        }

        if (!rooms[i]) {
            rooms[i] = malloc(sizeof(Room));
            if (!rooms[i]) {
                fprintf(stderr, "Memory allocation failed for room %d.\n", i);
                logToFile("Memory allocation failed for room.\n");
                fclose(file);
                return;
            }
        }

        Room *r = rooms[i];
        memset(r, 0, sizeof(Room));
        r->isInitialized = 1;

        fread(&r->roomId, sizeof(int), 1, file);
        fread(r->name, sizeof(r->name), 1, file);
        fread(r->description, sizeof(r->description), 1, file);

        fread(&r->itemCount, sizeof(int), 1, file);
        if (r->itemCount > MAX_ITEMS) r->itemCount = MAX_ITEMS;

        for (int j = 0; j < r->itemCount; ++j) {
            int id, picked;
            fread(&id, sizeof(int), 1, file);
            fread(&picked, sizeof(int), 1, file);
            Item *it = findItemById(id);
            if (it) {
                r->items[j] = *it;
                r->items[j].pickedUp = picked;
            } else {
                memset(&r->items[j], 0, sizeof(Item));
                r->items[j].itemId = id;
                r->items[j].pickedUp = picked;
            }
        }

        fread(&r->npcCount, sizeof(int), 1, file);
        if (r->npcCount > MAX_NPC) r->npcCount = MAX_NPC;
        fread(r->npcs, sizeof(NPC), r->npcCount, file);

        fread(&neighborIds[i][0], sizeof(int), 1, file);
        fread(&neighborIds[i][1], sizeof(int), 1, file);
        fread(&neighborIds[i][2], sizeof(int), 1, file);
        fread(&neighborIds[i][3], sizeof(int), 1, file);

        snprintf(logMessage, sizeof(logMessage),
                 "Loaded room %d. Items: %d, NPCs: %d.\n",
                 r->roomId, r->itemCount, r->npcCount);
        logToFile(logMessage);
    }

    // --- Rebuild neighbor pointers ---
    for (int i = 0; i < MAX_ROOMS; ++i) {
        if (!rooms[i]) continue;

        int n = neighborIds[i][0];
        int s = neighborIds[i][1];
        int e = neighborIds[i][2];
        int w = neighborIds[i][3];

        rooms[i]->north = (n >= 0 && n < MAX_ROOMS) ? rooms[n] : NULL;
        rooms[i]->south = (s >= 0 && s < MAX_ROOMS) ? rooms[s] : NULL;
        rooms[i]->east  = (e >= 0 && e < MAX_ROOMS) ? rooms[e] : NULL;
        rooms[i]->west  = (w >= 0 && w < MAX_ROOMS) ? rooms[w] : NULL;
    }

    fclose(file);
    loadGameSuccess = 1;
    typeWriterEffectLine("Game loaded successfully!");
    typeWriterEffectLine("\n-----------------------------\n");
    logToFile("Game loaded successfully.\n");

    setPlayerRoom(&player);
}
