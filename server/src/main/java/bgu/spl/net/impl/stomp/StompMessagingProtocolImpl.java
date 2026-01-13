package bgu.spl.net.impl.stomp;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {

    private boolean shouldTerminate = false;
    private int connectionId;
    private Connections<String> connections;

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
    }

    @Override
    public void process(String message) {
        String type = "";
        String body = "";
        HashMap<String, String> headers = new HashMap<>();

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

        System.out.println("Type: " + type + "\nHeaders" + headers.toString() + "\nBody: " + body);
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

}
