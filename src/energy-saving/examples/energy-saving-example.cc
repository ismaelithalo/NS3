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

static void
updateEnergy(Ptr<SimpleDeviceEnergyModel> device, double current)
{
    device->SetCurrentA(current);
}

int
main()
{
    Ptr<Node> node = CreateObject<Node>();

    BasicEnergySourceHelper basicSourceHelper;
    basicSourceHelper.Set("BasicEnergySourceInitialEnergyJ", DoubleValue(1000000));
    // basicSourceHelper.Set("BasicEnergySupplyVoltageV", DoubleValue(5.0));

    EnergySourceContainer sources = basicSourceHelper.Install(node);
    Ptr<ns3::EnergySource> energySrc = sources.Get(0);

    Ptr<SimpleDeviceEnergyModel> deviceEM = CreateObject<SimpleDeviceEnergyModel>();
    deviceEM->SetEnergySource(energySrc);
    deviceEM->SetNode(node);
    energySrc->AppendDeviceEnergyModel(deviceEM);

    // Ptr<EnergySourceContainer> energySrcCont = CreateObject<EnergySourceContainer>();
    // Ptr<LiIonEnergySource> energySrc = CreateObject<LiIonEnergySource>();
    // energySrcCont->Add(energySrc);
    // energySrc->SetNode(node);
    // deviceEM->SetEnergySource(energySrc);
    // energySrc->AppendDeviceEnergyModel(deviceEM);
    // deviceEM->SetNode(node);
    // node->AggregateObject(energySrcCont);

    deviceEM->SetCurrentA(2.33); // Isso é o que caracteriza um estado de descarga

    // Ptr<BasicEnergySource> basicSrc = CreateObject<BasicEnergySource>();
    // deviceEM->

    PrintCellInfo(energySrc);

    Simulator::Schedule(Seconds(100), &updateEnergy, deviceEM, 30);
    Simulator::Schedule(Seconds(300), &updateEnergy, deviceEM, 100);

    Simulator::Stop(Seconds(1000));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
