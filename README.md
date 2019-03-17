# AWS-IoTduino

An opionated communication with AWS IoT Core

Current library dependencies are:

- [PubSubClient](https://github.com/knolleary/pubsubclient)
- [AWS-MQTT-Websockets](https://github.com/odelot/aws-mqtt-websockets)

## Usage

###Basic usage

```cpp
#include <AWSIoTduino.h>

Thing device(THINGNAME, AWS_ENDPOINT, AWS_KEY, AWS_SECRET, AWS_REGION)

void setup() {
  //WiFi.begin(SSID, PSK);

  device.setup();
}

void loop() {
  device.loop();
}

```

###Publishing State

Using [ArduinoJSON](https://github.com/bblanchon/ArduinoJson)

```cpp
#include <ArduinoJson.h>

void updateAWS() {
  const size_t capacity = 3*JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2);
  DynamicJsonBuffer jsonBuffer(capacity);
  
  JsonObject& root = jsonBuffer.createObject();
  JsonObject& state = root.createNestedObject("state");
  JsonObject& reported = state.createNestedObject("reported");
  reported["hello"] = "world";

  char buffer[512];
  root.printTo(buffer, root.measureLength()+1);

  awsThing.sendState(buffer);
}
```

###Using callback

Example using MQTT callback. Function stub must be as follows

```cpp
//Also change this is PubSubClient.h
#define MQTT_MAX_PACKET_SIZE 512

void AWSCallback(char* topic, byte* payload, unsigned int length) {
  char buf[MQTT_MAX_PACKET_SIZE];
  strncpy(buf, (const char *)payload, length);
  buf[length] = '\0';

  //Check the topic for accepted/get for getting state
  if (strcmp(topic,"$aws/things/thingNameHere/shadow/update/accepted") == 0 || strcmp(topic,"$aws/things/thingNameHere/shadow/get/accepted") == 0) {
    DynamicJsonBuffer jsonBuffer(512);
    JsonObject& root = jsonBuffer.parseObject(buf);
    char *hello = root["state"]["desired"]["hello"];
    Serial.println(hello); //world
  }
}

void setup() {
  //...
  
  device.setup();
  device.setCallback(AWSCallback);
}
```

Example using Connected Callback. This function is called when the device 
connects to AWS. Handy if you want to get the current state of the shadow as 
soon as it reconnects

```cpp
void connectedCallback() {
  //if you do this too quickly after connect, you might not be subscribed to the
  //   correct queue to get the state 
  delay(100);
  awsThing.getState();
}

void setup() {
  //...

  device.setup();
  device.setConnectedCallback(connectedCallback);
}
```

### getState

Calling `void Thing::getState();` will publish a blank message to the get topic 
of your device ($aws/things/thingname/shadow/get). If you have a callback 
enabled then you should get a message back on the get/accepted topic with the 
shadow state. See the connectedCallback function under the callback section for 
more info.

## TODO

- Put proper debug statements in, instead of Serial.print
- Fix the logic on topic subscription. Is fairly hardcoded now
