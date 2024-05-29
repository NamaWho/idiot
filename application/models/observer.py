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

    def __init__(self,source_address, resource):
        self.address = source_address
        self.resource = resource
        self.database = Database()
        Record.set_db(self.database)
        self.start_observing()
       
    def start_observing(self):
        self.client = HelperClient(self.address)
        self.client.observe(self.resource, self.observer)
    
    def observer(self, response):

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

class ObserveActuator:

    def __init__(self,source_address, resource):
        self.address = source_address
        self.resource = resource
        self.database = Database()
        self.start_observing()
       
    def start_observing(self):
        self.client = HelperClient(self.address)
        self.client.observe(self.resource, self.observer)
    
    def observer(self, response):

        if not response.payload:
            print("Empty response payload")
            return
        
        data = json.loads(response.payload)
        
        try:
            if self.resource == "alarm":
                print("\nPREDICTED COMPONENT FAILURE:")
                label = ""
                match data["value"]:
                    case 0:
                        print("Component 1 is predicted to fail")
                        label = "Component 1"
                    case 1:
                        print("Component 2 is predicted to fail")
                        label = "Component 2"
                    case 2:
                        print("Component 3 is predicted to fail")
                        label = "Component 3"
                    case 3:
                        print("Component 4 is predicted to fail")
                        label = "Component 4"
                    case 4:
                        print("No component is predicted to fail")
                        label = "STATUS OK"
                    case _:
                        print("Unknown vibration level")
                
                if label != "":
                    Record.push_failure(label)
            else:
                print("Unknown resource:", self.resource)
        except KeyError as e:
            print("KeyError:", e)



