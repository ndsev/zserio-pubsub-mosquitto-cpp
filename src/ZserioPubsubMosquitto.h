#ifndef ZSERIO_PUBSUB_MOSQUITTO_ZSERIO_PUBSUB_MOSQUITTO_H
#define ZSERIO_PUBSUB_MOSQUITTO_ZSERIO_PUBSUB_MOSQUITTO_H

#include <map>
#include <memory>

#include "zserio/IPubsub.h"

namespace zserio_pubsub_mosquitto
{

/**
 * Sample Zseiro IPubsub implementation using mosquitto C library.
 */
class MosquittoClient : public zserio::IPubsub
{
public:
    class MosquittoSubscription;

    MosquittoClient(const std::string& host, uint16_t port);
    ~MosquittoClient();

    virtual void publish(const std::string& topic, const std::vector<uint8_t>& data, void* context) override;
    virtual SubscriptionId subscribe(const std::string& topic, const OnTopic& callback, void* context) override;
    virtual void unsubscribe(SubscriptionId id) override;

private:
    std::string m_host;
    uint16_t m_port;
    typedef std::unique_ptr<MosquittoSubscription> MosquittoSubscriptionPtr;
    std::map<SubscriptionId, MosquittoSubscriptionPtr> m_subscriptions;
    SubscriptionId m_numIds; // simple naive implementation, reusing of subscription ID is not safe

    static constexpr int KEEPALIVE = 60;
};

/**
 * Mosquitto initialization helper. Just create an instance at the beginning of your main function.
 */
class MosquittoInitializer
{
public:
    MosquittoInitializer();
    ~MosquittoInitializer();
};

} // namespace zserio_pubsub_mosquitto

#endif // ZSERIO_PUBSUB_MOSQUITTO_ZSERIO_PUBSUB_MOSQUITTO_H
