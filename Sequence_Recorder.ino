//creates recording files from F1 to F8
//plays back recorded files

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
    return;
  }
  Serial.println("card initialized.");
  f_name_which = 0; recording = false;
}

void loop() {
  while (Serial.available() == 0) { ;} //wait for character
  long next = millis(); char b = (char)Serial.read();

  if (b == 0x1B) { //function key
    FCheck();
    while (Serial.available() == 0) { ;} //wait for character
    long next = millis(); b = (char)Serial.read(); //get next character after F#
  }

  if (b == '*') { //player mode
    Serial.println("Player Mode");
    while (Serial.available() == 0) { ;} //wait for character
    b = (char)Serial.read(); //get next character after F#
    F_Player(b);
  }

  if (recording) {
    long delta = next - start;
    Serial.print("delay: "); Serial.println(delta);
    start = next;

    //create log file line
    String num = String(delta, DEC); String c = String(b);
    String val = "'" + c + "'," + num;

    F_Write_Opener(); //open correct file
  
    dataFile.println(val); Serial.println(val); //save, puts the /r/n into text
    
    dataFile.close(); //have to close the file to ensure commit data
  }
}

void F_Write_Opener () {
  switch (f_name_which) { //open correct file
    case '1':
      dataFile = SD.open("ONE.TXT", FILE_WRITE); break; //files always seem to b ucase
    case '2':
      dataFile = SD.open("TWO.TXT", FILE_WRITE); break;
    case '3':
      dataFile = SD.open("THREE.TXT", FILE_WRITE); break;
    case '4':
      dataFile = SD.open("FOUR.TXT", FILE_WRITE); break;
    case '5':
      dataFile = SD.open("FIVE.TXT", FILE_WRITE); break;
    case '6':
      dataFile = SD.open("SIX.TXT", FILE_WRITE); break;
    case '7':
      dataFile = SD.open("SEVEN.TXT", FILE_WRITE); break;
    case '8':
      dataFile = SD.open("EIGHT.TXT", FILE_WRITE); break;
    default:
      return; break;
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

void F_Player (char f_name) {
  switch (f_name) { //open correct file
    case '1':
      dataFile = SD.open("ONE.TXT", FILE_READ); break;
    case '2':
      dataFile = SD.open("TWO.TXT", FILE_READ); break;
    case '3':
      dataFile = SD.open("THREE.TXT", FILE_READ); break;
    case '4':
      dataFile = SD.open("FOUR.TXT", FILE_READ); break;
    case '5':
      dataFile = SD.open("FIVE.TXT", FILE_READ); break;
    case '6':
      dataFile = SD.open("SIX.TXT", FILE_READ); break;
    case '7':
      dataFile = SD.open("SEVEN.TXT", FILE_READ); break;
    case '8':
      dataFile = SD.open("EIGHT.TXT", FILE_READ); break;
    default:
      Serial.println(" play file not found");
      return; break;
  }
  
  while (dataFile.available()) { //loop through file
    String line = "";
    char in = 0x00; //NULL
    while (in != '\n') { //get 1 line
      in = dataFile.read();
      line = line + String(in);
    }
    
    int len = line.length();
    char note = line.charAt(1);
    String duration = line.substring(4);
    int dur = duration.toInt();

    //call player funciton here
    Serial.print("line: ");
    Serial.println(line); //test found line of text
    Serial.print("note: ");
    Serial.println(note);
    Serial.print("Dur: ");
    Serial.println(dur);
    
    long start = millis();
    if (dur <= 0) { break; }
    while (millis() - start < dur) { ;}
  }

  dataFile.close();
  Serial.println("Done Playing");
}

