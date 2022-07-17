#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "MQTTClient.h"
#include <cjson/cJSON.h>

#include <eie_device/eie_device.h>

/** EieDevice structure */
struct EieDevice {
    /** MQTT Client */
    MQTTClient *client;
    /** JSON containing Ditto features */
    char *features;
};

struct EieDevice * eie_device_create(struct EieDeviceConfig *cfg){
    int ret;
    struct EieDevice *device;

    device = (struct EieDevice *)calloc(1, sizeof(struct EieDevice));
    
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    MQTTClient_create(&(device->client), ADDRESS, CLIENTID,
                        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    ASSERT_NE(device->client, nullptr);

    ret = MQTTClient_setCallbacks(device->client, NULL, eie_device_conn_lost_cb, eie_device_msg_arrived_cb, eie_device_msg_delivered_cb);
    ASSERT_EQ(ret, 0);

    ret = MQTTClient_connect(device->client, &conn_opts);
    ASSERT_EQ(ret, MQTTCLIENT_SUCCESS) << "Failed to connect, ret: " << ret << std::endl;
    
    return device;
}

int eie_device_destroy(struct EieDevice *device){
    int ret;

    ret = MQTTClient_disconnect(device->client, 10000);
    ASSERT_EQ(ret, MQTTCLIENT_SUCCESS);

    MQTTClient_destroy(&(device->client));
    free(device);

    return ret;
}

int eie_device_start(struct EieDevice *device){
    int ret = 0;
    const char topic[100] = "example/test";
    char ch;

    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n", topic, CLIENTID, QOS);
    ret = MQTTClient_subscribe(device->client, topic, QOS);
    ASSERT_EQ(ret, 0);
    
    return ret;
}

int eie_device_send_message(struct EieDevice *device, char *msgJson){
    int ret;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    pubmsg.payload = "Test";
    pubmsg.payloadlen = strlen("Test");
    pubmsg.qos = 0;
    pubmsg.retained = 0;
    int deliveredtoken = 0;

    ret = MQTTClient_publishMessage(device->client, "eie_device/tests", &pubmsg, &token);
    ASSERT_EQ(ret, MQTTCLIENT_SUCCESS);

    printf("Waiting for publication");
    while(deliveredtoken != token);
    return ret;
}

int eie_device_msg_arrived_cb(void *context, char *topicName, int topicLen,
                              MQTTClient_message *message){
    printf("Message recieved.\n");
}

int eie_device_msg_delivered_cb(void *context, MQTTClient_deliveryToken dt){
    printf("Message delivered.\n");
}

int eie_device_conn_lost_cb(void *context, char *cause){
    printf("Connection lost.\n");
}