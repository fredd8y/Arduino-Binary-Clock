#include <DS3231.h>
#include <Wire.h>

#define SET_PIN 5         // Button to toggle setMode
#define SET_HOUR_PIN 4    // Button to increase hour
#define SET_MINUTE_PIN 3  // Button to increase minutes
#define SET_SECOND_PIN 2  // Button to increase seconds

#define LATCH_PIN 12  // STCP
#define CLOCK_PIN 13  // SHCP
#define DATA_PIN 10   // DS

#define SET_LED 6 // Led that indicates if we are in set mode

DS3231 rtc;

int debounceDelay = 50;
int setMode = LOW;
int setModeButtonState;
int setHourButtonState;
int setMinuteButtonState;
int setSecondButtonState;
int setModeLastButtonState = LOW;
int setHourLastButtonState = LOW;
int setMinuteLastButtonState = LOW;
int setSecondLastButtonState = LOW;
int hour = 0;
int minute = 0;
int second = 0;

bool h12 = false;
bool pm = false;

unsigned long setModeLastDebounceTime = 0;
unsigned long setHourLastDebounceTime = 0;
unsigned long setMinuteLastDebounceTime = 0;
unsigned long setSecondLastDebounceTime = 0;

void showNumber(int h, int m, int s) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, h);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, m);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, s);
  digitalWrite(LATCH_PIN, HIGH);
}

void setup() {
  Wire.begin();
  rtc.setClockMode(false);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(SET_LED, OUTPUT);
  pinMode(SET_PIN, INPUT);
  pinMode(SET_HOUR_PIN, INPUT);
  pinMode(SET_MINUTE_PIN, INPUT);
  pinMode(SET_SECOND_PIN, INPUT);
}

void loop() {
  int setModeReading = digitalRead(SET_PIN);
  if (setModeReading != setModeLastButtonState) {
    setModeLastDebounceTime = millis();
  }
  int setHourReading = digitalRead(SET_HOUR_PIN);
  if (setHourReading != setHourLastButtonState) {
    setHourLastDebounceTime = millis();
  }
  int setMinuteReading = digitalRead(SET_MINUTE_PIN);
  if (setMinuteReading != setMinuteLastButtonState) {
    setMinuteLastDebounceTime = millis();
  }
  int setSecondReading = digitalRead(SET_SECOND_PIN);
  if (setSecondReading != setSecondLastButtonState) {
    setSecondLastDebounceTime = millis();
  }
  if ((millis() - setModeLastDebounceTime) > debounceDelay) {
    if (setModeReading != setModeButtonState) {
      setModeButtonState = setModeReading;
      if (setModeButtonState == HIGH) {
        if (setMode) {
          // Esco dalla modalità set, vado a scrivere sul DS3231 l'ora impostata manualmente
          rtc.setHour(hour);
          rtc.setMinute(minute);
          rtc.setSecond(second);
        } else {
          // Entro in modalità set
          hour = rtc.getHour(h12, pm);
          minute = rtc.getMinute();
          second = rtc.getSecond();
        }
        setMode = !setMode;
      }
    }
  }
  if ((millis() - setHourLastDebounceTime) > debounceDelay) {
    if (setHourReading != setHourButtonState) {
      setHourButtonState = setHourReading;
      if (setHourReading == HIGH && setMode) {
        if (hour == 23) {
          hour = 0;
        } else {
          hour++;
        }
      }
    }
  }
  if ((millis() - setMinuteLastDebounceTime) > debounceDelay) {
    if (setMinuteReading != setMinuteButtonState) {
      setMinuteButtonState = setMinuteReading;
      if (setMinuteReading == HIGH && setMode) {
        if (minute == 59) {
          minute = 0;
        } else {
          minute++;
        }
      }
    }
  }
  if ((millis() - setSecondLastDebounceTime) > debounceDelay) {
    if (setSecondReading != setSecondButtonState) {
      setSecondButtonState = setSecondReading;
      if (setSecondReading == HIGH && setMode) {
        if (second == 59) {
          second = 0;
        } else {
          second++;
        }
      }
    }
  }

  if (!setMode) {
    showNumber(rtc.getHour(h12, pm), rtc.getMinute(), rtc.getSecond());
  } else {
    showNumber(hour, minute, second);
  }

  // Led per avere evidenza dello stato di set
  digitalWrite(SET_LED, setMode);

  setModeLastButtonState = setModeReading;
  setHourLastButtonState = setHourReading;
  setMinuteLastButtonState = setMinuteReading;
  setSecondLastButtonState = setSecondReading;
}
