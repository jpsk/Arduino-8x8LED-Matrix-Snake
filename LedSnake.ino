#include "LedControl.h"
#include "StackArray.h"


#define LEFT 1
#define UP 2
#define RIGHT 3
#define DOWN 4
#define ON 1

LedControl lc=LedControl(12,11,10,1);

unsigned long delaytime = 300;
int moveVector = RIGHT;
int snake[64][3] = {{1,4,ON},{0,4,ON}};
int food[2];
bool reset = false;

// Counts how long is the snake
int lenSnake() {
  int len = 0;
  int pos = 1;
  while(pos != 0) {
    pos = snake[len][2];
    len++;
  }
  return len-1;
};

// Moves snake by one frame
void moveSnake() {
  for (int i = lenSnake()-1; i > 0; i--) {
    snake[i][0] = snake[i-1][0];
    snake[i][1] = snake[i-1][1];
    snake[i][2] = snake[i-1][2];
  }
  moveHead();
}


// Moves snakes head one frame 
void moveHead() {
  snake[0][2] = moveVector;
  int snakeHead = snake[0][2];
  int snakeX = snake[0][0];
  int snakeY = snake[0][1];
  
  switch (snakeHead) {
  case LEFT:
    (snakeX == 0) ? snake[0][0] = 7 : snake[0][0]--;
    break;
  case RIGHT:
    (snakeX == 7) ? snake[0][0] = 0 : snake[0][0]++;
    break;
  case UP:
    (snakeY == 0) ? snake[0][1] = 7 : snake[0][1]--;
    break;
  case DOWN:
    (snakeY == 7) ? snake[0][1] = 0 : snake[0][1]++;
    break;
  }
}

void printSnake() {
  Serial.println("---Printing snake:---");
  for(int i = 0; i < lenSnake(); i++) {
    Serial.print(snake[i][0]);
    Serial.print(snake[i][1]);
    Serial.println(snake[i][2]);
    
  }
  Serial.println("-------------------");
}

void generateFood() {
  int randX = (rand() % 8);
  int randY = (rand() % 8);
  if (snake[0][0] != randX && snake[0][1] != randY) {
    food[0] = randX;
    food[1] = randY;
  } else {
    generateFood();
  }
}

bool checkCollusion() {
  for(int i=1; i < lenSnake(); i++){
    if (snake[0][0] == snake[i][0] && snake[0][1] == snake[i][1]){
      return true;
    }
  }
  return false;
}

void growSnake() {
  int len = lenSnake();
  snake[len][0] = snake[len-1][0];
  snake[len][1] = snake[len-1][1];
  snake[len][2] = ON;
}

void endGame() {
  while(!reset) {
    delay(500);
    lc.clearDisplay(0);
    delay(500);
    for(int i = 0; i < lenSnake(); i++) {
      lc.setLed(0, snake[i][1], snake[i][0], true);
    }
    lc.setLed(0, food[1], food[0], true);
  }
}

void setup() {
  Serial.begin(9600);
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);
  generateFood();
}

void loop() {

    // Debugging
  if (Serial.available() > 0) {
    int recievedChar = Serial.read();
    switch(recievedChar) {
      case 'u':
        moveVector = UP;
        break;
      case 'r':
        moveVector = RIGHT;
        break;
      case 'l':
        moveVector = LEFT;
        break;
      case 'd':
        moveVector = DOWN;
        break;
    }
  }

  // If snake hits it's own body then game is over
  if(checkCollusion()) {
    endGame();
  }
  
  // Clear display  
  lc.clearDisplay(0);

  // Set food on Display
  lc.setLed(0, food[1],food[0],true);

  // Draw snake 
  for(int i = 0; i < lenSnake() ; i++){
    lc.setLed(0, snake[i][1],snake[i][0],true);
  }



  // MoveSnake
  moveSnake();

  // If snake eats food then generate new food and grow snake
  if (snake[0][0] == food[0] && snake[0][1] == food[1]) {
    generateFood();
    growSnake();
  }


  
  delay(delaytime);

}
