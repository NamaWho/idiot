
from mysql.connector import Error
from coapthon.resources.resource import Resource
import json
import time
import threading
from models.observer import ObserveSensor #, ObserveSensorStaus
from models.database import Database

class Registration(Resource):

    COAP_PORT = 5683
    sensors = {
        "pressure": {"status": 0, "address": ""},
        "vibration": {"status": 0, "address": ""},
        "voltage": {"status": 0, "address": ""},
        "rotation": {"status": 0, "address": ""}
    }
    actuators = {
        "alarm": {"status": 0, "address": ""}
    }

    def __init__(self, name="Registration", database=None):
        super(Registration, self).__init__(name)
        self.payload = "Registration Resource"
        if not database:
            self.database = Database()
        else:
            self.database = database
        self.connection = self.database.connect_db()
    
    def render_GET(self, request):
        print("GET Registration Resource")
        
        try:
            ip_port = request.source
            type = request.payload

            if type in self.sensors:
                self.register_sensor(type, ip_port)

            elif type in self.actuators and type == "alarm":
                self.register_actuator(type, ip_port)
               
        except (Error, json.JSONDecodeError) as e:
            print(f"Error processing registration: {e}")
            self.payload = None
        
        self.payload = "Registration Successful"
        return self
    
    def register_sensor(self, type, ip_port):
        
        self.insert_device(type, ip_port)

        self.sensors[type]["status"] = 1
        self.sensors[type]["address"] = ip_port[0]
        ObserveSensor(ip_port, type, self.database)
        #ObserveSensorStaus(ip_port, type+"/status")

        print(f"Registered {type} sensor at {ip_port}")
    
    def register_actuator(self, type, ip_port):
        
        self.insert_device(type, ip_port)

        self.actuators[type]["status"] = 1
        self.actuators[type]["address"] = ip_port[0]

        print(f"Registered {type} actuator at {ip_port}")

    def insert_device(self, type, ip_port):
        try:
            # Insert node info into Sensor table if not already present
            if self.connection and self.connection.is_connected():
                cursor = self.connection.cursor()
                insert_node_query = """
                INSERT INTO sensor (ip_address, type, status) 
                VALUES (%s, %s, %s)
                ON DUPLICATE KEY UPDATE  ip_address = %s, type = %s, status = %s
                """
                cursor.execute(insert_node_query, (ip_port[0], type, 1, ip_port[0], type, 1))
                self.connection.commit()
                cursor.close()
        except Error as e:
            print(f"Error registering sensor: {e}")


       


        
        
        

    