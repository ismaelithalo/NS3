#include <ns3/core-module.h>
#include <ns3/energy-module.h>

using namespace ns3;

static void
PrintCellInfo(Ptr<LiIonEnergySource> es)
{
    std::cout << "At " << Simulator::Now().As(Time::S)
              << " Cell voltage: " << es->GetSupplyVoltage()
              << " V Remaining Capacity: " << es->GetRemainingEnergy() / (3.6 * 3600)
              << " Ah"
              //   << " V Remaining Energy: " << es->GetRemainingEnergy() << " J"
              << std::endl;

    if (!Simulator::IsFinished())
    {
        Simulator::Schedule(Seconds(10), &PrintCellInfo, es);
    }
}

int
main()
{
    Ptr<Node> node = CreateObject<Node>();

    Ptr<SimpleDeviceEnergyModel> deviceEM = CreateObject<SimpleDeviceEnergyModel>();
    // Ptr<EnergySourceContainer> energySrcCont = CreateObject<EnergySourceContainer>();
    Ptr<LiIonEnergySource> energySrc = CreateObject<LiIonEnergySource>();
    // energySrcCont->Add(energySrc);
    energySrc->SetNode(node);
    deviceEM->SetEnergySource(energySrc);
    energySrc->AppendDeviceEnergyModel(deviceEM);
    deviceEM->SetNode(node);
    // node->AggregateObject(energySrcCont);

    deviceEM->SetCurrentA(2.33); // Isso é o que caracteriza um estado de descarga

    // Ptr<BasicEnergySource> basicSrc = CreateObject<BasicEnergySource>();
    // deviceEM->

    PrintCellInfo(energySrc);

    Simulator::Stop(Seconds(100));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
