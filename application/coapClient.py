from coapthon.client.helperclient import HelperClient
from coapthon.utils import parse_uri
from mysql.connector import Error
from coapthon.resources.resource import Resource
import json
import sys

if __name__ == '__main__':  
    main()


def main():
    
    database = Database()
    connection = database.connect_db()

    # Retrieve the IP addressesses of the sensors
    sensors = {
        "pressure": {"status": 0, "ip": "", "port": 0},
        "vibration": {"status": 0, "ip": "", "port": 0},
        "voltage": {"status": 0, "ip": "", "port": 0},
        "rotation": {"status": 0, "ip": "", "port": 0}
    }

    try:
        if connection and connection.is_connected():
            cursor = connection.cursor()

            select_sensor_query = """
            SELECT ip_address, port, type, status
            FROM sensor
            """

            cursor.execute(select_sensor_query)

            sensor_data = cursor.fetchall()

            for row in sensor_data:
                ip_address, port, type = row

                # Update the sensor dictionary with the sensor data
                sensors[type]["ip"] = ip_address
                sensors[type]["port"] = port
                sensors[type]["status"] = status

            cursor.close()
    
    except Error as e:
        print(f"Error retrieving sensor data: {e}")
        sys.exit(1)



       clients = {
        "pressure": HelperClient(server=(sensors["pressure"]["ip"], sensors["pressure"]["port"])),
        "vibration": HelperClient(server=(sensors["vibration"]["ip"], sensors["vibration"]["port"])),
        "voltage": HelperClient(server=(sensors["voltage"]["ip"], sensors["voltage"]["port"])),
        "rotation": HelperClient(server=(sensors["rotation"]["ip"], sensors["rotation"]["port"]))
       } 

    try:
        # check the status of the sesnors
        pressure_path = "pressure/status"
        vibration_path = "vibration/status"
        voltage_path = "voltage/status"
        rotation_path = "rotation/status"

        status = {
            "pressure": json.loads(clients["pressure"].get(pressure_path).payload),
            "vibration": json.loads(clients["vibration"].get(vibration_path).payload),
            "voltage": json.loads(clients["voltage"].get(voltage_path).payload),
            "rotation": json.loads(clients["rotation"].get(rotation_path).payload)
        }

    except Error as e:
        print(f"Error retrieving sensor data: {e}")
        for client in clients.values():
            client.stop()
        sys.exit(1)

    
    # Stop the clients
    for client in clients.values():
        client.stop()

    
    for sensor_type, sensor in sensors.items():
        if sensor["status"] != status[sensor_type]:
            try:
                print(f"Updating {sensor_type} sensor status to {status[sensor_type]}")

                update_sensor_status(connection, sensor_type, sensor["ip"], status[sensor_type])

                print(f"Updated {sensor_type} sensor status to {status[sensor_type]}")

            except Error as e:
                print(f"Error updating sensor status: {e}")
                sys.exit(1)


def update_sensor_status(connection, sensor_type, ip, status):
    
    try:
        if connection and connection.is_connected():
            cursor = connection.cursor()

            update_sensor_query = """
            UPDATE sensor
            SET status = %s
            WHERE ip_address = %s
            """

            cursor.execute(update_sensor_query, (status, ip))
            connection.commit()
            cursor.close()
    except Error as e:
        # Throw exception to be caught by the caller
        raise Error(f"Error updating {sensor_type} sensor status: {e}")
    
