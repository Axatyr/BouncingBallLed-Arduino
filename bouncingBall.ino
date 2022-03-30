/*
 * Alessandro Palladino 2022
 * Info Assignment:
 *  1 red ledState
 *  1 potentiometer
 *  4 green ledState
 *  4 tactile buttons
 *  
 * Scopo del gioco:
 *  Fermare la pallina che rimbalza da un ledState all'altro attraverso i pulsanti collegati ai ledState.
 *  
 * Dettagli:
 * Stati del gioco:
 *  -Riposo: qualsiasi tasto risveglia il gioco
 *  -In Attesa: premendo il tasto T1 il gioco parte
 *  -In gioco: lo stato continua fino a quando l'utente non sbaglia
 *  Se nello stato di attesa non viene premuto il tasto T1 entro 10 secondi il gioco entra in modalità riposo
 *  
 * Funzionamendo del gioco:
 *  I ledState verdi si accendono e spengono in sequenza scorrendo avanti e indietro ad una velocità S e continuano per un tempo randomico T.
 *  Allo scadere del tempo la posizione in cui il ledState resta acceso sarà la posizione P.
 *  Collegata ad ogni posizione è presente un pulsante che dovrà essere premuto entro un determinato tempo T2 prestabilito.
 *  Se viene premuto entro il tempo limite allora lo score incrementerà e il tempo T2 diminuirà di un fattore F
 *  Se non viene premuto in tempo si avrà game over, ci sarà quindi una print su terminale per 10 secondi, dopodichè il gioco ripartirà dallo stato iniziale
 *  Prima di iniziare il gioco il potenziometro può essere utilizzato per decidere il level di difficoltà da cui partire in un range da 1 a 8
 *  Ciò su cui inciderà il potenziometro sarà il level del fattore F.
 * 
 */
#include <EnableInterrupt.h> 
#include <avr/sleep.h>

const int numLED = 4; //Number of Led
const int numBUTTON = 4; //Number of Button 
const int ledPin[4] = {11, 10, 9, 6}; //Position Led-Pin
const int buttonPin[4] = {12, 8, 7, 4}; //Position Button-Pin

#define ledState 5 //Position LedRed-Pin
int fadeAmount; //Manage fading
int currIntensity; //Manage fading

#define potPin A0 //Position Potentiometer
int currPot;
int levelPot;
/*
  level map for Potentiometer
  level 1 0 - 127
  level 2 128 - 255
  level 3 256 - 383
  level 4 384 - 511
  level 5 512 - 639
  level 6 640 - 767
  level 7 768 - 895
  level 8 896 -1023

  result = 1023:analogValueRilevate = 8 : levelCorrelate
  levelCorr = result+1;
*/

//Game setting
volatile int state = 0; //0 Waiting, 1 Gameloop, 2 Sleeping
const float factorConst = 1.20; //Factor default - level of difficulty
const unsigned long sleepTimer = 10000; //To go to sleep, also used for finish game
bool startCorrectTime = false; //Start the second time and if button pressed during this time the answer is right
int correctAnswer; //Used to check answer, 0 waiting, 1 right, 2 wrong
int rightPosition; //Save the position of ball
int i; //Index

void setup() {
  Serial.begin(9600);
  
  for(i=0; i<numLED; i++){
    pinMode(ledPin[i], OUTPUT);
    pinMode(buttonPin[i], INPUT);
  }

  enableInterrupt(buttonPin[0], goToGameLoop, RISING); //Set first button to start the game

  //Led state setting
  pinMode(ledState, OUTPUT);
  currIntensity = 0; 
  fadeAmount = 50; 
  //Potentiometer setting
  currPot = 0; 
}

void loop() {

  waiting(); //Initial state
  
}

//Wait for start the game
void waiting(){
  enableInterrupt(buttonPin[0], goToGameLoop, RISING); //Set first button to start the game
  bool start = false;
  bool sleep = false;

  Serial.println("Welcome to the Catch the Bouncing Led Ball Game. Press Key T1 to Start ");
  unsigned long startSleepTimer = millis(); //Start count for timer sleep
  
  while(millis() - startSleepTimer < sleepTimer && state==0) {
    //Fading red led
    digitalWrite(ledState, HIGH);
    delay(1000);
    digitalWrite(ledState, LOW);   
    delay(1000);

    //Potentiometer management
    int newValue = analogRead(potPin);
    if( newValue != currPot) {
      currPot = newValue; 
    }
  }

  if(state == 1){
    gameloop();
  } else {
    state = 2;
    Serial.println("Going to sleep mode");
    delay(100);
    sleeping();
  }
}

//Run the game
void gameloop(){
  digitalWrite(ledState, LOW); //Reset ledState

  for (i = 0; i < numBUTTON; i++) {
        disableInterrupt(buttonPin[i]);
  }
  
  bool gameover = false;
  int score = 0;
  
  levelPot = ((currPot*8)/1023)+1; //Calculate starting level
  
  //Set button interrupt
  enableInterrupt(buttonPin[0], checkPosition0, RISING);
  enableInterrupt(buttonPin[1], checkPosition1, RISING);
  enableInterrupt(buttonPin[2], checkPosition2, RISING);
  enableInterrupt(buttonPin[3], checkPosition3, RISING);
  
  Serial.println("Go!");
  
  while(!gameover){

    float factor = pow(factorConst, levelPot); //Set factor
    float speedBall = 2000/factor; //Set speed
    float secondTime = 5000/factor; //Set time to reply
    int ledPos = 0; //Start position of ball
    int way = 0; //Define if next ball pos will go at right or left
    correctAnswer = 0; //Used to check answer

    float firstTime = random(4000,8000); //Random time when ball can change position
    unsigned long startFirstTimer = millis(); 

    Serial.print("\nLevel: ");
    Serial.print(levelPot);

    //Start bouncing ball
    while(millis() - startFirstTimer < firstTime) {
      //Check position to change side
      if(ledPos == 3){ 
        way = -1;
      }
      ledPos += way;

      if(ledPos >= 0 && ledPos <= 3){
        digitalWrite(ledPin[ledPos-way], LOW);
      }
      
      digitalWrite(ledPin[ledPos], HIGH);
      delay(speedBall);

      //Check position to change side
      if(ledPos == 0){
        way = 1;
      }
    }
   
    rightPosition = ledPos; //Save last position to check answer
    unsigned long startSecondTimer = millis();
    startCorrectTime = true; //From now user can select one button
    while(millis() - startSecondTimer < secondTime) {
      //Wait for input user
    }
    startCorrectTime = false; //End time to get reply from user

    //Turn off all led
    for(i = 0; i<numLED; i++){
      digitalWrite(ledPin[i], LOW);
    }

    //Check the answer
    if(correctAnswer == 1){
      score++;
      Serial.print("\nNew point! Score: ");
      Serial.print(score);

      if(levelPot == 8){
        Serial.print("\nCongratulations! You have completed the game! Final Score: ");
        Serial.print(score);
        Serial.print("\n");
        gameover = true;
        state = 0;
      }

      levelPot++;
    } else {
      Serial.print("\nGame Over. Final Score: ");
      Serial.print(score);
      Serial.print("\n");
      gameover = true;
      state = 0;
    }
  } 

  for (i = 0; i < numBUTTON; i++) {
    disableInterrupt(buttonPin[i]);
  }

  delay(sleepTimer);
  if(state == 0){
    waiting();
  }
}

//Go to sleep mode
void sleeping(){
  for (i = 0; i < numBUTTON; i++) {
        disableInterrupt(buttonPin[i]);
        enableInterrupt(buttonPin[i], goToWaiting, RISING);
  }

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
  sleep_disable();
  
  for (i = 0; i < numBUTTON; i++) {
    disableInterrupt(buttonPin[i]);
  }

  if(state == 0){
    waiting();
  }
}

void checkPosition(int position){
  if(startCorrectTime == true){
    //Time reply correct, check right position
    if(rightPosition == position) {
      correctAnswer = 1; //Right
    } else {
      correctAnswer = 2; //Error
    }
  } else {
    //End of time, gameover
    correctAnswer = 2; 
  }
}

//Function used to pass value of button pressed
void checkPosition0(){
  checkPosition(0);
}

void checkPosition1(){
  checkPosition(1);
}

void checkPosition2(){
  checkPosition(2);
}

void checkPosition3(){
  checkPosition(3);
}

//Function used to change state of game
void goToGameLoop(){
  state = 1;
}

void goToWaiting(){
  state = 0;
}