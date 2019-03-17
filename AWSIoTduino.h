#ifndef __AWSIoTDuino_H__
#define __AWSIoTDuino_H__

//This before line below also needs to be adjusted in PubSubClient.h
//#define MQTT_MAX_PACKET_SIZE 512

#include <Stream.h>
#include <PubSubClient.h>

extern "C" {
  #include "user_interface.h"
}

class Thing {
  public:
    Thing(char *thingName, PubSubClient& client) : thingName(thingName), client(&client) {
      sprintf(topics[0], "$aws/things/%s/shadow/update/accepted", thingName);
      sprintf(topics[1], "$aws/things/%s/shadow/update/rejected", thingName);
      sprintf(topics[2], "$aws/things/%s/shadow/update/delta", thingName);
      sprintf(topics[3], "$aws/things/%s/shadow/get/accepted", thingName);
      sprintf(topics[4], "$aws/things/%s/shadow/get/rejected", thingName);
      sprintf(publishTopic, "$aws/things/%s/shadow/update", thingName);
      sprintf(getTopic, "$aws/things/%s/shadow/get", thingName);
    }
    
    void loop();
    bool connectToMQTT();
    void subscribeToTopics();
    void setCallback(MQTT_CALLBACK_SIGNATURE);
    void setConnectedCallback(std::function<void()> callback);
   
    void sendState(char *state);
    void getState();

    char* generateClientID();

  private:
    char *thingName, *ep, *key, *secret, *region;
    char topics[5][64], publishTopic[64], getTopic[64];
    int port;
    
    //AWSWebSocketClient *awsWSclient;
    PubSubClient *client;
    
    MQTT_CALLBACK_SIGNATURE;
    std::function<void()> connectedCallback;
};

#endif
