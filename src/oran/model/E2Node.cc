//
// Created by gabriel on 29/09/22.
//

#include "E2Node.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("E2Node");

enum ORAN_MESSAGE_TYPES
    {
        //RIC initiated
        RIC_SUBSCRIPTION_REQUEST,
        RIC_SUBSCRIPTION_DELETE_REQUEST,
        RIC_CONTROL_REQUEST,
        E2_SETUP_RESPONSE,
        E2_SETUP_FAILURE,
        RIC_SERVICE_QUERY,
        RIC_SERVICE_UPDATE_ACKNOWLEDGE,
        RIC_SERVICE_UPDATE_FAILURE,
        E2_NODE_CONFIGURATION_UPDATE_ACKNOWLEDGE,
        E2_NODE_CONFIGURATION_UPDATE_FAILURE,
        E2_CONNECTION_UPDATE_ACKNOWLEDGE,
        E2_CONNECTION_UPDATE_FAILURE,
        //E2 initiated
        RIC_SUBSCRIPTION_RESPONSE,
        RIC_SUBSCRIPTION_FAILURE,
        RIC_SUBSCRIPTION_DELETE_RESPONSE,
        RIC_SUBSCRIPTION_DELETE_FAILURE,
        RIC_SUBSCRIPTION_DELETE_REQUIRED,
        RIC_INDICATION,
        RIC_CONTROL_ACKNOWLEDGE,
        RIC_CONTROL_FAILURE,
        E2_SETUP_REQUEST,
        RIC_SERVICE_UPDATE,
        E2_NODE_CONFIGURATION_UPDATE,
        E2_CONNECTION_UPDATE,
        //RIC and E2 initiated
        RESET_REQUEST,
        RESET_RESPONSE,
        ERROR_INDICATION,
        E2_REMOVAL_REQUEST,
        E2_REMOVAL_RESPONSE,
        E2_REMOVAL_FAILURE,
    };

std::map<ORAN_MESSAGE_TYPES, std::string> oran_msg_str
    {
        {RIC_SUBSCRIPTION_REQUEST,                  "RIC_SUBSCRIPTION_REQUEST"},
        {RIC_SUBSCRIPTION_DELETE_REQUEST,           "RIC_SUBSCRIPTION_DELETE_REQUEST"},
        {RIC_CONTROL_REQUEST,                       "RIC_CONTROL_REQUEST"},
        {E2_SETUP_RESPONSE,                         "E2_SETUP_RESPONSE"},
        {E2_SETUP_FAILURE,                          "E2_SETUP_FAILURE"},
        {RIC_SERVICE_QUERY,                         "RIC_SERVICE_QUERY"},
        {RIC_SERVICE_UPDATE_ACKNOWLEDGE,            "RIC_SERVICE_UPDATE_ACKNOWLEDGE"},
        {RIC_SERVICE_UPDATE_FAILURE,                "RIC_SERVICE_UPDATE_FAILURE"},
        {E2_NODE_CONFIGURATION_UPDATE_ACKNOWLEDGE,  "E2_NODE_CONFIGURATION_UPDATE_ACKNOWLEDGE"},
        {E2_NODE_CONFIGURATION_UPDATE_FAILURE,      "E2_NODE_CONFIGURATION_UPDATE_FAILURE"},
        {E2_CONNECTION_UPDATE_ACKNOWLEDGE,          "E2_CONNECTION_UPDATE_ACKNOWLEDGE"},
        {E2_CONNECTION_UPDATE_FAILURE,              "E2_CONNECTION_UPDATE_FAILURE"},
        {RIC_SUBSCRIPTION_RESPONSE,                 "RIC_SUBSCRIPTION_RESPONSE"},
        {RIC_SUBSCRIPTION_FAILURE,                  "RIC_SUBSCRIPTION_FAILURE"},
        {RIC_SUBSCRIPTION_DELETE_RESPONSE,          "RIC_SUBSCRIPTION_DELETE_RESPONSE"},
        {RIC_SUBSCRIPTION_DELETE_FAILURE,           "RIC_SUBSCRIPTION_DELETE_FAILURE"},
        {RIC_SUBSCRIPTION_DELETE_REQUIRED,          "RIC_SUBSCRIPTION_DELETE_REQUIRED"},
        {RIC_INDICATION,                            "RIC_INDICATION"},
        {RIC_CONTROL_ACKNOWLEDGE,                   "RIC_CONTROL_ACKNOWLEDGE"},
        {RIC_CONTROL_FAILURE,                       "RIC_CONTROL_FAILURE"},
        {E2_SETUP_REQUEST,                          "E2_SETUP_REQUEST"},
        {RIC_SERVICE_UPDATE,                        "RIC_SERVICE_UPDATE"},
        {E2_NODE_CONFIGURATION_UPDATE,              "E2_NODE_CONFIGURATION_UPDATE"},
        {E2_CONNECTION_UPDATE,                      "E2_CONNECTION_UPDATE"},
        {RESET_REQUEST,                             "RESET_REQUEST"},
        {RESET_RESPONSE,                            "RESET_RESPONSE"},
        {ERROR_INDICATION,                          "ERROR_INDICATION"},
        {E2_REMOVAL_REQUEST,                        "E2_REMOVAL_REQUEST"},
        {E2_REMOVAL_RESPONSE,                       "E2_REMOVAL_RESPONSE"},
        {E2_REMOVAL_FAILURE,                        "E2_REMOVAL_FAILURE"},
    };

void
E2Node::HandlePayload(std::string endpoint, Json payload)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (payload.contains("TYPE"), "Payload addressed to " + endpoint + "does not contain a message type.");
  ORAN_MESSAGE_TYPES msg_type = payload.find("TYPE").value();

  // Check if we are not receiving invalid payloads
  if(m_endpointRoot == "/E2Node/0")
    {
      // E2T can't receive messages that should have originated on the RIC (himself)
      if(msg_type < RIC_SUBSCRIPTION_RESPONSE )
        {
           std::stringstream ss;
           ss << "E2T received message addressed to himself. Type=" << oran_msg_str.at(msg_type);
           NS_ASSERT(ss.str().c_str());
        }
    }
  else
    {
      // E2 nodes can't receive messages that should have originated on E2 Nodes (himselves)
      if(RIC_SUBSCRIPTION_RESPONSE <= msg_type && msg_type <= E2_NODE_CONFIGURATION_UPDATE )
        {
          std::stringstream ss;
          ss << m_endpointRoot <<" received message addressed to himself. Type=" << oran_msg_str[msg_type];
          NS_ASSERT(ss.str().c_str());
        }
    }

  NS_LOG_FUNCTION("Handling payload" + to_string (payload));

  // Handle all O-RAN messages
  switch(msg_type)
    {
      // O-RAN WG3 E2AP v2.02 8.2.1.2
      // RIC initiated
      case RIC_SUBSCRIPTION_REQUEST:
        {
        }
        break;
      // O-RAN WG3 E2AP v2.02 8.2.1.2
      // E2 initiated
      case RIC_SUBSCRIPTION_RESPONSE:
        {
        }
        break;
      // O-RAN WG3 E2AP v2.02 8.2.1.3
      // E2 initiated
      case RIC_SUBSCRIPTION_FAILURE:
        {
        }
        break;
      // O-RAN WG3 E2AP v2.02 8.2.2.2
      // RIC initiated
      case RIC_SUBSCRIPTION_DELETE_REQUEST:
        {
        }
        break;
      // O-RAN WG3 E2AP v2.02 8.2.2.2
      // E2 initiated
      case RIC_SUBSCRIPTION_DELETE_RESPONSE:
        {
        }
        break;
      // O-RAN WG3 E2AP v2.02 8.2.2.3
      // E2 initiated
      case RIC_SUBSCRIPTION_DELETE_FAILURE:
        {
        }
        break;
      // O-RAN WG3 E2AP v2.02 8.2.2A.2
      // E2 initiated
      case RIC_SUBSCRIPTION_DELETE_REQUIRED:
        {
        }
        break;
      // O-RAN WG3 E2AP v2.02 8.2.3.2
      // E2 initiated
      case RIC_INDICATION:
        {
        }
        break;
      // O-RAN WG3 E2AP v2.02 8.2.4.2
      // RIC initiated
      case RIC_CONTROL_REQUEST:
        {
        }
        break;
      // O-RAN WG3 E2AP v2.02 8.2.4.2
      // E2 initiated
      case RIC_CONTROL_ACKNOWLEDGE:
        {
        }
        break;
      // O-RAN WG3 E2AP v2.02 8.2.4.3
      // E2 initiated
      case RIC_CONTROL_FAILURE:
        {
        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.1.2
      // E2 initiated
      case E2_SETUP_REQUEST:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.1.2
      // RIC initiated
      case E2_SETUP_RESPONSE:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.1.3
      // RIC initiated
      case E2_SETUP_FAILURE:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.2.2
      // RIC or E2 initiated
      case RESET_REQUEST:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.2.3
      // RIC or E2 initiated
      case RESET_RESPONSE:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.3.2
      // RIC or E2 initiated
      case ERROR_INDICATION:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.4.2
      // RIC initiated
      case RIC_SERVICE_QUERY:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.4.2
      // E2 initiated
      case RIC_SERVICE_UPDATE:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.4.2
      // RIC initiated
      case RIC_SERVICE_UPDATE_ACKNOWLEDGE:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.4.3
      // RIC initiated
        case RIC_SERVICE_UPDATE_FAILURE:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.5.2
      // E2 initiated
      case E2_NODE_CONFIGURATION_UPDATE:
        {
          bool successful = true;
          if(payload.contains("COMPONENT_CONFIGURATION_ADDITION_LIST"))
            {
              for(std::string new_endpoint: payload.find("COMPONENT_CONFIGURATION_ADDITION_LIST").value())
                {
                  successful &= sRegisterEndpoint (endpoint, new_endpoint);
                }
            }
          if(payload.contains("COMPONENT_CONFIGURATION_UPDATE_LIST"))
            {
              std::cout<< " boop boop" << std::endl;

            }
          if(payload.contains("COMPONENT_CONFIGURATION_REMOVAL_LIST"))
            {
              std::cout<< " boop boop" << std::endl;

            }
          if(successful)
            {
              //todo: send acknowledge
            }
          else
            {
              //todo: send failure
            }
        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.5.2
      // RIC initiated
      case E2_NODE_CONFIGURATION_UPDATE_ACKNOWLEDGE:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.5.3
      // RIC initiated
      case E2_NODE_CONFIGURATION_UPDATE_FAILURE:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.6.2
      // RIC initiated
      case E2_CONNECTION_UPDATE:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.6.2
      // E2 initiated
      case E2_CONNECTION_UPDATE_ACKNOWLEDGE:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.6.3
      // E2 initiated
      case E2_CONNECTION_UPDATE_FAILURE:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.7.2
      // RIC or E2 initiated
      case E2_REMOVAL_REQUEST:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.7.2
      // RIC or E2 initiated
      case E2_REMOVAL_RESPONSE:
        {

        }
        break;
      // O-RAN WG3 E2AP v2.02 8.3.7.3
      // RIC or E2 initiated
      case E2_REMOVAL_FAILURE:
        {

        }
        break;
    default:
      std::cout << this->m_endpointRoot << " failed to handle payload with RPC=" << payload["RPC"] << std::endl;
    }
}

void E2Node::RegisterEndpoint(std::string endpoint)
{
  NS_LOG_FUNCTION (this);
  Json E2_NODE_CONFIGURATION_UPDATE_MESSAGE;
  E2_NODE_CONFIGURATION_UPDATE_MESSAGE["ENDPOINT"] = m_endpointRoot;
  E2_NODE_CONFIGURATION_UPDATE_MESSAGE["PAYLOAD"]["TYPE"] = E2_NODE_CONFIGURATION_UPDATE;
  E2_NODE_CONFIGURATION_UPDATE_MESSAGE["PAYLOAD"]["COMPONENT_CONFIGURATION_ADDITION_LIST"] = std::vector<std::string>{endpoint};
  //E2_NODE_CONFIGURATION_UPDATE_MESSAGE["PAYLOAD"]["COMPONENT_CONFIGURATION_UPDATE_LIST"] = {};
  //E2_NODE_CONFIGURATION_UPDATE_MESSAGE["PAYLOAD"]["COMPONENT_CONFIGURATION_REMOVAL_LIST"] = {};
  //std::cout << to_string(E2_NODE_CONFIGURATION_UPDATE_MESSAGE) << std::endl;
  if (m_endpointRoot != "/E2Node/0")
    {
      m_socket->SendTo (encodeJsonToPacket (E2_NODE_CONFIGURATION_UPDATE_MESSAGE), 0, m_node0Address);
    }
  else
    {
      HandlePayload (m_endpointRoot, E2_NODE_CONFIGURATION_UPDATE_MESSAGE.at ("PAYLOAD"));
    }
}