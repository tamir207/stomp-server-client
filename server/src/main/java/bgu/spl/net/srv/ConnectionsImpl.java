package bgu.spl.net.srv;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class ConnectionsImpl<T> implements Connections<T> {

    private final Map<Integer, ConnectionHandler<T>> connectedHandlers;
    private final Map<String, String> usernamesPasswords;
    private final Map<String, Map<Integer, Integer>> channels;
    private final Map<String, Integer> usernameID;
    private final Map<Integer, Map<Integer, String>> subscriptions;

    public ConnectionsImpl() {
        connectedHandlers = new ConcurrentHashMap<>();
        usernamesPasswords = new ConcurrentHashMap<>();
        channels = new ConcurrentHashMap<>();
        usernameID = new ConcurrentHashMap<>();
        subscriptions = new ConcurrentHashMap<>();
    }

    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> handler = connectedHandlers.get(connectionId);
        if (handler == null)
            return false;
        handler.send(msg);
        return true;
    }

    @Override
    public void send(String channel, T msg) {
        Map<Integer, Integer> subscribers = channels.get(channel);
        for (Map.Entry<Integer, Integer> subcriber : subscribers.entrySet()) {
            ConnectionHandler<T> handler = connectedHandlers.get(subcriber.getKey());
            if (handler != null)
                handler.send(msg);
        }
    }

    @Override
    public boolean addSubscriber(int connectionId, String topic, int subscriptionId) {
        if (topic == null) {
            return false;
        }
        synchronized (this) {
            Map<Integer, Integer> foundTopic = channels.get(topic);
            if (foundTopic == null) {
                channels.put(topic, new ConcurrentHashMap<>());
                foundTopic = channels.get(topic);
            }
            foundTopic.put(connectionId, subscriptionId);

            Map<Integer, String> userSubs = subscriptions.get(connectionId);
            if (userSubs == null) {
                subscriptions.put(subscriptionId, new ConcurrentHashMap<>());
                userSubs = subscriptions.get(subscriptionId);
            }
            userSubs.put(subscriptionId, topic);
        }
        return true;
    }

    @Override
    public boolean addNewClient(int connectionId, ConnectionHandler<T> handler) {
        if (connectedHandlers.containsKey(connectionId))
            return false;
        connectedHandlers.put(connectionId, handler);
        return true;
    }

    @Override
    public int addUser(int connectionId, String username, String password) {
        if (!usernamesPasswords.containsKey(username)) {
            usernamesPasswords.put(username, password);
            usernameID.put(username, connectionId);
            return 1;
        } else if (!usernamesPasswords.get(username).equals(password)) {
            return -1;
        } else if (usernameID.containsKey(username)) {
            return 0;
        } else {
            usernameID.put(username, connectionId);
            return 1;
        }
    }

    @Override
    public void disconnect(int connectionId) {
        connectedHandlers.remove(connectionId);
    }

    @Override
    public boolean isUserConnected(int connectionId) {
        return this.connectedHandlers.get(connectionId) != null;
    }

}
