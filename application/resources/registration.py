
from mysql.connector import Error
from coapthon.resources.resource import Resource
import json
import time
import threading
from models.observer import ObserveSensor
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

    all_sensors_registered = threading.Event()

    def __init__(self, name="Registration"):
        super(Registration, self).__init__(name)
        self.payload = "Registration Resource"
        self.database = Database()
        self.connection = self.database.connect_db()
    
    def render_GET(self, request):
        print("GET Registration Resource")
        self.payload = "GET Registration Resource"
        
        try:
            ip_port = request.source
            type = request.payload

            if type in self.sensors:
                self.register_sensor(type, ip_port)
            elif type in self.actuators:
                self.register_actuator(type, ip_port)
            else:   
                self.payload = "Invalid sensor/actuator type"
                return self
            
            if type in self.sensors and type == "alarm":
                self.wait_for_all_sensors(request)

        except (Error, json.JSONDecodeError) as e:
            print(f"Error processing registration: {e}")
            self.payload = f"Registration failed: {e}"

        return self
    
    def register_sensor(self, type, ip_port):
        self.sensors[type]["status"] = 1
        self.sensors[type]["address"] = ip_port[0]
        self.sensors[type]["port"] = ip_port[1]
        ObserveSensor(ip_port, type)

        print(f"Registered {type} sensor at {ip_port}", "ip:", ip_port[0], "port:", ip_port[1])
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
                self.payload = "Node registration successful"
            else:
                self.payload = "Database connection not available"
        except Error as e:
            print(f"Error registering sensor: {e}")
            self.payload = f"Sensor registration failed: {e}"

        if all(sensor["status"] for sensor in self.sensors.values()):
            self.all_sensors_registered.set()

    def register_actuator(self, type, ip_port):
        self.actuators[type]["status"] = 1
        self.actuators[type]["address"] = ip_port[0]
        self.actuators[type]["port"] = ip_port[1]

    def wait_for_all_sensors(self, request):
        self.all_sensors_registered.wait()

        sensor_data = {
            "pressure_ip_port": self.sensors["pressure"]["address"] + ":" + str(self.sensors["pressure"]["port"]),
            "vibration_ip_port": self.sensors["vibration"]["address"] + ":" + str(self.sensors["vibration"]["port"]),
            "voltage_ip_port": self.sensors["voltage"]["address"] + ":" + str(self.sensors["voltage"]["port"]),
            "rotation_ip_port": self.sensors["rotation"]["address"] + ":" + str(self.sensors["rotation"]["port"])
        }

        self.payload = json.dumps(sensor_data)
        self.send_response(request)

    def send_response(self, request):
        response = request.response
        response.payload = self.payload
        self.coap_server.send_response(request, response)