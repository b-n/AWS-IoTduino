#include "AWSIoTDuino.h"

void Thing::loop() {
  if (!client->connected()) {
    if (connectToMQTT()) {
      Serial.println("Connected to MQTT");
      subscribeToTopics();
      if (connectedCallback != NULL) connectedCallback();
    }
  }

  if (client->connected()) {
    client->loop();
  }
}

bool Thing::connectToMQTT() {  
  if (client->connected()) client->disconnect();

  client->setServer(ep, port);
  char *clientId = generateClientID();
  bool connected = client->connect(clientId);
  
  if (!connected) {
    Serial.print("failed to connect to MQTT, state=");
    Serial.println(client->state());
  }

  return connected;
}

void Thing::subscribeToTopics() {
  client->setCallback(callback);

  Serial.println("Subscribed to:");
  for (int i = 0; i < 5; i++) {
    Serial.println(topics[i]);
    client->subscribe(topics[i]);
  }
  Serial.println("MQTT subscribed");
}

void Thing::setCallback(MQTT_CALLBACK_SIGNATURE) {
  this->callback = callback;
}

void Thing::setConnectedCallback(std::function<void()> callback) {
  this->connectedCallback = callback;
}

void Thing::sendState(char *state) {
  client->publish(publishTopic, state);
}

void Thing::getState() {
  client->publish(getTopic, "");
}


char* Thing::generateClientID() {
  char* cID = new char[23]();
  for (int i=0; i<22; i+=1)
    cID[i]=(char)random(1, 256);
  return cID;  
}
