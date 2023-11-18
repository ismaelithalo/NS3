
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

// NS_LOG_COMPONENT_DEFINE("InfraTest");

int
main(int argc, char* argv[])
{
    // Create a new ns3 project
    // NS_LOG_INFO("Creating a new ns3 project");

    // Enable logging
    // LogComponentEnable("MobileNetwork", LOG_LEVEL_INFO);

    // Set up the simulation environment
    NodeContainer nodes;
    nodes.Create(2);

    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(5.0),
                                  "DeltaY",
                                  DoubleValue(10.0),
                                  "GridWidth",
                                  UintegerValue(3),
                                  "LayoutType",
                                  StringValue("RowFirst"));
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds",
                              RectangleValue(Rectangle(-50, 50, -50, 50)));
    mobility.Install(nodes);

    // Create a point-to-point link between two nodes
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices;
    devices = p2p.Install(nodes);

    // Install an internet stack on each node
    InternetStackHelper stack;
    stack.Install(nodes);

    // Assign IP addresses to each node
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Start the simulation and run it for a specified amount of time
    Simulator::Stop(Seconds(10));
    Simulator::Run();

    // Print out the simulation results
    // NS_LOG_INFO("Simulation completed");

    Simulator::Destroy();
    return 0;
}
