# clientbank-core
Console-based bank client management system in C++17 — persistent storage, transaction handling, and modular architecture.

Overview
ClientBank Core simulates the core operations of a bank's client management system entirely from the terminal. It manages client records — creation, lookup, updates, deletion, and financial transactions — with all data persisted to disk between sessions.
The project is structured to reflect real-world backend design patterns at a small scale: separation of data access, business logic, and presentation layers; consistent in-memory/on-disk state synchronization; and defensive handling of user input.
Preview
<table> <tr> <td align="center"><b>Main Menu</b></td> <td align="center"><b>Transactions Menu</b></td> </tr> <tr> <td><img src="./clientbank-core/img/Main.png" width="380"/></td> <td><img src="img/transactions.png" width="380"/></td> </tr> </table> <p align="center"><b>Deposit Flow</b></p> <p align="center"><img src="img/deposit.png" width="500"/></p>
Features
Client management — add, view, update, delete, and search client records
Transactions module — deposit and withdraw funds with balance validation
Persistent storage — custom flat-file format with a dedicated read/write layer (no external DB dependency)
Soft deletion — records are flagged rather than immediately erased, preserving data integrity
Input validation — guards against invalid menu input and insufficient-funds transactions
Modular design — logic split across .h/.cpp files by responsibility (data layer, models, UI/menu flow)


Getting Started
Prerequisites
A C++17-compatible compiler (g++, clang++, or MSVC)
Build
bash
g++ -std=c++17 -Wall -o Bank_v0.1.0 src/*.cpp -Iinclude
Run
bash
./Bank_v0.1.0


Learning Context
This project was built as a hands-on exercise in backend fundamentals — file-based persistence, struct-based data modeling, state management across a multi-menu CLI application, and debugging real synchronization bugs between in-memory and on-disk state. It's part of an ongoing progression toward backend and distributed-systems development in C#/.NET and, longer term, blockchain/Web3 systems.
