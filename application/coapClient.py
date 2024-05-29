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
        self.period = 5
        self.check_sensors()



    def check_sensors(self):

        timer = threading.Timer(self.period, self.check_sensors)
        timer.setDaemon(True)
        timer.start()

        sensors = {
            "pressure": {"status": 0, "ip": "", "port": 0},
            "vibration": {"status": 0, "ip": "", "port": 0},
            "voltage": {"status": 0, "ip": "", "port": 0},
            "rotation": {"status": 0, "ip": "", "port": 0}
        }

        try:
            if self.connection and self.connection.is_connected():
                cursor = self.connection.cursor()

                select_sensor_query = """
                SELECT ip_address, port, type, status
                FROM sensor
                """

                cursor.execute(select_sensor_query)

                sensor_data = cursor.fetchall()

                for row in sensor_data:
                    ip_address, port, type, status = row

                    # Update the sensor dictionary with the sensor data
                    sensors[type]["ip"] = ip_address
                    sensors[type]["port"] = port
                    sensors[type]["status"] = status

                cursor.close()
        
        except Error as e:
            print(f"Error retrieving sensor data: {e}")
            sys.exit(1)


        # for each sensor, check the status
        for sensor_type, sensor in sensors.items():
            try:
                if(sensor["ip"] == ""):
                    continue

                client = HelperClient(server=(sensor["ip"], 5683))
                path = f"{sensor_type}/status"
                
                max_attempts = 3

                status = None

                for attempt in range(max_attempts):
                    try:
                        data = json.loads(client.get(path).payload)
                        if "status" in data:
                            status = data["status"]
                            break
                        
                        else:
                            time.sleep(1)

                    except Exception as e:
                        print(f"Error retrieving sensor status: {e}")
                        time.sleep(1)
                
                client.stop()
                

                # After 3 attempts, if the status is still None, then the sensor is not reachable
                if status is None:
                    status = 0

                
                if sensor["status"] != status:
                    print(f"Updating {sensor_type} sensor status to {status}")

                    try:

                        self.update_sensor_status(self.connection, sensor_type, sensor["ip"], status)
                    
                    except Error as e:
                        print(f"Error updating sensor status: {e}")
                        sys.exit(1)
                    

            except Error as e:
                print(f"Error retrieving sensor data: {e}")
                sys.exit(1)



    def update_sensor_status(self, connection, sensor_type, ip, status):
        
        try:
            if connection and connection.is_connected():
                cursor = connection.cursor()

                update_sensor_query = """
                UPDATE sensor
                SET status = %s
                WHERE ip_address = %s
                """

                cursor.execute(update_sensor_query, (status, ip))
                connection.commit()
                cursor.close()

        except Error as e:
            # Throw exception to be caught by the caller
            raise Error(f"Error updating {sensor_type} sensor status: {e}")
        