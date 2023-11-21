#include <ns3/core-module.h>
#include <ns3/energy-module.h>

using namespace ns3;

static void
PrintCellInfo(Ptr<ns3::EnergySource> es)
{
    std::cout << "At " << Simulator::Now().As(Time::S)
              << " Cell voltage: " << es->GetSupplyVoltage()
              << " V Remaining Capacity: " << es->GetRemainingEnergy() / (3.6 * 3600) << " Ah"
              << " V Remaining Energy: " << es->GetRemainingEnergy() << " J" << std::endl;

    if (!Simulator::IsFinished())
    {
        Simulator::Schedule(Seconds(10), &PrintCellInfo, es);
    }
}

// static void
// updateEnergy(Ptr<SimpleDeviceEnergyModel> device, double current)
// {
//     device->SetCurrentA(current);
// }

int
main()
{
    NodeContainer nodes;
    nodes.Create(2);
    // Ptr<Node> node = CreateObject<Node>();
    Ptr<Node> node = nodes.Get(0);
    Ptr<Node> node2 = nodes.Get(1);

    BasicEnergySourceHelper basicSourceHelper;
    basicSourceHelper.Set("BasicEnergySourceInitialEnergyJ", DoubleValue(1000000));
    // basicSourceHelper.Set("BasicEnergySupplyVoltageV", DoubleValue(5.0));
    // basicSourceHelper.Set("PeriodicEnergyUpdateInterval", TimeValue(Seconds(updateIntervalS)));

    EnergySourceContainer sources = basicSourceHelper.Install(nodes);
    // sources = basicSourceHelper.Install(node2);
    Ptr<ns3::EnergySource> energySrc = sources.Get(0);

    Ptr<SimpleDeviceEnergyModel> deviceEM = CreateObject<SimpleDeviceEnergyModel>();
    deviceEM->SetNode(node);
    deviceEM->SetEnergySource(energySrc);
    energySrc->AppendDeviceEnergyModel(deviceEM);
    deviceEM->SetCurrentA(2.33); // Isso é o que caracteriza um estado de descarga

    Ptr<SimpleDeviceEnergyModel> deviceEM2 = CreateObject<SimpleDeviceEnergyModel>();
    deviceEM2->SetEnergySource(energySrc);
    deviceEM2->SetNode(node2);
    energySrc->AppendDeviceEnergyModel(deviceEM2);
    deviceEM2->SetCurrentA(5); // Isso é o que caracteriza um estado de descarga

    // Ptr<EnergySourceContainer> energySrcCont = CreateObject<EnergySourceContainer>();
    // Ptr<LiIonEnergySource> energySrc = CreateObject<LiIonEnergySource>();
    // energySrcCont->Add(energySrc);
    // energySrc->SetNode(node);
    // deviceEM->SetEnergySource(energySrc);
    // energySrc->AppendDeviceEnergyModel(deviceEM);
    // deviceEM->SetNode(node);
    // node->AggregateObject(energySrcCont);

    // Ptr<BasicEnergySource> basicSrc = CreateObject<BasicEnergySource>();
    // deviceEM->

    PrintCellInfo(energySrc);

    // Simulator::Schedule(Seconds(100), &updateEnergy, deviceEM, 30);
    // Simulator::Schedule(Seconds(300), &updateEnergy, deviceEM, 100);

    Simulator::Stop(Seconds(100));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
