#include <channel.h>

__USING_SYS

OStream cout;

void sender(){
	UDP_Channel udp;
	UDP::Address dst(IP::BROADCAST, 12345);
	udp.send(dst, "Hello World!", 12);
	cout << "Sender says: Good Bye!\n";
}

void receiver(){
	UDP::Address me(12345), src;
	UDP_Channel udp(me);

	char buff[50];
	int msg_size = udp.receive(src, buff, 50);
	buff[msg_size] = 0;

	cout << "Received " << msg_size << " bytes from " << src
		<< "\nMessage: [" << buff << "]\n";
}

int main(){
//	sender();
	receiver();
}

