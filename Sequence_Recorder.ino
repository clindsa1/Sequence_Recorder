//creates recording files from F1 to F8
//files Sequence_Recordersaved to SD card
//plays back recorded files

#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

char f_name_which; bool recording; long start;
File dataFile;
int mode; //0=stoped, 1=keyboard, 2=playing, 3=recording

void setup() {
  //50ms between characters = 200cps -> the kps from the host
  Serial.begin(115200); //so we need more than 9600
  
  Serial.println("Initializing SD card...");

  if (!SD.begin(chipSelect)) { // see if the card is present and can be initialized:
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("Card initialized.");
  f_name_which = '0'; recording = false;
  mode = 0;
}

void loop () {
  while (Serial.available() == 0) { ;} //wait for character
  long next = millis(); char input = (char)Serial.read();
  Serial.println(input);
  switch (mode) {
    case 0: //stopped
      //waiting for a mode character
      switch (input) {
        case '1':
          Serial.println("Keyboard");
          mode = 1; break;
        case '2':
          Serial.println("Player");
          mode = 2; break;
        case '3':
          Serial.println("Recording");
          mode = 3; break;
        //default: does nothing
      }
      break;
    case 1: //keyboard
      //keys play notes
      if (input == '0') { //stop goes back to mode 0
        mode = 0;
        Serial.println("Stop Playing");
      } else {
        //!!!call note player here!!!
      }
      break;
    case 2: //playing
      //next character is which file to play
      while (Serial.available() == 0) { ;} //wait for character
      f_name_which = (char)Serial.read();
      FNum_Player (f_name_which); //test
      mode = 0;//goes back to mode 0 when done
      break;
    case 3: //recording
    //!!!!!!!!!!!!!!!!!!!!!!!!
    //need to get character for which file to write, test, then set recording = true
    //!!!!!!!!!!!!!!!!!!!!!!!!
      if (recording) {
        //keys play and save
        if (input == '0') { //stop goes back to mode 0
          recording = false; mode = 0; f_name_which = 0;
        } else {
          //!!!call note player here!!!

          long delta = next - start;
          Serial.print("delay: "); Serial.println(delta);
          start = next;

          //create log file line
          String num = String(delta, DEC); String c = String(input);
          String val = "'" + c + "'," + num; //the quotes make the file CSV, but are not needed for us

          //save
          FNum_File_Opener (false, f_name_which);
          dataFile.println(val); Serial.println(val); //save, puts the \r\n into text
          dataFile.close(); //have to close the file to ensure commit data

          start = next; //may want to time the file open-write-close block
        }
      } else if (input == 0x1B) {
        //user needs to press an F# key
        char f = FNum_Press_Check ();
        FNum_File_Opener (false, f);
        f_name_which = f; recording = true;
      }      
      break;
    default: //FAIL
      Serial.println("Mode Error!");
      while (true) { ;} //infinite loop
      break;
  }
}

void FNum_File_Opener (bool RW, char file) {
  int fmode = (RW ? FILE_READ : FILE_WRITE); //other option is to pass RW as int constants in call
  switch (file) { //open correct file
    case '1':
      dataFile = SD.open("ONE.TXT", fmode); break; //files always seem to b ucase
    case '2':
      dataFile = SD.open("TWO.TXT", fmode); break;
    case '3':
      dataFile = SD.open("THREE.TXT", fmode); break;
    case '4':
      dataFile = SD.open("FOUR.TXT", fmode); break;
    case '5':
      dataFile = SD.open("FIVE.TXT", fmode); break;
    case '6':
      dataFile = SD.open("SIX.TXT", fmode); break;
    case '7':
      dataFile = SD.open("SEVEN.TXT", fmode); break;
    case '8':
      dataFile = SD.open("EIGHT.TXT", fmode); break;
    default:
      return; break;
  }

  if (!dataFile) { //something has gone very wrong
    Serial.println("error opening datalog");
    while (true) { ;} //infinte loop
  }
}

char FNum_Press_Check () { //function key format 0x1B, [, 1, F1 = 1 F2 = 2..., ~
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

  return (f);
}

void FNum_Player (char f_name) { //plays the contents of a single file
  FNum_File_Opener(true, f_name);
  Serial.println("Playing File");
  
  while (dataFile.available()) { //loop through file
    String line = ""; char in = 0x00; //NULL
    while (in != '\n') { line = line + String(dataFile.read()); } //get 1 line

    //it just seemed to be easier to create a line string then parse it
    int len = line.length(); char note = line.charAt(1);
    String duration = line.substring(4); int dur = duration.toInt();

    //!!!call note player!!!
    Serial.print("line: "); Serial.println(line); //test found line of text
    Serial.print("note: "); Serial.println(note); 
    Serial.print("duration: "); Serial.println(dur);
    
    long start = millis();
    if (dur <= 0) { break; } //no duration is kill condition
    while (millis() - start < dur) { ;} //wait till duration
  }

  dataFile.close();
  Serial.println("Done Playing File");

  while (Serial.available() > 0) { //remove anything sent to Arduino during playback
    char s = (char)Serial.read();
  }
}

