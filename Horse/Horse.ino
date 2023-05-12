#include <AFMotor.h>
#include "Helpers.h"
#include "arduinoFFT.h"

arduinoFFT FFT;
/*
These values can be changed in order to evaluate the functions
*/

int in1 = 5;
int in2 = 4;
int in3 = 3;
int in4 = 2;
int ENA = 9;
int ENB = 10;

int iteration = 0;
float i = 0.0;                // Added decimal point to initialize as a float
float start_value = 0.0;      // Changed the data type to float
float end_value = 1.0;        // Changed the data type to float
float increment_value = 0.1;  // Changed the data type to float

const uint16_t samples = 256;            //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 16000;  //Hz, must be less than 10000 due to ADC

unsigned int sampling_period_us;
unsigned long microseconds;

double vReal[samples];
double vImag[samples];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03


int step = 128;
int startMessage = 4096 - step;
int endMessage = startMessage - step;


#include <PDM.h>

#include "NoteCoder.h"

NoteCoder noteCoder = NoteCoder();

// buffer to read samples into, each sample is 16-bits
short sampleBuffer[256];

// number of samples read
volatile int samplesRead;

void setup() {
  //sampling_period_us = round(1000000*(1.0/samplingFrequency));
  sampling_period_us = 1 / 10000;

  Serial.begin(9600);
  // while (!Serial);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  //int _frequencies[3];
  //noteCoder.encodeDirection(_frequencies, 0.456);

  // int *p = &frequencies[0];
  // Serial.println(*p);

  //Serial.println(_frequencies[0]);
  //Serial.println(_frequencies[1]);
  //Serial.println(_frequencies[2]);

  //float direction = noteCoder.decodeDirection(_frequencies);
  //Serial.println(direction, 3);
  PDM.onReceive(onPDMdata);
  if (!PDM.begin(1, samplingFrequency)) {
    Serial.println("Failed to start PDM!");
    while (1)
      ;
  }
}

int frequencies[3];
int currentIndex = -1;
int frequencyIndex = 0;

void loop() {
  if (samplesRead) {
    microseconds = micros();
    for (int i = 0; i < samples; i++) {
      vReal[i] = sampleBuffer[i];
      vImag[i] = 0;
      while (micros() - microseconds < sampling_period_us) {
        //empty loop
      }
      microseconds += sampling_period_us;
    }
    FFT = arduinoFFT(vReal, vImag, samples, samplingFrequency);
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();
    ;


    double x = FFT.MajorPeak();

    int roundedPeak = floor(x / 128) * 128;
    if (roundedPeak == noteCoder.startMessage && currentIndex == -1) {
      Serial.println("Start");
      currentIndex = 0;
      frequencyIndex = 0;
    } else if (frequencyIndex >= 3) {
      currentIndex = -1;
      frequencyIndex = -1;
      double speed1;
      double speed2;

      Serial.print("Freq: ");
      Serial.print(frequencies[0]);
      Serial.print(" | ");
      Serial.print(frequencies[1]);
      Serial.print(" | ");
      Serial.println(frequencies[2]);

      if (frequencies[0] == noteCoder.endMessage && frequencies[1] == noteCoder.endMessage && frequencies[2] == noteCoder.endMessage) {
        speed1 = -1;
        speed2 = -1;
        // digitalWrite(in1, HIGH);
        // digitalWrite(in2, HIGH);
        // digitalWrite(in3, HIGH);
        // digitalWrite(in4, HIGH);
      } else {
        float direction = max(0, min(noteCoder.decodeDirection(frequencies), 1.0));
        // if (0 >= direction && direction <= 1.000)
        speed1 = Helpers::mapf(direction, start_value, end_value, 180, 255);
        speed2 = Helpers::mapf(direction, start_value, end_value, 255, 180);
        Serial.println(direction, 3);
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
      }

      analogWrite(ENA, speed1);
      analogWrite(ENB, speed2);
      delay(100);
    } else if (currentIndex >= 0) {
      if (currentIndex % 4 == 0 && currentIndex > 0) {
        frequencies[frequencyIndex] = roundedPeak;

        frequencyIndex++;
      }
      currentIndex++;
    }
    samplesRead = 0;
  }
}

void onPDMdata() {
  int bytesAvailable = PDM.available();
  PDM.read(sampleBuffer, bytesAvailable);
  samplesRead = bytesAvailable / 2;
}