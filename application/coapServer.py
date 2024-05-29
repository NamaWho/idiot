import mysql.connector
from mysql.connector import Error
from coapthon.server.coap import CoAP
from resources.registration import Registration
# from resources.telemetry import Telemetry
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
        self.db = Database()
        self.connection = self.db.connect_db()
        self.flush_sensor_table()
        self.add_resource("register/", Registration("Registration"))
        # self.add_resource("telemetry/", Telemetry(self.db))
        self.add_resource("control/", Control("Control"))

    def flush_sensor_table(self):
        """
        Flush the sensor table

        :return: None
        """
        if not self.connection.is_connected():
            print("Database connection lost")
            return

        try:
            cursor = self.connection.cursor()
            truncate_sensor_table_query = "TRUNCATE TABLE sensor"
            cursor.execute(truncate_sensor_table_query)
            self.connection.commit()
            cursor.close()
        except Error as e:
            print(f"Error truncating sensor table: {e}")

    def close(self):
        
        super(CoAPServer, self).close()
