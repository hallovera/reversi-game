//
// Reversi Game
// By: Gordon Lin
// This program implements the Reversi board game using a simple opponent move algorithm. 
//

#include <stdio.h>
#include <stdbool.h>
#include <string.h> 

// Returns if a given row/col coordinate is within the board bounds or not
bool positionInBounds(int n, int row, int col) {
    return (row < n && col < n && row >= 0 && col >= 0);
}

// Checks if given move works in a single direction
bool checkLegalInDirection(char board[][26], int n, int row, int col,
                           char colour, int deltaRow, int deltaCol) {
    char oppositePlayer;
    bool oppositeFound = false;
    char startChar;
    char endChar;

    oppositePlayer = colour == 'W' ? 'B' : 'W';

    if (board[row][col] == 'U') {
        startChar = 'U';
        endChar = colour;
    } else {
        startChar = colour;
        endChar = 'U';
    }

    row += deltaRow;
    col += deltaCol;

    while (positionInBounds(n, row, col)) {
        if (board[row][col] == startChar && oppositeFound && startChar != 'U') {
            break;
        } else if (board[row][col] == endChar && oppositeFound) {
            return true;
        } else if (board[row][col] == oppositePlayer) {
            oppositeFound = true;
        } else if (board[row][col] == endChar && !oppositeFound) {
            break;
        }
        row += deltaRow;
        col += deltaCol;
    }
    return false;
}

// Sets board to default position depending on its size n
void resetBoard(char board[][26], int n) {
    for (int row = 0; row < 26; row++) {
        for (int col = 0; col < 26; col++) {
            board[row][col] = 'U';
        }
    }
    board[n / 2 - 1][n / 2 - 1] = 'W';
    board[n / 2][n / 2] = 'W';
    board[n / 2 - 1][n / 2]= 'B';
    board[n / 2][n / 2 - 1] = 'B';
}

// Prints out current state of the game board
void printBoard(char board[][26], int n) {
    printf("  ");
    for (int col = 0; col < n; col++) {
        printf("%c", 'a' + col);
    }
    printf("\n");
    for (int row = 0; row < n; row++) {
        printf("%c ", 'a' + row);
        for (int col = 0; col < n; col++) {
            printf("%c", board[row][col]);
        }
        printf("\n");
    }
}

// Checks if current move is legal or not. Starts from empty 'U' position.  
bool isLegalMove(char board[][26], char player, int n, int row, int col, int direc[16]) {
    int deltaRow[] = {1,1,0,-1,-1,-1,0,1};
    int deltaCol[] = {0,-1,-1,-1,0,1,1,1};
    int r = 0;
    int c = 0;
    char opposite;
    int currentDir = 0;

    opposite = player == 'W' ? 'B' : 'W';

    if (board[row][col] != 'U') {
        return false;
    }

    for (int dir = 0; dir < 8; dir++) {
        r = row + deltaRow[dir];
        c = col + deltaCol[dir];

        if (board[r][c] != opposite) {
            continue;
        } else {
            while (positionInBounds(n, r, c)) {
                // if (board[r][c] == 'U') {
                //     continue;
                if (board[r][c] == player) {
                    direc[currentDir] = deltaRow[dir];
                    direc[currentDir + 1] = deltaCol[dir];
                    currentDir += 2;
                    break;
                }
                r += deltaRow[dir];
                c += deltaCol[dir];
            }
        }
    }
    return currentDir != 0;
}

int letterToInt(char character) {
    return (int) character - 'a';
}

// Sets the AI's move, according to the perscribed algorithm
void cpuMove(char board[][26], int n, char move[3], char colour) {
    int numMoves[n][n];
    int largest = 0;
    int r = 0, c = 0;
    int deltaRow[] = {1,1,0,-1,-1,-1,0,1};
    int deltaCol[] = {0,-1,-1,-1,0,1,1,1};
    char oppositeColour = colour == 'B' ? 'W' : 'B';

    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            numMoves[row][col] = 0;
        }
    }

    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (board[row][col] == 'B' || board[row][col] == 'W') {
                continue;
            } else {
                // check NUMBER OF ENEMIES CAPTURED
                for (int dir = 0; dir < 8; dir++) {
                    if (checkLegalInDirection(board, n, row, col, colour, deltaRow[dir], deltaCol[dir])) {
                        r = row + deltaRow[dir];
                        c = col + deltaCol[dir];
                        if (board[r][c] == 'U') {
                            continue;
                        }
                        while (board[r][c] != colour) {
                            if (board[r][c] == oppositeColour) {
                                numMoves[row][col]++;
                            }
                            r += deltaRow[dir];
                            c += deltaCol[dir];   
                        }
                    }
                }
            }
        }
    }

    // Search for spot w/ largest # of moves
    // If two largest have same #, use one with lower COL
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (numMoves[row][col] > largest) {
                largest = numMoves[row][col];
                move[0] = row + 'a';
                move[1] = col + 'a';
            }
        }
    }
}

// Returns true if given player has no remaining moves, false if they do
bool noMoreMoves(char board[][26], int n, char player, int direc[16]) {
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (isLegalMove(board, player, n, row, col, direc)) {
                return false;
            }
        }
    }
    return true;
}

// Determines if either player has won the game
// True if both players have no moves available to them
// Returns 'N' if no victory, 'B' for black victory, 'W' for white victory, 'D' for draw
char victoryAchieved(char board[][26], int n, int direc[16]) {
    int bCount = 0, wCount = 0;
    
    // No more moves for both players
    if (noMoreMoves(board, n, 'B', direc) && noMoreMoves(board, n, 'W', direc)) {
        for (int row = 0; row < n; row++) {
            for (int col = 0; col < n; col++) {
                if (board[row][col] == 'B') {
                    bCount++;
                } else if (board[row][col] == 'W') {
                    wCount++;
                }
            }
        }
    } else {
        return 'N'; // Return 'N' for No winner
    }
    
    if (bCount > wCount) {
        return 'B'; // Return 'B' for Black victory
    } else if (bCount < wCount) {
        return 'W'; // Return 'W' for White victory
    } else {
        return 'D'; // Return 'D' for Draw
    }
}

int main(void) {
    char board[26][26];
    int direc[16] = {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
    int n = 0;
    int row, col;
    char computer, user;
    int colConst, rowConst;
    char currentColour = 'B';
    char move[3] = "  ";
    
    printf("Enter the board dimension: ");
    scanf("%d%*c", &n);
    printf("Computer plays (B/W): ");
    scanf("%c%*c", &computer);
    user = computer == 'B' ? 'W' : 'B';

    resetBoard(board, n);
    printBoard(board, n);

    while (victoryAchieved(board, n, direc) == 'N') { 
        if (noMoreMoves(board, n, currentColour, direc)) {
            printf("%c player has no valid move.\n", currentColour);
            currentColour = currentColour == 'B' ? 'W' : 'B';
            continue;
        }

        if (computer == currentColour) {
            cpuMove(board, n, move, currentColour);
            printf("Computer places %c at %c%c.\n", currentColour, move[0], move[1]);
            rowConst = letterToInt(move[0]);
            colConst = letterToInt(move[1]);

            isLegalMove(board, currentColour, n, rowConst, colConst, direc);
            board[rowConst][colConst] = currentColour;
            row = rowConst + direc[0];
            col = colConst + direc[1];
            for (int i = 0; i < 15; i += 2) {
                if (direc[i] == 5) {
                    break;
                }

                while (board[row][col] != currentColour) {
                    board[row][col] = currentColour;

                    row += direc[i];
                    col += direc[i + 1];
                }

                row = rowConst + direc[i + 2];
                col = colConst + direc[i + 3];
            }
        } else {
            printf("Enter move for colour %c (RowCol): ", currentColour);
            fgets(move, 3, stdin);
            scanf("%*c");

            rowConst = letterToInt(move[0]);
            colConst = letterToInt(move[1]);

            if (!positionInBounds(n, rowConst, colConst) || !isLegalMove(board, currentColour, n, rowConst, colConst, direc)) {
                printf("Invalid move.\n");
                printf("%c player wins.", computer); // If user enters invalid move, computer auto-wins
                break;
            } else {
                isLegalMove(board, currentColour, n, rowConst, colConst, direc);

                board[rowConst][colConst] = currentColour;
                row = rowConst + direc[0];
                col = colConst + direc[1];
                for (int i = 0; i < 15; i += 2) {
                    if (direc[i] == 5) {
                        break;
                    }

                    while (board[row][col] != currentColour) {
                        board[row][col] = currentColour;

                        row += direc[i];
                        col += direc[i + 1];
                    }

                    row = rowConst + direc[i + 2];
                    col = colConst + direc[i + 3];
                }
            }
        }
        printBoard(board, n);
        currentColour = currentColour == 'B' ? 'W' : 'B';

        for (int dir = 0; dir < 16; dir++) {
            direc[dir] = 5;
        }
    }

    if (victoryAchieved(board, n, direc) == 'B' || victoryAchieved(board, n, direc) == 'W') {
        printf("%c player wins.", victoryAchieved(board, n, direc));
    } else if (victoryAchieved(board, n, direc) == 'D') {
        printf("Draw!");
    }
    return 0;
}
