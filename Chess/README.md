# Chess Qt

A modern, full-featured Chess application written in C++ with Qt 6.

![Chess Qt](file:///home/spejle/Chess/Chess.png)

## Features

- **Full Chess Engine & Rule Support**:
  - Legal move generation with strict King safety validation
  - Pawn double-step and **En Passant** capture
  - Kingside (`O-O`) and Queenside (`O-O-O`) **Castling** with check-path validation
  - **Pawn Promotion** dialog (Queen, Rook, Bishop, Knight)
  - Detection of **Check**, **Checkmate**, **Stalemate**, **50-Move Rule**, and **Insufficient Material** (K vs K, K+B vs K, K+N vs K, K+B vs K+B same color)
  - Full move stack for **Undo Move**

- **Interactive Chessboard UI**:
  - High-DPI antialiased vector pieces rendered with `QPainterPath` (scalable to any display resolution)
  - Click-to-move and intuitive smooth **Drag-and-Drop**
  - Real-time highlights: selected piece, legal move hints (dots for moves, rings for captures), last move tracking, and crimson King-in-check glow
  - File (`a-h`) and Rank (`1-8`) coordinate border labels
  - Board flip option (view from White or Black perspective)
  - 4 Board Themes: **Tournament Green**, **Classic Wood**, **Modern Slate**, and **Dark Glass**

- **Game Modes & Built-in AI Engines**:
  - **Player vs Player** (Local pass & play)
  - **Player vs Computer (Local Minimax)** with 3 difficulty levels: Easy, Medium, Hard
  - **Player vs Antigravity (AI Assistant) 🚀**: Pure native C++ AI engine with opening book, tactical evaluator, and live strategic commentary. Also supports interactive IPC move injection directly from the Antigravity session via `antigravity_move.json`!
  - 100% native C++ Qt: **Zero Python dependencies**.

- **Game Tools**:
  - Live Move History with standard algebraic notation (SAN: e.g., `e4`, `Nf3`, `exd5`, `O-O`, `Qh5#`)
  - Live **Antigravity Commentary & Thoughts** speech bubble in the sidebar
  - Captured pieces tray with automatic material advantage calculator (`+1`, `+3`, etc.)
  - Chess Clocks / Timers (Unlimited, 10 min Rapid, 5 min Blitz, 3 min Bullet)
  - **Export PGN** (copies to clipboard and saves to file for Lichess / Chess.com analysis)

---

## Project Structure

```
Chess/
├── Chess.pro            # QMake project file for Qt Creator
├── CMakeLists.txt       # Modern CMake build configuration
├── README.md            # Project documentation
├── src/
│   ├── types.h              # Data structures: BoardPos, Piece, Move, GameStatus
│   ├── piecerenderer.h/.cpp # High-DPI Staunton vector piece drawing engine
│   ├── chessgame.h/.cpp     # Core chess game rules, validation, history, and SAN
│   ├── chessai.h/.cpp       # Minimax AI with Alpha-Beta pruning & PST tables
│   ├── antigravityengine.h/.cpp # Native C++ Antigravity AI & IPC engine
│   ├── promotiondialog.h/.cpp # Pawn promotion piece selection dialog
│   ├── newgamedialog.h/.cpp # Game configuration & setup dialog
│   ├── chessboardwidget.h/.cpp # Interactive board widget with drag & drop
│   ├── mainwindow.h/.cpp    # Main window with sidebar, clocks, and menus
│   └── main.cpp             # Application entry point
└── tests/
    └── test_chess.cpp       # Unit test suite with QTest
```

---

## How to Open in Qt Creator

1. Launch **Qt Creator**.
2. Click **Open Project...** (`Ctrl+O`).
3. Select either:
   - `/home/spejle/Chess/Chess.pro` (to use the QMake build system), OR
   - `/home/spejle/Chess/CMakeLists.txt` (to use the CMake build system).
4. Configure the project with your installed Qt 6 kit.
5. Click **Run** (`Ctrl+R`) to build and play!

---

## How to Build from the Command Line

### Option A: Using CMake (Recommended)

```bash
cd /home/spejle/Chess
mkdir -p build && cd build
cmake ..
cmake --build . -j$(nproc)
./Chess
```

To run the automated unit tests:
```bash
ctest --output-on-failure
# or execute directly:
./test_chess
```

### Option B: Using QMake

```bash
cd /home/spejle/Chess
mkdir -p build-qmake && cd build-qmake
qmake6 ../Chess.pro
make -j$(nproc)
./Chess
```

---

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+N` | Start New Game |
| `Ctrl+Z` | Undo Move |
| `F`      | Flip Board View |
| `Ctrl+S` | Export PGN |
| `Ctrl+Q` | Quit Game |
