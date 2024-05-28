
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


    def __init__(self, name="Control"):
        super(Control, self).__init__(name)
    sensors = {
        "pressure": {"status": 0, "ip_address": ""},
        "vibration": {"status": 0, "ip_address": ""},
        "voltage": {"status": 0, "ip_address": ""},
        "rotation": {"status": 0, "ip_address": ""}
    }  

    def __init__(self, name="Control", coap_server=None):
        super(Control, self).__init__(name, coap_server, visible=True, observable=True, allow_children=False)
        self.payload = "Control Resource"
        self.database = Database()
        self.database = coap_server.db if coap_server else Database()
        self.connection = self.database.connect_db()

        self.period = 10       
        self.update(True)

    def render_GET(self, request):
        print(f"RENDER GET")
        self.fetch_sensors_from_db()
        return self
    
    def fetch_sensors_from_db(self):
        if not self.connection.is_connected():
            self.payload = None
            print("Database connection lost, Payload: None")
            return self

        sensors = {
            "pressure": {"status": 0, "ip_address": ""},
            "vibration": {"status": 0, "ip_address": ""},
            "voltage": {"status": 0, "ip_address": ""},
            "rotation": {"status": 0, "ip_address": ""}
        }  
        
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
                sensors[type]["status"] = int(status)
                sensors[type]["ip_address"] = ip_address

            # if all sesnors are registered and status is 1, then send the payload
            if all([values["ip_address"] != "" and values["status"] == 1 for values in sensors.values()]):
                json_payload = {
                    "pressure": sensors["pressure"]["ip_address"],
                    "vibration": sensors["vibration"]["ip_address"],
                    "voltage": sensors["voltage"]["ip_address"],
                    "rotation": sensors["rotation"]["ip_address"]
                }
                self.payload = json.dumps(json_payload)
                print(f"Payload: {self.payload}")

            else:
                self.payload = None
                print(f"Not all sensors are registered, Payload: {self.payload}")
                
        
        except Error as e:
            self.payload = None
            print(f"Error retrieving sensor data: {e}, Payload: {self.payload}")
        
        




