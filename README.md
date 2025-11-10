# **Slay Her - Game Design Document - EN**


Venture into a dark, mystical castle to face the Queen of Hell! Explore hidden chambers, treacherous halls, and secret passages. Gather powerful artifacts, make fateful choices, and determine your own ending—will you defeat her... or join her dark empire? A thrilling text adventure of danger, strategy, and secrets awaits!

**Author**: Peter Majerik

---

## Introduction

This project brings back the spirit of classic text-based RPGs, where imagination paints the world and every decision shapes your fate. Built entirely in **C**, it delivers an old-school adventure right in the terminal — no graphics, no hand-holding, just raw storytelling and player choice.

You step into the boots of a wandering hero, exploring, talking with mysterious NPCs, collecting powerful relics, and clashing with deadly foes. Every area hides secrets, and every choice can alter your journey — leading you toward the ultimate showdown against the Queen of Hell... or perhaps a different destiny entirely.

With a built-in save system, you can pause your quest anytime and return later using your name to pick up exactly where you left off — just like the RPGs of old.

It’s a minimalist adventure that proves one thing: you don’t need fancy visuals to tell a dark, compelling story — just courage, curiosity, and a bit of imagination.

---

## Inspiration

The main inspiration behind this project is the psychological horror visual novel **Slay the Princess**, a game that challenges players morality and perception through dialogue, choice, and atmosphere rather than combat or spectacle. In that game, you’re told to slay a princess locked away in a cabin — but as you progress, nothing is as it seems. The story twists your expectations, forcing you to question your role, your choices, and even your narrator.

This text adventure takes that same philosophical tension — the uncertainty of whether what you’re doing is right — and translates it into a more classic RPG format. While Slay the Princess uses voice acting, visual storytelling, and branching narratives, this project uses pure text and player imagination to achieve a similar emotional depth.

Instead of a single cabin and a single princess, the world expands into multiple areas, enemies, and decisions — but at its core, it shares that same haunting question:
*Are you truly the hero, or just someone trying to convince yourself you are?*

<p align="center">
   <img src="https://github.com/PeTrolejTV/Slay-Her/blob/main/SlayHer/gallery/slaytheprincess.jpg" alt="Slay the Princess game cover" width="60%">
   <br>
   <em>Figure 1 Preview of the cover for the game Slay the Princess</em>
</p>

---

## Gameplay

The gameplay embraces the charm of classic text-based adventures, where every action is chosen through typed commands and every scene unfolds through vivid descriptions. The player arrives and then navigates trough a mysterious castle — entirely through text, immersing themselves in the atmosphere and story rather than visuals.

<p align="center">
  <img src="https://github.com/PeTrolejTV/Slay-Her/blob/main/SlayHer/gallery/Introduction.png" alt="Games introduction screen" width="65%">
      <br>
   <em>Figure 2 Preview of the Games introduction</em>
</p>

Each area provides detailed information about the environment, visible items, nearby NPCs, and possible exits. You’ll decide how to move, what to collect, and who to speak with. Whether you’re exploring new areas, interacting with characters, or making moral decisions, every command pushes the story forward.

You’ll encounter friendly NPCs or enemies that test your courage. Items you collect may unlock new paths or turn the tide in battle. The save system lets you record your progress at any time, allowing you to continue your adventure later by simply entering your name and choosing to load your save.

<p align="center">
  <img src="https://github.com/PeTrolejTV/Slay-Her/blob/main/SlayHer/gallery/gameplay.png" alt="Game play screen" width="65%">
      <br>
   <em>Figure 3 Preview of the Game play</em>
</p>

It’s a slow-burn, choice-driven experience where text is your only weapon and curiosity your greatest ally. *Each prompt asks not just what you will do next — but who you’ll become by doing it.*

---

## Development Software

- **C Language**: The core programming language used to build the entire game logic, systems, and terminal interface.
- **CLion & Visual Studio Code**: Primary IDEs used for coding, debugging, and testing the project.
- **Adobe Photoshop**: Used to design and refine the games icon, giving the project a distinct visual identity despite its text-based nature.

---

## Class Design

- **Main**: Acts as the games central entry point. It handles player name input, displays the introduction, checks for existing save files, and manages the choice between loading or starting a new game. It also runs the primary game loop that ties everything together.
- **Game**: Manages the save and load systems, ensuring that the players progress, room states, NPC conditions, and inventory are preserved between sessions. It’s responsible for calling the appropriate save/load functions from other modules like item, npc, and player.
- **Item**: Handles everything related to items — adding them to rooms, displaying available items, picking them up, and managing their state. It ensures each item’s properties are correctly saved and restored when loading a saved game.
- **Npc**: Manages all non-player characters, including their behavior, interactions, and combat. It determines outcomes such as game over if the player dies, handles unique mechanics like the Queen of Hell’s special battle, and keeps NPC states consistent across saves.
- **Player**: Controls player-related data and mechanics. This includes initialization, inventory management, receiving items, tracking the players current room, and restoring the players stats and state when loading a saved game.
- **Room**: Defines the structure and logic of all rooms in the game world. It connects rooms through directions (north, east, south, west), displays environmental descriptions, spawns NPCs, and manages movement between areas. It also contains unique events like the treasure room trap and part of the interactive game loop.
- **Utils**: Provides utility functions that support the entire project, such as formatted logging, text effects, and helper comparisons. These functions add polish to the terminal experience and streamline code across modules.

---

## Commands

| Command  | Description                                                                               |
| -------- | ----------------------------------------------------------------------------------------- |
| **n**    | Move **North** – sends the player to the room located to the north.                       |
| **e**    | Move **East** – sends the player to the room located to the east.                         |
| **s**    | Move **South** – sends the player to the room located to the south.                       |
| **w**    | Move **West** – sends the player to the room located to the west.                         |
| **pick** | **Pick up an item** – allows the player to collect an item found in the current room.     |
| **inv**  | **Show inventory** – displays all items currently held by the player.                     |
| **int**  | **Interact with NPC** – lets the player talk or fight with non-player characters.         |
| **info** | **Look around** – reprints the room’s description, visible items, NPCs, and exits.        |
| **save** | **Save the game** – stores the current progress so it can be continued later.             |
| **quit** | **Quit the game** – exits the game and closes the application.                            |

---

## Conclusion

This text-based adventure reimagines the essence of classic terminal RPGs — simple in presentation, yet rich in atmosphere and choice. Developed entirely in **C**, it captures the tension and imagination of an old-school story-driven experience, where every typed command can change the course of fate.

Through exploration, dialogue, and combat, players uncover a world filled with mystery, danger, and moral uncertainty. Each decision — to fight or to explore — pushes the story forward and reveals a little more about who the player truly is.

There are no flashy visuals or cinematic effects here — just words, choices, and consequence. But sometimes, that’s all you need to feel the weight of a world on your shoulders.

And somewhere in the shadows of the underworld, the Queen of Hell awaits your arrival — whether as her conqueror... or her successor.

---

## 🎮 How to Play

1. **Download the project**
   - Click the green **"Code" → "Download ZIP"**, then extract it anywhere on your PC

2. **Open the folder**
   - Go to: `Slay-Her-main\SlayHer\game`

3. **Run the game**

   *OPTIONAL:*
   > If you want to modify the files in `main`, to rebuild the exe again do this:
   - To **build an EXE** → double-click `build_exe.bat`
   - Wait some time till everything downloads and builds the exe
   - Press any key in the Command Prompt or just close it
   
   *OTHERWISE:*
   - To **Play** the game → double-click on `SlayHer.exe`
