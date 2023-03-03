#include <Servo.h>
#include <Adafruit_Fingerprint.h>

#define mySerial Serial1

#define FINGER_REQUIRE 2
#define MATCH_STATUS 3
#define SERVO_CTRL 5
#define SERVO_POWER 6
#define OPERATE_STATUS 7
#define OPEN_REQUIRE 8
#define CLOSE_REQUIRE 9

#define OPEN_DEGREE 45
#define CLOSE_DEGREE 135

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
Servo servo;

void setup()
{
  servo.attach(SERVO_CTRL);
  pinMode(FINGER_REQUIRE, INPUT_PULLUP);
  pinMode(MATCH_STATUS, OUTPUT);
  pinMode(SERVO_POWER, OUTPUT);
  pinMode(OPERATE_STATUS, OUTPUT);
  pinMode(OPEN_REQUIRE, INPUT_PULLUP);
  pinMode(CLOSE_REQUIRE, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("fingerprint doorlock activation");

  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("found fingerprint sensor");
  } else {
    Serial.println("cannot find fingerprint sensor");
    while (1) {
      delay(1);
    }
  }

  servo_write(CLOSE_DEGREE);
}

void loop()
{
  if (digitalRead(FINGER_REQUIRE) == LOW) { // fingerprint required

    int match_status = 0;
    for (int i = 0; i < 20; i++) {

      if (getFingerprintID() == -1) {
        // failed to match fingerprint
        delay(100);
      } else {
        // success
        match_status = 1;
        digitalWrite(MATCH_STATUS, HIGH);
        servo_write(OPEN_DEGREE);
        break;
      }
    }
    if (match_status == 0) {
      for (int i = 0; i < 5; i++) {
        // blink to notify failure
        digitalWrite(MATCH_STATUS, HIGH);
        delay(100);
        digitalWrite(MATCH_STATUS, LOW);
        delay(100);
      }
    }
  }
  else if (digitalRead(OPEN_REQUIRE) == LOW) {
    digitalWrite(OPERATE_STATUS, HIGH);
    servo_write(OPEN_DEGREE);
  }
  else if (digitalRead(CLOSE_REQUIRE) == LOW) {
    digitalWrite(OPERATE_STATUS, HIGH);
    servo_write(CLOSE_DEGREE);
  } else {
    digitalWrite(MATCH_STATUS, LOW);
    digitalWrite(OPERATE_STATUS, LOW);
  }
}

void servo_write(int degree) {

  digitalWrite(SERVO_POWER, HIGH);
  servo.write(degree);
  delay(1000);
  digitalWrite(SERVO_POWER, LOW);

}

// returns -1 if failed, otherwise returns ID #
int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found 
  Serial.print("Found ID #");
  Serial.println(finger.fingerID);
  return finger.fingerID;
}
