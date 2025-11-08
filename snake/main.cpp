#include <iostream>
#include <conio.h>   // for keyboard input 
#include <windows.h> // for sleep fun

using namespace std;

bool gameOver;
const int wallWidth = 30;
const int wallHeight = 20;
int headX, headY, fruitX, fruitY, score;

enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN  };
eDirection dir;

void setup(){
    gameOver = false;
    dir = STOP;
    headX = wallWidth / 2;
    headY = wallHeight / 2;
    fruitX = rand() % wallWidth;
    fruitY = rand() % wallHeight;
    score = 0;
}

void draw(){
    system("cls");
    for(int i=0; i<wallWidth+2; i++) // top wall | Widht + 2 (means top and bot border)
        cout << "=";
    cout<<endl;
 
    for(int i=0; i<wallHeight; i++){     // all beside the top wall and bot wall
        for(int j=0; j<wallWidth; j++){
            if(j == 0) 
                cout << "[";  // left border 
            
            if(i == headY && j == headX) // snake head draw 
                cout << "?";
            else if(i == fruitY && j == fruitX) // fruit draw 
                cout << "$";
            else
                cout<< " ";

            if(j == wallWidth-1) // right border  | width - 1 ( we minus the left border)
                cout << "]";
        }
        cout<<endl;
    }

    for(int i=0; i<wallWidth+2; i++) // bottom wall
        cout << "=";
    cout << endl;
    cout << "Score:"<<score<<endl;
}

void input(){
    if (_kbhit()){
        switch(_getch()){
            case 'w':
                dir=UP;
            break;
            case 'a':
                dir=LEFT;
            break;
            case 's':
                dir=DOWN;
            break;
            case 'd':
                dir=RIGHT;
            break;
            case 'x':
                gameOver = true;
            break;
            case 'p':
                dir=STOP;
            break;
       }
    }
}
void logic(){
  switch(dir){
    case UP:
      headY--;
    break;
    case LEFT:
      headX--;
    break;
    case DOWN:
      headY++;
    break;
    case RIGHT:
      headX++;
    break;
  }   
  if (headX > wallWidth || headX < 0 || headY > wallHeight || headY < 0)
    gameOver = true;
    if (headX == fruitX && headY == fruitY ){
       fruitX = rand() % wallWidth;
       fruitY = rand() % wallHeight;
       score += 10000;
    }
}

int main(){
    std::cout<<"hellow world!";
    setup();
    while(!gameOver){
        draw();
        input();
        logic();
        Sleep(100); // sleep(10);
    }
    return 0;
}
