#include <iostream>
#include <conio.h>   // for keyboard input 
#include <windows.h> // for sleep fun
using namespace std;

bool gameOver;
const int wallWidth = 20;
const int wallHeight = 20;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN  };
eDirection dir;

int headX, headY, fruitX, fruitY, score;
int tailX[100], tailY[100]; // x/y cordinate store of the tail
int nTail; // length of the tail

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
    //system("cls");
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {0,0});
    for(int i=0; i<wallWidth+2; i++) // top wall | Widht + 2 (means top and bot border)
        cout << "#";
    cout<<endl;
 
    for(int i=0; i<wallHeight; i++){     // all beside the top wall and bot wall
        for(int j=0; j<wallWidth; j++){
            if(j == 0) 
                cout << "#";  // left border 
            
            if(i == headY && j == headX) // snake head draw 
                cout << "?";
            else if(i == fruitY && j == fruitX) // fruit draw 
                cout << "$";
            else 
            {
                bool print = false; // we avoid drawing space if we eat the fruit 
          // as to display the body we need space and if we did make it will overflow
                for(int k = 0; k < nTail; k++)
                    {
                        if(tailX[k] == j && tailY[k] == i)
                            {
                                print = true;
                                cout<<".";
                            }
                    }
                if (!print)
                    cout<< " ";
            }

            if(j == wallWidth-1) // right border  | width - 1 ( we minus the left border)
                cout << "#";
        }
        cout<<endl;
    }

    for(int i=0; i<wallWidth+2; i++) // bottom wall
        cout << "#";
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
  int prevTailX = tailX[0];
  int prevTailY = tailY[0];
  int prevTail2X, prevTail2Y;
  tailX[0] = headX;
  tailY[0] = headY;
  for(int i=1; i<nTail; i++){
    prevTail2X = tailX[i];
    prevTail2Y = tailY[i];
    tailX[i] = prevTailX;
    tailY[i] = prevTailY;
    prevTailX = prevTail2X;
    prevTailY = prevTail2Y;
  }

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
  // right wall collision we need to minus the left wall or it detect after it hit the wall
  //if (headX > wallWidth-1 || headX < 0 || headY > wallHeight-1 || headY < 0) 
    // gameOver = true;
  // snake no wall kill mode
  if(headX >= wallWidth) headX = 0;
  else if(headX < 0) headX = wallWidth - 1;

  if(headY >= wallHeight) headY = 0;
  else if(headY < 0) headY = wallHeight - 1;
  
/*
  for(int i = 0; i < nTail; i++)
    if(tailX[i] == headX && tailY[i] == headY)
      gameOver = true;
*/

    if (headX == fruitX && headY == fruitY ){
        fruitX = rand() % wallWidth;
        fruitY = rand() % wallHeight;
        score += 10000;
        nTail++;
    }
}

int main(){
    std::cout<<"hellow world!";
    system("cls");
    setup();
    while(!gameOver){
        draw();
        input();
        logic();
        Sleep(50); // sleep(10);
    }
    system("cls");
    cout<<"\n\nYou Died Looser ;) Score:"<<score<<"\n\n";
    return 0;
}
