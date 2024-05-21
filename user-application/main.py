import os
import time




def listOfcommands():
    print("AVAILABLE COMMANDS--->\n")
    print(
        "help \n"\
        "activate\n"\
        "log\n"\
        "bath\n"\
        "sim\n"\
        "change val\n"
        "exit\n\n")


if __name__ == "__main__":

    print("\nWelcome\n")
    listOfcommands()

    print("System is going to start----->\n")
    time.sleep(5)

    client = MqttClientData()
    thread = threading.Thread(target=client.mqtt_client, args=(cfg["tempMax"], cfg["tempMin"], cfg["humMax"], cfg["humMin"], cfg["co2Max"], cfg["co2Min"],"check"), kwargs={})
    thread.start()
    client1 = MqttClientBathFloat()
    thread1 = threading.Thread(target=client1.mqtt_client, args=(), kwargs={})
    thread1.start()

    server = CoAPServer(ip, port)
    thread2 = threading.Thread(target=server.listen, args=(), kwargs={})
    thread2.start()

    time.sleep(20)
    start = 0

    try:
        while 1:
            if ResExample.valves == 1 and ResExample.windows == 1:
                    
                    if start == 0:
                        print("\n🖥  🖥  System is running  🖥  🖥\n\n ")
                        print("Use command 'activate' to start sensor node\n\n ")
                        start = 1
                    command = input("COMMAND>")
                    command = command.lower()
                    
                    checkCommand(command, client, client1)
            else:

                print("\n⌛️ ⌛️ ⌛️ Controller is wating for all the sensors ⌛️ ⌛️ ⌛️\n ")
                time.sleep(5)

        
    except KeyboardInterrupt:
        # thread.join()
        # thread1.join()
        # server.close()
        # thread2.join()
        print("SHUTDOWN")
        os._exit(0)