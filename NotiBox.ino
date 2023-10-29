#include <SoftwareSerial.h>

void notify(String message);
void start();

#include <ESP8266WiFi.h>
#include <WiFiClientSecureAxTLS.h> 

// Wifi Config
#define WIFI_SSID "TjP"
#define WIFI_PASSWORD "987654321"

// Line Token
#define LINE_TOKEN "YYXQRwq5ATOrsEdrSepNhuh28E2V1JKk24JZD959zog"

int trigPin = 2;
int echoPin = 12;
long duration, cm, inches;

String message = "Mail received!!";

void setup() {

  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  WiFi.mode(WIFI_STA);

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	Serial.print("connecting");

	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(500);
	}

	Serial.println();
	Serial.print("connected: ");
	Serial.println(WiFi.localIP());
  start();
}

void loop()
{

  digitalWrite(trigPin, LOW);
  delayMicroseconds(1);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  inches = (duration / 2) / 74;
  Serial.println(inches);
  if (inches < 5) {
    Serial.println("Sending text Notification...");
    notify(message);

    delay(5000);
    Serial.println("Rescan...");
  }
  delay(2000);
}

void start() {
  for (int i = 10;i > 0;i--) {
    Serial.println(i);
    delay(1000);
  }
}

void notify(String message) {
	axTLS::WiFiClientSecure client; 

	if (!client.connect("notify-api.line.me", 443)) {
		Serial.println("connection failed");
		return;
	}

	String req = "";
	req += "POST /api/notify HTTP/1.1\r\n";
	req += "Host: notify-api.line.me\r\n";
	req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
	req += "Cache-Control: no-cache\r\n";
	req += "User-Agent: ESP8266\r\n";
	req += "Connection: close\r\n";
	req += "Content-Type: application/x-www-form-urlencoded\r\n";
	req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
	req += "\r\n";
	req += "message=" + message;
	// Serial.println(req);
	client.print(req);

	delay(20);

	while (client.connected()) {
		String line = client.readStringUntil('\n');
		if (line == "\r") {
			break;
		}
	}
}