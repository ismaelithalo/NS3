// Developed by: Ismael Ithalo, @ismaelithalo

#include <ns3/applications-module.h>
#include <ns3/core-module.h>
#include <ns3/energy-module.h>
#include <ns3/internet-module.h>
#include <ns3/lte-module.h>
#include <ns3/mobility-building-info.h>
#include <ns3/mobility-module.h>
#include <ns3/network-module.h>
#include <ns3/point-to-point-module.h>

#include <vector>

using namespace ns3;
using namespace std;

static double IDLE_A = 86.3;
static double TX_A = 742.2;
static double RX_A = 138.9;

vector<Ptr<SimpleDeviceEnergyModel>> simDevEMCont;

// enum State
// {
//     IDLE = 0,
//     TX_DL_CTRL = 1,
//     TX_DATA = 2,
//     TX_UL_SRS = 3,
//     RX_DL_CTRL = 4,
//     RX_DATA = 5,
//     RX_UL_SRS = 6
// };

// -------- Function called when there is a course change
static void
CourseChange(std::string context, Ptr<const MobilityModel> mobility)
{
    Vector pos = mobility->GetPosition();
    Vector vel = mobility->GetVelocity();
    std::cout << Simulator::Now().GetSeconds() << ", model=" << mobility << ", POS: x=" << pos.x
              << ", y=" << pos.y << "; VEL:" << vel.x << ", y=" << vel.y << ";" << std::endl;
}

void
NotifyHandoverEndOkEnb(std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
    std::cout << Simulator::Now().GetSeconds() << ", eNB CellId " << cellId
              << ": completed handover of UE with IMSI " << imsi << " RNTI " << rnti << std::endl;
}

void
simpleSchedule(uint64_t value)
{
    std::cout << "VALOR COLETADO: " << value << std::endl;
}

void
setCurrentDeviceVector(uint32_t id, double current)
{
    for (uint32_t i = 0; i < simDevEMCont.size(); ++i)
    {
        Ptr<SimpleDeviceEnergyModel> device = simDevEMCont[i];
        if (device->GetNode()->GetId() == id)
        {
            device->SetCurrentA(current);
        }
    }
}

void
sleepSchedule(Ptr<LteEnbNetDevice> enbDevice)
{
    double currentPoweSleep = 0.0;
    uint32_t id = enbDevice->GetNode()->GetId();

    setCurrentDeviceVector(id, currentPoweSleep);

    enbDevice->GetPhy()->SetTxPower(currentPoweSleep);
    double txPower = enbDevice->GetPhy()->GetTxPower();
    // enbDevice->GetPhy()->GetDlSpectrumPhy()->SetState(ns3::LteSpectrumPhy::State::IDLE);
    // enbDevice->GetPhy()->GetUlSpectrumPhy()->SetState(ns3::LteSpectrumPhy::State::IDLE);
    std::cout << "Sleep scheduled event for Cell " << id << std::endl;
    std::cout << "TxPower of enb " << id << ": " << txPower << std::endl;
}

// void
// ChangeStateEventUl(int32_t oldState, int32_t newState)
// {
//     // ChangeState(newState);
//     std::cout << "ESTADO DO UPLINK: " << newState << std::endl;
// }
void
ChangeStateEventUlv2(string idOnContext, int32_t oldState, int32_t newState)
{
    uint32_t id = stoi(idOnContext);

    switch (newState)
    {
    case 0:
        setCurrentDeviceVector(id, IDLE_A);
        break;
    case 1:
    case 2:
    case 3:
        setCurrentDeviceVector(id, TX_A);
        break;
    case 4:
    case 5:
    case 6:
        setCurrentDeviceVector(id, RX_A);
        break;

    default:
        break;
    }

    // ChangeState(newState);
    std::cout << "ESTADO DO UPLINK PARA O NÓ " << idOnContext << ": " << newState << std::endl;
}

// void
// ChangeStateEventDl(int32_t oldState, int32_t newState)
// {
//     // ChangeState(newState);
//     std::cout << "ESTADO DO DOWNLINK: " << newState << std::endl;
// }
void
ChangeStateEventDlv2(string idOnContext, int32_t oldState, int32_t newState)
{
    uint32_t id = stoi(idOnContext);

    switch (newState)
    {
    case 0:
        setCurrentDeviceVector(id, IDLE_A);
        break;
    case 1:
    case 2:
    case 3:
        setCurrentDeviceVector(id, TX_A);
        break;
    case 4:
    case 5:
    case 6:
        setCurrentDeviceVector(id, RX_A);
        break;

    default:
        break;
    }

    // ChangeState(newState);
    std::cout << "ESTADO DO DOWNLINK PARA O NÓ " << idOnContext << ": " << newState << std::endl;
}

void
PrintRemainEnergy(double oldValue, double value)
{
    std::cout << "Remaining energy of source: " << value << std::endl;
}

void
TotalEnergyConsumptionByDevice(vector<Ptr<SimpleDeviceEnergyModel>> deviceEMCont)
{
    double totalEnergyConsumption = 0;
    for (uint32_t i = 0; i < deviceEMCont.size(); ++i)
    {
        Ptr<SimpleDeviceEnergyModel> device = deviceEMCont[i];
        totalEnergyConsumption += device->GetTotalEnergyConsumption();
        std::cout << "Total energy consumption of node " << device->GetNode()->GetId() << " : "
                  << device->GetTotalEnergyConsumption() << std::endl;
    }
    std::cout << "Total energy consumption: " << totalEnergyConsumption << std::endl;
}

int
main()
{
    // -------- Define some model params

    uint16_t numberOfUes = 2;
    uint16_t numberOfEnbs = 2;
    double enbDistance =
        75; // m -> The size of environment is defined by enbDistance and numberOfEnbs
    double enbTxPowerDbm = 46.0;
    string ueMobilitySpeed = "3.0"; // m
    string ueMobilityTime = "1s";   // s

    double simuTime = 10;
    double sleepTime = 8;

    // Config::SetDefault("ns3::RadioBearerStatsCalculator::EpochDuration",TimeValue(Seconds(2)));
    // Config::SetDefault("ns3::RandomWalk2dMobilityModel::Mode",StringValue("Time"));
    // Config::SetDefault("ns3::RandomWalk2dMobilityModel::Time",StringValue("2s"));
    // Config::SetDefault("ns3::RandomWalk2dMobilityModel::Speed",StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    // Config::SetDefault("ns3::RandomWalk2dMobilityModel::Bounds", StringValue("0|50|0|50"));

    // -------- Create an LteHelper object
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);

    lteHelper->SetHandoverAlgorithmType("ns3::A3RsrpHandoverAlgorithm");
    lteHelper->SetHandoverAlgorithmAttribute("Hysteresis", DoubleValue(3.0));
    lteHelper->SetHandoverAlgorithmAttribute("TimeToTrigger", TimeValue(MilliSeconds(256)));

    Ptr<Node> pgw = epcHelper->GetPgwNode();

    // Create a single RemoteHost
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // Create the internet
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(1500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.010)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);

    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting;
    remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

    // -------- Create Node objects for the eNB(s) and the UEs
    NodeContainer enbNodes;
    NodeContainer ueNodes;
    enbNodes.Create(numberOfEnbs);
    ueNodes.Create(numberOfUes);

    // -------- Configure the Mobility model for all the nodes

    Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator>();

    uint16_t envX, envY;

    if (numberOfEnbs % 2 == 0 && numberOfEnbs > 2)
    {
        for (uint16_t i = 0; i < 2; i++)
        {
            for (uint16_t j = 0; j < (numberOfEnbs / 2); j++)
            {
                Vector enbPosition(enbDistance * (i + 1), enbDistance * (j + 1), 0);
                enbPositionAlloc->Add(enbPosition);
            }
        }
        envX = ((numberOfEnbs / 2) + 1) * enbDistance;
        envY = (enbDistance * 3);
    }
    else
    {
        for (uint16_t i = 1; i < numberOfEnbs + 1; i++)
        {
            Vector enbPosition(enbDistance * (i + 1), enbDistance, 0);
            enbPositionAlloc->Add(enbPosition);
        }
        envX = (numberOfEnbs + 1) * enbDistance;
        envY = (enbDistance * 2);
    }

    string envXStr = to_string(envX);
    string envYStr = to_string(envY);
    string envXCenterStr = to_string(envX / 2);
    string envYCenterStr = to_string(envY / 2);

    MobilityHelper enbMobility;
    enbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    enbMobility.SetPositionAllocator(enbPositionAlloc);
    enbMobility.Install(enbNodes);

    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                  "X",
                                  StringValue(envXCenterStr),
                                  "Y",
                                  StringValue(envYCenterStr),
                                  "Rho",
                                  StringValue("ns3::UniformRandomVariable[Min=0|Max=50]"));

    mobility.SetMobilityModel(
        "ns3::RandomWalk2dMobilityModel",
        "Mode",
        StringValue("Time"),
        "Time",
        StringValue(ueMobilityTime),
        "Speed",
        StringValue("ns3::ConstantRandomVariable[Constant=" + ueMobilitySpeed + "]"),
        "Bounds",
        StringValue("0|" + envXStr + "|0|" + envYStr));

    mobility.Install(ueNodes);

    // -------- For each course change event, print out a line of text
    // Config::Connect("/NodeList/*/$ns3::MobilityModel/CourseChange", MakeCallback(&CourseChange));

    // -------- Install an LTE protocol stack on nodes

    Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(enbTxPowerDbm));

    NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice(ueNodes);

    internet.Install(ueNodes);

    // assign IP address to UEs
    for (uint32_t u = 0; u < ueNodes.GetN(); ++u)
    {
        Ptr<Node> ue = ueNodes.Get(u);
        Ptr<NetDevice> ueLteDevice = ueLteDevs.Get(u);
        Ipv4InterfaceContainer ueIpIface;
        ueIpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueLteDevice));
        // set the default gateway for the UE
        Ptr<Ipv4StaticRouting> ueStaticRouting;
        ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(ue->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);

        uint16_t dlPort = 1234;
        PacketSinkHelper packetSinkHelper("ns3::UdpSocketFactory",
                                          InetSocketAddress(Ipv4Address::GetAny(), dlPort));
        ApplicationContainer serverApps = packetSinkHelper.Install(ue);
        serverApps.Start(Seconds(0.01));
        UdpClientHelper client(ueIpIface.GetAddress(0), dlPort);
        ApplicationContainer clientApps = client.Install(remoteHost);
        clientApps.Start(Seconds(0.01));
    }

    // Attach all UEs to the eNodeBs with the strongest RSRP
    lteHelper->Attach(ueLteDevs);

    Ptr<EpcTft> tft = Create<EpcTft>();
    EpcTft::PacketFilter pf;
    pf.localPortStart = 1234;
    pf.localPortEnd = 1234;
    tft->Add(pf);
    lteHelper->ActivateDedicatedEpsBearer(ueLteDevs,
                                          EpsBearer(EpsBearer::NGBR_VIDEO_TCP_DEFAULT),
                                          tft);

    // -------- Activate a data radio bearer between each UE and the eNB it is attached to
    // enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    // EpsBearer bearer(q);
    // lteHelper->ActivateDataRadioBearer(ueDevs, bearer);

    // Add X2 interface
    lteHelper->AddX2Interface(enbNodes);

    // Configura regra de encaminhamento para eNBs que não sejam o primeiro
    for (unsigned i = 1; i < numberOfEnbs; i++)
    {
        Ipv4StaticRoutingHelper ipv4RoutingHelper;
        Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(enbNodes.Get(i)->GetObject<Ipv4>());
        remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("10.0.0.6"),
                                                   Ipv4Mask("255.255.255.252"),
                                                   1);
    }

    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
                    MakeCallback(&NotifyHandoverEndOkEnb));

    // Ptr<NetDevice> device = enbLteDevs.Get(0);
    // Ptr<Node> node = device->GetNode();
    // Ptr<LteEnbNetDevice> mmwaveEnbDevice = DynamicCast<LteEnbNetDevice>(device);
    // Ptr<LteEnbPhy> mmwavePhy = mmwaveEnbDevice->GetPhy();

    // Ptr<LteSpectrumPhy> mmwaveDlSpectrumPhy = mmwavePhy->GetDlSpectrumPhy();
    // Ptr<LteSpectrumPhy> mmwaveUlSpectrumPhy = mmwavePhy->GetUlSpectrumPhy();

    // string id = to_string(device->GetNode()->GetId());

    // mmwaveDlSpectrumPhy->TraceConnect("State", id, MakeCallback(&ChangeStateEventDlv2));
    // mmwaveUlSpectrumPhy->TraceConnect("State", id, MakeCallback(&ChangeStateEventUlv2));

    for (uint32_t u = 0; u < enbLteDevs.GetN(); ++u)
    {
        Ptr<NetDevice> device = enbLteDevs.Get(u);
        Ptr<LteEnbNetDevice> lteEnbDeviceConfig = DynamicCast<LteEnbNetDevice>(device);
        Ptr<LteEnbPhy> ltePhy = lteEnbDeviceConfig->GetPhy();
        Ptr<LteSpectrumPhy> lteDlSpectrumPhy = ltePhy->GetDlSpectrumPhy();
        Ptr<LteSpectrumPhy> lteUlSpectrumPhy = ltePhy->GetUlSpectrumPhy();

        string id = to_string(device->GetNode()->GetId());

        lteDlSpectrumPhy->TraceConnect("State", id, MakeCallback(&ChangeStateEventDlv2));
        lteUlSpectrumPhy->TraceConnect("State", id, MakeCallback(&ChangeStateEventUlv2));
        // lteDlSpectrumPhy->TraceConnectWithoutContext("State",
        // MakeCallback(&ChangeStateEventDl));
        // lteUlSpectrumPhy->TraceConnectWithoutContext("State",
        // MakeCallback(&ChangeStateEventUl));
    }

    //     enum State
    // {
    //     IDLE = 0,
    //     TX_DL_CTRL = 1,
    //     TX_DATA = 2,
    //     TX_UL_SRS = 3,
    //     RX_DL_CTRL = 4,
    //     RX_DATA = 5,
    //     RX_UL_SRS = 6
    // };

    // Energy Framework
    double defaultCurrent = 86.3;
    int defaultSourceVoltage = IDLE_A;
    int defaultSourceInitialEnergyJ = 100000000;
    int defaultSourceUpdateInterval = 30;

    Ptr<BasicEnergySource> basicSource = CreateObject<BasicEnergySource>();
    basicSource->SetSupplyVoltage(defaultSourceVoltage);
    basicSource->SetEnergyUpdateInterval(Seconds(defaultSourceUpdateInterval));
    basicSource->SetInitialEnergy(defaultSourceInitialEnergyJ);
    basicSource->TraceConnectWithoutContext("RemainingEnergy", MakeCallback(&PrintRemainEnergy));

    // DeviceEnergyModelContainer deviceEMCont; //For some reason, the container coverts the
    // SimpleDeviceEnergyModel to DeviceEnergyModel, so some atributtes are not available
    // So, I created a vector of Ptr<SimpleDeviceEnergyModel> to store the energy models
    // vector<Ptr<SimpleDeviceEnergyModel>> simDevEMCont;

    for (uint32_t u = 0; u < enbNodes.GetN(); ++u)
    {
        Ptr<Node> enb = enbNodes.Get(u);
        basicSource->SetNode(enb);

        Ptr<SimpleDeviceEnergyModel> eneMod = CreateObject<SimpleDeviceEnergyModel>();
        eneMod->SetNode(enb);
        eneMod->SetEnergySource(basicSource);
        eneMod->SetCurrentA(defaultCurrent);
        // eneMod->GetNode()->GetId();

        basicSource->AppendDeviceEnergyModel(eneMod);
        // deviceEMCont.Add(eneMod);
        simDevEMCont.push_back(eneMod);
        // enb->AggregateObject(basicSource); //EXPLORAR ESSE PONTO
    }
    Simulator::Schedule(Seconds(simuTime), &TotalEnergyConsumptionByDevice, simDevEMCont);

    // -------- Enable PHY, MAC, RLC and PDCP level Key Performance Indicators (KPIs).
    // lteHelper->EnablePhyTraces(); // Uncomment this to enable LTE PHYSIC layer traces
    // lteHelper->EnableMacTraces(); // Uncomment this to enable LTE MAC layer traces
    lteHelper->EnableRlcTraces(); // Uncomment this to enable LTE RADIO LINK CONTROL layer traces
    // lteHelper->EnablePdcpTraces(); // Uncomment this to enable LTE PDCP layer traces

    Ptr<LteEnbNetDevice> enbDevice = enbLteDevs.Get(0)->GetObject<LteEnbNetDevice>();
    Simulator::Schedule(Seconds(sleepTime), &sleepSchedule, enbDevice);
    // uint64_t dlBandwidth = enbDevice->GetDlBandwidth();
    // uint64_t teste = enbDevice->GetRrc()->GetUeManager(2)->GetImsi();
    // Simulator::Schedule(Seconds(20), &simpleSchedule, teste);

    Simulator::Stop(Seconds(simuTime));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
