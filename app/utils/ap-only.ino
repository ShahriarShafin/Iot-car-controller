#include <ESP8266WiFi.h>

// Constants
const char* AP_SSID = "NodeMCU Car";

// Pin Definitions
const int PIN_STEERING_LEFT = D2;
const int PIN_DRIVE_FWD = D7;
const int PIN_STEERING_RIGHT = D4;
const int PIN_DRIVE_BWD = D8;
const int PIN_DRIVE_ENABLE = D5;
const int PIN_STEERING_ENABLE = D6;
const int PIN_HORN = D1;
const int PIN_LED = D0;   // Red LED on NodeMCU
const int PIN_BUTTON = D3; // Flash button on NodeMCU

// Motor Speed
int motorSpeed = 122;  // Default speed

// Create WiFi objects
WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  setupPins();
  setupAP();
  server.begin();
}

void loop() {
  processClientRequests();
}

// Initialize pins
void setupPins() {
  pinMode(PIN_STEERING_LEFT, OUTPUT);
  pinMode(PIN_DRIVE_FWD, OUTPUT);
  pinMode(PIN_STEERING_RIGHT, OUTPUT);
  pinMode(PIN_DRIVE_BWD, OUTPUT);
  pinMode(PIN_DRIVE_ENABLE, OUTPUT);
  pinMode(PIN_STEERING_ENABLE, OUTPUT);
  pinMode(PIN_HORN, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  stopMotors();  // Ensure motors are stopped initially
}

// Setup Access Point
void setupAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID);
  digitalWrite(PIN_LED, HIGH);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print(F("🌐 AP IP address: "));
  Serial.println(myIP);
}

// Process incoming client requests
void processClientRequests() {
  client = server.available();
  if (!client) return;

  String request = readClientRequest();
  if (request.isEmpty()) return;  // No valid data received

  Serial.println("Received data: " + request);
  executeCommand(request);

  ESP.wdtFeed();  // Feed the watchdog to prevent reset
}

// Read client request
String readClientRequest() {
  unsigned long timeout = millis() + 1000;  // 1-second timeout
  while (!client.available()) {
    if (millis() > timeout) return "";  // Return empty string if timeout
  }
  String request = client.readStringUntil('\r');
  client.flush();
  request.remove(0, 5);  // Remove "GET /" from the start
  request.remove(request.length() - 9, 9);  // Remove " HTTP/1.1" from the end
  return request;
}

// Execute command based on request
void executeCommand(const String& request) {
  switch (request[0]) {
    case 'f':
      moveForward();
      client.stop();
      break;
    case 'b':
      moveBackward();
      client.stop();
      break;
    case 'a':
      turnLeft();
      client.stop();
      break;
    case 'd':
      turnRight();
      client.stop();
      break;
    case 'l':
      wheelLeft();
      client.stop();
      break;
    case 'r':
      wheelRight();
      client.stop();
      break;
    case 'j':
      moveBackRight();
      client.stop();
      break;
    case 'k':
      moveBackLeft();
      client.stop();
      break;
    case 's':
      stopMotors();
      client.stop();
      break;
    case 'h':
      hornOn();
      client.stop();
      break;
    case 'g':
      hornOff();
      client.stop();
      break;
    case 'x':
      resetMCU();
      break;
    case 'w':
      reportWiFiSignalStrength();
      break;
    case 'p':
      handlePing();
      break;
    case 'm':
      monitorMemoryUsage();
      break;
    default:
      // Handle speed levels between 1 and 9
      if (request >= "1" && request <= "9") {
        setSpeed(request.toInt());  // Convert string to int and set speed
        client.stop();
      }
      break;
  }
}

// Set motor speed
void setSpeed(int level) {
  const int speedLevels[10] = {70, 81, 95, 105, 122, 150, 196, 272, 400, 1023};
  motorSpeed = speedLevels[level];
}

// Send HTTP response
void sendResponse(const String& jsonData) {
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Access-Control-Allow-Origin: *"));
  client.println(F("Access-Control-Allow-Methods: GET, POST, OPTIONS"));
  client.println(F("Access-Control-Allow-Headers: Content-Type"));
  client.println(F("Connection: close"));
  client.println();
  client.print(jsonData);
}

// Report WiFi signal strength
void reportWiFiSignalStrength() {
  int32_t rssi = WiFi.RSSI();
  sendResponse(String(rssi));
}

// Handle ping command
void handlePing() {
  unsigned long pingTime = millis();
  sendResponse(String(pingTime));
}

// Monitor memory usage
void monitorMemoryUsage() {
  int freeHeap = ESP.getFreeHeap();
  sendResponse(String(freeHeap));
}

// Motor control functions
void moveForward() {
  controlMotors(LOW, HIGH, LOW, motorSpeed, LOW, LOW);
}

void moveBackward() {
  controlMotors(LOW, HIGH, LOW, LOW, LOW, motorSpeed);
}

void turnLeft() {
  controlMotors(HIGH, LOW, HIGH, LOW, LOW, LOW);
}

void turnRight() {
  controlMotors(HIGH, LOW, LOW, LOW, HIGH, LOW);
}

void wheelLeft() {
  controlMotors(HIGH, HIGH, HIGH, motorSpeed, LOW, LOW);
}

void wheelRight() {
  controlMotors(HIGH, HIGH, LOW, motorSpeed, HIGH, LOW);
}

void moveBackLeft() {
  controlMotors(HIGH, HIGH, HIGH, LOW, LOW, motorSpeed);
}

void moveBackRight() {
  controlMotors(HIGH, HIGH, LOW, LOW, HIGH, motorSpeed);
}

void stopMotors() {
  controlMotors(LOW, LOW, LOW, LOW, LOW, LOW);
}

// Control motors
void controlMotors(int steeringEnable, int driveEnable, int leftState, int fwdState, int rightState, int bwdState) {
  digitalWrite(PIN_STEERING_ENABLE, steeringEnable);
  digitalWrite(PIN_DRIVE_ENABLE, driveEnable);
  digitalWrite(PIN_STEERING_LEFT, leftState);
  analogWrite(PIN_DRIVE_FWD, fwdState);
  digitalWrite(PIN_STEERING_RIGHT, rightState);
  analogWrite(PIN_DRIVE_BWD, bwdState);
}

// Horn control functions
void hornOn() {
  digitalWrite(PIN_HORN, HIGH);
}

void hornOff() {
  digitalWrite(PIN_HORN, LOW);
}

// Reset MCU
void resetMCU() {
  ESP.restart();
}