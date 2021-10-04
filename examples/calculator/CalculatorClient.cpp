#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>

#include "ZserioPubsubMosquitto.h"
#include "calculator/CalculatorClient.h"

void printHelp()
{
    std::cout << "Help:\n"
              << " INPUT        Any valid 32bit integer.\n"
              << " p            Subscribes/Unsubscribes calculator/power_of_two topic.\n"
              << " s            Subscribes/Unsubscribes calculator/square_root_of topic.\n"
              << " h            Prints this help.\n"
              << " q            Quits the client.\n"
              << "\n"
              << "Note that the letters before the '>' denotes the subscribed topics." << std::endl;
}

void publishRequest(calculator::CalculatorClient& client, const std::string& input)
{
    calculator::I32 request;
    try
    {
        size_t end;
        request.setValue(stoi(input, &end));
        if (end != input.size())
            throw std::invalid_argument("Parsed only to position: " + std::to_string(end));
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: '" << input << "' cannot be converted to int32!" << std::endl;
        std::cout << e.what() << std::endl;
        return;
    }

    client.publishRequest(request);
}

class PowerOfTwoCallback : public calculator::CalculatorClient::CalculatorClientCallback<calculator::U64>
{
public:
    virtual ~PowerOfTwoCallback() = default;

    virtual void operator()(zserio::StringView topic, const calculator::U64& value)
    {
        std::cout << "power of two: " << value.getValue() << std::endl;
    }
};

class SquareRootOfCallback : public calculator::CalculatorClient::CalculatorClientCallback<calculator::Double>
{
public:
    virtual ~SquareRootOfCallback() = default;

    virtual void operator()(zserio::StringView topic, const calculator::Double& value)
    {
        std::cout << "square root of: " << value.getValue() << std::endl;
    }
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
    uint16_t port = 1883; // default mosquitto broker port
    if (argc > 2)
        port = atoi(argv[2]);

    // initialize mosquitto C library
    zserio_pubsub_mosquitto::MosquittoInitializer mosquittoInitializer;

    std::cout << "Welcome to Zserio Calculator Mosquitto Pub/Sub Client example!\n"
              << "Creating client and subscriptions (terminate with ^C) ..." << std::flush;

    // instance of ZserioPubsubMosquitto client to be used as an IPubsub
    zserio_pubsub_mosquitto::MosquittoClient mosquittoClient(host, port);

    // calculator client uses the mosquitto client backend
    calculator::CalculatorClient client(mosquittoClient);

    std::shared_ptr<
            calculator::CalculatorClient::CalculatorClientCallback<calculator::U64>> powerOfTwoCallback(
                    new PowerOfTwoCallback());
    auto powerOfTwoId = client.subscribePowerOfTwo(powerOfTwoCallback);
    bool powerOfTwoSubscribed = true;

    std::shared_ptr<
            calculator::CalculatorClient::CalculatorClientCallback<calculator::Double>> squareRootOfCallback(
                    new SquareRootOfCallback());
    auto squareRootOfId = client.subscribeSquareRootOf(squareRootOfCallback);
    bool squareRootOfSubscribed = true;

    std::cout << " OK!\n"
              << "Write 'h' + ENTER for help." << std::endl;

    std::string input;
    while (true)
    {
        std::cout << (powerOfTwoSubscribed ? "p" : "")
                  << (squareRootOfSubscribed ? "s" : "")
                  << "> " << std::flush;
        getline(std::cin, input);

        if (input.empty())
            continue;

        if (input[0] == 'q')
        {
            std::cout << "Quit." << std::endl;
            return 0;
        }

        if (input[0] == 'h')
        {
            printHelp();
            continue;
        }

        if (input[0] == 'p')
        {
            if (powerOfTwoSubscribed)
            {
                client.unsubscribe(powerOfTwoId);
                powerOfTwoSubscribed = false;
            }
            else
            {
                powerOfTwoId = client.subscribePowerOfTwo(powerOfTwoCallback);
                powerOfTwoSubscribed = true;
            }
            continue;
        }

        if (input[0] == 's')
        {
            if (squareRootOfSubscribed)
            {
                client.unsubscribe(squareRootOfId);
                squareRootOfSubscribed = false;
            }
            else
            {
                squareRootOfId = client.subscribeSquareRootOf(squareRootOfCallback);
                squareRootOfSubscribed = true;
            }
            continue;
        }

        publishRequest(client, input);
    }
}
