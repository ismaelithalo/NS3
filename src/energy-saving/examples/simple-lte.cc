#include <ns3/core-module.h>
#include <ns3/lte-module.h>
#include <ns3/mobility-building-info.h>
#include <ns3/mobility-module.h>
#include <ns3/network-module.h>

using namespace ns3;

int
main()
{
    // Define some model params

    uint16_t numberOfUes = 1;
    uint16_t numberOfEnbs = 2;
    // double enbTxPowerDbm = 46.0;

    // Create an LteHelper object
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();

    // Create Node objects for the eNB(s) and the UEs
    NodeContainer enbNodes;
    NodeContainer ueNodes;
    enbNodes.Create(numberOfEnbs);
    ueNodes.Create(numberOfUes);

    // Configure the Mobility model for all the nodes
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(enbNodes);
    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobility.Install(ueNodes);

    // Rectangle (double _xMin, double _xMax, double _yMin, double _yMax)

    // Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(enbTxPowerDbm));

    // Install an LTE protocol stack on the eNB(s)
    NetDeviceContainer enbDevs;
    enbDevs = lteHelper->InstallEnbDevice(enbNodes);

    // Install an LTE protocol stack on the UEs
    NetDeviceContainer ueDevs;
    ueDevs = lteHelper->InstallUeDevice(ueNodes);

    lteHelper->Attach(ueDevs.Get(0), enbDevs.Get(1));
    // lteHelper->Attach(ueDevs.Get(1), enbDevs.Get(1));

    // Activate a data radio bearer between each UE and the eNB it is attached to
    enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    EpsBearer bearer(q);
    lteHelper->ActivateDataRadioBearer(ueDevs, bearer);

    // Enable PHY, MAC, RLC and PDCP level Key Performance Indicators (KPIs).
    // lteHelper->EnablePhyTraces();
    // lteHelper->EnableMacTraces();
    lteHelper->EnableRlcTraces();
    // lteHelper->EnablePdcpTraces();

    // Config::SetDefault("ns3::RadioBearerStatsCalculator::EpochDuration", TimeValue(Seconds(2)));

    Simulator::Stop(Seconds(1));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}