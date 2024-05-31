import os
import time
from models.database import Database
from models.observer import ObserveSensor
from mysql.connector import Error
from coapthon.client.helperclient import HelperClient

def listOfcommands():
    print("\n|----- AVAILABLE COMMANDS -----|")
    print("| 1. health                    |")
    print("| 2. monitor                   |")
    print("| 3. failures                  |")
    print("| 4. switch                    |")
    print("| 5. exit                      |")
    print("|------------------------------|\n")

def getSensorsAndActuator():
     
    connection = Database.connect_db()

    if connection is not None and connection.is_connected():
        sensors = {
            "pressure": {"status": 0, "ip_address": ""},
            "vibration": {"status": 0, "ip_address": ""},
            "voltage": {"status": 0, "ip_address": ""},
            "rotation": {"status": 0, "ip_address": ""},
            "alarm": {"status": 0, "ip_address": ""}
        }  
            
        try:
            cursor = connection.cursor()
            select_sensor_query = """
            SELECT ip_address, type, status
            FROM sensor
            """
            cursor.execute(select_sensor_query)
            sensor_data = cursor.fetchall()
            cursor.close()

            for row in sensor_data:
                ip_address, type, status = row
                sensors[type]["status"] = int(status)
                sensors[type]["ip_address"] = ip_address

            connection.close()
            return sensors
        except Error as e:
            print(f"Error retrieving sensor data: {e}")
            connection.close()
            return None
            
    
def getSystemHealth():
    try:
        sensors = getSensorsAndActuator()
        if sensors is not None:
            print("\n|------------ SYSTEM HEALTH ------------|")
            for key, value in sensors.items():
                print(f"| {key.capitalize()}:\t{value['status']} at {value['ip_address']} \t|")
            print("|---------------------------------------|\n")
        else:
            print("Error retrieving sensor data.")
    except Error as e:
        print("Error connecting to the database.")
   
    
def monitorSystem():
    # start observing sensors and actuator until stopped, creating an observer relationship with HelperClient
    sensors = getSensorsAndActuator()
    print("System is being monitored. Press 'Ctrl + C' to stop monitoring.")
    time.sleep(2) 
    observers = []

    if sensors is not None:
        for key, value in sensors.items():
            if key != "alarm":
                if value["status"] == 1:
                    observers.append(ObserveSensor((value["ip_address"], 5683), key))
    try:
        while 1:
            pass
    except KeyboardInterrupt:
        for observer in observers:
            observer.stop_observing()

def retrieveComponentFailuresFromDB():
    connection = Database.connect_db()

    if connection is not None and connection.is_connected():
        try:
            cursor = connection.cursor()
            select_failure_query = """
            SELECT * FROM failures
            """
            cursor.execute(select_failure_query)
            failure_data = cursor.fetchall()
            cursor.close()

            if len(failure_data) > 0:
                print("\n|------------ COMPONENT HEALTH CHECK ------------|")
                for row in failure_data:
                    print(f"| {row[1]}:\t{row[2]} \t|")
                print("|--------------------------------------------|\n")
            else:
                print("No component failures found.")
        except Error as e:
            print(f"Error retrieving failure data: {e}")
        
        connection.close()

def switchSensorStatus():
    sensors = getSensorsAndActuator()
    if sensors is not None:
        # print the sensor type
        print("\nSelect the sensor to switch on/off")
        sensor_list = []
        for i, key in enumerate(sensors):
            if key != "alarm":
                print(f"{i+1}. {key.capitalize()}")
                sensor_list.append(key)

        while 1:
            print("Enter the number of the sensor to switch on/off, or 'exit' to cancel")
            try:
                command  = input("SENSOR> ")

                if command.lower() == "exit":
                    break

                sensor_index = int(command)

                if sensor_index in range(1, len(sensor_list)+1):
                    print(f"Switching {sensor_list[sensor_index-1]} sensor status...")

                    print("Insert on to switch on, off to switch off")

                    while 1:
                        command = input("STATUS> ")
                        command = command.lower()

                        if command == "on":
                            switch_sensor(sensor_list[sensor_index-1], sensors[sensor_list[sensor_index-1]]["ip_address"], sensor_list[sensor_index-1] + '/status', 'on')
                            break
                        elif command == "off":
                            switch_sensor(sensor_list[sensor_index-1], sensors[sensor_list[sensor_index-1]]["ip_address"], sensor_list[sensor_index-1] + '/status', 'off')
                            break
                        else:
                            print("Invalid input. Try again.") 

                else:
                    print("Invalid input. Try again.")
            
            except ValueError:
                print("Invalid input. Try again.")

def switch_sensor(sensor_type, ip_address, resource, status):
    port = 5683
    client = HelperClient(server=(ip_address, port))

    try:

        if status == "on":
            payload = "value=1"
        else:
            payload = "value=0"

        response = client.put(resource, payload)
        
        if response.code == 68:
            print(f"Sensor {sensor_type} status switched {status}")

        else:
            print(f"Error switching sensor {sensor_type} status")

    except Exception as e:
        print(f"Error: {e}")

    client.stop()



if __name__ == "__main__":

    print(r'''
_________ ______    _________ _______ _________
\__   __/(  __  \   \__   __/(  ___  )\__   __/
   ) (   | (  \  )     ) (   | (   ) |   ) (   
   | |   | |   ) |     | |   | |   | |   | |   
   | |   | |   | |     | |   | |   | |   | |   
   | |   | |   ) |     | |   | |   | |   | |   
___) (___| (__/  )_ ___) (___| (___) |   | |   
\_______/(______/(_)\_______/(_______)   )_(                                                                                                   
''')

    listOfcommands()
    start = 0

    try:
        while 1:
            command = input("COMMAND> ")
            command = command.lower()

            if command == "health":
                getSystemHealth()
            elif command == "monitor":
                monitorSystem()
            elif command == "failures":
                retrieveComponentFailuresFromDB()
            elif command == "switch":
                print("Switching sensor status...")
                switchSensorStatus()
            elif command == "help":
                listOfcommands()
            elif command == "exit":
                print("Exiting...")
                break
            else:
                print("Invalid command. Type 'help' for available commands.")

    except KeyboardInterrupt:
        print("SHUTDOWN")
        os._exit(0)