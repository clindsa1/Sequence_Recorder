//creates recording files from F1 to F8

#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

char f_name_which; bool recording; long start;
File dataFile;

void setup() {
  //50ms between characters = 200cps -> the kps from the host
  Serial.begin(115200); //so we need more than 9600
  
  Serial.println("Initializing SD card...");

  if (!SD.begin(chipSelect)) { // see if the card is present and can be initialized:
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  f_name_which = 0; recording = false;
}

void loop() {
  while (Serial.available() == 0) { ;} //wait for character
  long next = millis(); char b = (char)Serial.read();

  if (b == 0x1B) {
    FCheck();
    while (Serial.available() == 0) { ;} //wait for character
    long next = millis(); b = (char)Serial.read(); //get next character after F#
  }

  if (recording) {
    long delta = next - start;
    Serial.print("delay: "); Serial.println(delta);
    start = next;

    //create log file line
    String num = String(delta, DEC); String c = String(b);
    String val = "'" + c + "'," + num;

    F_Opener(); //open correct file
  
    dataFile.println(val); Serial.println(val); //save
    
    dataFile.close(); //have to close the file to ensure commit data
  }
}

void F_Opener () {
  switch (f_name_which) { //open correct file
    case '1':
      dataFile = SD.open("one.txt", FILE_WRITE); break;
    case '2':
      dataFile = SD.open("two.txt", FILE_WRITE); break;
    case '3':
      dataFile = SD.open("three.txt", FILE_WRITE); break;
    case '4':
      dataFile = SD.open("four.txt", FILE_WRITE); break;
    case '5':
      dataFile = SD.open("five.txt", FILE_WRITE); break;
    case '6':
      dataFile = SD.open("six.txt", FILE_WRITE); break;
    case '7':
      dataFile = SD.open("seven.txt", FILE_WRITE); break;
    case '8':
      dataFile = SD.open("eight.txt", FILE_WRITE); break;
  }

  if (!dataFile) { //something has gone very wrong
    Serial.println("error opening datalog.txt");
    while (true) { ;} //infinte loop
  }
}

void FCheck () { //function key format 0x1B, [, 1, F1 = 1 F2 = 2..., ~
  //the 0x1B was parsed by calling function
  while (Serial.available() == 0) { ;} //wait for character
  char s = (char)Serial.read(); //[
  while (Serial.available() == 0) { ;} //wait for character
  char o = (char)Serial.read(); //1
  while (Serial.available() == 0) { ;} //wait for character
  char f = (char)Serial.read(); //F#
  while (Serial.available() == 0) { ;} //wait for character
  char e = (char)Serial.read(); //~

  //currently no error checking

  //start recorder
  f_name_which = f;
  recording = not(recording);
  if (recording) { //prompt
    Serial.print(f); Serial.println(" START");
    start = millis();
  } else {
    Serial.print(f); Serial.println(" STOP");
  }
}
