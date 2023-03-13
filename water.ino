#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
// #include <Effortless_SPIFFS.h>

char ssid[] = "";        // your network SSID (name)
char pass[] = ""; // your network password

int keyIndex = 0; // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS; //status of wifi
WiFiServer server(80);       //declare server object and spedify port, 80 is port used for internet
String header;
const int dry = 1100;
const int pumpPin = D1;
const int soilSensor = A0;
const unsigned long SECOND = 1000;
const unsigned long HOUR = 3600 * SECOND;

void setup()
{

  pinMode(pumpPin, OUTPUT);
  pinMode(soilSensor, INPUT);
  Serial.begin(9600);
  digitalWrite(pumpPin, HIGH);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop()
{
  StaticJsonDocument<256> doc;
  readvalues();
  WiFiClient client = server.available(); // listen for incoming clients

  if (client)
  {
    String currentLine = ""; // make a String to hold incoming data from the clien
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    { // loop while the client's connected
      currentTime = millis();
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        header += c;
        if (c == '\n')
        {
          if (currentLine.length() == 0)
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json"));
            Serial.println(header);
            if (header.indexOf("POST") >= 0)
            {
              Serial.println("");
              serializeJson(doc, Serial);
              serializeJsonPretty(doc, client);
            }
          }
        }
      }
      client.stop();
      break
    }
  }
}
void readvalues()
  {
    int moisture = analogRead(soilSensor);
    doc["Moist"] = moisture;
    doc["Date"] = "";
    Serial.println(moisture);
    delay(5000);

    if (moisture >= dry)
    {
      // the soil is too dry, water!
      Serial.println("Watering starts now..moisture is " + String(moisture));
      doc["Water"] = "yes";
      digitalWrite(pumpPin, LOW);

      // keep watering for 2 sec
      delay(2000);

      // turn off water
      digitalWrite(pumpPin, HIGH);
      Serial.println("Done watering.");
    }
    else
    {
      Serial.println("Moisture is adequate. No watering needed " + String(moisture));
      doc["Water"] = "no";
    }
  }

  