package bgu.spl.net.impl.stomp;

import java.util.HashMap;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {

    private boolean shouldTerminate = false;
    private int connectionId;
    private Connections<String> connections;
    private ConnectionHandler<String> handler;
    final String VERSION = "1.2";
    private String username;

    @Override
    public void start(int connectionId, Connections<String> connections, ConnectionHandler<String> handler) {
        this.connectionId = connectionId;
        this.connections = connections;
        this.handler = handler;
        this.username = "";
    }

    @Override
    public void process(String message) {
        String type = "";
        HashMap<String, String> headers = new HashMap<>();
        String body = "";

        String[] lines = message.split("\n");
        type = lines[0];
        int i = 1;
        for (; i < lines.length - 1 && !lines[i].isEmpty(); i++) {
            String[] header = lines[i].split(":");
            headers.put(header[0], header[1]);
        }

        for (; i < lines.length - 1; i++) {
            body += lines[i] + "\n";
        }

        if (type == "CONNECT") {
            /**
             * Add username as user id to connections
             * (con and channels) with value of it's connection handler instance.
             * If needed, on the creation of StompMessagingProtoolImpl, pass the
             * connectionHandler
             * (who is the creator of this StompProtocol) as instance.
             * 
             * The only connection that is connections from the start is the
             * sqlServer (connectionHandler)
             * Which is added manually from the Reactor/TPC class.
             */
            String clientVersion = headers.get("accept-version");
            if (!VERSION.equals(clientVersion)) {
                boolean sent = connections.send(connectionId,
                        generateErrorMessage(headers.get("receipt"), "unsupported version", message,
                                "Client tried to connect with unsupported version. Supported version: " + VERSION));
                if (!sent) {
                    logNoHandlerError(connectionId);
                    return;
                }

                connections.disconnect(connectionId);
            } else {
                int status = connections.addUser(this.connectionId, headers.get("login"), headers.get("passcode"));
                if (status == -1) {
                    boolean sent = connections.send(connectionId,
                            generateErrorMessage(headers.get("receipt"), "wrong password", message,
                                    "Client tried to connect with wrong password"));
                    if (!sent) {
                        logNoHandlerError(connectionId);
                        return;
                    }
                    connections.disconnect(this.connectionId);
                } else if (status == 0) {
                    boolean sent = connections.send(connectionId,
                            generateErrorMessage(headers.get("receipt"), "user already active", message,
                                    "user with the same username already logged in"));
                    if (!sent) {
                        logNoHandlerError(connectionId);
                        return;
                    }
                    connections.disconnect(this.connectionId);
                } else {
                    this.username = headers.get("login");
                    boolean sent = connections.send(connectionId, "CONNECTED\nversion:" + VERSION + "\n\n^@");
                    if (!sent) {
                        logNoHandlerError(connectionId);
                        return;
                    }
                }
            }
        } else if (type == "SUBSCRIBE") {
            String des = headers.get("destination");
            if (des != null){
                // boolean hasSubscribed = connections.addSubscriber();
            }

        } else if (type == "UNSUBSCRIBE") {

        } else if (type == "SEND") {

        } else if (type == "DISCONNECT") {

        }

        System.out.println("Type: " + type + "\nHeaders" + headers.toString() + "\nBody: " + body);
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    private void logNoHandlerError(int connectionId) {
        System.err.println("[ERROR] Could not find handler for connected id: " + connectionId);
    }

    private String generateErrorMessage(String receiptID, String message, String body, String description) {
        String res = "ERROR\n";

        if (receiptID != null) {
            res += "receipt-id: " + receiptID;
        }

        res += "\nmessage:" + message;

        if (body != null) {
            res += "\n\nThe message: " + "\n-----\n" + body + "\n-----\n";
        }

        if (description != null) {
            res += description;
        }

        return res + "\n^@";
    }

}
