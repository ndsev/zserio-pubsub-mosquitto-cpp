#include <math.h>
#include <string.h>
#include <string>
#include <iostream>
#include <future>

#include "ZserioPubsubMosquitto.h"
#include "calculator/SquareRootOfProvider.h"

class SquareRootOfProviderCallback :
        public calculator::SquareRootOfProvider::SquareRootOfProviderCallback<calculator::I32>
{
public:
    explicit SquareRootOfProviderCallback(calculator::SquareRootOfProvider& squareRootOfProvider) :
            m_squareRootOfProvider(squareRootOfProvider)
    {
    }

    virtual void operator()(zserio::StringView topic, const calculator::I32& value) override
    {
        std::cout << "SquareRootOfProvider: request=" << value.getValue() << std::endl;
        calculator::Double response{sqrt(static_cast<double>(value.getValue()))};
        m_squareRootOfProvider.publishSquareRootOf(response);
    }

private:
    calculator::SquareRootOfProvider& m_squareRootOfProvider;
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

    // square root of provider uses the mosquitto client backend
    calculator::SquareRootOfProvider squareRootOfProvider(mosquittoClient);

    std::shared_ptr<SquareRootOfProviderCallback> callback(
            new SquareRootOfProviderCallback(squareRootOfProvider));
    squareRootOfProvider.subscribeRequest(callback);

    std::cout << "Square root of provider, waiting for calculator/request..." << std::endl;
    std::cout << "Press Ctrl+C to quit." << std::endl;

    // wait forever
    std::promise<void>().get_future().wait();

    return 0;
}
