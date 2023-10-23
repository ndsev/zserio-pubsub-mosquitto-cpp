#include <thread>
#include <future>
#include <chrono>

#include <mosquitto.h>

#include "ZserioPubsubMosquitto.h"

extern "C"
{
    static void message_callback(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* msg);
}

namespace zserio_pubsub_mosquitto
{

struct MosquittoDestroyer
{
    void operator()(struct mosquitto* mosq) const
    {
        mosquitto_destroy(mosq);
    }
};

typedef std::unique_ptr<struct mosquitto, MosquittoDestroyer> MosquittoPtr;

class MosquittoClient::MosquittoSubscription
{
public:
    MosquittoSubscription(const std::string& host, uint16_t port,
            zserio::IPubsub::SubscriptionId id, zserio::StringView topic,
            const std::shared_ptr<zserio::IPubsub::OnTopicCallback>& callback);

    ~MosquittoSubscription();

    MosquittoSubscription(const MosquittoSubscription& other) = delete;
    MosquittoSubscription& operator=(const MosquittoSubscription& other) = delete;

    MosquittoSubscription(MosquittoSubscription&& other) = delete;
    MosquittoSubscription& operator=(MosquittoSubscription&& other) = delete;

    void callback(const struct mosquitto_message* msg);
    void loopThread(std::future<void> futureObj);

private:
    std::string m_host;
    uint16_t m_port;
    std::string m_topic;
    std::shared_ptr<zserio::IPubsub::OnTopicCallback> m_callback;

    MosquittoPtr m_mosq;
    std::thread m_loopThread;
    std::promise<void> m_exitSignal;
};

MosquittoClient::MosquittoSubscription::MosquittoSubscription(const std::string& host, uint16_t port,
        zserio::IPubsub::SubscriptionId id, zserio::StringView topic,
        const std::shared_ptr<zserio::IPubsub::OnTopicCallback>& callback)
:   m_host(host), m_port(port), m_topic(topic.data(), topic.size()), m_callback(callback)
{
    m_mosq.reset(mosquitto_new(nullptr, true, this));
    if (!m_mosq)
        throw std::runtime_error("Failed to create mosquitto instance!");
    mosquitto_message_callback_set(m_mosq.get(), message_callback);
    int rc = mosquitto_connect(m_mosq.get(), m_host.c_str(), m_port, 60);
    if (rc)
    {
        throw std::runtime_error(std::string("MosquittoSubscription " + std::to_string(id) +
                " failed to connect! ") + mosquitto_strerror(rc));
    }
    mosquitto_subscribe(m_mosq.get(), nullptr, m_topic.c_str(), 0);

    m_loopThread =  std::thread(&MosquittoClient::MosquittoSubscription::loopThread, this,
            m_exitSignal.get_future());
}

MosquittoClient::MosquittoSubscription::~MosquittoSubscription()
{
    if (m_mosq)
    {
        mosquitto_unsubscribe(m_mosq.get(), nullptr, m_topic.c_str());
        m_exitSignal.set_value();
        m_loopThread.join();
        mosquitto_disconnect(m_mosq.get()); // must be called after m_loopThread is finished
    }
}

void MosquittoClient::MosquittoSubscription::callback(const struct mosquitto_message* msg)
{
    uint8_t* payload = static_cast<uint8_t*>(msg->payload);
    uint32_t payloadlen = msg->payloadlen;
    std::vector<uint8_t> data(payload, payload + payloadlen);
    (*m_callback)(zserio::StringView(msg->topic), zserio::Span<const uint8_t>(data));
}

void MosquittoClient::MosquittoSubscription::loopThread(std::future<void> futureObj)
{
    while (futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout)
    {
        const int rc = mosquitto_loop(m_mosq.get(), -1, 1);
        if (rc)
        {
            throw std::runtime_error(std::string("MosquittoSubscription failed to loop! ") +
                    mosquitto_strerror(rc));
        }
    }
}

class MosquittoClient::MosquittoPublisher
{
public:
    MosquittoPublisher(const std::string& host, uint16_t port);

    ~MosquittoPublisher();

    MosquittoPublisher(const MosquittoPublisher& other) = delete;
    MosquittoPublisher& operator=(const MosquittoPublisher& other) = delete;

    MosquittoPublisher(MosquittoPublisher&& other) = delete;
    MosquittoPublisher& operator=(MosquittoPublisher&& other) = delete;

    void publish(zserio::StringView topic, zserio::Span<const uint8_t> data);

private:
    MosquittoPtr m_mosq;
};

MosquittoClient::MosquittoPublisher::MosquittoPublisher(const std::string& host, uint16_t port)
:   m_mosq(mosquitto_new(nullptr, true, this))
{
    int rc = mosquitto_connect(m_mosq.get(), host.c_str(), port, 60);
    if (rc)
    {
        throw std::runtime_error(std::string("MosquittoClient failed to connect! ") +
                mosquitto_strerror(rc));
    }
}

MosquittoClient::MosquittoPublisher::~MosquittoPublisher()
{
    mosquitto_disconnect(m_mosq.get());
}

void MosquittoClient::MosquittoPublisher::publish(zserio::StringView topic, zserio::Span<const uint8_t> data)
{
    const std::string topicString(topic.data(), topic.size());
    mosquitto_publish(m_mosq.get(), nullptr, topicString.c_str(), static_cast<int>(data.size()), data.data(),
            0, 0);
}

MosquittoClient::MosquittoClient(const std::string& host, uint16_t port)
:   m_host(host), m_port(port), m_numIds(0), m_publisher(new MosquittoPublisher(host, port))
{
}

// no inline to because MosquittoSubscription uses unique_ptr
MosquittoClient::~MosquittoClient()
{
}

void MosquittoClient::publish(zserio::StringView topic, zserio::Span<const uint8_t> data, void*)
{
    m_publisher->publish(topic, data);
}

MosquittoClient::SubscriptionId MosquittoClient::subscribe(zserio::StringView topic,
        const std::shared_ptr<OnTopicCallback>& callback, void*)
{
    // TODO: use the context
    m_subscriptions.emplace(m_numIds, MosquittoSubscriptionPtr(
            new MosquittoSubscription(m_host, m_port, m_numIds, topic, callback)));
    return m_numIds++;
}

void MosquittoClient::unsubscribe(SubscriptionId id)
{
    m_subscriptions.erase(id);
}

MosquittoInitializer::MosquittoInitializer()
{
    mosquitto_lib_init();
}

MosquittoInitializer::~MosquittoInitializer()
{
    mosquitto_lib_cleanup();
}

} // namespace zserio_pubsub_mosquitto

extern "C"
{
    static void message_callback(struct mosquitto*, void* userdata, const struct mosquitto_message* msg)
    {
        static_cast<zserio_pubsub_mosquitto::MosquittoClient::MosquittoSubscription*>(userdata)->callback(msg);
    }
}
