#include <ns3/applications-module.h>
#include <ns3/core-module.h>
#include <ns3/internet-module.h>
#include <ns3/lte-module.h>
#include <ns3/mobility-building-info.h>
#include <ns3/mobility-module.h>
#include <ns3/network-module.h>
#include <ns3/point-to-point-module.h>

using namespace ns3;
using namespace std;

// -------- Function called when there is a course change
static void
CourseChange(std::string context, Ptr<const MobilityModel> mobility)
{
    Vector pos = mobility->GetPosition();
    Vector vel = mobility->GetVelocity();
    std::cout << Simulator::Now() << ", model=" << mobility << ", POS: x=" << pos.x
              << ", y=" << pos.y << "; VEL:" << vel.x << ", y=" << vel.y << ";" << std::endl;
}

int
main()
{
    // -------- Define some model params

    uint16_t numberOfUes = 5;
    uint16_t numberOfEnbs = 4;
    double enbDistance =
        75; // m -> The size of environment is defined by enbDistance and numberOfEnbs
    double enbTxPowerDbm = 46.0;
    string ueMobilitySpeed = "1.0"; // m
    string ueMobilityTime = "2s";   // s

    // Config::SetDefault("ns3::RadioBearerStatsCalculator::EpochDuration",TimeValue(Seconds(2)));
    // Config::SetDefault("ns3::RandomWalk2dMobilityModel::Mode",StringValue("Time"));
    // Config::SetDefault("ns3::RandomWalk2dMobilityModel::Time",StringValue("2s"));
    // Config::SetDefault("ns3::RandomWalk2dMobilityModel::Speed",StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    // Config::SetDefault("ns3::RandomWalk2dMobilityModel::Bounds", StringValue("0|50|0|50"));

    // -------- Create an LteHelper object
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);

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
    // interface 0 is localhost, 1 is the p2p device
    // Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress(1);

    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting;
    remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    // remoteHostStaticRouting->AddNetworkRouteTo(epcHelper->GetEpcIpv4NetworkAddress(),
    //                                            Ipv4Mask("255.255.0.0"),
    //                                            1);
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
                                  StringValue("ns3::UniformRandomVariable[Min=0|Max=200]"));

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
    Config::Connect("/NodeList/*/$ns3::MobilityModel/CourseChange", MakeCallback(&CourseChange));

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

    // Attach all UEs to the first eNodeB
    // for (uint16_t i = 0; i < numberOfUes; i++)
    // {
    //     lteHelper->Attach(ueLteDevs.Get(i), enbLteDevs.Get(0));
    // }
    lteHelper->Attach(ueLteDevs);

    Ptr<EpcTft> tft = Create<EpcTft>();
    EpcTft::PacketFilter pf;
    pf.localPortStart = 1234;
    pf.localPortEnd = 1234;
    tft->Add(pf);
    lteHelper->ActivateDedicatedEpsBearer(ueLteDevs,
                                          EpsBearer(EpsBearer::NGBR_VIDEO_TCP_DEFAULT),
                                          tft);

    // NetDeviceContainer enbDevs;
    // enbDevs = lteHelper->InstallEnbDevice(enbNodes);

    // NetDeviceContainer ueDevs;
    // ueDevs = lteHelper->InstallUeDevice(ueNodes);

    // lteHelper->Attach(ueDevs.Get(0), enbDevs.Get(0));
    // lteHelper->Attach(ueDevs.Get(1), enbDevs.Get(1));

    // -------- Activate a data radio bearer between each UE and the eNB it is attached to
    // enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    // EpsBearer bearer(q);
    // lteHelper->ActivateDataRadioBearer(ueDevs, bearer);

    // -------- Enable PHY, MAC, RLC and PDCP level Key Performance Indicators (KPIs).
    // lteHelper->EnablePhyTraces(); // Uncomment this to enable LTE PHYSIC layer traces
    // lteHelper->EnableMacTraces(); // Uncomment this to enable LTE MAC layer traces
    lteHelper->EnableRlcTraces(); // Uncomment this to enable LTE RADIO LINK CONTROL layer traces
    // lteHelper->EnablePdcpTraces(); // Uncomment this to enable LTE PDCP layer traces

    Simulator::Stop(Seconds(10));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
