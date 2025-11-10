#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "room.h"
#include "player.h"
#include "utils.h"

Player player;

void setName() {
    strncpy(player.name, "", sizeof(player.name));
    player.inventoryCount = 0;

    typeWriterEffectLine("Enter your name, adventurer: ");
    logToFile("Prompting user for name.");

    char input[100];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    if (strlen(input) == 0) {
        strncpy(player.name, "Adventurer", sizeof(player.name));
        logToFile("User entered no name. Defaulting to 'Adventurer'.");
    } else {
        strncpy(player.name, input, sizeof(player.name));
        logToFile("User entered name: ");
        logToFile(player.name);
    }

    typeWriterEffectLine("----------------------------");
    char welcomeMessage[100];
    snprintf(welcomeMessage, sizeof(welcomeMessage), "\nWelcome, %s!", player.name);
    typeWriterEffect(welcomeMessage);
}

int promptLoadGame() {
    char input[3];
    char *endPtr;
    int response = -1;

    typeWriterEffect("A save file for your character exists.\n");
    typeWriterEffect("Do you want to load it?\n");
    logToFile("Prompting user to load a save file.");

    typeWriterEffectLine("1. Yes");
    typeWriterEffectLine("2. No, start a new game.");
    typeWriterEffectLine("\n-----------------------------\n");
    typeWriterEffectLine("ACTION TO TAKE: ");

    while (response != 1 && response != 2) {
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) {continue;}

        if (input[0] == '\0') {
            typeWriterEffectLine("Please enter a valid choice (1 or 2).");
            continue;
        }

        response = strtol(input, &endPtr, 10);

        if (*endPtr != '\0' || (response != 1 && response != 2)) {
            typeWriterEffectLine("Invalid input. Please enter a valid choice (1 or 2).");
            response = -1;
        }
    }

    logToFile("User choice for loading save file: ");
    char responseMsg[50];
    snprintf(responseMsg, sizeof(responseMsg), "%d", response);
    logToFile(responseMsg);

    return response == 1;
}

void introduction(Player *player) {
    typeWriterEffectLine("--------------------------------------------");
    typeWriterEffect("Welcome to the Text Adventure game Slay Her!");
    typeWriterEffectLine("--------------------------------------------\n");
    logToFile("Welcoming the player.");

    if (strlen(player->name) == 0) {
        setName();
    }

    char saveFile[150];
    snprintf(saveFile, sizeof(saveFile), "%s_save.dat", player->name);

    FILE *file = fopen(saveFile, "rb");
    if (file) {
        fclose(file);
        if (promptLoadGame()) {
            typeWriterEffectLine("\n-----------------------------\n");
            typeWriterEffectLine("Attempting to load your saved game...");
            logToFile("Attempting to load saved game.");

            loadGame(saveFile);
            if (loadGameSuccess) {
                char welcomeBackMessage[100];
                snprintf(welcomeBackMessage, sizeof(welcomeBackMessage), "Welcome back, %s!", player->name);
                typeWriterEffect(welcomeBackMessage);
                typeWriterEffect("Continuing your journey...");
                typeWriterEffectLine("\n-----------------------------\n");
                logToFile("Loaded saved game successfully.");
                return;
            }
            typeWriterEffectLine("\n-----------------------------\n");
            typeWriterEffectLine("Failed to load save file.\nStarting a new adventure...");
            typeWriterEffectLine("\n-----------------------------\n");
            logToFile("Failed to load saved game.");
        } else {
            typeWriterEffectLine("\n-----------------------------\n");
            typeWriterEffect("Starting a new adventure...");
            typeWriterEffectLine("\n-----------------------------\n");
            logToFile("User chose to start a new game.");
        }
    } else {
        logToFile("No previous save file found.");
    }

    logToFile("Starting introduction.");
    typeWriterEffect("You can call me \"The Narrator\".");
    typeWriterEffect("I will be guiding you through this journey.");
    typeWriterEffect("Now then, let us begin where your story starts...");
    typeWriterEffectLine("-------------------------------------------------\n");
    logToFile("Game introduction begins.");
    typeWriterEffect("You find yourself approaching a spooky-looking castle, shrouded in fog...");
    typeWriterEffect("You see the gates of an ancient castle, its shadow stretching across the desolate land.");
    typeWriterEffect("Legends speak of the Queen of Hell, a fearsome ruler who dwells deep within these halls.");
    typeWriterEffect("She commands both the living and the dead, her power unmatched.");
    typeWriterEffect("You must venture through the castle, facing trials that will test your courage and wits.");
    typeWriterEffect("Only by confronting the Queen can this dark reign come to an end.");
    typeWriterEffect("But be warned, strength alone may not secure your victory.");
    typeWriterEffect("Some whispers hint that the Queen's wrath masks something deeper, something that can only be uncovered by those with an open mind.");
    typeWriterEffect("Prepare yourself, for the path ahead is fraught with danger, but perhaps not all is as it seems...");
    typeWriterEffectLine("--------------------------------------------------------------------------------------------------\n");
    logToFile("Introduction narrative finished.");
}

int main(const int argc, char *argv[]) {
    logToFile("\nGAME STARTED\n");

    if (argc > 1) {
        strncpy(player.name, argv[1], sizeof(player.name));
        logToFile("Player name set via command-line argument: ");
        logToFile(player.name);
    } else {
        player.name[0] = '\0';
    }

    if (loadGameSuccess) {
        typeWriterEffect("Continuing from your last saved game...\n");
    } else {
        introduction(&player);
        initializeRooms();
    }

    gameLoop();
    logToFile("\nGAME ENDED\n");
    return 0;
}