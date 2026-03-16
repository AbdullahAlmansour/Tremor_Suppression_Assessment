// Pin definitions for the encoder
#define ENC_A 2 // Yellow wire from encoder
#define ENC_B 3 // Green wire from encoder (not used)


volatile long pulseCount = 0; // Interrupt Service Routine for the encoder

void setup() {
  // Set encoder pins with pull-up resistors
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  // Attach interrupt to the encoder A channel
  attachInterrupt(digitalPinToInterrupt(ENC_A), encoderISR, CHANGE);

  // Start serial communication
  Serial.begin(115200);
  Serial.println("Rotate the motor shaft for one revolution");
  Serial.println("and take not of the Pulse Per Revolution (PPR) count.");
}

void loop() {
  static long lastCount = 0;

  if (pulseCount != lastCount) {
    noInterrupts();
    long count = pulseCount;
    interrupts();

    Serial.print("Pulse Count = ");
    Serial.println(count);

    lastCount = count;
  }
}

void encoderISR() {
  pulseCount++;
}