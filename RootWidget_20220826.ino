#include <Keyboard.h>
/*
Judge me kindly, the intention in this program is for ease of readability and not performance.

To do:

clean up code, add comments
search for ??? to find other stuff to fix
add stackoverflow pages

good reference for the codes for various keys:
https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardmodifiers/

full list of defined keycodes:
https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.h
*/

#define BUTTON1 6
#define BUTTON2 7
#define BUTTON3 8
#define BUTTON4 9

#define JUMPER1 14
#define JUMPER2 16
#define JUMPER3 10

// leading 0s offset arrays so that the indices correspond to the actual button numbers, e.g. button1 is on nowStates[1]
int BUTTON[] = {0, BUTTON1, BUTTON2, BUTTON3, BUTTON4};
int nowStates[] = {0,1,1,1,1};
int lastStates[] = {0,1,1,1,1};   // assume all buttons start out not depressed, which means they'll read 1 b/c of pullup
long timerStart[] = {0,0,0,0,0};

// default OS mode
int OSMode = 1;                   // 1 = MacOS, 2 = Win, 3 = Linux

void setup() {
  // These inputs don't have pull up resistors on them, so they *must* use the internal pull up mode
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);

  pinMode(JUMPER1, INPUT_PULLUP);
  pinMode(JUMPER2, INPUT_PULLUP);
  pinMode(JUMPER3, INPUT_PULLUP);
  
  // functionally disables the onboard RX/TX LEDs so they're not blinking blinding light all the time
  pinMode(17, INPUT);
  pinMode(30, INPUT);
  
  // make random() more random, less pseudo by grabbing noise from the ADC
  randomSeed(analogRead(4));

  // initialize control over the keyboard
  Keyboard.begin();

  // read the jumper states to determine config; making a weak assumption here that no more than one will be active
  // blink LEDs to indicate that this worked
  if     (digitalRead(JUMPER1)){ OSMode = 1; ledBlinks(1);} 
  else if(digitalRead(JUMPER2)){ OSMode = 2; ledBlinks(2);} 
  else if(digitalRead(JUMPER3)){ OSMode = 3; ledBlinks(3);}
}

void loop() {
 
  for(int i = 1; i<5; i++){
    // record current values of all 4 buttons
    
    nowStates[i] = digitalRead(BUTTON[i]);
    
    // since these are pulled up, they will read 0 if the button is pressed, so the logic is inverted using '!'
    
    
    // if a button is down and it wasn't before, start timing it
    if(!nowStates[i] && lastStates[i]) {timerStart[i] = millis();}
    
    // or if a button was down before and isn't now (i.e. it's been released)
    else if(nowStates[i] && !lastStates[i]){
      
      // if the released button has been held down for more than five seconds...
      if(millis() - timerStart[i] > 5000){
        // then do the press-and-hold fxn
        switch(i) {
          case 1:
            // special case; if 1 and 4 have both been held down, go into hidden alt keys mode
            if(millis() - timerStart[4] > 4000 && millis() - timerStart[4] < 12000){
              OSMode = 0;
              // special '10' code blinks both RX and TX LEDs
              ledBlinks(10);
              break;
            } 
            else {OSMode = 1; ledBlinks(1);}
            break;
          case 2:
            OSMode = 2;
            ledBlinks(2);
            break;
          case 3:
            OSMode = 3;
            ledBlinks(3);
            break;
          case 4:
          // if you hold down 4 for >5 seconds, it'll take you to this very source code!
            launchBrowser();
            Keyboard.println("https://create.arduino.cc/editor/jenesaisdiq/f11c66e6-a6aa-4807-92d9-243253258a4a/preview");
            break;
        }
      }
      else {
        // otherwise, do regular short press functions
        if(OSMode > 0){
          if(i == 1){ exitVim(); }
          else {
            launchBrowser();
            switch(i){
              case 2:
                Keyboard.println("https://root.vc");
                break;
              case 3:
                randomXKCD();
                break;
              case 4:
                randomStackOverflow();
                break;
            }
          }
        }
        else {   // it's in OSMode 0, mappable/alt keys mode
          switch(i){
            /* you can change these to whatever's the most useful. 
            you could also change the starting OSMode on line ~30 to 0 if you always want to do this (if you didn't cut the traces, or resoldered them)
            */
            case 1:
              Keyboard.write(KEY_PAGE_UP);
              break;
            case 2:
              Keyboard.write(KEY_PAGE_DOWN);
              break;
            case 3:
              Keyboard.write(KEY_HOME);
              break;
            case 4:
              Keyboard.write(KEY_END);
              break;
          }
        }
      }
    }
    // make sure to store all the current states for the next loop
    lastStates[i] = nowStates[i];
  }
  
  delay(100); //slow down the loop execution a little bit
} // end of loop()

// this function uses a weird trick where changing the mode of the RX/TX LED pins seems to flash them, no digitalWrite(high) required
void ledBlinks(int times){
  // special blinks
  if(times == 10){
    for(int i = 0; i < 4; i++){
      pinMode(17,OUTPUT);
      pinMode(30,OUTPUT);
      delay(200);
      pinMode(17,INPUT);
      pinMode(30,INPUT);
      delay(200);
    }
  } 
  // regular blinks
  else {
    for(int i = 0; i < times; i++){
      pinMode(17,OUTPUT);
      delay(200);
      pinMode(17,INPUT);
      delay(200);
    }
  }
}

void randomXKCD(){
  // magic number 2600 is here so you don't request a comic that hasn't been written yet. also, 1337
  Keyboard.print("https://xkcd.com/"+(String)random(1,2600));
  Keyboard.write(KEY_RETURN);
}

void randomStackOverflow(){
  int rando = random(0,4);
  switch(rando){
    case 0:
      Keyboard.println("https://stackoverflow.com/questions/1732348/regex-match-open-tags-except-xhtml-self-contained-tags");
      Keyboard.write(KEY_RETURN);
      break;
    case 1:
      Keyboard.println("https://stackoverflow.com/questions/184618/what-is-the-best-comment-in-source-code-you-have-ever-encountered");
      Keyboard.write(KEY_RETURN);
      break;
    case 2:
      Keyboard.println("https://stackoverflow.blog/2017/06/15/developers-use-spaces-make-money-use-tabs/");
      Keyboard.write(KEY_RETURN);
      break;
    case 3:
      Keyboard.println("https://stackoverflow.com/questions/84556/whats-your-favorite-programmer-cartoon");
      Keyboard.write(KEY_RETURN);
      break;
    case 4:
      Keyboard.println("https://stackoverflow.com/questions/1995113/strangest-language-feature");
      Keyboard.write(KEY_RETURN);
      break;
  }
}

// https://phoenixnap.com/kb/how-to-exit-vim
void exitVim(){
  Keyboard.write(KEY_ESC);
  // delay(200);
  // Keyboard.println(":q");
  // delay(200);
  // Keyboard.write(KEY_RETURN);
  delay(400);
  Keyboard.println(":wq");  //if you want to save first
  delay(400);
  Keyboard.write(KEY_RETURN);
}

void launchBrowser(){
  switch(OSMode){
    case 1:
      launchBrowserMac();
      break;
    case 2:
      launchBrowserWin();
      break;
    case 3:
      launchBrowserUbuntu();
      break;
  }
}

void launchBrowserMac(){
  Keyboard.press(KEY_RIGHT_GUI);
  Keyboard.press(' ');
  delay(500);
  Keyboard.releaseAll();
  Keyboard.println("Safari");
  delay(400);
  Keyboard.write(KEY_RETURN);
  Keyboard.releaseAll();
  delay(2000); // wait for browser to launch
  
  // make new tab
  Keyboard.press(KEY_RIGHT_GUI);
  Keyboard.press('t');
  delay(400);
  Keyboard.releaseAll();
  delay(600);
}

void launchBrowserWin(){
  Keyboard.write(KEY_RIGHT_GUI);
  delay(1000);
  Keyboard.releaseAll();
  Keyboard.println("Edge");
  delay(300);
  Keyboard.write(KEY_RETURN);
  Keyboard.releaseAll();
  delay(3000);
  
  // the above code makes a new window, so the below new tab isn't necessary even if chrome's already open
  // make new tab
  // Keyboard.press(KEY_RIGHT_CTRL); 
  // Keyboard.press('t');
  // delay(200);
  // Keyboard.releaseAll();
  // delay(800);
}


// https://linuxhint.com/100_keyboard_shortcuts_linux/
void launchBrowserUbuntu(){
  Keyboard.write(KEY_LEFT_GUI); // is this "super"???
  delay(400);
  Keyboard.releaseAll();
  Keyboard.println("Firefox");
  delay(200);
  Keyboard.write(KEY_RETURN);
  Keyboard.releaseAll();
  delay(3000);

  // open new tab
  Keyboard.press(KEY_RIGHT_CTRL);  // ??? is this the right one
  Keyboard.press('t');
  delay(200);
  Keyboard.releaseAll();
  delay(200);
  
  // focus address bar
  Keyboard.press(KEY_RIGHT_CTRL);  // ??? is this the right one
  Keyboard.press('l');
  delay(200);
  Keyboard.releaseAll();
  delay(200);
}
