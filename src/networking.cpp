#include "networking.h"

//constructor - does nothing
Networking::Networking(void) {
    }

bool Networking::init(void) {
    Serial.println("init wired network");
    initwired();
    Serial.println("init wifi network");
    initwifi();
    return true;
    }

bool Networking::initwired(void) {
    //if interface is disabled, break here and return true
    if (_network_data.interfacestatewired == OFF) 
        return true;

    //continue if interface is DHCP or STATIC
    _chipid = ESP.getEfuseMac();                  //The chip ID is essentially its MAC address(length: 6 bytes).
    Serial.printf("[INFO] ESP32 Chip ID = %04X",(uint16_t)(_chipid>>32));//print High 2 bytes
    Serial.printf("%08X\n",(uint32_t)_chipid);//print Low 4bytes.
  
    WiFi.onEvent(WiFiEvent);

    //if interface is DHCP
    if (_network_data.interfacestatewired == DHCP) {
        if (!ETH.begin())
            return false;
        else
            Serial.println("[INFO] wired network initialized (dhcp)");
        }
    
    //static ip configuration
    if (_network_data.interfacestatewired == STATIC) {
        //IPAddress local_IP(myStorage.network_ip[0],myStorage.network_ip[1],myStorage.network_ip[2],myStorage.network_ip[3]);
        //IPAddress subnet(myStorage.network_mask[0],myStorage.network_mask[1],myStorage.network_mask[2],myStorage.network_mask[3]);
        //IPAddress gateway(myStorage.network_gateway[0],myStorage.network_gateway[1],myStorage.network_gateway[2],myStorage.network_gateway[3]);
        //IPAddress primaryDNS(myStorage.network_dns1[0],myStorage.network_dns1[1],myStorage.network_dns1[2],myStorage.network_dns1[3]);
        //IPAddress secondaryDNS(myStorage.network_dns2[0],myStorage.network_dns2[1],myStorage.network_dns2[2],myStorage.network_dns2[3]);
        
        if (!ETH.begin())
            return false;
        else
            Serial.println("[INFO] wired network initialized (static ip)");
        //https://github.com/espressif/arduino-esp32/issues/1762
        ETH.config(_network_data.wired_ip, _network_data.wired_gateway, _network_data.wired_subnet, _network_data.wired_dns1, _network_data.wired_dns2);
        }

    return true;
    }

bool Networking::initwifi(void) {
    //if interface is disabled, break here and return true
    if (_network_data.interfacestatewifi == OFF) 
        return true;

    //wifi used as access point
    if (_network_data.wifistate == AP) {
        Serial.print("[INFO] AP - opening AP...");
        //IPAddress local_IP(myStorage.network_ip[0],myStorage.network_ip[1],myStorage.network_ip[2],myStorage.network_ip[3]);
        //IPAddress subnet(myStorage.network_mask[0],myStorage.network_mask[1],myStorage.network_mask[2],myStorage.network_mask[3]);
        //IPAddress gateway(myStorage.network_gateway[0],myStorage.network_gateway[1],myStorage.network_gateway[2],myStorage.network_gateway[3]);
        Serial.println(_network_data.wifi_subnet);
        WiFi.softAPConfig(_network_data.wifi_ip, _network_data.wifi_gateway, _network_data.wifi_subnet);
        Serial.println(WiFi.softAP(_network_data.network_ssid, _network_data.network_password) ? "[INFO] AP Ready" : "[ERROR] AP Failed!");
        Serial.println("[INFO] access point SSID: "+String(_network_data.network_ssid));
        Serial.print("[INFO] access point IP address: ");
        Serial.println(WiFi.softAPIP());
        }

    //wifi used as client
    if (_network_data.wifistate == CLIENT) {
        
        //ip configuration static
        if (_network_data.interfacestatewifi == STATIC) {
            Serial.println("[INFO] connecting to wireless network with static ip");
            //IPAddress local_IP(myStorage.network_ip[0],myStorage.network_ip[1],myStorage.network_ip[2],myStorage.network_ip[3]);
            //IPAddress subnet(myStorage.network_mask[0],myStorage.network_mask[1],myStorage.network_mask[2],myStorage.network_mask[3]);
            //IPAddress gateway(myStorage.network_gateway[0],myStorage.network_gateway[1],myStorage.network_gateway[2],myStorage.network_gateway[3]);
            //IPAddress primaryDNS(myStorage.network_dns1[0],myStorage.network_dns1[1],myStorage.network_dns1[2],myStorage.network_dns1[3]);
            //IPAddress secondaryDNS(myStorage.network_dns2[0],myStorage.network_dns2[1],myStorage.network_dns2[2],myStorage.network_dns2[3]);
            if (!WiFi.config(_network_data.wifi_ip, _network_data.wifi_gateway, _network_data.wifi_subnet, _network_data.wifi_dns1, _network_data.wifi_dns2)) { 
                Serial.println("STA failed to configure");
                return false;
                }
            }    

        //start wireless network client
        //if it fails, open an AP with SSID: BMS, Password: no password, IP: 192.168.4.1
        WiFi.mode(WIFI_STA);
        WiFi.begin(_network_data.network_ssid, _network_data.network_password);
        if (WiFi.waitForConnectResult() != WL_CONNECTED) {
            Serial.println("[ERROR] WiFi failed!");
            //open AP with predefined values
            IPAddress ip(192,168,4,1);
            IPAddress gateway(192,168,4,254);
            IPAddress subnet(255,255,255,0);
            Serial.print("[INFO] Emergency AP Opening AP...");
            WiFi.softAPConfig(ip, gateway, subnet);
            Serial.println(WiFi.softAP("BMS") ? "[INFO] AP ready" : "[ERROR] AP failed!");
            Serial.print("[INFO] soft-AP IP address: ");
            Serial.println(WiFi.softAPIP());
            }
        else {
            Serial.print("[INFO] IP Address: ");
            Serial.println(WiFi.localIP());
            }

        }    


    return true;
    }

void Networking::WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
        case SYSTEM_EVENT_ETH_START:
            Serial.println("[INFO] ethernet started");
            ETH.setHostname("Zweistein");                    //set eth hostname here
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            Serial.println("[INFO] ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            Serial.print("[INFO] ethernet MAC: ");
            Serial.print(ETH.macAddress());
            Serial.print(", IPv4: ");
            Serial.print(ETH.localIP());
            if (ETH.fullDuplex()) {
                Serial.print(", FULL_DUPLEX");
                }
            Serial.print(", ");
            Serial.print(ETH.linkSpeed());
            Serial.println("Mbps");
            //eth_connected = true;
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            Serial.println("[INFO] ethernet disconnected");
            //eth_connected = false;
            break;
        case SYSTEM_EVENT_ETH_STOP:
            Serial.println("[INFO] ethernet stopped");
            //eth_connected = false;
            break;
        default:
            break;
        }
    }