package bgu.spl.net.impl.stomp;

import java.util.HashMap;
import java.util.Map;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.data.LoginStatus;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {

    private boolean shouldTerminate = false;
    private int connectionId;
    private Connections<String> connections;
    final String VERSION = "1.2";

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
    }

    @Override
    public void process(String message) {
        if (message.isEmpty())
            return;
        String type = "";
        HashMap<String, String> headers = new HashMap<>();
        String body = "";

        System.out.println("=======================");
        System.out.println(message);
        System.out.println("=======================");

        String[] lines = message.split("\n");

        System.out.println("=======================");
        System.out.println("lines---:" + lines);
        System.out.println("=======================");
        type = lines[0];
        int i = 1;
        for (; i < lines.length && !lines[i].isEmpty(); i++) {
            String[] header = lines[i].split(":");
            headers.put(header[0], header[1]);
        }

        for (; i < lines.length; i++) {
            body += lines[i] + "\n";
        }

        // if (!connections.isUserConnected(connectionId)) {
        // System.err.println("[ERROR] Could not find handler for connected id: " +
        // connectionId);
        // return;
        // }

        if (!"CONNECT".equals(type) && !connections.isUserConnected(connectionId)) {
            connections.send(connectionId,
                    generateErrorMessage(null, "Not connected", message, "You must send a CONNECT frame first"));
            return;
        }

        if ("CONNECT".equals(type)) {
            String clientVersion = headers.get("accept-version");
            if (!VERSION.equals(clientVersion)) {
                connections.send(connectionId,
                        generateErrorMessage(headers.get("receipt"), "Could not connect to server", message,
                                "Client tried to connect with unsupported version. Supported version: " + VERSION));
                connections.disconnect(connectionId);
                shouldTerminate = true;
            } else {
                System.out.println("ffffffffffffffffffffffff");
                System.out.println(headers.get("passcode"));
                System.out.println(headers);
                System.out.println("ffffffffffffffffffffffff");
                LoginStatus status = connections.addUser(this.connectionId, headers.get("login"),
                        headers.get("passcode"));

                if (LoginStatus.WRONG_PASSWORD.equals(status)) {
                    connections.send(connectionId,
                            generateErrorMessage(headers.get("receipt"), "Wrong password", message,
                                    "Client tried to connect with wrong password"));
                    connections.disconnect(this.connectionId);
                    shouldTerminate = true;
                } else if (LoginStatus.ADDED_NEW_USER.equals(status) || LoginStatus.LOGGED_IN_SUCCESSFULLY.equals(status)) {
                    System.out.println("::::::::::::::: GOOD CONNECTION ID: :::::::::::::: " + connectionId);
                    connections.send(connectionId, "CONNECTED\nversion:" + VERSION + "\n\n" + '\0');
                    sendReceipt(headers.get("receipt"));
                } else if (LoginStatus.ALREADY_LOGGED_IN.equals(status)) {
                    System.out.println("==================== User already loggin in!!!!!!!!");
                    connections.send(connectionId,
                            generateErrorMessage(headers.get("receipt"), "User already logged in", message,
                                    "User with the same username already logged in"));
                    System.out.println("::::::::::::::: BAD CONNECTION ID: :::::::::::::: " + connectionId);
                    connections.disconnect(this.connectionId);
                    shouldTerminate = true;
                } else if (LoginStatus.CLIENT_ALREADY_CONNECTED.equals(status)) {
                    connections.send(connectionId,
                            generateErrorMessage(headers.get("receipt"), "client already connected", message,
                                    "The client is already logged in, log out before trying again"));
                    connections.disconnect(this.connectionId);
                    shouldTerminate = true;
                }

            }
        } else if ("SUBSCRIBE".equals(type)) {
            String des = headers.get("destination");
            if (des != null) {
                boolean hasSubscribed = connections.addSubscriber(this.connectionId, des, headers.get("id"));
                if (!hasSubscribed) {
                    connections.send(connectionId,
                            generateErrorMessage(headers.get("receipt"), "subscription failed", message,
                                    "failed to subscribe " + this.connectionId + " to " + des));
                    connections.disconnect(this.connectionId);
                    shouldTerminate = true;
                }

                sendReceipt(headers.get("receipt"));
            }
        } else if ("UNSUBSCRIBE".equals(type)) {
            String idString = headers.get("id");
            if (idString == null) {
                connections.send(
                        connectionId,
                        generateErrorMessage(
                                headers.get("receipt"),
                                "unsubscribe failed", message,
                                "failed to unsubscribe because 'id' header is missing "));
                connections.disconnect(connectionId);
                shouldTerminate = true;
                return;
            }

            boolean hasUnsubscribed = connections.unsubscribe(connectionId, idString);
            if (!hasUnsubscribed) {
                connections.send(
                        connectionId,
                        generateErrorMessage(
                                headers.get("receipt"),
                                "unsubscribe failed", message,
                                "failed to unsubscribe the subscription specified doesn't exists "));
                connections.disconnect(connectionId);
                shouldTerminate = true;
                return;
            }

            sendReceipt(headers.get("receipt"));
        } else if ("SEND".equals(type)) {
            String des = headers.get("destination");
            if (des == null) {
                connections.send(
                        connectionId,
                        generateErrorMessage(
                                headers.get("receipt"),
                                "send failed", message,
                                "failed to send message because destination not specified. "));
                connections.disconnect(connectionId);
                shouldTerminate = true;
                return;
            } else {
                Map<Integer, String> subscribers = connections.getSubscribers(des);
                if (subscribers != null && subscribers.get(connectionId) == null) {
                    connections.send(connectionId, generateErrorMessage(
                            headers.get("receipt"),
                            "Unauthorized",
                            message,
                            "Can't send message to unsubcribed topic. Please subscribe first."));
                    connections.disconnect(connectionId);
                    shouldTerminate = true;
                    return;
                } else if (subscribers != null) {
                    int messageId = connections.incrementAndGetId();
                    for (Map.Entry<Integer, String> subcriber : subscribers.entrySet()) {
                        String subscriptionId = subcriber.getValue();
                        String msg = "MESSAGE\nsubscription:" + subscriptionId +
                                "\nmessage-id:" + messageId + "\ndestination:" + des + "\n\n" + body + '\0';
                        connections.send(subcriber.getKey(), msg);
                    }

                    sendReceipt(headers.get("receipt"));
                }
            }
        } else if ("DISCONNECT".equals(type)) {
            sendReceipt(headers.get("receipt"));
            connections.disconnect(connectionId);
        }
        // DELETE
        System.out.println("Type: " + type + "\nHeaders" + headers.toString() + "\nBody: " + body);
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    private void sendReceipt(String receipt) {
        if (receipt != null) {
            connections.send(connectionId, "RECEIPT\nreceipt-id:" + receipt + "\n\n" + '\0');
        }
    }

    private String generateErrorMessage(String receiptID, String message, String body, String description) {
        String res = "ERROR\n";

        if (receiptID != null) {
            res += "receipt-id:" + receiptID;
        }

        res += "\nmessage:" + message;

        if (body != null) {
            res += "\n\nThe message: " + "\n-----\n" + body + "\n-----\n";
        }

        if (description != null) {
            res += description;
        }

        System.out.println("Sending Error: " + res);
        return res + "\n" + '\0';
    }

}
