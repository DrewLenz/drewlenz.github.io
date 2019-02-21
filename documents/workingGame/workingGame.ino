/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x32 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/
#include <EEPROM.h>
#include <workingGameClass.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);



/////////Buttons///////////
const int up_pin = 2;
const int down_pin = 3;
const int left_pin = 18;
const int right_pin = 19;
const int startPin = 22;
/////////End Buttons///////

long debounce_time = 15; // time in ms
volatile unsigned long last_micros;

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// GLOBAL Variables
// Position values to be incremented during button interrupts
int INTERRUPT_X;
int INTERRUPT_Y;
int NUM_FRAMES;
int LEVELS_CLEARED;

void setup()   {                
  setUpScreen();

  // Clear the buffer.
  display.clearDisplay();

  //initialize buttons as pin inputs
  pinMode(up_pin, INPUT);
  pinMode(down_pin, INPUT);
  pinMode(left_pin, INPUT);
  pinMode(right_pin, INPUT);
  pinMode(startPin, INPUT);


  // Initialize Interrupts for directional buttons
  attachInterrupt(digitalPinToInterrupt(up_pin), upPressed, RISING);
  attachInterrupt(digitalPinToInterrupt(down_pin), downPressed, RISING);
  attachInterrupt(digitalPinToInterrupt(left_pin), leftPressed, RISING);
  attachInterrupt(digitalPinToInterrupt(right_pin), rightPressed, RISING);

  INTERRUPT_X = 32;
  INTERRUPT_Y = 0;
  NUM_FRAMES = 0;
  LEVELS_CLEARED = 0;
}

void loop() {

  startScreen();
  
  Frog frog = Frog(32, 0, 8, 8, 0, 1);
  
  // Row 1 cars
  Car1 car1_1 = Car1(0, 8, 8, 8, 1, 1);
  Car1 car1_2 = Car1(24, 8, 8, 8, 1, 1);
  Car1 car1_3 = Car1(48, 8, 8, 8, 1, 1);
  
  // Row 2 cars
  Car2 car2_1 = Car2(56, 16, 8, 8, -4, 1);
  
  // Row 3 buses
  Bus bus1_1 = Bus(56, 24, 16, 8, 2, 1);
  Bus bus1_2 = Bus(0, 24, 16, 8, 2, 1);

  // Row 4 cars
  Car2 car4_1 = Car2(28, 32, 8, 8, -8, 1);

  // Row 5 cars
  Car1 car5_1 = Car1(12, 40, 8, 8, 1, 1);
  Car1 car5_2 = Car1(36, 40, 8, 8, 1, 1);
  Car1 car5_3 = Car1(60, 40, 8, 8, 1, 1);

  // Row 7 Logs
  Log log7_1 = Log(0, 56, 24, 7, -1, 0);
  Log log7_2 = Log(32, 56, 24, 7, -1, 0);
  Log log7_3 = Log(64, 56, 24, 7, -1, 0);
  Log log7_4 = Log(96, 56, 24, 7, -1, 0);

  // Row 8 Logs
  Log log8_1 = Log(12, 64, 32, 7, 2, 0);
  Log log8_2 = Log(60, 64, 32, 7, 2, 0);
  Log log8_3 = Log(108, 64, 32, 7, 2, 0);

  // Row 9 logs
  Log log9_1 = Log(4, 72, 48, 7, -4, 0);
  Log log9_2 = Log(80, 72, 48, 7, -4, 0);

  // Row 10 logs
  Log log10_1 = Log(0, 80, 32, 7, 2, 0);
  Log log10_2 = Log(48, 80, 32, 7, 2, 0);
  Log log10_3 = Log(96, 80, 32, 7, 2, 0);

  // Row 11 logs
  Log log11_1 = Log(0, 88, 24, 7, -1, 0);
  Log log11_2 = Log(32, 88, 24, 7, -1, 0);
  Log log11_3 = Log(64, 88, 24, 7, -1, 0);
  Log log11_4 = Log(96, 88, 24, 7, -1, 0);

  //Homes
  Home home1 = Home(8);
  Home home2 = Home(28);
  Home home3 = Home(48);
    
  Obj *objects[] = {&car1_1, &car1_2, &car1_3, &car2_1, &bus1_1, &bus1_2, &car4_1, &car5_1, &car5_2, &car5_3, 
                    &log7_1, &log7_2, &log7_3, &log7_4, &log8_1, &log8_2, &log8_3, &log9_1, &log9_2, &log10_1,
                    &log10_2, &log10_3, &log11_1, &log11_2, &log11_3, &log11_4};
  Home *homes[] = {&home1, &home2, &home3};
  
  bool collision_detected = false;
  while(1){
    updateFrog(frog);
    incrementObjects(frog, objects);
    collision_detected = detectCollisions(frog, objects, homes);
    drawObjects(frog, objects, homes,collision_detected);
  }
}

void setUpScreen(){
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  
  display.display();
  delay(1);
   
  
}

void startScreen(){

  while (!digitalRead(startPin)){
    display.clearDisplay();
  
    display.setRotation(3);
    
    // text display tests
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("   Hello!\nWelcome to\n Frogging  About!\n");
  
    display.println("Avoid the cars and  cross the river to  get home.\n");
    
    display.println(" Press the  yellow");
    display.println("  start");
    display.println("  button");
    display.println(" to begin!");
    
    
    display.display();
  }
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Starting  game...");
  display.display();
  delay(1000);
  display.setRotation(1);

  return;
}


void updateFrog(Obj &f)
{
  // Update frog position to global interrupt position
  f.setX(INTERRUPT_X);
  f.setY(INTERRUPT_Y);

  // Update color if the frog is over the river
  if(f.Y() > 48){
    f.setColor(0);
  }
  else{
    f.setColor(1);
  }

  // Update speed to 0 if frog is on land
  if(f.Y() <= 48)
  {
    f.set_Speed(0);
  }
}

// Function to increment the X position of the frog and objects
// Parameters:
//    frog Obj passed by reference
//    objects Ptr to array of Obj ptrs
// Returns: void
void incrementObjects(Obj &frog, Obj **objects)
{
  int new_pos;
  for (int i = 0; i < 26; i++){
    // For each item in object, set a new position
    // new_pos = X_position + (Speed + (2 * number of levels cleared))
    // Using the number of levels cleared in this equation allows for the speed of objects to
    // Increase as the player beats levels
    new_pos = (objects[i]->X()+((objects[i]->Speed()+2*LEVELS_CLEARED)));

    if (objects[i]->Speed() > 0 && new_pos > 104)
    {
      // Set new_pos to -8 if a positive direction object clears 96
      // 96 is chosen to allow for a delay when the objects wraps back around the screen
      // -8 is chosen to allow for the object to appear bit by bit when re-entering the screen
      new_pos = -32;
    }
    
    if(objects[i]->Speed() < 0 && new_pos < -objects[i]->Width())
    {
      new_pos = 104;
    }
    objects[i]->setX(new_pos);
  }
  new_pos = frog.X()+(frog.Speed()+2*LEVELS_CLEARED);
  INTERRUPT_X = new_pos;
  frog.setX(new_pos);
}

// Looks at the location of frog and all objects to detect collisions
// Parameters:
//    frog Obj passed by reference
//    objects Ptr to array of Obj ptrs
// Returns true if collision found, otherwise returns false
bool detectCollisions(Obj &frog, Obj **objects, Home **homes)
{
  
  if(frog.Y() == 8){
    // Frog is in row 1, compare x position with each object in row 1
    for(int i = 0; i < 3; i++)
    {
      // +- Width - 1 used to ensure any part of frog touching object is registered as collision
      if (frog.X() >= (objects[i]->X() - objects[i]->Width() - 1) && frog.X() <= (objects[i]->X() + objects[i]->Width() - 1)){ 
        return true;
      }
    }
  }
  else if(frog.Y() == 16){
    // Frog is in row 2, compare x position with car in row 2
    if ((frog.X() >= objects[3]->X()- objects[3]->Width() - 1) && frog.X() <= (objects[3]->X()+ objects[3]->Width() - 1)){
      return true;
    }
  }
  else if(frog.Y() == 24){
    // Frog is in row 3, compare with each bus in row 3
    for(int i = 4; i < 6; i++){
      // -7 is used to account for the bus objects being twice as long as the frog
      if((frog.X() >= objects[i]->X()-7) && frog.X() <= (objects[i]->X()+objects[i]->Width() - 1)){
        return true;
      }
    }
  }
  else if(frog.Y() == 32){
    // Frog is in row 4, compare x position with car in row 4
    if ((frog.X() >= objects[6]->X()-objects[6]->Width() - 1)&& frog.X() <= (objects[6]->X()+objects[6]->Width() - 1)){
      return true;
    }
  }
  else if(frog.Y() == 40){
    // Frog is in row 5, compare x position with each car in row 5
    for (int i = 7; i < 10; i++){
      if((frog.X() >= objects[i]->X()-objects[i]->Width()-1) && frog.X() <= (objects[i]->X()+objects[i]->Width() - 1)){
        return true;
      }
    }
  }
  else if(frog.Y() == 56){
    for (int i = 10; i < 14; i++){
      if((frog.X() >= objects[i]->X()) && frog.X() <= (objects[i]->X()+objects[i]->Width() - 8)){
        frog.set_Speed(objects[i]->Speed());
        frog.setColor(1);
        if(frog.X() < 0 || frog.X() > 56){
          return true;
        }
        else{
          return false; 
        }
      }
    }
    return true;
  }
  else if(frog.Y() == 64){
    for (int i = 14; i < 17; i++){
      if((frog.X() >= objects[i]->X()) && frog.X() <= (objects[i]->X()+objects[i]->Width() - 8)){
        frog.set_Speed(objects[i]->Speed());
        frog.setColor(1);
        if(frog.X() < 0 || frog.X() > 56){
          return true;
        }
        else{
          return false; 
        }
      }
    }
    return true;
  }
  else if (frog.Y() == 72){
    for (int i = 17; i < 19; i++){
      if((frog.X() >= objects[i]->X()) && frog.X() <= (objects[i]->X()+objects[i]->Width() - 8)){
        frog.set_Speed(objects[i]->Speed());
        frog.setColor(1);
        if(frog.X() < 0 || frog.X() > 56){
          return true;
        }
        else{
          return false; 
        }
      }
    }
    return true;
  }
  else if (frog.Y() == 80){
    for (int i = 19; i < 22; i++){
      if((frog.X() >= objects[i]->X()) && frog.X() <= (objects[i]->X()+objects[i]->Width() - 8)){
        frog.set_Speed(objects[i]->Speed());
        frog.setColor(1);
        if(frog.X() < 0 || frog.X() > 56){
          return true;
        }
        else{
          return false; 
        }
      }
    }
    return true;
  }
  else if (frog.Y() == 88){
    for (int i = 22; i < 26; i++){
      if((frog.X() >= objects[i]->X()) && frog.X() <= (objects[i]->X()+objects[i]->Width() - 8)){
        frog.set_Speed(objects[i]->Speed());
        frog.setColor(1);
        if(frog.X() < 0 || frog.X() > 56){
          return true;
        }
        else{
          return false; 
        }
      }
    }
    return true;
  }
  else if (frog.Y() == 96){
    // Frog is in the home row, determine if collision with home (good) or wall (bad)
    // Compare frog.X with the location of each home
    for (int i = 0; i < 3; i++){
      // +- 3 to allow the user to get most of the frog in the home without having pixel perfect allignment
      if (frog.X() >= homes[i]->Location() - 3 && frog.X() <= homes[i]->Location() + 3){
        if (homes[i]->prevOccupied()){
          // If home is previously occupied, collision is detected/frog dies
          return true;
        }
        else{
          // Frog is now in home, update home's occupied status and return no collision
          homes[i]->setOccupied(true);
          return false; 
        }
      }
    }
    // Frog collided with wall instead of home, collision detected
    return true;
  }  
  else if (frog.X() < 0 || frog.X() > 56){
    // Frog has jumped off screen
    return true;
  }
  else if (frog.Y() < 0){
    // Frog has jumped off screen
    return true;
  }

  // Otherwise, no collisions have been detected
  return false;
}

// This function draws each object
// Parameters:
//    frog Obj passed by reference
//    objects Ptr to array of Obj ptrs 
//    collision_detected bool to indicate collision status
// Returns: void
void drawObjects(Obj &frog, Obj **objects, Home **homes, bool collision_detected)
{
  // Draw background
  display.fillRect(0, 55, 64, 41, 1);
  display.fillRect(8, 96, 8, 8, 1);
  display.fillRect(28, 96, 8, 8, 1);
  display.fillRect(48, 96, 8, 8, 1);
  
  // For each object, draw its bitmap
  for (int i = 0; i < 10; i++)
  {
    display.drawBitmap(objects[i]->X(), objects[i]->Y(), objects[i]->Bitmap, objects[i]->Width(), objects[i]->Height(), objects[i]->Color());
  }


  // Draw logs as rectangles, unsure why this won't support doing all this in one loop but we need to keep moving
  for (int i = 10; i < 26; i++){
    display.fillRect(objects[i]->X(), objects[i]->Y(), objects[i]->Width(), objects[i]->Height(), 0);
  }

  // Draw the frog bitmap
  display.drawBitmap(frog.X(), frog.Y(), frog.Bitmap, frog.Width(), frog.Height(), frog.Color());

  // Display the drawings
  display.display();

  if (collision_detected){
    // Flash screen to indicate loss of life
    for (int i = 0; i < 2; i++){
      display.invertDisplay(true);
      delay(100);
      display.invertDisplay(false);
      delay(100);
    }

    // Reset Frog X and Y positions
    INTERRUPT_X = 32;
    INTERRUPT_Y = 0;
    frog.setX(32);
    frog.setY(0);
    frog.set_Speed(0);
  }

  for (int i = 0; i < 3; i++){
    if(homes[i]->isOccupied()){
      display.drawBitmap(homes[i]->Location(), 96, frog.Bitmap, 8, 8, 0);
      if(!homes[i]->prevOccupied()){
        INTERRUPT_X = 32;
        INTERRUPT_Y = 0;
        frog.setX(32);
        frog.setY(0);
        homes[i]->setPrevOccupied(true);
      }
    }
  }

  // Redraw frog and display
  display.drawBitmap(frog.X(), frog.Y(), frog.Bitmap, frog.Height(), frog.Width(), frog.Color());
  display.display();

  // Delay for 25 ms to allow smooth motion
  delay(25);

  // Increment number of frames for object animation and timer
  NUM_FRAMES++;

  // Clear display for next drawing
  display.clearDisplay();
}


// ISRs

// Interrupt service routine for up button
// Returns: void
void upPressed(){
  if((long)(micros() - last_micros) >= debounce_time*1000){
    // Increment Y position
    INTERRUPT_Y = INTERRUPT_Y + 8;
    last_micros = micros();
  }
}

// Interrupt service routine for down button
// Returns: void
void downPressed(){
  if((long)(micros() - last_micros) >= debounce_time*1000){
    // Decrement Y position
    INTERRUPT_Y = INTERRUPT_Y - 8;
    last_micros = micros();
  }
}

// Interrupt service routine for left button
// Returns: void
void leftPressed(){
  if((long)(micros() - last_micros) >= debounce_time*1000){
    // Increment X position
    INTERRUPT_X = INTERRUPT_X + 8;
    last_micros = micros();
  }
}

// Interrupt service routine for right button
// Returns: void
void rightPressed(){
  if((long)(micros() - last_micros) >= debounce_time*1000){
    // Decrement X position
    INTERRUPT_X = INTERRUPT_X - 8;
    last_micros = micros();
  }
}
