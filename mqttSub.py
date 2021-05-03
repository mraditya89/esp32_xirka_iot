import paho.mqtt.client as mqtt
import json

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("house")

def on_message(client, userdata, msg):
    try: 
        topic = msg.topic
        x = json.loads(msg.payload.decode('utf-8'))
        print("-------------------------------------------")
        print(f'Pesan masuk : {x}')

        if (x["payload"]["people"]>50):
            print("NYALAKAN BUZZER BAHAYA")
        elif (x["payload"]["people"]>25):
            print("Para pengunjung silakan jaga jarak anda")
        print("-------------------------------------------")
    except Exception as e:
        print("ERROR", e);

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set("admin", "mypassword")
client.connect("192.168.43.145", 1883, 60)

client.loop_forever()

# MQTTFX {"timestamp":1619854969731, "payload": { "humidity": 28} }
# >mosquitto_pub -t house -m "{\"timestamp\":21, \"payload\":{\"people\":60}}"
