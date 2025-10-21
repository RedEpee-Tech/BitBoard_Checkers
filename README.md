# 🏁 BitBoard Checkers

A classic **Checkers (Draughts)** game implemented in **C**, using an efficient **bitboard representation** for all pieces.  
This project allows two players to play locally, with support for **kinging**, **multi-captures**, **saving/loading**, and **win detection**.

---

## 🧩 Features

✅ Bitboard-based logic (fast and memory-efficient)  
✅ Move validation with legal/illegal checks  
✅ Multi-jump (chain capture) support  
✅ King promotion and movement  
✅ Save and load current game state  
✅ Terminal-based visual board with * marking unusable squares  
✅ Win detection and replay prompt  
✅ Simple Makefile for easy build & test management  

---

## ⚙️ Requirements

- GCC or Clang compiler  
- `make` utility  
- (Optional) A Unix-style terminal (e.g. Git Bash on Windows)

---

## 🛠️ Build Instructions

Make sure your folder looks like this:
BitBoard_Checkers/
├── BitBoard_Checkers.c
├── Makefile
└── README.md

Then, open a terminal in that directory and run:
bash:

  make

  make run

  ./checkers

#🎮 Gameplay Instructions
🎲 Objective

Capture all of your opponent’s pieces or leave them with no legal moves.


🧍Players

Red moves first.

Black moves second.

🧩 Board Layout

- Only dark squares (marked with _) are playable.

- * marks light, unusable squares.

🔤 Input Format
<from> <to>
ex:
e3 f4

Columns: a through h

Rows: 1 through 8

Input is not case sensitive (E3 F4 works too)

👑 Kinging

- A red piece becomes a King when it reaches row 8.

- A black piece becomes a King when it reaches row 1.

- Kings can move and capture in any diagonal direction.

⚔️ Capturing

- Jump over your opponent’s piece to capture it.

- After a capture, if another capture is available, the game will prompt:

    You can capture again! Enter next landing square (e.g. c1):
  
🧠 Win Condition

When one side loses all their pieces, the game will display:
    Game over! Red wins.
Then it will ask the user:
    Would you like to play again? (y/n)
💾 Saving and Loading Games
During play, you can save and load your progress via Makefile commands:
bash:
      make save
      make load
the saved game will be stored in checkers_save.dat
