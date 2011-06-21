#include <tcp.h>
#include <utility/string.h>
#include <mutex.h>

__USING_SYS

OStream cout;

class HTTPServer : public TCP::ServerSocket {
public:
    HTTPServer() : TCP::ServerSocket(TCP::Address(tcp()->ip()->address(),80)) {}

    TCP::Socket* incoming(const TCP::Address& from) {
        // we can clone here to accept multiple connections
        // or just return itself
        return this;
    }

    void connected() {
        cout << "Connection from " << remote() << endl;
    }

    void closed() {
        cout << "Disconnected from " << remote() << endl;
        
        listen();
    }

    void error(short err) {
        cout << "Connection error" << endl;
    }

    void sent(u16 size) {} 

    void received(const char *data,u16 size) {
        cout << "Received "<<size<<" bytes: " << data << endl;
        
        const char * msg = "200 HTTP 1.1\r\n\r\nHello world!\r\n";
        send(msg, 30);
        close();
    }
}; 

class WebClient : public TCP::ClientSocket {
public:
    WebClient() : 
        TCP::ClientSocket(
                TCP::Address("74.125.234.84:80"),
                TCP::Address(tcp()->ip()->address(),55000 + Pseudo_Random::random() % 10000)) 
    {
        m.lock(); 
    }

    void connected() {
        cout << "Connected to " << remote() << endl;
        send("GET / HTTP/1.1\n\rHost: www.google.com\n\r\n\r",40);
        close();
    }

    void closing() {
        close();
    }

    void closed() {
        cout << "Disconnected from " << remote() << endl;
        m.unlock();    
    }

    void error(short err) {
        cout << "Connection error" << endl;
        m.unlock();
    }

    void sent(u16 size) {
        cout << "Bytes sent: " << size << endl;
    } 

    void received(const char *data,u16 size) {
        cout << "Received "<<size<<" bytes: " << endl;
        int p;
        for(p=0;p<size;p++)
            cout << *data++;
    }

    void wait() { m.lock(); }
protected:
    Mutex m;
};

int main()
{
    IP * ip = IP::instance();

    ip->set_address(IP::Address(10,0,2,15));
    ip->set_gateway(IP::Address(10,0,2,2));
    ip->set_netmask(IP::Address(255,255,255,0));

    //HTTPServer httpd(&tcp);
    //Thread::self()->suspend();

    WebClient web;

    web.wait();
    delete ip; // kill IP thread
}
