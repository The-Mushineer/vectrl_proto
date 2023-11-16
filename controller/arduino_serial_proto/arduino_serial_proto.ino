const uint8_t START_PACKET = 0xE9;
enum MessageType {
  MSG_BTN_DOWN = 1,
  MSG_BTN_UP = 2,
  MSG_TURN = 3,
};

const size_t BUTTON_COUNT = 4;
const int BUTTON_PINS[BUTTON_COUNT] = { 8, 9, 10, 11 };
const int ENCODER1_PINS[2] = { 2, 3 };
const uint32_t ENCODER_INTERVAL = 10;
const uint32_t BUTTON_DEBOUNCE_INTERVAL = 20;

volatile int8_t encoder1_count = 0;

void encoder1Isr() {
  if (digitalRead(ENCODER1_PINS[0]) == digitalRead(ENCODER1_PINS[1])) {
    encoder1_count--;
  } else {
    encoder1_count++;
  }
}

void sendButtonEvent(uint8_t button, bool down) {
  static uint8_t message[4] = {START_PACKET, 0, 0, 0};
  message[1] = down ? MSG_BTN_DOWN : MSG_BTN_UP;
  message[2] = button;
  message[3] = message[0] + message[1] + message[2];
  Serial.write(message, 4);
}

void sendRotationEvent(uint8_t encoder, int8_t count) {
  static uint8_t message[5] = {START_PACKET, MSG_TURN, 0, 0, 0};
  message[2] = encoder;
  message[3] = count;
  message[4] = message[0] + message[1] + message[2] + message[3];
  Serial.write(message, 5);
}

void setup() {
  for (size_t idx = 0; idx < BUTTON_COUNT; idx++) {
    pinMode(BUTTON_PINS[idx], INPUT_PULLUP);
  }
  for (size_t idx = 0; idx < 2; idx++) {
    pinMode(ENCODER1_PINS[idx], INPUT);
  }
  attachInterrupt(digitalPinToInterrupt(ENCODER1_PINS[0]), encoder1Isr, FALLING);
  Serial.begin(115200);
}

void loop() {
  static uint8_t buttons = 0;
  static uint32_t last_enc_check = 0;
  static uint32_t last_btn_change = 0;
  uint32_t current_time = millis();

  if (current_time - last_enc_check > ENCODER_INTERVAL) {
    last_enc_check = current_time;
    int8_t counter = 0;
    noInterrupts();
    counter = encoder1_count;
    encoder1_count = 0;
    interrupts();
    if (counter != 0) {
      sendRotationEvent(0, counter);
    }
  }

  uint8_t newButtons = 0;
  for (size_t idx = 0; idx < BUTTON_COUNT; idx++) {
    bool pressed = digitalRead(BUTTON_PINS[idx]) == LOW;
    newButtons |= pressed << idx;
  }
  if (newButtons != buttons && current_time - last_btn_change > BUTTON_DEBOUNCE_INTERVAL) {
    last_btn_change = current_time;
    for (size_t idx = 0; idx < BUTTON_COUNT; idx++) {
      auto pressed = ((newButtons >> idx) & 1);
      if (pressed != ((buttons >> idx) & 1)) {
        sendButtonEvent(idx, pressed);
      }
    }
    buttons = newButtons;
  }
}
