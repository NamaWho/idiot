package org.example;

import org.example.resources.SQLResource;
import org.eclipse.californium.core.CoapServer;

public class RegistrationServer extends CoapServer {

    public static void main(String args[]) {
        RegistrationServer server = new RegistrationServer();
        server.add(new SQLResource("registration"));
        server.start();
        System.out.println("\nServer starts!\n");
    }

}