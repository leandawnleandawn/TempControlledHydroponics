#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#define DHTPIN 6
#define DHTTYPE DHT22

// Discrete Values

const int water_air_solenoid = 5;
const int thermoelectric = 3;

// Analog Values

const int soil_moisture_L1 = A0;
const int soil_moisture_L2 = A1;
const int pt1000 = A2;

// Ultrasonic Sensor
const int trigPin_ultrasonic = 9;
const int echoPin_ultrasonic = 10;

// LCD Screen
LiquidCrystal_I2C lcd(0x27, 16, 2);

DHT dht(DHTPIN, DHTTYPE);
// Push Buttons
const int start_button = 7;
const int stop_button = 8;

// Pilot Lights
const int pilot_green = 2;
const int pilot_red = 4;
const int pilot_orange = 12;

// Float Values
float water_level = 0.0;
float env_temp = 0.0;
float water_temp = 0.0;
float sm_level_L1 = 0.0;
float sm_level_L2 = 0.0;

const float container_height = 15.4;

bool start_present_state = false;
bool stop_present_state = false;

bool START = false;
bool STOP = false;

const float vt_factor = 1.88;
const float offset = -14;

 
void setup() {
  // put your setup code here, to run once:

  pinMode(water_air_solenoid, OUTPUT);
  pinMode(thermoelectric, OUTPUT);

  pinMode(soil_moisture_L1, INPUT);
  pinMode(soil_moisture_L2, INPUT);
  pinMode(pt1000, INPUT);

  pinMode(trigPin_ultrasonic, OUTPUT);
  pinMode(echoPin_ultrasonic, INPUT);

  pinMode(start_button, INPUT_PULLUP);
  pinMode(stop_button, INPUT_PULLUP);

  pinMode(pilot_green, OUTPUT);
  pinMode(pilot_red, OUTPUT);
  pinMode(pilot_orange, OUTPUT);

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  dht.begin();
  Serial.println("Initializing...");
  lcd.setCursor(0, 0);
  lcd.print("Beginning...");
  Serial.println("Initializing Done.");
  Serial.println("Starting the Program...");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press to do");
  lcd.setCursor(0, 1);
  lcd.print("Something");
  reset();
}


void loop() {
  // put your main code here, to run repeatedly:
  water_level = water_level_measure();
  selection();
  if (START) {
    if(water_level > 5){
      start_program();
    }else{
      START = false;
      STOP = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Overflowing!!!");
      reset();
      digitalWrite(pilot_red, LOW);
      digitalWrite(pilot_green, LOW);
      digitalWrite(pilot_orange, HIGH);
      delay(3000);
         
    }
  }
  if (STOP) {
    reset();
  }

  delayMicroseconds(500);
}

void start_program() {
  lcd.clear();
  digitalWrite(pilot_red, LOW);
  digitalWrite(pilot_green, HIGH);
  digitalWrite(pilot_orange, LOW);
  monitoring();
  monitoring_lcd();
  digitalWrite(water_air_solenoid, HIGH);
  delayMicroseconds(300);
  if(water_temp >= 23.00){
    digitalWrite(thermoelectric, HIGH);
  }else{
    digitalWrite(thermoelectric, LOW);
  }      
  selection();
}

void reset() {
  delay(50);
  digitalWrite(water_air_solenoid, LOW);
  digitalWrite(thermoelectric, LOW);
  digitalWrite(pilot_red, HIGH);
  digitalWrite(pilot_green, LOW);
  digitalWrite(pilot_orange, LOW);
}


float water_level_measure() {
  digitalWrite(trigPin_ultrasonic, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_ultrasonic, HIGH);
  delayMicroseconds(10);
  long duration = pulseIn(echoPin_ultrasonic, HIGH);
  float distance = duration * 0.034 / 2.0;
  return distance;
}

void monitoring() {
  env_temp= dht.readTemperature();
  int sensorValue = analogRead(pt1000);
  float voltage = sensorValue * (5.0/1023.0);
  water_temp = ((voltage * 100.0) / vt_factor) + offset;
  float water_level_percentage = ((container_height - water_level) / container_height) * 100;
  sm_level_L1 = map(analogRead(soil_moisture_L1), 0, 1024, 100, 0); 
  sm_level_L2 = map(analogRead(soil_moisture_L2), 0, 1024, 100, 0); 
  Serial.print("Water Level: ");
  Serial.print(water_level);
  Serial.print("DHT: ");
  Serial.print(env_temp);
  Serial.print("C PT1000: ");
  Serial.print(water_temp);
  Serial.print("C L1: ");
  Serial.print(sm_level_L1);
  Serial.print("% L2: ");
  Serial.print(sm_level_L2);
  Serial.println("%");
  delay(250);

}

void monitoring_lcd(){
  lcd.clear();
  lcd.print("Water Level = ");
  lcd.setCursor(0, 1);
  lcd.print(((container_height - water_level) / container_height) * 100);
  delay(1000);
  lcd.clear();
  lcd.print("Water Temp = ");
  lcd.setCursor(0, 1);
  lcd.print(water_temp);
  delay(1000);
  lcd.clear();
  lcd.print("Env Temp = ");
  lcd.setCursor(0, 1);
  lcd.print(env_temp);
  delay(1000);
  lcd.clear();
  lcd.print("Soil % = ");
  lcd.setCursor(0, 1);
  lcd.print((sm_level_L1 + sm_level_L2)/2);
  delay(1000);
}

void selection(){
  lcd.setCursor(0, 0);
  lcd.print("Press to do");
  lcd.setCursor(0, 1);
  lcd.print("Something");

  start_present_state = digitalRead(start_button);
  stop_present_state = digitalRead(stop_button);

  if(start_present_state == 0){
    Serial.println("Starting Program");
    START = true;
    STOP = false;
  }

  if(stop_present_state == 0){
    Serial.println("Program Stopped");
    START = false;
    STOP = true;
  }
  delay (50);

}
