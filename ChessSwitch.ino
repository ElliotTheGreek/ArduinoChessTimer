#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define LED_RIGHT_PIN 2
#define BUTTON_RIGHT_PIN 3

#define LED_LEFT_PIN 4
#define BUTTON_LEFT_PIN 5

#define BUTTON_RESET_PIN 6
int lastButtonResetValue = 1;

#define BUTTON_TIMING_PIN 7
int lastButtonTimingValue = 1;

int movedLeft = 0;
int movedRight = 0;

bool isPlaying = false;
bool isPaused = false;
int onSide = 0;

int timingMode = 0; // 0 is 90 minutes, 1 is 10 minutes
int defaultHour = 1;
int defaultMinute = 29;
int defaultSecond = 59;

int leftHour=1;
int leftMinute=29;
int leftSecond=59;
int leftMillis = 9;

int rightHour=1;
int rightMinute=29;
int rightSecond=59;
int rightMillis = 9;

static uint32_t last_time, now = 0; // RTC

void setup() {
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
  pinMode(LED_RIGHT_PIN, OUTPUT);

  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(LED_LEFT_PIN, OUTPUT);

  pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
  pinMode(BUTTON_TIMING_PIN, INPUT_PULLUP);

  digitalWrite(LED_RIGHT_PIN, 0);
  digitalWrite(LED_LEFT_PIN, 0);
  lcd.begin();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print(" Ready To Begin");

}

void toggleTiming() {
  timingMode++;
  if (timingMode > 5) {
    timingMode = 0;
  }

// 0 is 90 minutes, 1 is 10 minutes
  if(timingMode == 0) {         // 90 minutes
    defaultHour = 1;
    defaultMinute = 29;
    defaultSecond = 59;
  } else if(timingMode == 1) {  // 30 minutes
    defaultHour = 0;
    defaultMinute = 29;
    defaultSecond = 59;
  } else if(timingMode == 2) {  // 10 minutes
    defaultHour = 0;
    defaultMinute = 9;
    defaultSecond = 59;
  } else if(timingMode == 3) {  // 5 minutes
    defaultHour = 0;
    defaultMinute = 4;
    defaultSecond = 59;
  } else if(timingMode == 4) {  // 1 minutes
    defaultHour = 0;
    defaultMinute = 0;
    defaultSecond = 59;
  } else if(timingMode == 5) {  // 10 hours
    defaultHour = 9;
    defaultMinute = 59;
    defaultSecond = 59;
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Ready To Begin");

  leftHour=defaultHour;
  leftMinute=defaultMinute;
  leftSecond=defaultSecond;
  
  rightHour=defaultHour;
  rightMinute=defaultMinute;
  rightSecond=defaultSecond;

  movedLeft = 0;
  movedRight = 0;

  isPlaying = false;
}

void startGame(bool flashLights) {
  if(flashLights) {
    for(int i=0; i<3; i = i+1) {
      digitalWrite(LED_RIGHT_PIN, 1);
      digitalWrite(LED_LEFT_PIN, 1);
      delay(250);
    
      digitalWrite(LED_RIGHT_PIN, 0);
      digitalWrite(LED_LEFT_PIN, 0);
      delay(250);
    }
  }
  movedLeft = 0;
  movedRight = 0;

  leftHour=defaultHour;
  leftMinute=defaultMinute;
  leftSecond=defaultSecond;
  
  rightHour=defaultHour;
  rightMinute=defaultMinute;
  rightSecond=defaultSecond;

  onSide = 0;

  digitalWrite(LED_RIGHT_PIN, 0);
  digitalWrite(LED_LEFT_PIN, 1);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" <------");
  lcd.print(" Move 1");

  isPlaying = true;
}

void finishGame() {
  // display total moves and time
  isPlaying = false;

  digitalWrite(LED_RIGHT_PIN, 0);
  digitalWrite(LED_LEFT_PIN, 0);

  if (onSide == 0) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("<-- Winner (");
    lcd.print(movedLeft);
    lcd.print(")");
  } else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" Winner (");
    lcd.print(movedRight);
    lcd.print(") -->");
  }

}

void switchSides() {
  if (onSide == 0) {
    movedLeft++;
    onSide = 1;
    digitalWrite(LED_RIGHT_PIN, 1);
    digitalWrite(LED_LEFT_PIN, 0);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" Move ");
    lcd.print(movedRight + 1);
    lcd.print(" ------>");
  } else {
    onSide = 0;
    movedRight++;
    digitalWrite(LED_RIGHT_PIN, 0);
    digitalWrite(LED_LEFT_PIN, 1);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" <------");
    lcd.print(" Move ");
    lcd.print(movedLeft + 1);
  }
}

void loop() {
/*  make 5 time 200ms loop, for faster Button response  */
  for ( int i=0 ;i<10 ;i++) {
    while ((now-last_time)<100) { 
      now=millis();
    }

/* handle button start/checkmate */
    int currentButtonResetValue = digitalRead(BUTTON_RESET_PIN);
    if (lastButtonResetValue != currentButtonResetValue) {
      lastButtonResetValue = currentButtonResetValue;
      if (currentButtonResetValue == LOW) {
        if (!isPlaying) {
          startGame(true);
        } else {
          finishGame();
        }
      }
    }
  
/* handle button timing/pause */
    int currentButtonTimingValue = digitalRead(BUTTON_TIMING_PIN);
    if (lastButtonTimingValue != currentButtonTimingValue) {
      lastButtonTimingValue = currentButtonTimingValue;
      if (currentButtonTimingValue == LOW) {
        if (!isPlaying) {
          toggleTiming();
        } else {
          isPaused = !isPaused;
        }
      }
    }

    if (isPaused) {
      lcd.setCursor(0,0);
      lcd.print("<--- PAUSED --->");
    } else {
      if (isPlaying) {
        if (onSide == 1) {
          lcd.setCursor(0,0);
          lcd.print(" Move ");
          lcd.print(movedRight + 1);
          lcd.print(" ------>");
        } else {
          lcd.setCursor(0,0);
          lcd.print(" <------");
          lcd.print(" Move ");
          lcd.print(movedLeft + 1);
        }
      }
      if (onSide == 0) {  // 0 is left, 1 is right
        if (digitalRead(BUTTON_LEFT_PIN) == LOW) {
          switchSides();
        }
      } else {
        if (digitalRead(BUTTON_RIGHT_PIN) == LOW) {
          switchSides();
        }
      }
    }
    last_time=now;
    
    lcd.setCursor(0,1);
    lcd.print(leftHour);
    lcd.print(":");
    if(leftMinute<10)lcd.print("0");
    lcd.print(leftMinute);
    lcd.print(":");
    if(leftSecond<10)lcd.print("0");
    lcd.print(leftSecond);
    lcd.print(" ");
    lcd.print(" ");
  
    lcd.print(rightHour);
    lcd.print(":");
    if(rightMinute<10)lcd.print("0");
    lcd.print(rightMinute);
    lcd.print(":");
    if(rightSecond<10)lcd.print("0");
    lcd.print(rightSecond);
    lcd.print(" ");

    if(isPlaying && !isPaused) {
      if (onSide == 0) {
        leftMillis -= 1;
      } else {
        rightMillis -= 1;
      }
    }
  }
/*  once a second below*/
  if (onSide == 0) {
    if (leftMillis < 0) {
      leftMillis = 9;
      leftSecond -= 1;
    }
    if (leftSecond<0) {
      leftSecond = 59;
      leftMinute -= 1;
    }
    if (leftMinute<0) {
      leftMinute = 59;
      leftHour -= 1;
    }
    if (leftHour <= 0 && leftMinute <= 0 && leftSecond <= 0) {
      switchSides();
      finishGame();
    }
  } else {
    if (rightMillis < 0) {
      rightMillis = 9;
      rightSecond -= 1;
    }
    if (rightSecond<0) {
      rightSecond = 59;
      rightMinute -= 1;
    }
    if (rightMinute<0) {
      rightMinute = 59;
      rightHour -= 1;
    }

    if (rightHour <= 0 && rightMinute <= 0 && rightSecond <= 0) {
      switchSides();
      finishGame();
    }
  }
}
