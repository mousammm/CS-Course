#include <iostream>
using namespace std;

bool gameOver;
const int wallWidth = 20;
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
    for(int i=0; i<wallWidth+2; i++) // top wall
        cout << "#";
    cout<<endl;
 
    for(int i=0; i<wallHeight; i++){
        for(int j=0; j<wallWidth; j++){
            if(j == 0) 
                cout << "#";
            
                cout<< " ";

            if(j == wallWidth-1)
                cout << "#";
        }
        cout<<endl;
    }

    for(int i=0; i<wallWidth+2; i++) // bottom wall
        cout << "#";
    cout << "#";
    
}
void input(){
}
void logic(){
}

int main(){
    std::cout<<"hellow world!";
    setup();
    while(!gameOver){
        draw();
        input();
        logic();
    }
    // Sleep(10); // sleep(10);
    return 0;
}
