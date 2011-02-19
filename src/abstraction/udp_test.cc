#include <ip/dhcp.h>
#include <alarm.h>


__USING_SYS

OStream cout;

int main()
{
    IP ip(0);

    ip.set_address(IP::Address(0,0,0,0));
    ip.set_gateway(IP::Address(0,0,0,0));
    ip.set_netmask(IP::Address(0,0,0,0));

    UDP udp(&ip);
    DHCP::Client dhcpc(&udp);
    dhcpc.configure();

    Alarm::delay(5000000);

    cout << "IP Address: " << ip.address() << endl;
}
