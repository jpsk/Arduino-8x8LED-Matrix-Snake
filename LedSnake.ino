#include "LedControl.h"
#include "StackArray.h"

// Define interrupt pins
#define I_LEFT 3
#define I_UP 2
#define I_DOWN 18
#define I_RIGHT 19

// Define vector constants
#define LEFT 1
#define UP 2
#define RIGHT 3
#define DOWN 4
#define ON 1


// Program variables
unsigned long delaytime = 300;
int moveVector = RIGHT;
int snake[64][3] = {{1,4,ON},{0,4,ON}};
int food[2];

bool leftAttached = false;
bool rightAttached = false;
bool upAttached = true;
bool downAttached = true;

LedControl lc=LedControl(12,11,10,1);


// This hack was made because my interrupt buttons are not debounced. It was often triggering LOW multiple times, so
// on each Interrupt i detach it and opposite pin (ex: I_DOWN and I_UP) and attach others if needed.
// The 'cleaner' method would be to debounce each trigger programatically, but this was a first idea and it worked :)
// So maybe later.

void handleInterrupts() {
  if (moveVector == RIGHT || moveVector == LEFT) {
    detachInterrupt(digitalPinToInterrupt(I_RIGHT));
    detachInterrupt(digitalPinToInterrupt(I_LEFT));
    leftAttached = false;
    rightAttached = false;
    if (!downAttached) { attachInterrupt(digitalPinToInterrupt(I_DOWN), isr_down, FALLING); };
    if (!upAttached) { attachInterrupt(digitalPinToInterrupt(I_UP), isr_up, FALLING); };   
    upAttached = true;
    downAttached = true;
  } else if (moveVector == UP || moveVector == DOWN) {
    detachInterrupt(digitalPinToInterrupt(I_UP));
    detachInterrupt(digitalPinToInterrupt(I_DOWN));
    upAttached = false;
    downAttached = false;
    if (!rightAttached) { attachInterrupt(digitalPinToInterrupt(I_RIGHT), isr_right, FALLING); };
    if (!leftAttached) { attachInterrupt(digitalPinToInterrupt(I_LEFT), isr_left, FALLING); };
    leftAttached = true;
    rightAttached = true;
  }
}

// Define interrupt methods
void isr_left() {
  if(moveVector != RIGHT && snake[0][2] != RIGHT) {
    moveVector = LEFT; 
    handleInterrupts();
  }
}

void isr_right() {
  if(moveVector != LEFT && snake[0][2] != LEFT) {
    moveVector = RIGHT;
    handleInterrupts();
  }
}

void isr_up() {
  if(moveVector != DOWN && snake[0][2] != DOWN) {
    moveVector = UP;
    handleInterrupts();
  }
}

void isr_down() {
  if(moveVector != UP && snake[0][2] != UP) {
    moveVector = DOWN;
    handleInterrupts();
  }
}

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
  while(true) {
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

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(I_UP), isr_up, LOW);
  attachInterrupt(digitalPinToInterrupt(I_DOWN), isr_down, LOW);
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
  if(checkCollision()) {
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