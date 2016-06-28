#include <Adafruit_GPS.h>
#include <Wire.h>
#include <SoftwareSerial.h>

// GPS Pins: 5v/Ground - TX: 3, RX: 2
SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);

#define SURETRAC_I2C_SLAVE_ADDR 0x08

// Uncomment if you need to DEBUG the Arduino/Adafruit and have it write to Serial Console
//#define DEBUG 

void setup() {
  Wire.begin(SURETRAC_I2C_SLAVE_ADDR);
  Wire.onRequest(requestHandler);

  #ifdef DEBUG
  Serial.begin(115200);
  #endif

  // GPS Code
  GPS.begin(9600);
  GPS.sendCommand(PMTK_API_SET_FIX_CTL_1HZ); // Fix Position every second
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

  #ifdef DEBUG
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA); // DEBUG
  #else
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY); // Get Only Position
  #endif
}

String currentSentence = "";
String sentenceBuffer = "";
String sentenceBufferBuffer = "";
int i = 0;

// I2C Interrupt - Sends data to Omega whenever Omega requests latest data
void requestHandler() {
  Wire.write(currentSentence.charAt(i));
  i++;
  
  if (i > currentSentence.length()) {
    i = 0;
    currentSentence = sentenceBuffer;
  }
}

/*
 * This function checks if the sentence is valid and RMC - then extract lat/long,
 * convert degrees into seconds and then store it as the latest data for
 * the I2C bus to read.
 */
void setData(String sentence) {
  sentence = verify(sentence);

  if (sentence != "") {
      #ifdef DEBUG
      sentenceBuffer = sentence;
      Serial.println(sentenceBuffer);
      #else 
      if (sentence.substring(0, 6) == "$GPRMC") {
          sentenceBuffer = sentence;
      }
      #endif
  }
}

/*
 * We first verify the checksum on the data
 * NMEA checksums are always the two bytes of the sentence immediately following an '*'
 * 
 * The Sum is calculated as the XOR of all the bytes between the $ and * of the sentence (Exclusive)
 */
String verify(String sentence) {
  sentence = sentence.substring(3, sentence.length()-1);
  int calcCheckSum = sentence[0];
  String originalCheckSum = "";
  originalCheckSum += GPS.read();
  originalCheckSum += GPS.read();

  for (int i = 1; i < sentence.length(); i++) {
     calcCheckSum = sentence[i] ^ calcCheckSum;
  }

  String finalCalcSum = String(calcCheckSum, HEX);
  finalCalcSum.toUpperCase();

  if (originalCheckSum == finalCalcSum) {
    return ("$" + sentence + "*" + originalCheckSum);
  } else {
    return "";
  }
}

void loop() {
  char c = GPS.read();

  if (c) {
    sentenceBufferBuffer += c;
    
    if (c == '*') {
      setData(sentenceBufferBuffer);
      sentenceBufferBuffer = "";
    }
  }
}



