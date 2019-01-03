//press and F# key
//press key
//  press another key
//    if F# then stop
//  count delta time
//  write both to file
//loop

#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

File dataFile;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200); //this speed does not matter
  //50ms between characters = 200cps -> the kps from the host

  
  Serial.println("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    Serial.println("Start typing");
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
    while (true) { ;}
  }

  dataFile.close();
}

void loop() {
  //first character
  while (Serial.available() == 0) { ;} //wait for character
  long start = millis(); char a = (char)Serial.read();

  //NEED TO RECORD FIRST CHAR -> 'char',0
  
  while (true) {
    while (Serial.available() == 0) { ;} //wait for character
    char b = (char)Serial.read();
    if (b == 0x1B) { checkF(); } //validate function key press
    long next = millis();

    //delta
    long delta = next - start;
    Serial.print("delay: "); Serial.println(delta);
    start = next;

    //create log file line
    String num = String(delta, DEC);
    String c = String(b);
    String val = "'" + c + "'," + num;

    dataFile = SD.open("datalog.txt", FILE_WRITE);
    dataFile.println(val);
    Serial.println(val);
    dataFile.close(); //have to close the file to ensure commit data
  }
}

//function key format
// 0x1B
// [
// 1
// F1 = 1, F2 = 2, ...
// ~
void checkF() {
  while (Serial.available() == 0) { ;} //wait for character
  char s = (char)Serial.read(); //should be [
  if (s != '[') { return }
  while (Serial.available() == 0) { ;} //wait for character
  char one = (char)Serial.read(); //should be 1
  if (one != '1') { return }
  while (Serial.available() == 0) { ;} //wait for character
  char F = (char)Serial.read(); //should be 1-9
  if (F < '1' or F > '9') { return }
  while (Serial.available() == 0) { ;} //wait for character
  char end = (char)Serial.read(); //should be ~
  if (one != '~') { return }
  
  //now start recording for F#
}

//if detect function key
//  enter recording mode, should bells still play?
//loop
//if detect function key
//  stop recording

