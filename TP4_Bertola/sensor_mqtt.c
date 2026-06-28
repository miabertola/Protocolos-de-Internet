#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "SensorTempC"
#define TOPIC       "sensors/temp"
#define QOS         1
#define TIMEOUT     10000L

int main() {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_create(&client, ADDRESS, CLIENTID,
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS) {
        printf("Error conectando al broker MQTT\n");
        return 1;
    }

    printf("Conectado al broker MQTT\n");

    for (int i = 0; i < 100; i++) {
        char payload[100];
        float temperatura = 20 + rand()%10;
    float co2 = 500 + rand()%1200;
    float presion = 980 + rand()%50;

    snprintf(payload, sizeof(payload),
    "{\"temperatura\":%.2f,"
    "\"co2\":%.2f,"
    "\"presion\":%.2f}",
    temperatura,
    co2,
    presion
    );
        MQTTClient_message pubmsg = MQTTClient_message_initializer;
        pubmsg.payload = payload;
        pubmsg.payloadlen = (int)strlen(payload);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;

        MQTTClient_deliveryToken token;
        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        MQTTClient_waitForCompletion(client, token, TIMEOUT);

        printf("Publicado: %s\n", payload);
        sleep(5);  // Esperar 5 segundos
    }

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return 0;
}