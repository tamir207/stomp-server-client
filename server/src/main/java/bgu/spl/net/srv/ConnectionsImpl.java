package bgu.spl.net.srv;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

public class ConnectionsImpl<T> implements Connections<T> {
    private final Map<Integer, ConnectionHandler<T>> connectedHandlers;// Map<SubscriptionID, ConnectionHandler<T>>
    private final Map<String, Map<Integer, String>> channels;// Map<ChannelName, Map<ConnectionID, SubscriptionID>>
    private final Map<String, Integer> usernameToID;
    private final Map<Integer, String> IDToUsername;
    private final Map<Integer, Map<String, String>> subscriptions;// Map<ConnectionID, Map<SubscriptionID, ChannelName>>
    private AtomicInteger messageId;

    // Persistant data between sessions:
    private final Map<String, String> usernamesPasswords;

    public ConnectionsImpl() {
        connectedHandlers = new ConcurrentHashMap<>();
        usernamesPasswords = new ConcurrentHashMap<>();
        channels = new ConcurrentHashMap<>();
        usernameToID = new ConcurrentHashMap<>();
        IDToUsername = new ConcurrentHashMap<>();
        subscriptions = new ConcurrentHashMap<>();
        messageId = new AtomicInteger(1);
    }

    @Override
    public Map<Integer, String> getSubscribers(String channel) {
        return channels.get(channel);
    }

    @Override
    public int getMessageId() {
        return messageId.get();
    }

    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> handler = connectedHandlers.get(connectionId);
        if (handler == null)
            return false;
        handler.send(msg);
        messageId.incrementAndGet();
        return true;
    }

    @Override
    public void send(String channel, T msg) {
        Map<Integer, String> subscribers = channels.get(channel);
        for (Map.Entry<Integer, String> subcriber : subscribers.entrySet()) {
            ConnectionHandler<T> handler = connectedHandlers.get(subcriber.getKey());
            if (handler != null)
                handler.send(msg);
        }
        messageId.incrementAndGet();
    }

    @Override
    public boolean addSubscriber(int connectionId, String topic, String subscriptionId) {
        if (topic == null) {
            return false;
        }
        synchronized (this) {
            Map<Integer, String> foundTopic = channels.get(topic);
            if (foundTopic == null) {
                channels.put(topic, new ConcurrentHashMap<>());
                foundTopic = channels.get(topic);
            }
            foundTopic.put(connectionId, subscriptionId);

            Map<String, String> userSubs = subscriptions.get(connectionId);
            if (userSubs == null) {
                subscriptions.put(connectionId, new ConcurrentHashMap<>());
                userSubs = subscriptions.get(subscriptionId);
            }
            userSubs.put(subscriptionId, topic);
        }
        return true;
    }

    @Override
    public boolean unsubscribe(int connectionId, String subscriptionId) {
        Map<String, String> userSubs = subscriptions.get(connectionId);
        String topic = userSubs.get(subscriptionId);
        if (topic == null)
            return false;
        userSubs.remove(subscriptionId);
        Map<Integer, String> foundTopic = channels.get(topic);
        foundTopic.remove(connectionId);
        return true;
    }

    @Override
    public boolean addNewClient(int connectionId, ConnectionHandler<T> handler) {
        if (connectedHandlers.containsKey(connectionId))
            return false;
        connectedHandlers.put(connectionId, handler);
        System.out.println("ADDED NEW CLIENT: " + connectionId);
        return true;
    }

    @Override
    public int addUser(int connectionId, String username, String password) {
        if (!usernamesPasswords.containsKey(username)) {
            System.out.println("=======================");
            System.out.println("==========connectionId======= " + connectionId);
            System.out.println("==========username=========== " + username);
            System.out.println("==========password=========== " + password);
            System.out.println("=======================");
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

        ConnectionHandler<T> handler = connectedHandlers.get(connectionId);
        try {
            handler.close();
        } catch (Exception e) {
            System.err.println("Failed to close connection");
        }

        connectedHandlers.remove(connectionId);

        if (username != null) {
            usernameToID.remove(username);
        }
        
        IDToUsername.remove(connectionId);

        Map<String, String> userSubs = subscriptions.get(connectionId);
        subscriptions.remove(connectionId);

        if (userSubs != null) {
            for (Map.Entry<String, String> registeredChannels : userSubs.entrySet()) {
                String channelName = registeredChannels.getValue();
                Map<Integer, String> channel = channels.get(channelName);
                channel.remove(connectionId);
            }
        }
    }

    @Override
    public boolean isUserConnected(int connectionId) {
        return this.connectedHandlers.get(connectionId) != null;
    }

}
