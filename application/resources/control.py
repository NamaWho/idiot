
from mysql.connector import Error
from coapthon.resources.resource import Resource
from models.database import Database
from coapthon.client.helperclient import HelperClient
import json
import time
import threading
import re

class Control(Resource):

    sensors = {
        "pressure": {"status": 0, "ip_address": "", "port": 0},
        "vibration": {"status": 0, "ip_address": "", "port": 0},
        "voltage": {"status": 0, "ip_address": "", "port": 0},
        "rotation": {"status": 0, "ip_address": "", "port": 0}
    }  

    def __init__(self, name="Control", coap_server=None):
        super(Control, self).__init__(name, coap_server, visible=True, observable=True, allow_children=False)
        self.payload = "Control Resource"
        self.database = Database()
        self.connection = self.database.connect_db()
        self.period = 30       
        self.update()

    def render_GET(self, request):
        print(f"Checking sensor status")
        self.fetch_sensors_from_db()
        self.update_sensors_status()
        return self
    
    def fetch_sensors_from_db(self):
        if not self.connection.is_connected():
            print("Database connection lost")
            self.payload = None
            return self
        
        try:
            cursor = self.connection.cursor()
            select_sensor_query = """
            SELECT ip_address, port, type, status
            FROM sensor
            WHERE type IN ('pressure', 'vibration', 'voltage', 'rotation')
            """
            cursor.execute(select_sensor_query)
            sensor_data = cursor.fetchall()
            cursor.close()

            for row in sensor_data:
                ip_address, port, type, status = row
                self.sensors[type]["status"] = status
                self.sensors[type]["ip_address"] = ip_address
                self.sensors[type]["port"] = port
        
        except Error as e:
            print(f"Error retrieving sensor data: {e}")
            self.payload = None

    def update_sensors_status(self):
        try:
            for key, values in self.sensors.items():
                if values["ip_address"] == "":
                    print(f"{key} sensor not registered")
                    continue

                self.retrieve_and_update_sensor_status(key, values["status"])
        
        except Error as e:
            print(f"Error retrieving sensor status: {e}")
            self.payload = None

        sensor_data = {
            sensor: {
                "ip_port": f"{values['ip_address']}:{values['port']}",
                "status": self.sensors[sensor]["status"]
            } for sensor, values in self.sensors.items()
        }
        
        self.payload = json.dumps(sensor_data)
        print(f"Sensor data: {self.payload}")


    # def render_GET(self, request):
    #     # This method is called when a GET request is made to the resource

    #     print(f"Checking sensor status")
 
    #     try:
    #         if self.connection and self.connection.is_connected():
    #             cursor = self.connection.cursor()

    #             select_sensor_query = """
    #             SELECT ip_address, port, type, status
    #             FROM sensor
    #             WHERE type IN ('pressure', 'vibration', 'voltage', 'rotation')
    #             """
    #             cursor.execute(select_sensor_query)
    #             sensor_data = cursor.fetchall()

    #             # If 4 sensors are not registered, return None
    #             #if len(sensor_data) != 4:
    #                # print("Not all sensors are registered")
    #                 #self.payload = None
    #                 #return self

        
    #         for row in sensor_data:
    #             ip_address, port, type, status = row
    #             # Update the sensor dictionary with the sensor data
    #             self.sensors[type]["status"] = status
    #             self.sensors[type]["ip_address"] = ip_address
    #             self.sensors[type]["port"] = port

    #         cursor.close()

    #     except Error as e:
    #         print(f"Error retrieving sensor data: {e}")
    #         self.payload = None
    #         return self

    #     # Retrieve the current status for each sensor
    #     try:
    #         for key, values in self.sensors.items():
    #             if values["ip_address"] == "":
    #                 print(f"{key} sensor not registered")
    #                 continue

    #             self.retrieve_and_update_sensor_status(key, values["status"])
        
    #     except Error as e:
    #         print(f"Error retrieving sensor status: {e}")
    #         self.payload = None
    #         return self

    #     sensor_data = {
    #         "pressure": {
    #             "ip_port": self.sensors["pressure"]["ip_address"] + ":" + str(self.sensors["pressure"]["port"]), 
    #             "status": self.sensors["pressure"]["status"]
    #             },
    #         "vibration": {
    #             "ip_port": self.sensors["vibration"]["ip_address"] + ":" + str(self.sensors["vibration"]["port"]), 
    #             "status": self.sensors["vibration"]["status"]
    #             },
    #         "voltage": {
    #             "ip_port": self.sensors["voltage"]["ip_address"] + ":" + str(self.sensors["voltage"]["port"]), 
    #             "status": self.sensors["voltage"]["status"]
    #             },
    #         "rotation": {
    #             "ip_port": self.sensors["rotation"]["ip_address"] + ":" + str(self.sensors["rotation"]["port"]),
    #             "status": self.sensors["rotation"]["status"]
    #             }
    #     }

    #     self.payload = json.dumps(sensor_data)
    #     return self

    def update(self):
        self.payload = "Observable Resource"

        if not self._coap_server.stopped.isSet():
            timer = threading.Timer(self.period, self.update)
            timer.setDaemon(True)
            timer.start()

            if self._coap_server is not None:
                self._coap_server.notify(self)
            

    def retrieve_and_update_sensor_status(self, sensor_type, status=0):
        MAX_RETRIES = 3
        TIMEOUT = 2

        for attempt in range(MAX_RETRIES):
            try:
                client = HelperClient(server=(self.sensors[sensor_type]["ip_address"], self.sensors[sensor_type]["port"]))
                path = sensor_type + "/status"
                response = client.get(path, timeout=TIMEOUT).payload

                # If the sensor is not reachable, set the status to 0
                if not response:
                    print(f"{sensor_type} sensor not reachable")
                    sensor_status = 0
                else:
                    sensor_data = json.loads(response)
                    sensor_status = sensor_data.get("status", 0)

                client.stop()

                if status != sensor_status:
                    self.update_sensor_in_db(sensor_type, sensor_status)
                    self.sensors[sensor_type]["status"] = sensor_status
                    print(f"Updated {sensor_type} sensor status to {sensor_status}")
                
                break
            except Exception as e:
                if attempt < MAX_RETRIES - 1:
                    print(f"Error retrieving {sensor_type} sensor status: {e}. Retrying...")
                    time.sleep(2)
                else:
                    print(f"Error retrieving {sensor_type} sensor status: {e}")
                    raise e
                
    def update_sensor_in_db(self, sensor_type, sensor_status):
        if self.connection and self.connection.is_connected():
            try:
                cursor = self.connection.cursor()
                update_sensor_query = f"""
                UPDATE sensor
                SET status = {sensor_status}
                WHERE ip_address = %s AND port = %s AND type = %s
                """
                cursor.execute(update_sensor_query, (self.sensors[sensor_type]["ip_address"], self.sensors[sensor_type]["port"], sensor_type))
                self.connection.commit()
                cursor.close()
            except Error as e:
                print(f"Error updating {sensor_type} sensor status: {e}")
