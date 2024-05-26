import mysql.connector
from mysql.connector import Error
from coapthon.server.coap import CoAP
from resources.registration import Registration
from resources.telemetry import Telemetry
from resources.control import Control
import json
from models.database import Database

class CoAPServer(CoAP):
    
    def __init__(self, host, port):
        """
        Initialize the CoAP server

        :param host: The host to bind the server to
        :param port: The port to bind the server to

        :return: None
        """
        CoAP.__init__(self, (host, port), False)
        print(host, port)
        self.add_resource("register/", Registration())
        self.add_resource("telemetry/", Telemetry())
        self.add_resource("control/", Control(coap_server=self))

        self.db = Database()
        self.connection = self.db.connect_db()

    def close(self):
        if self.connection is not None and self.connection.is_connected():
            self.connection.close()
            print("MySQL connection closed")
        super(CoAPServer, self).close()
