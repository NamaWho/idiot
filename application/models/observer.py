import json
from coapthon.server.coap import CoAP
from coapthon.resources.resource import Resource
from coapthon.messages.request import Request
from coapthon.messages.response import Response
from coapthon.client.helperclient import HelperClient
from coapthon.utils import parse_uri
from models.database import Database
from models.record import Record
import re

class ObserveSensor:

    def __init__(self,source_address, resource, database=None):
        self.address = source_address
        self.resource = resource
        self.database = database
        Record.set_db(self.database)
        self.start_observing()
       
    def start_observing(self):
        self.client = HelperClient(self.address)
        self.client.observe(self.resource, self.observer)
    
    def observer(self, response):

        # if not response:
        #     fix_format = re.sub(r"b'", "", str(response.payload))
        #     fix_format = re.sub(r"'", "", fix_format)
        #     data = json.loads(fix_format)
        #     print(f"Observer response: {data}")
        # else:
        #     print(f"Observer response: {response}")
        #     return

        if not response.payload:
            print("Empty response payload")
            return
        
        data = json.loads(response.payload)
        
        try:
            if self.resource == "vibration":
                print("\n📳📳📳📳 VIBRATION TELEMETRY 📳📳📳📳 : " + str(data["value"]))
                Record.set_vibration(data["value"])
            elif self.resource == "rotation":
                print("\n🔄🔄🔄🔄 ROTATION TELEMETRY 🔄🔄🔄🔄 : " + str(data["value"]))
                Record.set_rotation(data["value"])
            elif self.resource == "pressure":
                print("\n🔘🔘🔘🔘 PRESSURE TELEMETRY 🔘🔘🔘🔘 : " + str(data["value"]))
                Record.set_pressure(data["value"])
            elif self.resource == "voltage":
                print("\n🔋🔋🔋🔋 VOLTAGE TELEMETRY 🔋🔋🔋🔋 : " + str(data["value"]))
                Record.set_voltage(data["value"])
            else:
                print("Unknown resource:", self.resource)
        except KeyError as e:
            print("KeyError:", e)



# class ObserveSensorStaus:

#     def __init__(self,source_address, resource, database=None):
#         self.address = source_address
#         self.resource = resource
#         self.start_observing()
#         if not database:
#             self.database = Database()
#         else:
#             self.database = database
        
#     def start_observing(self):
#         self.client = HelperClient(self.address)
#         self.client.observe(self.resource, self.observer)
    
#     def observer(self, response):

#         print("Observer response:", response.payload)

#         data = json.loads(fix_format)
        
#         try:
#             if self.resource == "vibration":
#                 print("\n📳📳📳📳 VIBRATION SENSOR STATUS CHANGED 📳📳📳📳\n")
                
#                 self.change_status(self.resource, self.address, data["status"], self.database)


#                 print("Vibration sensor status updated")

            
#             elif self.resource == "rotation":
#                 print("\n🔄🔄🔄🔄 ROTATION SENSOR STATUS CHANGED 🔄🔄🔄🔄\n")
                
#                 self.change_status(self.resource, self.address, data["status"], self.database)

#                 print("Rotation sensor status updated")
            
#             elif self.resource == "pressure":
#                 print("\n🔘🔘🔘🔘 ROTATION SENSOR STATUS CHANGED 🔘🔘🔘🔘\n")
                
#                 self.change_status(self.resource, self.address, data["status"], self.database)

#                 print("Pressure sensor status updated")
            
#             elif self.resource == "voltage":
#                 print("\n🔋🔋🔋🔋 ROTATION SENSOR STATUS CHANGED 🔋🔋🔋🔋\n")
                
#                 self.change_status(self.resource, self.address, data["status"], self.database)
#                 print("Voltage sensor status updated")
            
#             else:
#                 print("Unknown resource:", self.resource)
#         except KeyError as e:
#             print("KeyError:", e)

#     def change_status(self, sensor_type, ip_address, status):
        
#         connection = self.database.connect_db()

#         try:
#             with connection.cursor() as cursor:
#                 cursor = connection.cursor()
#                 sql = "UPDATE sensor SET status = %s WHERE ip_address = %s AND type = %s"
#                 cursor.execute(sql, (status, ip_address, sensor_type))
#                 connection.commit()
#                 connection.close()
                
#         except Exception as e:
#             print(f"Error inserting record: {e}")
#             connection.close()