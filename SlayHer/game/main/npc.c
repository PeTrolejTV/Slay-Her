#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "room.h"
#include "utils.h"

void logMessage(const char *message) {
    FILE *logFile = fopen("log.txt", "a");
    if (logFile == NULL) {
        perror("Error opening log file");
        return;
    }

    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm);

    fprintf(logFile, "[%s] %s\n", timeStr, message);
    fclose(logFile);
}

void displayNpc(Room *room) {
    bool hasAliveNpcs = false;

    for (int i = 0; i < room->npcCount; i++) {
        if (!room->npcs[i].isDead) {
            hasAliveNpcs = true;
            break;
        }
    }

    if (room->npcCount == 0 || !hasAliveNpcs) {
        typeWriterEffect("No NPCs in this area.\n");
        logMessage("No NPCs in the area.");
    } else {
        typeWriterEffect("NPCs in this area:\n");
        logMessage("Displaying NPCs in the area.");
        for (int i = 0; i < room->npcCount; i++) {
            if (room->npcs[i].isDead) {
                char logMessage[256];
                snprintf(logMessage, sizeof(logMessage), "NPC '%s' is dead and will not be displayed.", room->npcs[i].name);
                logToFile(logMessage);
                continue;
            }

            char message[256];
            snprintf(message, sizeof(message), "- %s: %s (%s)\n",
                     room->npcs[i].name,
                     room->npcs[i].description,
                     room->npcs[i].isHostile ? "Hostile" : "Friendly");
            typeWriterEffectLine(message);
            logToFile(message);
        }
    }
}

void gameOver() {
    typeWriterEffectLine("\n-----------------------\n");
    typeWriterEffect("GAME OVER!");
    typeWriterEffect("Better luck next time!");
    typeWriterEffectLine("\n-----------------------\n");
    logMessage("Game Over - Player lost.");
    getchar();
    exit(0);
}

int rngSuccess() {
    srand((unsigned int)time(NULL) ^ rand());
    return rand() % 2 == 0;
}

void handleCombatOptions(NPC *npc, const Item *inventory, const int *inventoryCount, const char *playerName, Player player) {
    while (1) {
        typeWriterEffect("Choose your action:\n");
        typeWriterEffectLine("1. Use an item");
        typeWriterEffectLine("2. Attempt to flee");
        typeWriterEffectLine("\n-----------------------------\n");
        typeWriterEffectLine("ENTER YOUR CHOICE: ");
        logMessage("Player is choosing an action.");

        char input[10];
        int choice = -1;
        char *endPtr;

        while (choice != 1 && choice != 2) {
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0';
            if (strlen(input) != 0) {
                if (input[0] == '\0') {
                    typeWriterEffectLine("Invalid input. Please enter '1' or '2'.");
                    logMessage("Invalid input detected.");
                    continue;
                }

                choice = strtol(input, &endPtr, 10);

                if (*endPtr != '\0' || (choice != 1 && choice != 2)) {
                    typeWriterEffectLine("Invalid choice. Please enter a valid number (1 or 2).");
                    logMessage("Invalid choice entered.");
                    choice = -1;
                }
            }
        }

        if (choice == 1) {
            typeWriterEffectLine("\n-----------------------------\n");
            typeWriterEffect("Your inventory contains:\n");
            logMessage("Player chose to use an item.");
            for (int i = 0; i < *inventoryCount; i++) {
                char message[256];
                snprintf(message, sizeof(message), "%d: %s - %s", i + 1, inventory[i].name, inventory[i].description);
                typeWriterEffectLine(message);
            }

            int itemChoice = -1;
            typeWriterEffectLine("\n-----------------------------\n");
            typeWriterEffectLine("ENTER THE NUMBER OF THE ITEM TO USE: ");
            while (itemChoice < 1 || itemChoice > *inventoryCount) {
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';

                if (input[0] == '\0') {
                    typeWriterEffectLine("Invalid input. Please enter a valid item number.");
                    logMessage("Invalid input while choosing an item.");
                    continue;
                }

                itemChoice = strtol(input, &endPtr, 10);

                if (*endPtr != '\0' || itemChoice < 1 || itemChoice > *inventoryCount) {
                    typeWriterEffectLine("Invalid item choice. Please enter a valid number.");
                    logMessage("Invalid item choice entered.");
                    itemChoice = -1;
                }
            }

            Item selectedItem = inventory[itemChoice - 1];
            logMessage("Player selected an item.");
            if (strCaseInsensitiveCompare(selectedItem.name, "Legendary Sword") == 0) {
                typeWriterEffectLine("\n-----------------------------\n");
                typeWriterEffect("You wield the Legendary Sword and strike at the Queen of Hell!");
                logMessage("Player used the Legendary Sword.");
                typeWriterEffect("Your attack is successful, and she falls to the ground, defeated.");
                typeWriterEffect("The Queen of Hell drops her Crown.\n");

                player.hasCrown = 1;
                giveItemToPlayer(&player,11);

                logMessage("Player defeated the Queen of Hell and obtained the Crown.");
                typeWriterEffect("You have saved the world!\n");
                char message[256];
                snprintf(message, sizeof(message), "Victory! %s is hailed as the savior of the world!", playerName);
                typeWriterEffect(message);
                typeWriterEffectLine("\n-----------------------------\n");
                logMessage("Game ended with a victory.");
                getchar();
                exit(0);
            }
            if (strCaseInsensitiveCompare(selectedItem.name, "Magical Ring") == 0) {
                typeWriterEffectLine("\n-----------------------------\n");
                typeWriterEffect("The Queen of Hell notices your Magical Ring.");
                logMessage("Player used the Magical Ring.");
                typeWriterEffect("She offers you a chance to rule the world by her side.\n");
                typeWriterEffect("Do you accept her offer?\n");
                typeWriterEffectLine("1. Yes, accept the offer");
                typeWriterEffectLine("2. No, refuse the offer and prepare for battle");
                typeWriterEffectLine("\n-----------------------------\n");
                typeWriterEffectLine("ENTER YOUR CHOICE: ");

                int response = -1;
                while (response != 1 && response != 2) {
                    fgets(input, sizeof(input), stdin);
                    input[strcspn(input, "\n")] = '\0';

                    if (input[0] == '\0') {
                        typeWriterEffectLine("Please enter a valid choice (1 or 2).");
                        logMessage("Invalid input while deciding on the Queen's offer.");
                        continue;
                    }

                    response = strtol(input, &endPtr, 10);

                    if (*endPtr != '\0' || (response != 1 && response != 2)) {
                        typeWriterEffectLine("Invalid input. Please enter a valid choice (1 or 2).");
                        logMessage("Invalid choice while deciding on the Queen's offer.");
                        response = -1;
                    }
                }

                if (response == 1) {
                    typeWriterEffectLine("\n-----------------------------\n");
                    typeWriterEffect("You accept the offer and marry the Queen of Hell.");
                    logMessage("Player accepted the Queen's offer and chose to rule the world with her.");
                    char message[256];
                    snprintf(message, sizeof(message), "Together, %s and the Queen rule the world with power and grace.\n", playerName);
                    typeWriterEffect(message);
                    typeWriterEffect("Victory! You have achieved a different ending!");
                    typeWriterEffectLine("\n-----------------------------\n");
                    logMessage("Game ended with the alternate ending (ruling the world).");
                    getchar();
                    exit(0);
                }
                typeWriterEffectLine("\n-----------------------------\n");
                typeWriterEffect("You refuse her offer and prepare for battle.");
                typeWriterEffectLine("\n-----------------------------");
                logMessage("Player refused the Queen's offer.");
            } else if (strCaseInsensitiveCompare(selectedItem.name, "Sword") == 0) {
                char message[256];
                typeWriterEffectLine("\n-----------------------------\n");
                snprintf(message, sizeof(message), "You attack %s with the Sword, but it proves ineffective.\nPerhaps a more powerful weapon is needed.\n%s strikes you down...", npc->name, npc->name);
                typeWriterEffect(message);
                logMessage("Player attacked with the Sword but failed and was defeated.");
                gameOver();
            } else {
                typeWriterEffectLine("\n-----------------------------\n");
                typeWriterEffect("This item cannot help in combat.");
                typeWriterEffectLine("\n-----------------------------\n");
                logMessage("Player used an ineffective item.");
            }
        }

        if (choice == 2) {
            typeWriterEffectLine("\n-----------------------------\n");
            typeWriterEffect("You try to flee...");
            logMessage("Player attempted to flee.");
            if (rngSuccess()) {
                typeWriterEffect("You successfully escaped!");
                typeWriterEffectLine("\n-----------------------------\n");
                typeWriterEffectLine("ACTION TO TAKE: ");
                logMessage("Player successfully fled.");
                return;
            }
            typeWriterEffect("You failed to flee.\nThe enemy kills you...");
            typeWriterEffectLine("\n-----------------------------\n");
            logMessage("Player failed to flee and was defeated.");
            gameOver();
        }
    }
}

void interactWithQueenOfHell(NPC *npc, const Item *inventory, const int *inventoryCount, const char *playerName, const Player player) {
    typeWriterEffectLine("\n-----------------------------\n");
    typeWriterEffect("You encounter the Queen of Hell!\n");
    logMessage("Player encountered the Queen of Hell.");

    char message[256];
    snprintf(message, sizeof(message), "%s: %s\n", npc->name, npc->description);
    typeWriterEffect(message);
    typeWriterEffectLine("-----------------------------\n");
    logMessage("Queen of Hell description displayed.");

    int hasCombatItems = 0;
    logMessage("Checking player's inventory for combat items.");
    for (int i = 0; i < *inventoryCount; i++) {
        if (strCaseInsensitiveCompare(inventory[i].name, "Sword") == 0 ||
            strCaseInsensitiveCompare(inventory[i].name, "Legendary Sword") == 0 ||
            strCaseInsensitiveCompare(inventory[i].name, "Magical Ring") == 0) {
            hasCombatItems = 1;
            logMessage("Player has at least one combat item.");
            break;
            }
    }

    if (!hasCombatItems) {
        typeWriterEffect("You have no items to defend yourself.\nThe Queen of Hell kills you...");
        typeWriterEffectLine("\n-----------------------------\n");
        logMessage("Player has no combat items and was defeated by the Queen of Hell.");
        gameOver();
    } else {
        logMessage("Player has combat items. Initiating combat options.");
        handleCombatOptions(npc, inventory, inventoryCount, playerName, player);
    }
}

void interactWithNpcById(Room *room, const Item *inventory, const int *inventoryCount, const char *playerName) {
        int allDead = 1;
        for (int i = 0; i < room->npcCount; i++) {
            if (!room->npcs[i].isDead) {
                allDead = 0;
                break;
            }
        }

        if (room->npcCount == 0 || allDead) {
        typeWriterEffectLine("\n-----------------------------\n");
        typeWriterEffect("There are no NPCs in this Area.\n");
        typeWriterEffectLine("-----------------------------\n");
        typeWriterEffectLine("ACTION TO TAKE: ");
        logMessage("No NPCs are present in the current area.");
        return;
    }

    logMessage("Listing NPCs in the area.");
    char message[256];
    typeWriterEffectLine("\n-----------------------------\n");
    typeWriterEffect("NPCs in this Area:\n");
    for (int i = 0; i < room->npcCount; i++) {
        if (!room->npcs[i].isDead) {
            snprintf(message, sizeof(message), "%d. %s\n", i + 1, room->npcs[i].name);
            typeWriterEffectLine(message);
        }
    }

    typeWriterEffectLine("-----------------------------\n");
    typeWriterEffectLine("NUMBER OF THE NPC TO INTERACT WITH: ");
    logMessage("Waiting for player to select an NPC.");
    int npcId;
    char input[10];

    while (1) {
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) != 0) {
            char *endPtr;
            npcId = strtol(input, &endPtr, 10);

            if (*endPtr != '\0' || npcId < 1 || npcId > room->npcCount) {
                typeWriterEffectLine("Invalid NPC number. Please try again.");
                logMessage("Invalid NPC ID entered.");
                continue;
            }

            if (room->npcs[npcId - 1].isDead) {
                typeWriterEffectLine("You can't interact with that NPC, they are no longer alive.\n");
                logMessage("Attempted interaction with a dead NPC.");
                continue;
            }
            break;
        }
    }

    logMessage("Player selected an NPC.");
    NPC *npc = &room->npcs[npcId - 1];

    if (npc->isDead) {
        typeWriterEffectLine("This NPC is no longer alive.\n");
        logMessage("Player attempted interaction with a dead NPC.");
        return;
    }

    if (strcmp(npc->name, "Queen of Hell") == 0) {
        logMessage("Player interacting with the Queen of Hell.");
        interactWithQueenOfHell(npc, inventory, inventoryCount, playerName, player);
        return;
    }

    if (npc->isHostile) {
        typeWriterEffectLine("\n-----------------------------\n");
        snprintf(message, sizeof(message), "You encounter a hostile %s!\n", npc->name);
        typeWriterEffect(message);
        logMessage("Player encountered a hostile NPC.");

        snprintf(message, sizeof(message), "%s: %s\n", npc->name, npc->description);
        typeWriterEffect(message);

        logMessage("Checking player's inventory for a sword.");
        int hasSword = 0;
        for (int j = 0; j < *inventoryCount; j++) {
            if (strCaseInsensitiveCompare(inventory[j].name, "Sword") == 0) {
                hasSword = 1;
                logMessage("Player has a sword for combat.");
                break;
            }
        }

        if (hasSword) {
            typeWriterEffectLine("-----------------------------\n");
            snprintf(message, sizeof(message), "You slay the %s with your sword!", npc->name);
            typeWriterEffect(message);
            logMessage("Player defeated the hostile NPC.");
            npc->isDead = 1; // Mark NPC as dead

            if (strcmp(npc->name, "Ghost") == 0) {
                snprintf(message, sizeof(message), "The %s drops a Legendary Sword!\n", npc->name);
                typeWriterEffect(message);
                logMessage("Player obtained a Legendary Sword from the Ghost.");
                giveItemToPlayer(&player, 7);
            }
        } else {
            typeWriterEffectLine("-----------------------------\n");
            typeWriterEffect("You don't have a weapon...");
            snprintf(message, sizeof(message), "The %s kills you...", npc->name);
            typeWriterEffect(message);
            typeWriterEffectLine("\n-----------------------------\n");
            logMessage("Player was killed by the hostile NPC due to lack of a weapon.");
            gameOver();
        }
    } else {
        typeWriterEffectLine("\n-----------------------------\n");
        snprintf(message, sizeof(message), "You meet a friendly %s.\n", npc->name);
        typeWriterEffect(message);
        logMessage("Player encountered a friendly NPC.");

        snprintf(message, sizeof(message), "%s: %s\n", npc->name, npc->description);
        typeWriterEffect(message);

        logMessage("Determining the gift to be given by the NPC.");
        if (strcmp(npc->name, "Guard") == 0) {
            giveItemToPlayer(&player, 8);
        } else if (strcmp(npc->name, "Librarian") == 0) {
            giveItemToPlayer(&player, 9);
        } else {
            giveItemToPlayer(&player, 10);
        }
    }
}