#include <string.h>
#include <string>
#include <iostream>
#include <future>

#include "ZserioPubsubMosquitto.h"
#include "calculator/PowerOfTwoProvider.h"

class PowerOfTwoProviderCallback :
        public calculator::PowerOfTwoProvider::PowerOfTwoProviderCallback<calculator::I32>
{
public:
    explicit PowerOfTwoProviderCallback(calculator::PowerOfTwoProvider& powerOfTwoProvider) :
            m_powerOfTwoProvider(powerOfTwoProvider)
    {
    }

    virtual void operator()(zserio::StringView topic, const calculator::I32& value) override
    {
        std::cout << "PowerOfTwoProvider: request=" << value.getValue() << std::endl;

        const uint64_t absValue = value.getValue() > 0
                ? static_cast<uint64_t>(value.getValue())
                : static_cast<uint64_t>(-value.getValue());

        calculator::U64 response{absValue * absValue};
        m_powerOfTwoProvider.publishPowerOfTwo(response);
    }

private:
    calculator::PowerOfTwoProvider& m_powerOfTwoProvider;
};


int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        if (strncmp(argv[1], "-h", 2) == 0 || strncmp(argv[1], "--help", 6) == 0)
        {
            std::cout << "Usage: " << argv[0] << " [HOST [PORT]]" << std::endl;
            return 0;
        }
    }

    std::string host = "localhost";
    if (argc > 1)
        host = argv[1];

    uint16_t port = 1883;
    if (argc > 2)
        port = atoi(argv[2]);

    // initialize mosquitto C library
    zserio_pubsub_mosquitto::MosquittoInitializer mosquittoInitializer;

    // instance of ZserioPubsubMosquitto client to be used as an IPubsub
    zserio_pubsub_mosquitto::MosquittoClient mosquittoClient(host, port);

    // power of two provider uses the mosquitto client backend
    calculator::PowerOfTwoProvider powerOfTwoProvider(mosquittoClient);

    std::shared_ptr<PowerOfTwoProviderCallback> callback(new PowerOfTwoProviderCallback(powerOfTwoProvider));
    powerOfTwoProvider.subscribeRequest(callback);

    std::cout << "Power of two provider, waiting for calculator/request..." << std::endl;
    std::cout << "Press Ctrl+C to quit." << std::endl;

    // wait forever
    std::promise<void>().get_future().wait();

    return 0;
}
