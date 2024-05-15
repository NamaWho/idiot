package org.example.resources;

import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.coap.Response;
import org.eclipse.californium.core.server.resources.CoapExchange;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;

import java.net.InetAddress;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;

/**
 * A CoAP resource for handling SQL operations on actuators.
 *
 * This class extends the CoapResource class and provides functionality for handling POST requests
 * related to SQL operations on actuators. It receives JSON payloads containing information about
 * actuators and performs database operations to store or update the information.
 *
 * */

public class SQLResource extends CoapResource {

    private static final String url = "jdbc:mysql://localhost:3306/SmartGreenHouse";
    private static final String username = "root";
    private static final String password = "Mysql2023!";

    public SQLResource(String name) {
        super(name);
    }


    /**
     * Handles incoming POST requests.
     *
     * This method handles incoming POST requests to the SQLResource. It expects a JSON payload containing
     * information about the actuator, including its name and status. The method parses the JSON payload,
     * retrieves the necessary information, and performs a REPLACE operation on the database table to store
     * or update the actuator information. The method generates a CoAP response based on the success or failure
     * of the database operation and sends it back to the client.
     *
     * @param exchange the CoapExchange object representing the CoAP request and response exchange
     */
    public void handlePOST(CoapExchange exchange) {
        byte[] request = exchange.getRequestPayload();
        String s = new String(request);
        JSONObject json = null;
        try {
            JSONParser parser = new JSONParser();
            json = (JSONObject) parser.parse(s);
        }catch (Exception err){
            System.err.println("Json format not valid!");
        }

        Response response;
        if (json.containsKey("name")){
            InetAddress addr = exchange.getSourceAddress();
            System.out.println(addr);
            try (Connection connection = DriverManager.getConnection(url, username, password)) {

                PreparedStatement ps = connection.prepareStatement("REPLACE INTO actuators (ip,resource,status) VALUES(?,?,?);");
                ps.setString(1,String.valueOf(addr).substring(1));
                ps.setString(2, (String)json.get("name"));
                ps.setString(3,(String)json.get("status"));
                ps.executeUpdate();
                if(ps.getUpdateCount()<1){
                    response = new Response(CoAP.ResponseCode.INTERNAL_SERVER_ERROR);
                }else{
                    response = new Response(CoAP.ResponseCode.CREATED);
                }
            } catch (SQLException e) {

                response = new Response(CoAP.ResponseCode.INTERNAL_SERVER_ERROR);
                System.err.println("Cannot connect the database!");
            }

        }else{
            response = new Response(CoAP.ResponseCode.BAD_REQUEST);
        }
        exchange.respond(response);
    }
}