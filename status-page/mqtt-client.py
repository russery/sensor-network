import json
import os
import paho.mqtt.client as mqtt
from time import time, sleep

sensor_types = []
data_store = {}

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("$SYS/#")


def on_message(client, userdata, msg):
    if "sensor-" in msg.topic:
        sensor_name, sensor_type = msg.topic.split("/")
        sensor_name = sensor_name.replace("sensor-", "")
        sensor_data = float(msg.payload)
        if sensor_name not in data_store:
            data_store[sensor_name] = {sensor_type: sensor_data}
        else:
            data_store[sensor_name][sensor_type] = sensor_data
        if sensor_type not in sensor_types:
            sensor_types.append(sensor_type)


def main():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("printer.local", 1883, 60)
    client.subscribe([("+/temperature", 0), ("+/humidity", 0)])

    last_write_time = time()
    last_live_update_time = time()
    if not os.path.exists("static/data/"):
        os.makedirs("static/data/")
    while True:
        client.loop()
        if time() - last_live_update_time > 1:
            last_live_update_time = time()
            # Write latest data to JSON:
            with open("static/data/sensor_data.json", "w") as json_file:
                json.dump(data_store, json_file)
        if time() - last_write_time > 60 * 5:
            last_write_time = time()
            # Append data to log CSV:
            for sensor_type in sensor_types:
                with open("static/data/" + sensor_type + "_data.csv",
                          "a") as csv_file:
                    for sensor in data_store:
                        if sensor_type in data_store[sensor]:
                            line = "{},{},{}\n".format(
                                sensor, int(time()),
                                data_store[sensor][sensor_type])
                            csv_file.write(line)


if __name__ == '__main__':
    main()
