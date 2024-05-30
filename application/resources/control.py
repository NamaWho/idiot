
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
        self.payload = "Control Resource"
        self.database = Database()
        self.connection = self.database.connect_db()

    def render_GET(self, request):
        print(f"RENDER GET")
        # Print parameters of the request
        
        print(f"Request parameters: {request.uri_query}")
    
        # Take the value of the query parameter
        query = request.uri_query

        self.fetch_sensor_from_db(query)

        return self
    
    def fetch_sensor_from_db(self, type):
        if not self.connection.is_connected():
            self.payload = None
            print("Database connection lost, Payload: None")
            return self
 
        try:
            cursor = self.connection.cursor()
            select_sensor_query = """
            SELECT ip_address, type, status
            FROM sensor
            WHERE type = %s
            """
            cursor.execute(select_sensor_query, (type,))

            sensor_data = cursor.fetchall()
            cursor.close()

            sensor = {}

            if sensor_data:
                for row in sensor_data:
                    ip_address, type, status = row
                    sensor[type] = {"status": int(status), "ip_address": ip_address}

                if sensor[type]["status"] == 1:
                    response = {
                        "sensor": type,
                        "ip_address": sensor[type]["ip_address"]
                    }
                    self.payload = json.dumps(response, separators=(',', ':'))
                    print(f"Payload: {self.payload}")



            else:
                self.payload = None
                print(f"No sensor found for type: {type}. Payload: {self.payload}")



            # # if all sesnors are registered and status is 1, then send the payload
            # if all([values["ip_address"] != "" and values["status"] == 1 for values in sensors.values()]):
            #     ip_addresses = [
            #         sensors["pressure"]["ip_address"].replace("fd00::", ""),
            #         sensors["vibration"]["ip_address"].replace("fd00::", ""),
            #         sensors["voltage"]["ip_address"].replace("fd00::", ""),
            #         sensors["rotation"]["ip_address"].replace("fd00::", "")
            #     ]
            #     self.payload = ";".join(ip_addresses)
            #     print(f"Payload: {self.payload}")

            # else:
            #     self.payload = None
            #     print(f"Not all sensors are registered, Payload: {self.payload}")
        
        except Error as e:
            self.payload = None
            print(f"Error retrieving sensor data: {e}, Payload: {self.payload}")
        
        




