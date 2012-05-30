#include <machine.h>
#include <mutex.h>
#include <neighborhood.h>
#include <network.h>
#include <utility/observer.h>

#define SINK true
#define SENSOR false

__USING_SYS


class LinkApp: Conditional_Observer
{
public:
    LinkApp(bool is_sink)
    {
        _id = Traits<CMAC<Radio_Wrapper> >::ADDRESS;
        net = new Network();
        net->protocol(PROT);
        net->attach(this,PROT);
        if (is_sink) {
            led_green();
            sink();
        } else {
            led_red();
            sensor();
        }
    }

    ~LinkApp()
    {
        delete net;
    }

    enum {
        PROT = 21
    };

private:
    void sink()
    {
        while (true) {
            Alarm::delay(5000000);

            char msg[100];

            unsigned int size =
                Neighborhood::get_instance()->neighborhood(msg, 100);

            if (size == 0)
                continue;

            mut_display.lock();
            cout << "!" << (int) _id << "|" << (int) size << "|";
            for (int i = 0; i < (int) size; i++) {
                cout << (int) msg[i] << "|";
            }
            cout << "\n";
            mut_display.unlock();
        }
    }

    void update(Conditionally_Observed * o, int p)
    {
        Network::Address from;

        int size = net->receive(&from, msg, sizeof(msg));

        if (size == 0)
            return;

        mut_display.lock();
        cout << "!" << (int) from.id() << "|" << (int) size << "|";
        for (int i = 0; i < size; i++) {
            cout << (int) msg[i] << "|";
        }
        cout << "\n";
        mut_display.unlock();
    }

    void sensor()
    {
        Network::Address sink(10,0,1,15);

        for (unsigned int i = 0; i < 100; i++) {
            msg[i] = i;
        }

        while (true) {
            unsigned int size =
                Neighborhood::get_instance()->neighborhood(msg, 100);

            net->send(sink, msg, size);

            cout << "sent: ";

            mut_display.lock();
            cout << (int) _id << "|" << (int) size << "|";
            for (int i = 0; i < (int) size; i++) {
                cout << (int) msg[i] << "|";
            }
            cout << "\n";
            mut_display.unlock();

            Alarm::delay(5000000);
        }
    }

    void led_red() {
        unsigned int *GPIO_BASE = (unsigned int*)0x80000000;
        *GPIO_BASE = 1 << 23;
    }

    void led_green() {
        unsigned int *GPIO_BASE = (unsigned int*)0x80000000;
        *GPIO_BASE = 1 << 24;
    }

private:
    char msg[100];
    unsigned int _id;

    OStream cout;
    Mutex mut_display;
    Network * net;

};

int main() {
    LinkApp app(SINK);
    //LinkApp app(SENSOR);

    return 0;
}

