
from mysql.connector import Error
from coapthon.resources.resource import Resource
import json
from models.observer import ObserveSensor
from models.database import Database

class Registration(Resource):

    pressure_sensor = 0
    vibration_sensor = 0
    voltage_sensor = 0
    rotation_sensor = 0
    alarm_actuator = 0

    pressure_address = ""
    vibration_address = ""
    voltage_address = ""
    rotation_address = ""
    alarm_address = 0

    def __init__(self, name="Registration"):
        super(Registration, self).__init__(name)
        self.payload = "Registration Resource"
    
    def render_GET(self, request):
        self.payload = "GET Registration Resource"
        
        try:
            ip_address = request.source
            type = request.payload
            status = 1

            if type == "pressure":
                self.pressure_sensor = 1
                self.pressure_address = ip_address
                ObserveSensor(ip_address, "pressure")
            elif type == "vibration":
                self.vibration_sensor = 1
                self.vibration_address = ip_address
                ObserveSensor(ip_address, "vibration")
            elif type == "voltage":
                self.voltage_sensor = 1
                self.voltage_address = ip_address
                ObserveSensor(ip_address, "voltage")
            elif type == "rotation":
                self.rotation_sensor = 1
                self.rotation_address = ip_address
                ObserveSensor(ip_address, "rotation")
            elif type == "alarm":
                self.alarm_actuator = 1
                self.alarm_address = ip_address
            else:
                self.payload = "Invalid node type"
                return self

            # Insert node info into Sensor table if not already present
            if self.coap_server and self.coap_server.connection:
                cursor = self.coap_server.connection.cursor()
                insert_node_query = """
                INSERT INTO Sensor (type, status, ip_address) 
                VALUES (%s, %s, %s)
                ON DUPLICATE KEY UPDATE ip_address=%s, type=%s, status=%s
                """
                cursor.execute(insert_node_query, (type, status, ip_address, type, status, ip_address))
                self.coap_server.connection.commit()
                cursor.close()

                self.payload = "Node registration successful"
            else:
                self.payload = "Database connection not available"

        except (Error, json.JSONDecodeError) as e:
            print(f"Error processing registration: {e}")
            self.payload = f"Registration failed: {e}"

        return self
