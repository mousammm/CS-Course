#include <iostream>
#include <conio.h>   // for keyboard input
#include <windows.h> // for sleep function
#include <ctime>     // for random seed
using namespace std;

bool gameOver;
const int wallWidth = 20;
const int wallHeight = 20;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

int headX, headY, fruitX, fruitY, score;
const int MAX_TAIL = 100; // Maximum tail length
int tailX[MAX_TAIL], tailY[MAX_TAIL];
int nTail;

void setup() {
    gameOver = false;
    dir = STOP;
    headX = wallWidth / 2;
    headY = wallHeight / 2;

    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));

    fruitX = rand() % wallWidth;
    fruitY = rand() % wallHeight;

    score = 0;
    nTail = 0;

    // Initialize tail arrays
    for (int i = 0; i < MAX_TAIL; i++) {
        tailX[i] = -1;
        tailY[i] = -1;
    }
}

void draw() {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {0, 0});

    // Draw top wall
    for (int i = 0; i < wallWidth + 2; i++)
        cout << "#";
    cout << endl;

    // Draw game area
    for (int i = 0; i < wallHeight; i++) {
        for (int j = 0; j < wallWidth; j++) {
            if (j == 0)
                cout << "#";  // left border

            // Check what to draw at this position
            if (i == headY && j == headX)
                cout << "O";  // Snake head
            else if (i == fruitY && j == fruitX)
                cout << "F";  // Fruit
            else {
                bool isTail = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "o";  // Snake body
                        isTail = true;
                        break;
                    }
                }
                if (!isTail)
                    cout << " ";
            }
            if (j == wallWidth - 1)
                cout << "#";  // right border
        }
        cout << endl;
    }

    // Draw bottom wall
    for (int i = 0; i < wallWidth + 2; i++)
        cout << "#";
    cout << endl;
    
    cout << "Score: " << score << " | Tail: " << nTail << endl;
    cout << "Controls: WASD to move, P to pause, X to exit" << endl;
}

void input() {
    if (_kbhit()) {
        char key = _getch();
        switch (key) {
            case 'w':
                if (dir != DOWN) dir = UP;
                break;
            case 'a':
                if (dir != RIGHT) dir = LEFT;
                break;
            case 's':
                if (dir != UP) dir = DOWN;
                break;
            case 'd':
                if (dir != LEFT) dir = RIGHT;
                break;
            case 'x':
                gameOver = true;
                break;
            case 'p':
                dir = STOP;
                break;
        }
    }
}

void logic() {
    // Move tail segments
    if (nTail > 0) {
        // Store current tail positions
        int prevX = tailX[0];
        int prevY = tailY[0];
        int tempX, tempY;

        tailX[0] = headX;
        tailY[0] = headY;

        // Update remaining tail segments
        for (int i = 1; i < nTail; i++) {
            tempX = tailX[i];
            tempY = tailY[i];
            tailX[i] = prevX;
            tailY[i] = prevY;
            prevX = tempX;
            prevY = tempY;
        }
    }

    // Move head based on direction
    switch (dir) {
        case UP:    headY--; break;
        case LEFT:  headX--; break;
        case DOWN:  headY++; break;
        case RIGHT: headX++; break;
        default: break;
    }

    // Wall collision (wrap-around)
    if (headX >= wallWidth) headX = 0;
    else if (headX < 0) headX = wallWidth - 1;

    if (headY >= wallHeight) headY = 0;
    else if (headY < 0) headY = wallHeight - 1;

    // Self-collision detection
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == headX && tailY[i] == headY) {
            gameOver = true;
            break;
        }
    }

    // Fruit collection
    if (headX == fruitX && headY == fruitY) {
        // Generate new fruit position (ensure it's not on snake)
        bool validPosition;
        do {
            validPosition = true;
            fruitY = rand() % wallHeight;
            
            // Check if fruit spawns on snake head
            if (fruitX == headX && fruitY == headY)
                validPosition = false;
            
            // Check if fruit spawns on any tail segment
            for (int i = 0; i < nTail && validPosition; i++) {
                if (fruitX == tailX[i] && fruitY == tailY[i]) {
                    validPosition = false;
                    break;
                }
            }
        } while (!validPosition);

        score += 10;
        if (nTail < MAX_TAIL) {
            nTail++;
        }
    }
}



int main() {

    system("cls");

    setup();

    while (!gameOver) {

        draw();

        input();

        logic();

        Sleep(100); // Adjust game speed

    }

    

    system("cls");

    cout << "\n\nGame Over! Final Score: " << score << "\n\n";

    return 0;

}
