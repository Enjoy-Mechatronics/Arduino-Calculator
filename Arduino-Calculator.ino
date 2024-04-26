#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

const byte ROWS = 4;  // Number of rows on the keypad
const byte COLS = 4;  // Number of columns on the keypad

char keys[ROWS][COLS] = {
  { '1', '2', '3', '+' },
  { '4', '5', '6', '-' },
  { '7', '8', '9', '*' },
  { 'C', '0', '=', '/' }
};

byte rowPins[ROWS] = { 9, 8, 7, 6 };  // Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins
byte colPins[COLS] = { 5, 4, 3, 2 };  // Connect keypad COL0, COL1, COL2 and COL3 to these Arduino pins

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);

String input = "";
char* expressionToParse;
bool cleared = true;

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
  char key = keypad.getKey();

  if (key != NO_KEY) {

    if (!cleared) {
      clearDisplay();
    }

    if (key == '=') {
      calculateResult();
    } else if (key == 'C') {
      clearDisplay();
    } else {
      lcd.print(key);
      input = input + key;
    }
  }
}

  void calculateResult() {
    lcd.setCursor(0, 1);
    lcd.print("Result:");

    input.trim();

    expressionToParse = input.c_str();
    lcd.setCursor(8, 1);
    float result = expression();
    if(isinf(result) == 0 && isnan(result)==0){
      lcd.print(result);
    }else{
      lcd.print("Invalid");
    }
    cleared = false;
  }

  void clearDisplay() {
    lcd.clear();
    input = "";
    cleared = true;
  }

  char peek() {
    return *expressionToParse;
  }

  char get() {
    return *expressionToParse++;
  }

  float number() {
    float result = get() - '0';
    while (peek() >= '0' && peek() <= '9') {
      result = 10 * result + get() - '0';
    }
    return result;
  }

  float factor() {
    if (peek() >= '0' && peek() <= '9')
      return number();
    else if (peek() == '(') {
      get();  // '('
      float result = expression();
      get();  // ')'
      return result;
    } else if (peek() == '-') {
      get();
      return -factor();
    }
    return 0;  // error
  }

  float term() {
    float result = factor();
    while (peek() == '*' || peek() == '/')
      if (get() == '*')
        result *= factor();
      else
        result /= factor();
    return result;
  }

  float expression() {
    float result = term();
    while (peek() == '+' || peek() == '-')
      if (get() == '+')
        result += term();
      else
        result -= term();
    return result;
  }
