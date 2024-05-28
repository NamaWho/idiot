
from mysql.connector import Error
from coapthon.resources.resource import Resource
from models.database import Database
from coapthon.client.helperclient import HelperClient
import json
import time
import threading
import re
from coapthon import defines

class Control(Resource):

    sensors = {
        "pressure": {"status": 0, "ip_address": ""},
        "vibration": {"status": 0, "ip_address": ""},
        "voltage": {"status": 0, "ip_address": ""},
        "rotation": {"status": 0, "ip_address": ""}
    }  

    def __init__(self, name="Control", coap_server=None):
        super(Control, self).__init__(name, coap_server, visible=True, observable=True, allow_children=False)
        self.payload = "Control Resource"
        self.database = coap_server.db if coap_server else Database()
        self.connection = self.database.connect_db()
        self.period = 10       
        self.update(True)

    def render_GET(self, request):
        print(f"RENDER GET")
        self.fetch_sensors_from_db()
        self.update_sensors_status()
        print(f"Payload sent: {self.payload}")
        return self
    
    def fetch_sensors_from_db(self):
        if not self.connection.is_connected():
            print("Database connection lost")
            self.payload = None
            return self
        
        try:
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
                self.sensors[type]["status"] = int(status)
                self.sensors[type]["ip_address"] = ip_address
        
        except Error as e:
            print(f"Error retrieving sensor data: {e}")
            self.payload = None

    def update_sensors_status(self):
        try:
            for key, values in self.sensors.items():
                if values["ip_address"] == "":
                    print(f"{key} sensor not registered")
                    continue
                
                self.retrieve_and_update_sensor_status(key, int(values["status"]))
        
        except Error as e:
            print(f"Error retrieving sensor status: {e}")
            self.payload = None

        ip_status = [f"{values['ip_address'].replace('fd00::', '')}-{values['status']}" for values in self.sensors.values()]
        self.payload = ";".join(ip_status)

    def retrieve_and_update_sensor_status(self, sensor_type, status=0):
        MAX_RETRIES = 3
        TIMEOUT = 2
        COAP_PORT = 5683

        for attempt in range(MAX_RETRIES):
            try:
                client = HelperClient(server=(self.sensors[sensor_type]["ip_address"], COAP_PORT))
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
                WHERE ip_address = %s AND type = %s
                """
                cursor.execute(update_sensor_query, (self.sensors[sensor_type]["ip_address"], sensor_type))
                self.connection.commit()
                cursor.close()
            except Error as e:
                print(f"Error updating {sensor_type} sensor status: {e}")

    def update(self, first=False):
        print("Updating sensor status from UPDATE method")

        if not self._coap_server.stopped.isSet():
            timer = threading.Timer(self.period, self.update)
            timer.daemon = True
            timer.start()

            if not first and self._coap_server is not None:
                self._coap_server.notify(self)
                self.observe_count += 1

