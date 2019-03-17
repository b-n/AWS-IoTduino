#include "AWSIoTDuino.h"

void Thing::setup() {
  awsWSclient->setAWSRegion(region);
  awsWSclient->setAWSDomain(ep);
  awsWSclient->setAWSKeyID(key);
  awsWSclient->setAWSSecretKey(secret);
  awsWSclient->setUseSSL(true);
}

void Thing::loop() {
  if (!mqttclient->connected()) {
    if (connectToMQTT()) {
      Serial.println("Connected to MQTT");
      subscribeToTopics();
      if (connectedCallback != NULL) connectedCallback();
    }
  }

  if (mqttclient->connected()) {
    mqttclient->loop();
  }
}

bool Thing::connectToMQTT() {  
  if (mqttclient->connected()) mqttclient->disconnect();

  mqttclient->setServer(ep, port);
  char *clientId = generateClientID();
  bool connected = mqttclient->connect(clientId);
  
  if (!connected) {
    Serial.print("failed to connect to MQTT, state=");
    Serial.println(mqttclient->state());
  }

  return connected;
}

void Thing::subscribeToTopics() {
  mqttclient->setCallback(callback);

  Serial.println("Subscribed to:");
  for (int i = 0; i < 5; i++) {
    Serial.println(topics[i]);
    mqttclient->subscribe(topics[i]);
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
  mqttclient->publish(publishTopic, state);
}

void Thing::getState() {
  mqttclient->publish(getTopic, "");
}


char* Thing::generateClientID() {
  char* cID = new char[23]();
  for (int i=0; i<22; i+=1)
    cID[i]=(char)random(1, 256);
  return cID;  
}
