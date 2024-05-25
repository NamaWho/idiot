
from typing import Any
from models.database import Database

class Record:
    """
    The record class to store the telemetry data
    """

    vibration = 0
    rotation = 0
    pressure = 0
    voltage = 0

    @classmethod
    def set_vibration(cls, value):
        """
        Set the vibration value

        :param value: The vibration value

        :return: None
        """
        cls.vibration = value
        cls.check_all_values()

    @classmethod
    def set_rotation(cls, value):
        """
        Set the rotation value

        :param value: The rotation value

        :return: None
        """
        cls.rotation = value
        cls.check_all_values()

    @classmethod
    def set_pressure(cls, value):
        """
        Set the pressure value

        :param value: The pressure value

        :return: None
        """
        cls.pressure = value
        cls.check_all_values()

    @classmethod
    def set_voltage(cls, value):
        """
        Set the voltage value

        :param value: The voltage value

        :return: None
        """
        cls.voltage = value
        cls.check_all_values()

    @staticmethod
    def check_all_values():
        if (all([Record.vibration, Record.rotation, Record.pressure, Record.voltage])):
            Record.insert_telemetry()
            Record.vibration = 0
            Record.rotation = 0
            Record.pressure = 0
            Record.voltage = 0
        
    @staticmethod
    def insert_telemetry():
        """
        Insert the record into the database

        :return: None
        """
        db = Database()
        connection = db.connect_db()

        try:
            with connection.cursor() as cursor:
                cursor = connection.cursor()
                sql = "INSERT INTO Telemetry (vibration, rotation, pressure, voltage) VALUES (%s, %s, %s, %s)"
                cursor.execute(sql, (Record.vibration, Record.rotation, Record.pressure, Record.voltage))
                connection.commit()
                print(f"Record inserted: {Record.__str__()}")
                connection.close()
        except Exception as e:
            print(f"Error inserting record: {e}")
            connection.close()

    @staticmethod
    def __str__():
        """
        Return the string representation of the record

        :return: The string representation of the record
        """
        return f"Vibration: {Record.vibration}, Rotation: {Record.rotation}, Pressure: {Record.pressure}, Voltage: {Record.voltage}"
    