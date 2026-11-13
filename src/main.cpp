#include <Arduino.h>
#include <SoftwareSerial.h>


// LED on Pin 13 (Built-in LED)
const int ledPin = 13;

constexpr int numRows = 2;
constexpr int numCols = 3;

//
const int rows[numRows] = { 10, 9};
const int cols[numCols] = {6,5,4};

// Track the currently pressed key
int lastRow = -1;
int lastCol = -1;

// Create SoftwareSerial: RX pin 11, TX pin 12
SoftwareSerial printerSerial(11, 12);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize SoftwareSerial for printer
  printerSerial.begin(115200);
  delay(1000);
  
  // Set LED pin as output
  pinMode(ledPin, OUTPUT);

  Serial.println("G-Code Keypad started!");
  for (int i=0;i<numRows;i++) {
    pinMode( rows[i], OUTPUT);
  }
  for (int i=0;i<numCols;i++) {
    pinMode( cols[i], INPUT_PULLUP);
  }
  for (int i=0;i<numRows;i++) {
    digitalWrite( rows[i], HIGH);
  }
  
  printerSerial.println("G91");
  Serial.println("G91");
}

void printGCode(int row, int col) {
  char axis[5];
  
  // Determine axis: col 0 = X, col 1 = Y, col 2 = Z
  if (col == 0) {
    strcpy(axis, "X");
  } else if (col == 1) {
    strcpy(axis, "Y");
  } else {
    strcpy(axis, "Z");
  }
  
  // Relative mode: row 0 = negative move, row 1 = positive move
  int distance = (row == 0) ? -5 : 5;
  
  char buf[30];
  snprintf(buf, sizeof(buf), "G1 %s%d F300", axis, distance);
  printerSerial.println(buf);
  Serial.println(buf);
}

void loop() {
  int foundRow = -1;
  int foundCol = -1;
  
  // Scan the keypad matrix
  for (int i=0;i<numRows;i++) {
    digitalWrite( rows[i], LOW);
    delay(5);
    for (int j=0;j<numCols;j++) {
      if (LOW == digitalRead( cols[j]))
      {
        foundRow = i;
        foundCol = j;
      }
    }
    digitalWrite( rows[i], HIGH);
    delay(1);
  }
  
  // If a key is currently pressed
  if ((foundRow != -1) && (foundCol != -1)) {
    // If it's a new key press
    if ((foundRow != lastRow) || (foundCol != lastCol)) {
      lastRow = foundRow;
      lastCol = foundCol;
      digitalWrite(ledPin, HIGH);
    }
    
    // Print G-code continuously while key is pressed
    printGCode(foundRow, foundCol);
    delay(100);
  } 
  else {
    // Key was released
    if ((lastRow != -1) && (lastCol != -1)) {
      lastRow = -1;
      lastCol = -1;
    }
    digitalWrite(ledPin, LOW);
  }

}