
from mysql.connector import Error
from coapthon.resources.resource import Resource
import json
import time
import threading
from models.observer import ObserveSensor #, ObserveSensorStaus
from models.database import Database

class Registration(Resource):

    sensors = {
        "pressure": {"status": 0, "address": "", "port": 0},
        "vibration": {"status": 0, "address": "", "port": 0},
        "voltage": {"status": 0, "address": "", "port": 0},
        "rotation": {"status": 0, "address": "", "port": 0}
    }
    actuators = {
        "alarm": {"status": 0, "address": "", "port": 0}
    }

   
    def __init__(self, name="Registration"):
        super(Registration, self).__init__(name)
        self.payload = "Registration Resource"
        self.database = Database()
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
        self.sensors[type]["port"] = ip_port[1]
        ObserveSensor(ip_port, type)
        #ObserveSensorStaus(ip_port, type+"/status")

        print(f"Registered {type} sensor at {ip_port}", "ip:", ip_port[0], "port:", ip_port[1])

    
    
    def register_actuator(self, type, ip_port):
        
        self.insert_device(type, ip_port)

        self.actuators[type]["status"] = 1
        self.actuators[type]["address"] = ip_port[0]
        self.actuators[type]["port"] = ip_port[1]

        print(f"Registered {type} actuator at {ip_port}", "ip:", ip_port[0], "port:", ip_port[1])



    def insert_device(self, type, ip_port):
        try:
            # Insert node info into Sensor table if not already present
            if self.connection and self.connection.is_connected():
                cursor = self.connection.cursor()
                insert_node_query = """
                INSERT INTO sensor (ip_address, port, type, status) 
                VALUES (%s, %s, %s, %s)
                ON DUPLICATE KEY UPDATE  ip_address = %s, port = %s, type = %s, status = %s
                """
                cursor.execute(insert_node_query, (ip_port[0], ip_port[1], type, 1, ip_port[0], ip_port[1], type, 1))
                self.connection.commit()
                cursor.close()
            
        except Error as e:
            print(f"Error registering sensor: {e}")


       


        
        
        

    