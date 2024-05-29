from coapthon.client.helperclient import HelperClient
from coapthon.utils import parse_uri
from mysql.connector import Error
from coapthon.resources.resource import Resource
import json
import sys
import time
import threading
from models.database import Database

class CoapClient:

    def __init__(self):
        self.client = None
        self.database = Database()
        self.connection = self.database.connect_db()
        self.period = 10
        self.check_sensors()

    def check_sensors(self):

        timer = threading.Timer(self.period, self.check_sensors)
        timer.setDaemon(True)
        timer.start()

        # sensors = {
        #     "pressure": {"status": 0, "ip": ""},
        #     "vibration": {"status": 0, "ip": ""},
        #     "voltage": {"status": 0, "ip": ""},
        #     "rotation": {"status": 0, "ip": ""}
        # }

        
        sensors = self.retrieve_nodes_from_db()

        if sensors is None:
            return

        for key, value in sensors.items():

            if value['ip'] == "":
                continue

            print (f"Checking {key} sensor at {value['ip']}")

            current_status = self.check_sensor_status(value['ip'], key + "/status")

            if current_status != value['status']:
               print(f"Sensor {key} status changed from {value['status']} to {current_status}")
               
               self.update_sensor_status(key, value['ip'], current_status)

        


    def retrieve_nodes_from_db(self):

        sensors = {
            "pressure": {"status": 0, "ip": ""},
            "vibration": {"status": 0, "ip": ""},
            "voltage": {"status": 0, "ip": ""},
            "rotation": {"status": 0, "ip": ""}
        }

        try:
            if self.connection and self.connection.is_connected():
                
                cursor = self.connection.cursor()

                select_sensor_query = """
                    SELECT ip_address, type, status
                    FROM sensor
                    WHERE type IN ('pressure', 'vibration', 'voltage', 'rotation')
                """

                cursor.execute(select_sensor_query)
                sensor_data = cursor.fetchall()
                cursor.close()

                for row in sensor_data:
                    ip_address, type, status = row
                    sensors[type]["status"] = int(status)
                    sensors[type]["ip"] = ip_address

                return sensors

            else:
                print("Database connection lost")
                return None

        except Error as e:
            print(f"Error retrieving sensor data: {e}")
            return None
            

    def check_sensor_status(self, ip_address, resource):

        port = 5683
        client = HelperClient(server=(ip_address, port))

        max_attempts = 3

        for attempt in range(max_attempts):
            try:
                response = client.get(resource)

                client.stop()

                data = json.loads(response.payload)

                return data['status']
            
            except Exception as e:
                print(f"Error: {e}")
                client.stop()

        print(f"Failed to get status of sensor at {ip_address}")        

        return 0

    def update_sensor_status(self, type, ip_address, status):
            
            try:

                if self.connection and self.connection.is_connected():
                    
                    cursor = self.connection.cursor()

                    update_sensor_query = """
                        UPDATE sensor
                        SET status = %s
                        WHERE ip_address = %s AND type = %s
                    """

                    cursor.execute(update_sensor_query, (status, ip_address, type))
                    self.connection.commit()
                    cursor.close()

                    print(f"Updated {type} sensor status to {status}")
                
                else:
                    print("Database connection lost")
                   
            except Error as e:
                print(f"Error updating sensor status: {e}")
               
            
                











   
        