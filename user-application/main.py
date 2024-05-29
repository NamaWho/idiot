import os
import time
from models.database import Database
from models.observer import ObserveSensor
from mysql.connector import Error

def listOfcommands():
    print("|----- AVAILABLE COMMANDS -----|")
    print("| 1. health                    |")
    print("| 2. monitor                   |")
    print("| 3. failures                  |")
    print("| 4. stop                      |")
    print("| 5. exit                      |")
    print("|------------------------------|\n")

def getSensorsAndActuator(connection):
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

            return sensors
        except Error as e:
            print(f"Error retrieving sensor data: {e}")
            return None
    
def getSystemHealth(connection):
    if connection is not None and connection.is_connected():
        sensors = getSensorsAndActuator(connection)
        if sensors is not None:
            print("\n|------------ SYSTEM HEALTH ------------|")
            for key, value in sensors.items():
                print(f"| {key.capitalize()}:\t{value['status']} at {value['ip_address']} \t|")
            print("|---------------------------------------|\n")
        else:
            print("Error retrieving sensor data.")
    else:
        print("Error connecting to the database.")
   
    
def monitorSystem(connection):
    # start observing sensors and actuator until stopped, creating an observer relationship with HelperClient
    sensors = getSensorsAndActuator(connection)
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
        

def retrieveComponentFailuresFromDB(connection):
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
                print("\n|------------ COMPONENT FAILURES ------------|")
                for row in failure_data:
                    print(f"| {row[1]}:\t{row[2]} at {row[3]} \t|")
                print("|--------------------------------------------|\n")
            else:
                print("No component failures found.")
        except Error as e:
            print(f"Error retrieving failure data: {e}")


def stopSensorsAndActuator():
    print("Stopping all sensors and the actuator...")
    # Add code to stop sensors and actuator
    time.sleep(2)  # Simulating sensor and actuator stopping time


if __name__ == "__main__":

    print(r'''
_________ _______ _________   _______  _______  _______  ______  _________ _______ _________ _______  _______ 
\__   __/(  ___  )\__   __/  (  ____ )(  ____ )(  ____ \(  __  \ \__   __/(  ____ \\__   __/(  ___  )(  ____ )
   ) (   | (   ) |   ) (     | (    )|| (    )|| (    \/| (  \  )   ) (   | (    \/   ) (   | (   ) || (    )|
   | |   | |   | |   | |     | (____)|| (____)|| (__    | |   ) |   | |   | |         | |   | |   | || (____)|
   | |   | |   | |   | |     |  _____)|     __)|  __)   | |   | |   | |   | |         | |   | |   | ||     __)
   | |   | |   | |   | |     | (      | (\ (   | (      | |   ) |   | |   | |         | |   | |   | || (\ (   
___) (___| (___) |   | |     | )      | ) \ \__| (____/\| (__/  )___) (___| (____/\   | |   | (___) || ) \ \__
\_______/(_______)   )_(     |/       |/   \__/(_______/(______/ \_______/(_______/   )_(   (_______)|/   \__/
                                                                                                              
''')

    listOfcommands()
    start = 0
    database = Database()
    connection = database.connect_db()

    try:
        while 1:
            command = input("COMMAND> ")
            command = command.lower()

            if command == "health":
                getSystemHealth(connection)
            elif command == "monitor":
                monitorSystem(connection)
            elif command == "failures":
                retrieveComponentFailuresFromDB(connection)
            elif command == "stop":
                print("Stopping sensors and actuator...")
                stopSensorsAndActuator()
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