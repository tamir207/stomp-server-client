package bgu.spl.net.srv;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class ConnectionsImpl<T> implements Connections<T> {
    private final Map<Integer, ConnectionHandler<T>> connectedHandlers;
    private final Map<String, Map<Integer, Integer>> channels;
    private final Map<String, Integer> usernameToID;
    private final Map<Integer, String> IDToUsername;
    private final Map<Integer, Map<Integer, String>> subscriptions;
    int messageId;

    // Persistant data between sessions:
    private final Map<String, String> usernamesPasswords;

    public ConnectionsImpl() {
        connectedHandlers = new ConcurrentHashMap<>();
        usernamesPasswords = new ConcurrentHashMap<>();
        channels = new ConcurrentHashMap<>();
        usernameToID = new ConcurrentHashMap<>();
        IDToUsername = new ConcurrentHashMap<>();
        subscriptions = new ConcurrentHashMap<>();
        messageId = 1;
    }

    public Map<Integer, Integer> getSubscribers(String channel) {
        return channels.get(channel);
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
                subscriptions.put(connectionId, new ConcurrentHashMap<>());
                userSubs = subscriptions.get(subscriptionId);
            }
            userSubs.put(subscriptionId, topic);
        }
        return true;
    }

    @Override
    public boolean unsubscribe(int connectionId, int subscriptionId) {
        Map<Integer, String> userSubs = subscriptions.get(connectionId);
        String topic = userSubs.get(subscriptionId);
        if (topic == null)
            return false;
        userSubs.remove(subscriptionId);
        Map<Integer, Integer> foundTopic = channels.get(topic);
        foundTopic.remove(connectionId);
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
            usernameToID.put(username, connectionId);
            IDToUsername.put(connectionId, username);
            return 1;
        } else if (!usernamesPasswords.get(username).equals(password)) {
            return -1;
        } else if (usernameToID.containsKey(username)) {
            return 0;
        } else {
            usernameToID.put(username, connectionId);
            IDToUsername.put(connectionId, username);
            return 1;
        }
    }

    @Override
    public void disconnect(int connectionId) {
        String username = IDToUsername.get(connectionId);

        connectedHandlers.remove(connectionId);
        usernameToID.remove(username);
        IDToUsername.remove(connectionId);

        Map<Integer, String> userSubs = subscriptions.get(connectionId);
        subscriptions.remove(connectionId);

        for (Map.Entry<Integer, String> registeredChannels : userSubs.entrySet()) {
            String channelName = registeredChannels.getValue();
            Map<Integer, Integer> channel = channels.get(channelName);
            channel.remove(connectionId);
        }
    }

    @Override
    public boolean isUserConnected(int connectionId) {
        return this.connectedHandlers.get(connectionId) != null;
    }

}
