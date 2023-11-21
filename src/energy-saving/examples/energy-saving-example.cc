#include <ns3/core-module.h>
#include <ns3/energy-module.h>

using namespace ns3;

// static void
// PrintCellInfo(Ptr<ns3::EnergySource> es)
// {
//     std::cout << "At " << Simulator::Now().As(Time::S)
//               << " Cell voltage: " << es->GetSupplyVoltage()
//               << " V Remaining Capacity: " << es->GetRemainingEnergy() / (3.6 * 3600)
//               << " Ah"
//               //   << " V Remaining Energy: " << es->GetRemainingEnergy() << " J"
//               << std::endl;

//     if (!Simulator::IsFinished())
//     {
//         Simulator::Schedule(Seconds(10), &PrintCellInfo, es);
//     }
// }

void
PrintRemainEnergy(double oldValue, double value)
{
    std::cout << "Remaining energy of source: " << value << std::endl;
}

// static void
// updateEnergy(Ptr<SimpleDeviceEnergyModel> device, double current)
// {
//     device->SetCurrentA(current);
// }

// void
// TotalEnergyConsumption(Ptr<SimpleDeviceEnergyModel> device)
// {
//     std::cout << "Total energy consumption of node: " << device->GetNode()->GetId() << " : "
//               << device->GetTotalEnergyConsumption() << std::endl;
// }

// void
// TotalEnergyConsumption(double oldValue, double totalEnergyConsumption)
// {
//     std::cout << "Total energy consumption: " << totalEnergyConsumption << std::endl;
// }

void
TotalEnergyConsumptionByDevice(DeviceEnergyModelContainer deviceEMCont)
{
    double totalEnergyConsumption = 0;
    for (uint32_t i = 0; i < deviceEMCont.GetN(); ++i)
    {
        Ptr<DeviceEnergyModel> device = deviceEMCont.Get(i);
        totalEnergyConsumption += device->GetTotalEnergyConsumption();
        std::cout << "Total energy consumption of node " << i << " : "
                  << device->GetTotalEnergyConsumption() << std::endl;
    }
    std::cout << "Total energy consumption: " << totalEnergyConsumption << std::endl;
}

int
main()
{
    uint16_t numberOfEnbs = 4;

    int defaultCurrent = 10;
    int defaultSourceVoltage = 5;
    int defaultSourceInitialEnergyJ = 1000000;
    int defaultSourceUpdateInterval = 75;

    double simuTime = 1000;

    NodeContainer enbNodes;
    enbNodes.Create(numberOfEnbs);

    Ptr<BasicEnergySource> basicSource = CreateObject<BasicEnergySource>();
    basicSource->SetSupplyVoltage(defaultSourceVoltage);
    basicSource->SetEnergyUpdateInterval(Seconds(defaultSourceUpdateInterval));
    basicSource->SetInitialEnergy(defaultSourceInitialEnergyJ);
    basicSource->TraceConnectWithoutContext("RemainingEnergy", MakeCallback(&PrintRemainEnergy));

    DeviceEnergyModelContainer deviceEMCont;

    // deviceEM->TraceConnectWithoutContext("TotalEnergyConsumption",
    //                                      MakeCallback(&TotalEnergyConsumption));

    for (uint32_t u = 0; u < enbNodes.GetN(); ++u)
    {
        Ptr<Node> enb = enbNodes.Get(u);
        basicSource->SetNode(enb);

        Ptr<SimpleDeviceEnergyModel> eneMod = CreateObject<SimpleDeviceEnergyModel>();
        eneMod->SetNode(enb);
        eneMod->SetEnergySource(basicSource);
        eneMod->SetCurrentA(defaultCurrent);

        basicSource->AppendDeviceEnergyModel(eneMod);
        deviceEMCont.Add(eneMod);
    }

    // EnergySourceContainer sources = basicSourceHelper.Install(nodes);
    // sources = basicSourceHelper.Install(node2);
    // Ptr<ns3::EnergySource> energySrc = sources.Get(0);

    // Ptr<EnergySourceContainer> energySrcCont = CreateObject<EnergySourceContainer>();
    // Ptr<LiIonEnergySource> energySrc = CreateObject<LiIonEnergySource>();
    // energySrcCont->Add(energySrc);
    // energySrc->SetNode(node);
    // deviceEM->SetEnergySource(energySrc);
    // energySrc->AppendDeviceEnergyModel(deviceEM);
    // deviceEM->SetNode(node);
    // node->AggregateObject(energySrcCont);

    // PrintCellInfo(energySrc);

    Simulator::Schedule(Seconds(simuTime), &TotalEnergyConsumptionByDevice, deviceEMCont);
    // Simulator::Schedule(Seconds(simuTime), &TotalEnergyConsumption, deviceEM);
    // Simulator::Schedule(Seconds(simuTime - 2), &TotalEnergyConsumption, deviceEM2);
    // Simulator::Schedule(Seconds(100), &updateEnergy, deviceEM, 30);
    // Simulator::Schedule(Seconds(300), &updateEnergy, deviceEM, 100);

    Simulator::Stop(Seconds(simuTime));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
