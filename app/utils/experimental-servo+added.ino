#include <ESP8266WiFi.h>
#include <Servo.h>

// Create a Servo object
Servo myServo;

// Constants
const char* WIFI_SSID = "Bachelor";
const char* WIFI_PASSWORD = "Bachelor";
const char* AP_SSID = "NodeMCU Car";
const unsigned long RECONNECT_INTERVAL = 5000;  // 5 seconds

// Pin Definitions
// const int PIN_STEERING_LEFT = D4;
const int servoPin = D4;
const int PIN_DRIVE_FWD = D8;
const int PIN_STEERING_RIGHT = D2; // previously D3
const int PIN_DRIVE_BWD = D7;
const int PIN_DRIVE_ENABLE = D5;
const int PIN_STEERING_ENABLE = D6;
const int PIN_HORN = D1;
const int PIN_LED = D0; // Red LED on NodeMCU
const int PIN_BUTTON = D3; // Flash button on NodeMCU

// Motor Speed
int motorSpeed = 122;  // Default speed

// State
bool isAPMode = false;
unsigned long lastReconnectAttempt = 0;

// Create WiFi objects
WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  setupPins();
  connectToWiFi();
  server.begin();
  myServo.attach(servoPin);
  myServo.write(90);

}

void loop() {
  handleWiFiReconnection();
  processClientRequests();
  checkForAPModeSwitch();  // Check if the button is pressed to switch to AP mode
}

// Initialize pins
void setupPins() {
  // pinMode(PIN_STEERING_LEFT, OUTPUT);
  pinMode(PIN_DRIVE_FWD, OUTPUT);
  pinMode(PIN_STEERING_RIGHT, OUTPUT);
  pinMode(PIN_DRIVE_BWD, OUTPUT);
  pinMode(PIN_DRIVE_ENABLE, OUTPUT);
  pinMode(PIN_STEERING_ENABLE, OUTPUT);
  pinMode(PIN_HORN, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);  // Set button pin as input with pull-up resistor

  stopMotors();  // Ensure motors are stopped initially
}

// Connect to WiFi
void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print(F("Connecting to WiFi"));
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 7000) {
    if (digitalRead(PIN_BUTTON) == LOW) {
      Serial.println(F("\nButton pressed during WiFi connection attempt. Switching to AP mode..."));
      enterAPMode();  // Switch to AP mode
      return;
    }
    delay(300);
    Serial.print(F("."));
  }

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(PIN_LED, HIGH);
    Serial.println(F("\n🟢 Connected to WiFi."));
    Serial.print(F("🌐 IP Address: "));
    Serial.println(WiFi.localIP());
  } else {
    digitalWrite(PIN_LED, LOW);
    Serial.println(F("\n🔴 Failed to connect to WiFi."));
  }
}

// Handle WiFi reconnection attempts
void handleWiFiReconnection() {
  if (!isAPMode && WiFi.status() != WL_CONNECTED) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastReconnectAttempt >= RECONNECT_INTERVAL) {
      lastReconnectAttempt = currentMillis;
      connectToWiFi();
    }
  }
}

// Process incoming client requests
void processClientRequests() {
  client = server.available();
  if (!client) return;

  String request = readClientRequest();
  if (request.isEmpty()) return;  // No valid data received

  Serial.println("Received data: " + request);
  executeCommand(request);

  // ESP.wdtFeed();  // Feed the watchdog to prevent reset
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
  myServo.write(90);
}

void moveBackward() {
  controlMotors(LOW, HIGH, LOW, LOW, LOW, motorSpeed);
  myServo.write(90);
}

void turnLeft() {
  controlMotors(HIGH, LOW, HIGH, LOW, LOW, LOW);
  myServo.write(0);
}

void turnRight() {
  controlMotors(HIGH, LOW, LOW, LOW, HIGH, LOW);
  myServo.write(180);
}

void wheelLeft() {
  controlMotors(HIGH, HIGH, HIGH, motorSpeed, LOW, LOW);
  myServo.write(0);
}

void wheelRight() {
  controlMotors(HIGH, HIGH, LOW, motorSpeed, HIGH, LOW);
  myServo.write(180);
}

void moveBackLeft() {
  controlMotors(HIGH, HIGH, HIGH, LOW, LOW, motorSpeed);
  myServo.write(0);
}

void moveBackRight() {
  controlMotors(HIGH, HIGH, LOW, LOW, HIGH, motorSpeed);
  myServo.write(180);
}

void stopMotors() {
  controlMotors(LOW, LOW, LOW, LOW, LOW, LOW);
  myServo.write(90);
}

// Control motors
void controlMotors(int steeringEnable, int driveEnable, int leftState, int fwdState, int rightState, int bwdState) {
  digitalWrite(PIN_STEERING_ENABLE, steeringEnable);
  digitalWrite(PIN_DRIVE_ENABLE, driveEnable);
  // digitalWrite(PIN_STEERING_LEFT, leftState);
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

// Check for button press to switch to AP mode
void checkForAPModeSwitch() {
  if (digitalRead(PIN_BUTTON) == LOW) {  // Button pressed (active LOW)
    Serial.println(F("Switching to AP mode..."));
    enterAPMode();
  }
}

// Enter Access Point (AP) mode
void enterAPMode() {
  isAPMode = true;
  WiFi.disconnect();  // Disconnect from current WiFi
  delay(1000);  // Short delay before starting AP mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID);
  digitalWrite(PIN_LED, HIGH);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print(F("🌐 AP IP address: "));
  Serial.println(myIP);
  server.begin();
}
