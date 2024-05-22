
from typing import Any
from models.database import Database

class Record:
    """
    The record class to store the telemetry data
    """

    def __init__(self):
        """
        Initialize the record with default values
        """
        self.vibration = None
        self.rotation = None
        self.pressure = None
        self.voltage = None

    def __init__(self, vibration, rotation, pressure, voltage):
        """
        Initialize the record with the provided values

        :param vibration: The vibration value
        :param rotation: The rotation value
        :param pressure: The pressure value
        :param voltage: The voltage value

        :return: None
        """
        self.vibration = vibration
        self.rotation = rotation
        self.pressure = pressure
        self.voltage = voltage

    def __setattr__(self, name: str, value: Any) -> None:
        """
        Set the value of the attribute

        :param name: The name of the attribute
        :param value: The value of the attribute

        :return: None
        """
        # if all the values are present, call insert method
        if all([self.vibration, self.rotation, self.pressure, self.voltage]):
            self.insert_telemetry()
            # flush the values
            self.vibration = None
            self.rotation = None
            self.pressure = None
            self.voltage = None
        else:
            super().__setattr__(name, value)
        
    def __str__(self):
        """
        Return the string representation of the record

        :return: The string representation of the record
        """
        return f"Vibration: {self.vibration}, Rotation: {self.rotation}, Pressure: {self.pressure}, Voltage: {self.voltage}"
    
    def insert_telemetry(self):
        """
        Insert the record into the database

        :return: None
        """
        self.db = Database()
        self.connection = self.db.connect_db()

        with self.connection.cursor() as cursor:
            cursor = self.connection.cursor()
            sql = "INSERT INTO Telemetry (vibration, rotation, pressure, voltage) VALUES (%s, %s, %s, %s)"
            cursor.execute(sql, (self.vibration, self.rotation, self.pressure, self.voltage))
            self.connection.commit()
            print(f"Record inserted: {self}")
            self.connection.close()