#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "room.h"
#include "game.h"
#include "utils.h"

Room* rooms[MAX_ROOMS];
Room *currentRoom;

bool playing = true;
int npcCounter = 0;

void addNpcToRoom(Room *room, const char *name, const char *description, int isHostile, bool isDead) {
    if (room->npcCount >= MAX_NPC) {
        logToFile("Room NPC limit reached. Could not add NPC.");
        return;
    }

    if (isDead) {
        char logMessage[256];
        snprintf(logMessage, sizeof(logMessage), "NPC '%s' is dead. Not adding to room '%s'.", name, room->name);
        logToFile(logMessage);
        return;
    }

    NPC npcToAdd;
    npcToAdd.npcId = room->npcCount;
    snprintf(npcToAdd.name, sizeof(npcToAdd.name), "%s", name);
    snprintf(npcToAdd.description, sizeof(npcToAdd.description), "%s", description);
    npcToAdd.isHostile = isHostile;
    npcToAdd.isDead = isDead;

    room->npcs[room->npcCount] = npcToAdd;
    room->npcCount++;

    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "NPC '%s' added to room '%s'.", name, room->name);
    logToFile(logMessage);
}

Room* createRoom(const char *name, const char *description) {
    Room *room = malloc(sizeof(Room));
    if (!room) {
        fprintf(stderr, "Memory allocation failed for Room.\n");
        exit(EXIT_FAILURE);
    }

    room->roomId = -1;
    room->north = room->south = room->east = room->west = NULL;
    room->isInitialized = 0;

    if (name) {
        strncpy(room->name, name, sizeof(room->name) - 1);
        room->name[sizeof(room->name) - 1] = '\0';
    } else {
        strcpy(room->name, "Unknown Room");
    }

    if (description) {
        strncpy(room->description, description, sizeof(room->description) - 1);
        room->description[sizeof(room->description) - 1] = '\0';
    } else {
        strcpy(room->description, "No description available.");
    }

    room->itemCount = 0;
    room->npcCount = 0;

    for (int i = 0; i < MAX_NPC; i++) {
        memset(&room->npcs[i], 0, sizeof(NPC));
    }

    return room;
}

void connectRooms(Room *room1, Room *room2, const char *direction) {
    char logMessage[256];

    if (strcmp(direction, "north") == 0) {
        room1->north = room2;
        room2->south = room1;

        formatLogMessage(logMessage, sizeof(logMessage), "Connected room '%s' to room '%s' in the north direction.\n", room1->name, room2->name);
        logToFile(logMessage);

    } else if (strcmp(direction, "south") == 0) {
        room1->south = room2;
        room2->north = room1;

        formatLogMessage(logMessage, sizeof(logMessage), "Connected room '%s' to room '%s' in the south direction.\n", room1->name, room2->name);
        logToFile(logMessage);

    } else if (strcmp(direction, "east") == 0) {
        room1->east = room2;
        room2->west = room1;

        formatLogMessage(logMessage, sizeof(logMessage), "Connected room '%s' to room '%s' in the east direction.\n", room1->name, room2->name);
        logToFile(logMessage);

    } else if (strcmp(direction, "west") == 0) {
        room1->west = room2;
        room2->east = room1;

        formatLogMessage(logMessage, sizeof(logMessage), "Connected room '%s' to room '%s' in the west direction.\n", room1->name, room2->name);
        logToFile(logMessage);
    }
}

void freeRoom(Room *room) {
    if (room == NULL) {
        return;
    }

    char logMessage[256];
    formatLogMessage(logMessage, sizeof(logMessage), "Freeing memory for room '%s'.\n", room->name);
    logToFile(logMessage);

    free(room);
}

void freeAllRooms() {
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (rooms[i] != NULL) {
            char logMessage[256];
            formatLogMessage(logMessage, sizeof(logMessage), "Freeing memory for room '%s'.\n", rooms[i]->name);
            logToFile(logMessage);

            free(rooms[i]);
            rooms[i] = NULL;
        }
    }
}

void displayRoom(Room *room)

{
    if (room == NULL) {
        logToFile("Error: Null pointer passed to displayRoom.\n");
        typeWriterEffect("You are in an unknown area.\n");
        return;
    }

    char logMessage[512];
    snprintf(logMessage, sizeof(logMessage),
        "Displaying details for room '%s': %s\nExits: North: %p, South: %p, East: %p, West: %p\n",
        room->name[0] ? room->name : "Unnamed room",
        room->description[0] ? room->description : "No description available.",
        room->north, room->south, room->east, room->west);
    logToFile(logMessage);

    typeWriterEffectLine("\n---------------------------------------------------\n");
    char roomDetails[300];
    snprintf(roomDetails, sizeof(roomDetails),
        "Current Area: %s\nDescription: %s\n",
        room->name[0] ? room->name : "Unnamed room",
        room->description[0] ? room->description : "No description available.");
    typeWriterEffect(roomDetails);
    typeWriterEffectLine("---------------------------------------------------\n");

    displayItems(room);
    typeWriterEffectLine("-----------------------------\n");

    displayNpc(room);
    typeWriterEffectLine("-----------------------------\n");

    typeWriterEffect("Exits:\n");
    int hasExits = 0;
    if (room->north) {
        typeWriterEffectLine("> North\n");
        hasExits = 1;
    }
    if (room->south) {
        typeWriterEffectLine("> South\n");
        hasExits = 1;
    }
    if (room->east) {
        typeWriterEffectLine("> East\n");
        hasExits = 1;
    }
    if (room->west) {
        typeWriterEffectLine("> West\n");
        hasExits = 1;
    }

    if (!hasExits) {
        typeWriterEffect("No visible exits.\n");
    }
    typeWriterEffectLine("-----------------------------\n");
}

bool isTreasureRoomTrapped() {
    srand((unsigned int)time(NULL) ^ rand());
    bool isTrapped = rand() % 2 == 0;

    char logMessage[100];
    snprintf(logMessage, sizeof(logMessage),
        "Treasure room trap check: %s", isTrapped ? "Trapped" : "Safe");
    logToFile(logMessage);

    return isTrapped;
}

void gameOver(void);

Room* move(Room *currentRoom, Player *player, char *input) {
    Room *nextRoom = currentRoom;
    bool activeCommands = true;

    while (activeCommands) {
        fgets(input, 20, stdin);
        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) { continue; }

        if (strcmp(input, "quit") == 0) {
            playing = false;
            activeCommands = false;
            typeWriterEffectLine("\n-----------------------------\n");
            typeWriterEffect("Thank you for playing!\nGoodbye!\n");
            logToFile("Player chose to quit the game.");
        } else if (strcmp(input, "save") == 0) {
            char saveFile[150];
            snprintf(saveFile, sizeof(saveFile), "%s_save.dat", player->name);
            saveGame(saveFile);
            logToFile("Game saved successfully.");
        } else if (strcmp(input, "info") == 0) {
            logToFile("Player requested information about the current room.");
            return nextRoom;
        } else if (strcmp(input, "inv") == 0) {
            displayInventory(player);
            logToFile("Player viewed their inventory.");
        } else if (strcmp(input, "int") == 0) {
            interactWithNpcById(currentRoom, player->inventory, &player->inventoryCount, player->name);
            logToFile("Player interacted with an NPC.");
        } else if (strcmp(input, "pick") == 0) {
            pickUpItemById(currentRoom, player->inventory, &player->inventoryCount);
            logToFile("Player picked up an item.");
        } else {
            Room *targetRoom = NULL;
            const char *direction = NULL;

            if (strcmp(input, "north") == 0 || strcmp(input, "n") == 0) {
                targetRoom = currentRoom->north;
                direction = "north";
            } else if (strcmp(input, "south") == 0 || strcmp(input, "s") == 0) {
                targetRoom = currentRoom->south;
                direction = "south";
            } else if (strcmp(input, "east") == 0 || strcmp(input, "e") == 0) {
                targetRoom = currentRoom->east;
                direction = "east";
            } else if (strcmp(input, "west") == 0 || strcmp(input, "w") == 0) {
                targetRoom = currentRoom->west;
                direction = "west";
            }

            if (targetRoom) {
                activeCommands = false;
                nextRoom = targetRoom;

                player->currentRoomId = nextRoom->roomId;

                typeWriterEffectLine("\n-----------------------------\n");
                char message[50];
                snprintf(message, sizeof(message), "You walk to the %s.\n", direction);
                typeWriterEffect(message);
                typeWriterEffectLine("-----------------------------\n");

                char logMessage[100];
                snprintf(logMessage, sizeof(logMessage), "Player moved to a new room: %s", direction);
                logToFile(logMessage);
                snprintf(logMessage, sizeof(logMessage), "Player's current room ID updated: %d", player->currentRoomId);
                logToFile(logMessage);
            } else {
                typeWriterEffectLine("\n-----------------------------\n");
                typeWriterEffect("You can't go that way! Try a different direction.\n");
                typeWriterEffectLine("-----------------------------\n");
                typeWriterEffectLine("ACTION TO TAKE: ");

                char logMessage[100];
                snprintf(logMessage, sizeof(logMessage), "Player tried to move in an invalid direction: %s", input);
                logToFile(logMessage);
            }
        }
        if (!player->treasureRoomChecked){
            if (nextRoom && strcmp(nextRoom->name, "Treasure Room") == 0) {
                typeWriterEffectLine("\n---------------------------------------------------\n");
                typeWriterEffect("You enter the Treasure Room...\n");
                logToFile("Player entered the Treasure Room.");
                if (isTreasureRoomTrapped()) {
                    typeWriterEffect("Oh no! The room was trapped!\nA deadly mechanism activates, killing you instantly...");
                    typeWriterEffectLine("\n---------------------------------------------------\n");
                    logToFile("The Treasure Room was trapped, and the player died.");
                    gameOver();
                    return NULL;
                }
                typeWriterEffect("The room is quiet.\nYou seem to be safe for now.");
                typeWriterEffectLine("\n---------------------------------------------------\n");
                logToFile("Player found the Treasure Room and survived.");
                player->treasureRoomChecked = 1;
            }
        }
    }
    return nextRoom;
}

void initializeRooms() {
    char logMessage[256];

    Room *entrance = createRoom("Entrance", "A spooky castle gate shrouded in mist.");
    Room *hall = createRoom("Hall", "A grand hall with tall ceilings and a chandelier.");
    Room *kitchen = createRoom("Kitchen", "A dimly lit kitchen with old utensils.");
    Room *library = createRoom("Library", "A quiet library filled with dusty books.");
    Room *bedroom = createRoom("Bedroom", "A dark room with a large, torn canopy bed.");
    Room *throneRoom = createRoom("Throne Room", "A majestic hall with a golden throne.");
    Room *treasureRoom = createRoom("Treasure Room", "A sparkling chamber filled with riches.");

    Room *allRooms[] = {entrance, hall, kitchen, library, bedroom, throneRoom, treasureRoom};

    for (int i = 0; i < sizeof(allRooms) / sizeof(allRooms[0]); i++) {
        if (!rooms[i]) {
            rooms[i] = allRooms[i];
            rooms[i]->roomId = i;
        }
        rooms[i]->isInitialized = 1;
    }

    formatLogMessage(logMessage, sizeof(logMessage), "Rooms created and initialized.");
    logToFile(logMessage);

    connectRooms(entrance, hall, "north");
    connectRooms(hall, kitchen, "west");
    connectRooms(hall, library, "east");
    connectRooms(hall, bedroom, "north");
    connectRooms(bedroom, throneRoom, "north");
    connectRooms(throneRoom, treasureRoom, "east");

    formatLogMessage(logMessage, sizeof(logMessage), "Room connections established.");
    logToFile(logMessage);

    addItemToRoom(bedroom, "Duck");
    addItemToRoom(bedroom, "Plushie");
    addItemToRoom(library, "Book");
    addItemToRoom(kitchen, "Frying pan");
    addItemToRoom(treasureRoom, "Sack of Gold");
    addItemToRoom(treasureRoom, "Golden Trophy");

    formatLogMessage(logMessage, sizeof(logMessage), "Items added to specific rooms.");
    logToFile(logMessage);

    addNpcToRoom(entrance, "Guard", "A stern-looking guard stands by the gate.", 0, 0);
    addNpcToRoom(kitchen, "Ghost", "A pale ghost floats silently in the air.", 1, 0);
    addNpcToRoom(library, "Librarian", "A friendly librarian with a stack of books.", 0, 0);
    addNpcToRoom(throneRoom, "Queen of Hell", "A powerful and beautiful woman with a strong desire to rule over everything, both the living and the dead.", 1, 0);

    formatLogMessage(logMessage, sizeof(logMessage), "NPCs added to specific rooms.");
    logToFile(logMessage);

    currentRoom = entrance;
}

void gameLoop() {
    Room *currentRoom = rooms[player.currentRoomId];

    while (true) {
        char input[20];
        if (playing == false) break;

        displayRoom(currentRoom);
        typeWriterEffect("What do you want to do?\n");
        typeWriterEffectLine("(type 'n' - go North, 'e' - go East, 's' - go South, 'w' - go West)");
        typeWriterEffectLine("(type 'pick' to pick up an item)");
        typeWriterEffectLine("(type 'inv' to see your inventory)");
        typeWriterEffectLine("(type 'int' to interact with an NPC)");
        typeWriterEffectLine("(type 'info' to look around again)");
        typeWriterEffectLine("(type 'save' to save the game)");
        typeWriterEffectLine("(type 'quit' to exit)\n");
        typeWriterEffectLine("-----------------------------\n");
        typeWriterEffectLine("ACTION TO TAKE: ");

        currentRoom = move(currentRoom, &player, input);
    }

    logToFile("Game over, freeing all rooms.");
    freeAllRooms();
}