package bgu.spl.net.srv;

import java.io.IOException;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    boolean addNewClient(int connectionId, ConnectionHandler<T> handler);

    int addUser(int connectionId, String username, String password);
}
