
from mysql.connector import Error
from coapthon.resources.resource import Resource
import json
import time

class Control(Resource):
    def __init__(self, name="Control", coap_server=None):
        super(Control, self).__init__(name, coap_server, visible=True, observable=True, allow_children=False)
        self.payload = "Control Resource"
        self.database = Database()
        self.connection = self.database.connect_db()
        self.period = 30       
        self.update(True)

    
    def render_GET(self, request):
        # This method is called when a GET request is made to the resource

        print(f"Checking sensor status")

        sensors = {
            "pressure": {"status": 0, "address": "", "port": 0},
            "vibration": {"status": 0, "address": "", "port": 0},
            "voltage": {"status": 0, "address": "", "port": 0},
            "rotation": {"status": 0, "address": "", "port": 0}
        }   

        try:
            if self.connection and self.connection.is_connected():
            cursor = self.connection.cursor()

            
            select_sensor_query = """
            SELECT ip_address, port, type, status
            WHERE type IN ('pressure', 'vibration', 'voltage', 'rotation')
            FROM sensor
            """

        
            cursor.execute(select_sensor_query)

        
            sensor_data = cursor.fetchall()

        
            for row in sensor_data:
                ip_address, port, type, status = row
                print(f"Sensor IP: {ip_address}, Port: {port}, Type: {type}, Status: {status}")

                # Update the sensor dictionary with the sensor data
                sensors[type]["status"] = status
                sensors[type]["address"] = ip_address
                sensors[type]["port"] = port


            cursor.close()

        except Error as e:
        print(f"Error retrieving sensor data: {e}")

        if all(sensor["status"] for sensor in self.sensors.values()):

            sensor_data = {
                "pressure_ip_port": self.sensors["pressure"]["address"] + ":" + str(self.sensors["pressure"]["port"]),
                "vibration_ip_port": self.sensors["vibration"]["address"] + ":" + str(self.sensors["vibration"]["port"]),
                "voltage_ip_port": self.sensors["voltage"]["address"] + ":" + str(self.sensors["voltage"]["port"]),
                "rotation_ip_port": self.sensors["rotation"]["address"] + ":" + str(self.sensors["rotation"]["port"])
            }

            print("All sensors correctly registered and active")

            # if here, all sensors are registered -> send their ip and port to the actuator
        
            self.payload = json.dumps(sensor_data)

        else:
            self.payload = None

    return self


    def update(self, first=False):
        self.payload = "Observable Resource"

        if not self._coap_server.stopped.isSet():

            timer = threading.Timer(self.period, self.update)
            timer.setDaemon(True)
            timer.start()

            if not first and self._coap_server is not None:
                logger.debug("Periodic Check for sensor status")
                self._coap_server.notify(self)
            
