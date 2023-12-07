// Developed by: Ismael Ithalo, @ismaelithalo

#include <ns3/applications-module.h>
#include <ns3/core-module.h>
#include <ns3/energy-module.h>
#include <ns3/flow-monitor-module.h>
#include <ns3/internet-module.h>
#include <ns3/lte-module.h>
#include <ns3/mobility-building-info.h>
#include <ns3/mobility-module.h>
#include <ns3/network-module.h>
#include <ns3/point-to-point-module.h>

#include <algorithm>
#include <vector>

using namespace ns3;
using namespace std;

static double IDLE_A = 0.3;
static double TX_A = 1.38;
static double RX_A = 0.46;
static double SLEEP_A = 0.04;

vector<Ptr<SimpleDeviceEnergyModel>> simDevEMCont;
vector<Ptr<SimpleDeviceEnergyModel>> simDevEMCont2;
vector<uint32_t> sleepingStations;

ofstream outFileEnergy;
ofstream outFileQuality;

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
setCurrentDeviceVector(uint32_t id, double current, bool type)
{
    if (type)
    {
        for (uint32_t i = 0; i < simDevEMCont.size(); ++i)
        {
            Ptr<SimpleDeviceEnergyModel> device = simDevEMCont[i];
            if (device->GetNode()->GetId() == id)
            {
                device->SetCurrentA(current);
                break;
            }
        }
    }
    else
    {
        for (uint32_t i = 0; i < simDevEMCont2.size(); ++i)
        {
            Ptr<SimpleDeviceEnergyModel> device = simDevEMCont2[i];
            if (device->GetNode()->GetId() == id)
            {
                device->SetCurrentA(current);
                break;
            }
        }
    }
}

void
sleepSchedule(Ptr<LteEnbNetDevice> enbDevice)
{
    double currentPoweSleep = SLEEP_A;
    uint32_t id = enbDevice->GetNode()->GetId();
    uint16_t cid = enbDevice->GetCellId();

    setCurrentDeviceVector(id, currentPoweSleep, 1);
    setCurrentDeviceVector(id, currentPoweSleep, 0);

    enbDevice->GetPhy()->SetTxPower(0.0);
    double txPower = enbDevice->GetPhy()->GetTxPower();
    // sleepingStations.clear();
    sleepingStations.push_back(id);

    // enbDevice->GetPhy()->GetDlSpectrumPhy()->SetState(ns3::LteSpectrumPhy::State::IDLE);
    // enbDevice->GetPhy()->GetUlSpectrumPhy()->SetState(ns3::LteSpectrumPhy::State::IDLE);

    std::cout << "Sleep scheduled event for Cell " << cid << std::endl;
    std::cout << "TxPower of enb " << id << ": " << txPower << std::endl;
}

void
activeSchedule(Ptr<LteEnbNetDevice> enbDevice)
{
    double currentPoweSleep = IDLE_A;
    uint32_t id = enbDevice->GetNode()->GetId();
    uint16_t cid = enbDevice->GetCellId();

    setCurrentDeviceVector(id, currentPoweSleep, 1);
    setCurrentDeviceVector(id, currentPoweSleep, 0);

    enbDevice->GetPhy()->SetTxPower(20.0);
    double txPower = enbDevice->GetPhy()->GetTxPower();
    sleepingStations.erase(std::remove(sleepingStations.begin(), sleepingStations.end(), id),
                           sleepingStations.end());

    // enbDevice->GetPhy()->GetDlSpectrumPhy()->SetState(ns3::LteSpectrumPhy::State::IDLE);
    // enbDevice->GetPhy()->GetUlSpectrumPhy()->SetState(ns3::LteSpectrumPhy::State::IDLE);

    std::cout << "Active scheduled event for Cell " << cid << std::endl;
    std::cout << "TxPower of enb " << id << ": " << txPower << std::endl;
}

void
ChangeStateEventUlv2(string idOnContext, int32_t oldState, int32_t newState)
{
    uint32_t id = stoi(idOnContext);
    if (std::find(sleepingStations.begin(), sleepingStations.end(), id) == sleepingStations.end())
    {
        switch (newState)
        {
        case 0:
            setCurrentDeviceVector(id, IDLE_A, 1);
            break;
        case 1:
        case 2:
        case 3:
            setCurrentDeviceVector(id, TX_A, 1);
            break;
        case 4:
        case 5:
        case 6:
            setCurrentDeviceVector(id, RX_A, 1);
            break;

        default:
            break;
        }
    }

    // ChangeState(newState);
    // std::cout << Simulator::Now().GetSeconds() << " ESTADO DO UPLINK PARA O NÓ " << idOnContext
    //           << ": " << newState << std::endl;
}

void
ChangeStateEventDlv2(string idOnContext, int32_t oldState, int32_t newState)
{
    uint32_t id = stoi(idOnContext);
    if (std::find(sleepingStations.begin(), sleepingStations.end(), id) == sleepingStations.end())
    {
        switch (newState)
        {
        case 0:
            setCurrentDeviceVector(id, IDLE_A, 0);
            break;
        case 1:
        case 2:
        case 3:
            setCurrentDeviceVector(id, TX_A, 0);
            break;
        case 4:
        case 5:
        case 6:
            setCurrentDeviceVector(id, RX_A, 0);
            break;

        default:
            break;
        }
    }

    // ChangeState(newState);
    // std::cout << Simulator::Now().GetSeconds() << " ESTADO DO DOWNLINK PARA O NÓ " << idOnContext
    //           << ": " << newState << std::endl;
}

void
PrintRemainEnergy(double oldValue, double value)
{
    std::cout << "Remaining energy of source: " << value << std::endl;
}

void
TotalEnergyConsumptionByDevice(double simulationTime, string rootFilename)
{
    outFileEnergy.open(rootFilename + "energyfile.csv", std::ios_base::out | std::ios_base::trunc);
    outFileEnergy << "enbNode,cell,ulConsumption,dlConsumption" << std::endl;
    double totalEnergyConsumption = 0;
    for (uint32_t i = 0; i < simDevEMCont.size(); ++i)
    {
        Ptr<SimpleDeviceEnergyModel> device = simDevEMCont[i];
        totalEnergyConsumption += device->GetTotalEnergyConsumption();
        Ptr<SimpleDeviceEnergyModel> device2 = simDevEMCont2[i];
        totalEnergyConsumption += device2->GetTotalEnergyConsumption();

        Ptr<LteEnbNetDevice> enbDevice =
            DynamicCast<LteEnbNetDevice>(device->GetNode()->GetDevice(0));
        uint16_t cid = enbDevice->GetCellId();
        uint16_t nid = device->GetNode()->GetId();
        double ulConsumption = device->GetTotalEnergyConsumption() / simulationTime;
        double dlConsumption = device2->GetTotalEnergyConsumption() / simulationTime;

        std::cout << "Total energy consumption of node " << nid << " that serves cell " << cid
                  << ": "
                  << " UL = " << ulConsumption << " W &"
                  << " DL = " << dlConsumption << " W" << std::endl;

        outFileEnergy << nid << "," << cid << "," << ulConsumption << "," << dlConsumption
                      << std::endl;
    }

    std::cout << "Total energy consumption: " << totalEnergyConsumption / simulationTime << " W"
              << std::endl;
    outFileEnergy.close();
}

// void
// PrintUeMeasurements(
//     std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>> a,
//     uint16_t rnti,
//     uint16_t cellId,
//     double rsrp,
//     double rsrq,
//     bool attached,
//     u_char ccid)
// {
//     outFileQuality << Simulator::Now().GetSeconds() << "," << rnti << "," << cellId << ","
//                    << attached << "," << rsrp << "," << rsrq << std::endl;
//     // std::cout << Simulator::Now().GetSeconds() << " RNTI: " << rnti << " CellId: " << cellId
//     //           << " Attached: " << attached << " RSRP: " << rsrp << " RSRQ: " << rsrq <<
//     //           std::endl;
// }

void
MonitorManager(FlowMonitorHelper* fmhelper, Ptr<FlowMonitor> flowMon)
{
    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats();
    Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier>(fmhelper->GetClassifier());

    int counter = 0;
    double accThroughput = 0;
    double accLostPackets = 0;
    double accJitter = 0;
    double accMeanJitter = 0;

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin();
         stats != flowStats.end();
         ++stats)
    {
        Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow(stats->first);
        if (fiveTuple.sourceAddress == Ipv4Address("1.0.0.2") && fiveTuple.destinationPort == 1234)
        {
            accThroughput += (stats->second.rxBytes * 8.0 /
                              (stats->second.timeLastRxPacket.GetSeconds() -
                               stats->second.timeFirstTxPacket.GetSeconds()) /
                              1024);
            accJitter += (stats->second.jitterSum.GetSeconds());
            accLostPackets += (stats->second.lostPackets);
            accMeanJitter += (stats->second.jitterSum.GetSeconds() / (stats->second.rxPackets));

            counter++;
        }
    }

    // std::cout << Simulator::Now() << " Throughput: " << accThroughput / counter << " Kbps"
    //           << " Jitter: " << accJitter / counter << " s "
    //           << " Total Lost Packets: " << accLostPackets << " Value: " << accMeanJitter /
    //           counter
    //           << std::endl;

    outFileQuality << Simulator::Now().GetSeconds() << "," << accThroughput / counter << ","
                   << accJitter / counter << "," << accMeanJitter / counter << "," << accLostPackets
                   << std::endl;

    Simulator::Schedule(Seconds(0.2), &MonitorManager, fmhelper, flowMon);
    // flowMon->SerializeToXmlFile("MonitorOutput.xml", true, true);
}

int
main()
{
    // -------- Define some model params

    double simulationTime = 30; // s

    uint16_t numberOfUes = 40;
    uint16_t numberOfEnbs = 4;

    double enbDistance = 50;        // m -> (numberOfEnbs + 1) * enbDistance x (enbDistance * 2)
    double enbTxPowerDbm = 20;      // dBm
    string ueMobilitySpeed = "1.0"; // m
    string ueMobilityTime = "1s";   // s

    bool attachUe = true; // Attach UEs to eNBs, disable to evaluate zero-connectivity energy

    bool sleep = true;                        // Activate sleep mode
    double sleepMoments[] = {10, 35, 60, 85}; // Time that sleep mode will be activated
    int sleepCells[] = {0, 1, 2, 3};          // Select cells that will be in sleep mode
    bool sleepRotation = true;                // Activate sleep rotation

    double defaultCurrent = IDLE_A;                     // A
    double defaultSourceVoltage = 5;                    // V
    long int defaultSourceInitialEnergyJ = 10000000000; // J
    int defaultSourceUpdateInterval = 30;               // s

    bool handoverLogs = true;      // Activate handover logs
    bool remainEnergyLogs = false; // Activate remain source energy logs
    bool courseChangeLogs = false; // Activate ue course change logs
    bool totalEnergyLogs = false;  // Activate energy logs and create energy logs file
    bool qualityLogs = false;      // Create quality logs file
    bool modelLogs = true;         // Print model params

    // -------- Print defined model params

    if (modelLogs)
    {
        cout << endl << "Simulation Model Values: " << endl << endl;
        cout << "Nº of eNB(s): " << numberOfEnbs << endl;
        cout << "eNB distance: " << enbDistance << " m" << endl;
        cout << "eNB Tx Power: " << enbTxPowerDbm << " dBm" << endl;

        cout << "Nº of UE(s): " << numberOfUes << endl;
        cout << "UE Mobility Speed: " << ueMobilitySpeed << " m/s" << endl;

        cout << endl << "Simulation Configs: " << endl << endl;
        if (sleep)
        {
            cout << "Sleep Mode: "
                 << "True" << endl;
            cout << "Sleep Rotation: " << sleepRotation << endl;

            // cout << "Sleep Moment: " << sleepMoment << " s" << endl;
            cout << "Sleep Cells: ";
            for (int i : sleepCells)
            {
                cout << i << " ";
            }
            cout << endl;
            cout << "Sleep Moments: ";
            for (double i : sleepMoments)
            {
                cout << i << " ";
            }
            cout << endl;
        }
        else
        {
            cout << "Sleep Mode: "
                 << "False" << endl;
        }
        cout << "Simulation Time: " << simulationTime << " s" << endl;
        cout << "Handover Logs: " << handoverLogs << endl;
        cout << "Remain Energy Logs: " << remainEnergyLogs << endl;
        cout << "Course Change Logs: " << courseChangeLogs << endl;
        cout << "Total Energy Logs: " << totalEnergyLogs << endl;
    }

    // Config::SetDefault("ns3::RadioBearerStatsCalculator::EpochDuration", TimeValue(Seconds(2)));
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
                                  StringValue("ns3::UniformRandomVariable[Min=0|Max=40]"));

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
    if (courseChangeLogs)
        Config::Connect("/NodeList/*/$ns3::MobilityModel/CourseChange",
                        MakeCallback(&CourseChange));

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
    if (attachUe)
        lteHelper->Attach(ueLteDevs);

    Ptr<EpcTft> tft = Create<EpcTft>();
    EpcTft::PacketFilter pf;
    pf.localPortStart = 1234;
    pf.localPortEnd = 1234;
    tft->Add(pf);
    lteHelper->ActivateDedicatedEpsBearer(ueLteDevs,
                                          EpsBearer(EpsBearer::NGBR_VIDEO_TCP_DEFAULT),
                                          tft);

    // Add X2 interface
    lteHelper->AddX2Interface(enbNodes);

    // outFileQuality.open("qualityfile.csv", std::ios_base::out | std::ios_base::trunc);
    // outFileQuality << "time,rnti,cell,attached,rsrp,rsrq" << std::endl;
    // outFileQuality.close();

    // Config::Connect("/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/"
    //                 "LteUePhy/ReportUeMeasurements",
    //                 MakeCallback(&PrintUeMeasurements));

    if (handoverLogs)
        Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
                        MakeCallback(&NotifyHandoverEndOkEnb));

    // Configura regra de encaminhamento para eNBs
    for (uint32_t i = 0; i < enbNodes.GetN(); i++)
    {
        Ipv4StaticRoutingHelper ipv4RoutingHelper;
        Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(enbNodes.Get(i)->GetObject<Ipv4>());
        remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("10.0.0.6"),
                                                   Ipv4Mask("255.255.255.252"),
                                                   1);
    }

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
    }

    // Energy Framework
    Ptr<BasicEnergySource> basicSource = CreateObject<BasicEnergySource>();
    basicSource->SetSupplyVoltage(defaultSourceVoltage);
    basicSource->SetEnergyUpdateInterval(Seconds(defaultSourceUpdateInterval));
    basicSource->SetInitialEnergy(defaultSourceInitialEnergyJ);

    if (remainEnergyLogs)
        basicSource->TraceConnectWithoutContext("RemainingEnergy",
                                                MakeCallback(&PrintRemainEnergy));

    // DeviceEnergyModelContainer deviceEMCont; //For some reason, the container coverts the
    // SimpleDeviceEnergyModel to DeviceEnergyModel, so some atributtes are not available
    // Therefore, I created a vector of Ptr<SimpleDeviceEnergyModel> to store the energy models
    // vector<Ptr<SimpleDeviceEnergyModel>> simDevEMCont;

    for (uint32_t u = 0; u < enbNodes.GetN(); ++u)
    {
        Ptr<Node> enb = enbNodes.Get(u);
        basicSource->SetNode(enb);

        Ptr<SimpleDeviceEnergyModel> eneMod =
            CreateObject<SimpleDeviceEnergyModel>(); // One device for UL
        Ptr<SimpleDeviceEnergyModel> eneMod2 =
            CreateObject<SimpleDeviceEnergyModel>(); // Other device for DL

        eneMod->SetNode(enb);
        eneMod->SetEnergySource(basicSource);
        eneMod->SetCurrentA(defaultCurrent);

        basicSource->AppendDeviceEnergyModel(eneMod);
        simDevEMCont.push_back(eneMod);

        eneMod2->SetNode(enb);
        eneMod2->SetEnergySource(basicSource);
        eneMod2->SetCurrentA(defaultCurrent);

        basicSource->AppendDeviceEnergyModel(eneMod2);
        simDevEMCont2.push_back(eneMod2);
    }

    // -------- Enable PHY, MAC, RLC and PDCP level Key Performance Indicators (KPIs).
    // lteHelper->EnablePhyTraces(); // Uncomment this to enable LTE PHYSIC layer traces
    // lteHelper->EnableMacTraces(); // Uncomment this to enable LTE MAC layer traces
    // lteHelper->EnableRlcTraces(); // Uncomment this to enable LTE RADIO LINK CONTROL layer traces
    // lteHelper->EnablePdcpTraces(); // Uncomment this to enable LTE PDCP layer traces
    // Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats();
    // rlcStats->SetAttribute("EpochDuration", TimeValue(Seconds(1.0)));

    string rootFilename;
    if (sleep)
    {
        long unsigned int j = 0;

        for (int i : sleepCells)
        {
            Simulator::Schedule(Seconds(sleepMoments[j++]),
                                &sleepSchedule,
                                enbLteDevs.Get(i)->GetObject<LteEnbNetDevice>());

            if (sleepRotation && j < sizeof(sleepMoments) / sizeof(double))
            {
                if (j < sizeof(sleepCells) / sizeof(int))
                {
                    Simulator::Schedule(Seconds(sleepMoments[j]),
                                        &activeSchedule,
                                        enbLteDevs.Get(i)->GetObject<LteEnbNetDevice>());
                }
            }
        }

        if (sizeof(sleepCells) / sizeof(int) == 1)
            rootFilename =
                "outputs/UE" + to_string(numberOfUes) + "SM" + to_string(sleepCells[0]) + "-";
        else
            rootFilename = "outputs/UE" + to_string(numberOfUes) + "SMR-";
    }
    else
    {
        rootFilename = "outputs/UE" + to_string(numberOfUes) + "SMF-";
    }

    // flowMonitor declaration
    FlowMonitorHelper fmHelper;
    Ptr<FlowMonitor> flowMon = fmHelper.InstallAll();
    flowMon->CheckForLostPackets();

    if (qualityLogs)
    {
        outFileQuality.open(rootFilename + "qualityfile.csv",
                            std::ios_base::out | std::ios_base::trunc);
        outFileQuality << "time,throughput,accJitter,meanJitter,lostPackets" << std::endl;
        MonitorManager(&fmHelper, flowMon);
    }

    if (totalEnergyLogs)
        Simulator::Schedule(Seconds(simulationTime),
                            &TotalEnergyConsumptionByDevice,
                            simulationTime,
                            rootFilename);

    cout << endl << "Simulation started" << endl << endl;
    Simulator::Stop(Seconds(simulationTime));
    Simulator::Run();

    if (qualityLogs)
        outFileQuality.close();

    cout << endl << "Simulation finished." << endl;
    Simulator::Destroy();
    return 0;
}
