#include <ncurses.h>
#include <stdlib.h>

#define SIDE 9
#define notTwo(x) (x & (x - 1))
#define val(y,x) game[y/3][x/3][y%3][x%3]

int game[3][3][3][3];

int cursY, cursX;
int i, j, n;
int print;

void printOutline();
void printSpot(int row, int col);
void clearBoard();
void moveCursor();
int fix(int row, int col);
int stats(int row, int col);
void recurse(int row, int col);
int lawg(int num);

int main() {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();

    clearBoard();
    printOutline();
    mvchgat(0, 1, 3, A_REVERSE, 0, NULL);
    moveCursor();
    endwin();

    return 0;
}

void printOutline() {
    for(n = 0; n < 41; n++) {
        mvaddch( 5, n, ACS_HLINE);
        mvaddch(11, n, ACS_HLINE);
    }
    for(n = 0; n < 17; n++) {
        mvaddch(n, 13, ACS_VLINE);
        mvaddch(n, 27, ACS_VLINE);
    }
    mvaddch( 5, 13, ACS_PLUS);
    mvaddch( 5, 27, ACS_PLUS);
    mvaddch(11, 13, ACS_PLUS);
    mvaddch(11, 27, ACS_PLUS);
}

void printSpot(int row, int col) {
    if(print) {
        mvprintw(2*row, (14*(col/3))+4*(col%3)+1, "%03o", val(row, col));
    } else {
        if(val(row, col) && notTwo(val(row, col)) == 0) {
            mvprintw(2*row, (14*(col/3))+4*(col%3)+1, " %d ", lawg(val(row, col)));
        } else {
            mvprintw(2*row, (14*(col/3))+4*(col%3)+1, "   ");
        }
    }
}

void clearBoard() {
    int y, x;
    for(y = 0; y < SIDE; y++) {
        for(x = 0; x < SIDE; x++) {
            val(y, x) = 0777;
            printSpot(y, x);
        }
    }
}

void moveCursor() {
    static int ch;
    for(ch = getch(); ch != 'q'; ch = getch()) {
        mvchgat(2*cursY, (14*(cursX/3))+4*(cursX%3)+1, 3, A_NORMAL, 0, NULL);
        if(ch == 'w' || ch == 'k' || ch == KEY_UP) {
            cursY = (cursY + SIDE - 1) % SIDE;
        } else if(ch == 'a' || ch == 'h' || ch == KEY_LEFT) {
            cursX = (cursX + SIDE - 1) % SIDE;
        } else if(ch == 's' || ch == 'j' || ch == KEY_DOWN) {
            cursY = (cursY + SIDE + 1) % SIDE;
        } else if(ch == 'd' || ch == 'l' || ch == KEY_RIGHT) {
            cursX = (cursX + SIDE + 1) % SIDE;
        } else if(ch == 'c') {
            clearBoard();
        } else if(ch == 'n') {
            int change;
            do {
                change = 0;
                for(i = 0; i < SIDE * SIDE; i++) {
                    if(notTwo(val(i/SIDE, i%SIDE))) {
                        change += stats(i/SIDE, i%SIDE);
                    }
                }
            } while(change);
        } else if(ch == 'p') {
            if(--print) {
                print = 1;
            }
            for(i = 0; i < SIDE; i++) {
                for(j = 0; j < SIDE; j++) {
                    printSpot(i, j);
                }
            }
        } else if(ch >= '1' && ch <= '9') {
            ch -= 48;
            i = 1 << (ch - 1);
            if(notTwo(val(cursY, cursX)) == 0) {
                j = fix(cursY, cursX);
                if(i & j) {
                    val(cursY, cursX) = i;
                    stats(cursY, cursX);
                    printSpot(cursY, cursX);
                }
            } else if(val(cursY, cursX) & i) {
                val(cursY, cursX) = i;
                stats(cursY, cursX);
                printSpot(cursY, cursX);
            }
        } else if(ch == '0' || ch == KEY_BACKSPACE || ch == KEY_DC) {
            val(cursY, cursX) = fix(cursY, cursX);
            printSpot(cursY, cursX);
        }
        mvchgat(2*cursY, (14*(cursX/3))+4*(cursX%3)+1, 3, A_REVERSE, 0, NULL);
    }
}

int fix(int row, int col) {
    j = 0;
    for(n = 0; n < SIDE; n++) {
        if(n != row && notTwo(val(n, col)) == 0) {
            j |= val(n, col);
        } else {
            val(n, col) |= val(row, col);
            printSpot(n, col);
        }
        if(n != col && notTwo(val(row, n)) == 0) {
            j |= val(row, n);
        } else {
            val(row, n) |= val(row, col);
            printSpot(row, n);
        }
        if((row % 3 != n/3 || col % 3 != n%3) && notTwo(game[row/3][col/3][n/3][n%3]) == 0) {
            j |= game[row/3][col/3][n/3][n%3];
        } else {
            game[row/3][col/3][n/3][n%3] |= val(row, col);
            printSpot(3*(row/3)+n/3, 3*(col/3)+n%3);
        }
    }
    return 0777 & ~j;
}

int stats(int row, int col) {
    int z, y, x;
    int change = 0;
    if(notTwo(val(row, col)) == 0) {
        for(z = 0; z < SIDE; z++) {
            y = row/3;
            x = col/3;
            if(notTwo(val(z, col))) {
                val(z, col) &= ~val(row, col);
                recurse(z, col);
                change++;
            }
            if(notTwo(val(row, z))) {
                val(row, z) &= ~val(row, col);
                recurse(row, z);
                change++;
            }
            if(notTwo(game[y][x][z/3][z%3])) {
                game[y][x][z/3][z%3] &= ~val(row, col);
                recurse(3*y+z/3, 3*x+z%3);
                change++;
            }
        }
    } else {
        j = 0777;
        for(z = 0; z < SIDE; z++) {
            if(row != z) {
                j &= (val(row, col) & ~val(z, col));
            }
        }
        if(j > 0) {
            val(row, col) &= j;
            change++;
            recurse(row, col);
        }
        j = 0777;
        for(z = 0; z < SIDE; z++) {
            if(col != z) {
                j &= (val(row, col) & ~val(row, z));
            }
        }
        if(j > 0) {
            val(row, col) &= j;
            change++;
            recurse(row, col);
        }
        j = 0777;
        for(y = 0; y < 3; y++) {
            for(x = 0; x < 3; x++) {
                if(row % 3 != y || col %3 != x) {
                    j &= (val(row, col) & ~game[row/3][col/3][y][x]);
                }
            }
        }
        if(j > 0) {
            val(row, col) &= j;
            change++;
            recurse(row, col);
        }
    }
    printSpot(row, col);
    return change;
}

void recurse(int row, int col) {
    if(notTwo(val(row, col)) == 0) {
        stats(row, col);
    }
    printSpot(row, col);
}

int lawg(int num) {
    int val = 0;
    while(num != 0) {
        val++;
        num >>= 1;
    }
    return val;
}
