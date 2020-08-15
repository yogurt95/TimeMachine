#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//Potmeter
#define OUTPUT_A 5 //data
#define OUTPUT_B 4  //clock
#define BTN 0  //btn

const char* ssid = "Martin Router King";
const char* password = "password";
const int port = 26;

int pingReturnValue = 0;
int requestDisconnect = 0; //Set to 1 to initiate controlled disconnect

WiFiServer server(port);
WiFiClient client;

bool clientDisconnectNotify = true;

//Potmeter
int counter = 0;
int aState;
int aLastState;
bool lockRotaryEncoder = false;
int currentLengthOfSequence;



void setup() {
  pinMode (OUTPUT_A, INPUT);
  pinMode (OUTPUT_B, INPUT);
  pinMode(BTN, INPUT);


  Serial.begin(74880);
  Serial.println("");

  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Port: ");
  Serial.println(port);

  // Start the TCP server
  server.begin();
}

void loop() {
  ReadEncoder();

  // Listen for connecting clients
  client = server.available();
  if (client) {
    Serial.println("");
    Serial.println("Client connected");
    clientDisconnectNotify = false;


    while (client.connected()) {
      
      ReadEncoder();
      
      if (digitalRead(BTN) == 0) {
        requestDisconnect = 1;
      }

      if (client.available() > 0) {
        Serial.print("Data Available:");
        String tempString;

        Serial.print("Reading...");
        while (client.available() > 0) {
          char c = client.read();
          tempString += c;
        }
        Serial.println(" Done!");
        Serial.print("Recived Data: ");
        Serial.println(tempString);

        if(getValue(tempString,':',0)=="Ping"){
          pingReturnValue = getValue(tempString,':',1).toInt();
        }

        
      }

      float sensorVal = counter;

      String toPc = "VAL:" + String(sensorVal)+ ":" + String(requestDisconnect)+":"+pingReturnValue;
      // Send the distance to the client, along with a break to separate our messages
      client.print(toPc);
      client.print('\r');

      if(requestDisconnect == 1){
        client.stop();
        requestDisconnect = 0;
      }

      // Delay before the next reading
      //delay(10);
    }
  } else {
    if (!clientDisconnectNotify) {
      Serial.println("Client Disconnected");
      clientDisconnectNotify = true;
    }

  }


  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("");
    Serial.print("Wifi disconnected, trying to reconnect");

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println(" Wifi reconnect successful");
    Serial.println("");
  }
}


//Potmeter
void ReadEncoder() {
  aState = digitalRead(OUTPUT_A); // Reads the "current" state of the outputA
  // If the previous and the current state of the outputA are different, that means a Pulse has occured
  if (aState != aLastState) {
    // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
    if (digitalRead(OUTPUT_B) != aState) {
      counter ++;
    } else {
      counter --;
    }
    //Serial.print("Position: ");
    //Serial.println(counter);
  }
  aLastState = aState; // Updates the previous state of the outputA with the current state

  if(lockRotaryEncoder){
    ContentPositionCorrection();
  }
}
void ContentPositionCorrection() {
  if (counter > 0) {
    counter = 0;
  }
  if (counter < -currentLengthOfSequence + 8) {
    counter = -currentLengthOfSequence + 8;
  }
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
