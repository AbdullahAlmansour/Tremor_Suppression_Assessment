// Pin definitions
#define ENC_A 2 // Yellow wire from encoder
#define ENC_B 3 // Green wire from encoder (not used)
#define PPR 200 // Pulses Per Revolution (depends on the encoder)
#define ENA_motor 6 // PWM pin for motor speed control

// Global variables
volatile long pulseCount = 0; // Interrupt Service Routine (ISR) for the encoder

float motorRPM = 0;
float f_des = 3; // Desired frequency in Hz 
float rpm_des = f_des * 60.0; // Desired speed in RPM
float motor_duty = 0; 

// PI control variables
float e = 0.0;
float e_acc = 0.0;
float kp = 20.0;
float ki = 2.0;

// Timer variables for speed calculation
unsigned long prevMillis = 0; // Timer for speed calculation
unsigned long t_prev = 0.0;
float t_diff = 0.0;


void setup() {
  // Set encoder pins with pull-up resistors
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  // Set motor control pin as output
  pinMode(ENA_motor, OUTPUT);
  // Attach interrupt to the encoder A channel
  attachInterrupt(digitalPinToInterrupt(ENC_A), encoderISR, CHANGE);

  Serial.begin(115200); // Start serial communication
  t_prev = millis(); // Initalise the timer
}

void loop() {
  unsigned long t_now = millis();
  t_diff = (float)(t_now - t_prev);
  
  // Update speed every 20ms
  if (t_diff >= 20) {
    // Calculate motor speed in RPM
    noInterrupts();
    long count = pulseCount;
    pulseCount = 0;
    interrupts();
    motorRPM = ((float)count / (float)PPR) * 60.0 * (1000.0 / t_diff);

    // PI control
    e = (rpm_des - motorRPM); // Calculate error in speed
    
    e_acc += e * (t_diff / 1000.0); // Accumulate error over time (integral term)
    motor_duty = kp * e + ki * e_acc; // PI control law to calculate motor duty cycle

    // Clamp duty [0, 255]
    if (motor_duty < 0) motor_duty = 0;
    if (motor_duty > 255) motor_duty = 255;

    // Set motor speed
    analogWrite(ENA_motor, (int)motor_duty);
    t_prev = t_now; // Update timer for next speed calculation
  }
}

void encoderISR() {
  pulseCount++;
}
