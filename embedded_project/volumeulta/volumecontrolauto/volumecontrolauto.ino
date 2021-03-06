#define trigPin D5
#define echoPin D6
#include <SPI.h>
byte address = 0x11;
int CS = D2;
int i = 0;
const int numReadings = 10;
int cv = 0;
int nv = 0;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
long dmax = 0;
long d1 = 0;
long d2 = 0;
int inputPin = A0;
int range = 0;
void setup() {
  pinMode (CS, OUTPUT);
  pinMode(D5, OUTPUT);
  SPI.begin();
  // adjust high and low resistance of potentiometer
  // adjust Highest Resistance .
  digitalPotWrite(0x00);
  delay(1000);
  // adjust  wiper in the  Mid point  .
  digitalPotWrite(0x80);
  delay(1000);
  // adjust Lowest Resistance .
  digitalPotWrite(0xFF);
  delay(1000);
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.println("Start");
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  for (int i = 0; i < 25; i++) {
    d1 = dist();
    if (d1 > dmax)
      dmax = d1;
  }
  Serial.println("Maximum value is " + dmax);
  delay(5000);
}
double normd = 0;
double brightness = 0;
long d = 0;
void loop() {
  Serial.println("Maximum value is ");
  Serial.println(dmax);
  d = dist();
  Serial.println("distance :");
  Serial.println(d);
  /// averaging
  // subtract the last reading:
  if (d < dmax) {
    total = total - readings[readIndex];
    // read from the sensor:
    readings[readIndex] = d;
    // add the reading to the total:
    total = total + readings[readIndex];
    // advance to the next position in the array:
    readIndex = readIndex + 1;

    // if we're at the end of the array...
    if (readIndex >= numReadings) {
      // ...wrap around to the beginning:
      readIndex = 0;
    }

    // calculate the average:
    average = total / numReadings;
  }
  // send it to the computer as ASCII digits
  Serial.println(average);
  delay(1);

  //mapping
  range = map(average, 0, dmax, 255, 0);
  Serial.println("Range= ");
  Serial.println(range);
  volume(range);
  delay(20);
}
void volume(int range) {
  if (range < 20) {
    nv = 0;
    vlmcntrl(nv, cv);
    cv = nv;
    Serial.println("No sound");

  }
  else if (range < 50) {
    nv = 50;
    vlmcntrl(nv, cv);
    cv = nv;
    Serial.println("Volume level 50");
  }
  else if (range < 100) {
    nv = 100;
    vlmcntrl(nv, cv);
    cv = nv;
    Serial.println("Volume level 100");
  }
  else if (range > 120) {
    nv = 200;
    vlmcntrl(nv, cv);
    cv = nv;
    Serial.println("Volume level 200");
  }
}
long dist() {
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
  return distance;
}
void vlmcntrl(int nvol, int cvol) {
  if (nvol > cvol) {
    for (int i = cvol; i <= nvol; i = i + 10) {
      delay(500);
      Serial.println(i);
      digitalPotWrite(i);
    }
  }
  else {
    for (int i = cvol; i >= nvol; i = i - 10) {
      delay(500);
      Serial.println(i);
      digitalPotWrite(i);
    }
  }
}

int digitalPotWrite(int value)
{
  digitalWrite(CS, LOW);
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(CS, HIGH);
}
