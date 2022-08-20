#include <math.h>

#define WindSensorPin 35 // The pin location of the anemometer sensor

volatile unsigned long lastInterrupt;
volatile unsigned int deltaTime;
float WindSpeed; // speed miles per hour
float rps = 0;

void setup() {
  Serial.begin(115200);

  pinMode(WindSensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(WindSensorPin), isr_rotation, FALLING);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // Serial.println("Davis Wind Speed Test");
  //Serial.println("Rotations\tMPH");
}

void loop() {

  //Rotations = 0; // Set Rotations count to 0 ready for calculations

  //  sei(); // Enables interrupts
  //  delay (3000); // Wait 3 seconds to average
  //  cli(); // Disable interrupts

  // convert to mp/h using the formula V=P(2.25/t)
  // V = P(2.25/3) = P * 0.75

  if (millis() - lastInterrupt > 5000)
  {
    rps = 0;
  }
  else {
    rps = 1000. / deltaTime;
  }

  WindSpeed = rps * 3.66;
  Serial.println(WindSpeed);

  //Serial.print(Rotations); Serial.print("\t\t");
  //Serial.println(WindSpeed);

}

// This is the function that the interrupt calls to increment the rotation count
void isr_rotation () {
  deltaTime = millis() - lastInterrupt;
  lastInterrupt = millis();
}
