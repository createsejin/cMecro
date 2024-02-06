#include <Keyboard.h>
#include <Mouse.h>
#include <MouseTo.h>

String GetString = "";

void setup() {
  Serial.begin(9600);
  Mouse.begin();
  Keyboard.begin();
  MouseTo.setScreenResolution(3440, 1440);
  MouseTo.setCorrectionFactor(1);
}

void loop() {
  GetString = "";
  char data = -1;
  while (Serial.available()) {
    data = Serial.read();
    GetString.concat(data);
  }

  if (GetString != "") {
    String sub1 = GetString.substring(0, 1);
    if (sub1 == "M") {
      sub1 = GetString.substring(0, 2);
      int x = GetString.substring(2, 6).toInt();
      int y = GetString.substring(6).toInt();
      if (sub1 == "MZ" || sub1 == "Mz") {
        if (sub1 == "MZ") {
          MouseTo.setTarget(0, 0);
          while (MouseTo.move() == false) {}
          MouseTo.setTarget(x, y, 0);
          while (MouseTo.move() == false) {}
        } else if (sub1 == "Mz") {
          MouseTo.setTarget(x, y, 0);
          while (MouseTo.move() == false) {}
        }
      } else if (sub1 == "MX") {
        Mouse.move(x, y, 0);
      }
    }

    else if (sub1 == "R") {
      sub1 = GetString.substring(0, 2);
      if (sub1 == "RC") {
        Mouse.click(MOUSE_RIGHT);
      }
      else if (sub1 == "RH") {
        Mouse.press(MOUSE_RIGHT);
      }
      else if (sub1 == "RR") {
        Mouse.release(MOUSE_RIGHT);
      }
    }

    else if (sub1 == "L") {
      sub1 = GetString.substring(0, 2);
      if (sub1 == "LC") {
        Mouse.click(MOUSE_LEFT);
      }
      else if (sub1 == "LH") {
        Mouse.press(MOUSE_LEFT);
      }
      else if (sub1 == "LR") {
        Mouse.release(MOUSE_LEFT);
      }
      else if (sub1 == "LD") {
        Mouse.click(MOUSE_LEFT);
        Mouse.click(MOUSE_LEFT);
      }
    }

    else if (sub1 == "K") {
      String sub1 = GetString.substring(0, 2);
      String sub2 = GetString.substring(2);
      if (sub1 == "KP") {
        Keyboard.print(sub2);
      }
      else if (sub2 == "RC") {
        SendKey(sub1, KEY_RIGHT_CTRL);
      }
      else if (sub2 == "AL") {
        SendKey(sub1, KEY_LEFT_ARROW);
      }
      else if (sub2 == "AU") {
        SendKey(sub1, KEY_UP_ARROW);
      }
      else if (sub2 == "AR") {
        SendKey(sub1, KEY_RIGHT_ARROW);
      }
      else if (sub2 == "AD") {
        SendKey(sub1, KEY_DOWN_ARROW);
      }
      else if (sub2 == "BS") {
        SendKey(sub1, KEY_BACKSPACE);
      }
      else if (sub2 == "ET") {
        SendKey(sub1, KEY_RETURN);
      }
      else if (sub2 == "TB") {
        SendKey(sub1, KEY_TAB);
      }
      else if (sub2 == "SB") {
        SendKey(sub1, ' ');
      }
      else {
        if (sub1 == "KD") {
          Keyboard.press(sub2[0]);
        }
        else if (sub1 == "KU") {
          Keyboard.release(sub2[0]);
        }
      }
    }
  }
}

void SendKey(String sub1, uint8_t sub2) {
  if (sub1 == "KD") {
    Keyboard.press(sub2);
  }
  else if (sub1 == "KU") {
    Keyboard.release(sub2);
  }
}