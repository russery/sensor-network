import paho.mqtt.client as mqtt

handler_list = {};

class SensorHandler(object):
	def __init__(self, name):
		last_write = None
		name = name

	def handle(data):
		pass




def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("$SYS/#")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

def main():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("printer.local", 1883, 60)
    client.subscribe([("+/temperature", 0), ("+/humidity", 0)])

    while True:
        client.loop()

if __name__ == '__main__':
    main()