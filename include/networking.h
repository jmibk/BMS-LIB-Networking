#ifndef NETWORKING_H
#define NETWORKING_H
#include <Arduino.h>
#include <WiFiClient.h>
#include <ETH.h>
#include <WiFi.h>

//#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
//#define ETH_PHY_POWER 12

//WiFiClient espClient;

enum interfaceState {
    OFF,
    DHCP,
    STATIC
    };

enum wifiState {
    AP,
    CLIENT
    };

enum localInterface {
    WIRED,
    WIFI
    };

enum addressType {
    IP,
    GATEWAY,
    SUBNET,
    DNS1,
    DNS2
    };

//bool eth_connected = false;

class Networking {
    public:
        Networking(void);
        bool init(void);
        bool initwired(void);
        bool initwifi(void);

    private:
        uint64_t _chipid;
        //https://forum.arduino.cc/t/trying-to-insert-a-wifievent-callback-inside-my-class/692048/6
        static void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);

    struct network_data_struct { 
        interfaceState interfacestatewired = DHCP;
        interfaceState interfacestatewifi = DHCP;
        wifiState wifistate = CLIENT;

        //wired
        IPAddress wired_ip = (192, 168, 4, 1);
        IPAddress wired_subnet = (255, 255, 255, 0);
        IPAddress wired_gateway = (192,168,4,254);
        IPAddress wired_dns1 = (192,168,4,254);
        IPAddress wired_dns2 = (8,8,8,8);

        //wifi
        IPAddress wifi_ip = (192,168,4,1);
        IPAddress wifi_subnet = (255,255,255,0);
        IPAddress wifi_gateway = (192,168,4,254);
        IPAddress wifi_dns1 = (192,168,4,254);
        IPAddress wifi_dns2 = (8,8,8,8);
        char network_ssid[30] = "SmartHome";                            //wifi ssid as char array
        char network_password[30] = "Affe123456";                       //wifi password as char array

        };
    network_data_struct _network_data;

    };
#endif