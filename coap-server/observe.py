import json
from coapthon.server.coap import CoAP
from coapthon.resources.resource import Resource
from coapthon.messages.request import Request
from coapthon.messages.response import Response
from coapthon.client.helperclient import HelperClient
from coapthon.utils import parse_uri
from models.database import Database
from models.record import Record

class ObserveSensor:

    def __init__(self,source_address, resource):
        self.db = Database()
        self.connection = self.db.connect_db()
        self.address = source_address
        self.resource = resource
        self.record = Record()
        self.start_observing()
       
    def start_observing(self):
        self.client = HelperClient(self.address)
        self.client.observe(self.resource, self.observer)
    
    def observer(self, response):
        data = json.loads(response.payload)
        
        if self.resource == "vibration":
            print("\n📳📳📳📳 VIBRATION TELEMETRY 📳📳📳📳\n")
            self.record.__setattr__("vibration", data["vibration"])
        elif self.resource == "rotation":
            print("\n🔄🔄🔄🔄 ROTATION TELEMETRY 🔄🔄🔄🔄\n")
            self.record.__setattr__("rotation", data["rotation"])
        elif self.resource == "pressure":
            print("\n🔘🔘🔘🔘 PRESSURE TELEMETRY 🔘🔘🔘🔘\n")
            self.record.__setattr__("pressure", data["pressure"])
        elif self.resource == "voltage":
            print("\n🔋🔋🔋🔋 VOLTAGE TELEMETRY 🔋🔋🔋🔋\n")
            self.record.__setattr__("voltage", data["voltage"])