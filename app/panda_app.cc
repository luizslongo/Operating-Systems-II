// Dummy application for the PandaBoard platform

//#include <utility/ostream.h>
#include <alarm.h>

__USING_SYS

OStream cout;

int main() {
	int x = sizeof(unsigned long);
	int y = sizeof(unsigned long long);
	/*
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("nop\n");
	ASMV("ldr	r1,=0xabc\n");
	ASMV("ldr	r2,=0xbcd\n");
	if(x==4)
		ASMV("ldr	r1,=0x4\n");
	else if(x==8)
		ASMV("ldr	r1,=0x8\n");
	if(y==4)
		ASMV("ldr	r2,=0x4\n");
	else if(y==8)
		ASMV("ldr	r2,=0x8\n");
	//ASMV("ldr	r1, %[l]\n" : : [l] "r" (x));
	//ASMV("ldr	r2, %[ll]\n" : : [ll] "r" (y));
	ASMV("ldr	r3,=0xcde\n");
	ASMV("ldr	r4,=0xdef\n");
	ASMV("ldr	r5,=0xefa\n");
	ASMV("ldr	r6,=0xfab\n");
	ASMV("ldr	r7,=0xabc\n");
	ASMV("ldr	r8,=0xbcd\n");
	//ASMV("ldr	r9,=0xcde\n");
*/
    //cout << "Hello world from ARM Cortex-A9 PandaBoard !\n";
	/*
    
    CPU::int_enable();
    
    void * gpio1_base = (void*)(0x4a310000);
    void * gpio1_oe;
    gpio1_oe = (gpio1_base + 0x0134);
    void * gpio1_dataout;
    //gpio1_dataout = (gpio1_base + 0x013C);
    gpio1_dataout = (gpio1_base + 0x013C);

    //*((unsigned *) gpio1_oe) = (*(unsigned *)gpio1_oe) & ~(1 << 7);
    *((unsigned *) gpio1_oe) = 0xFFFFFE7F; //enable GPIO PINs 7 and 8

    int sum1 = 0;
    int sum2 = 0;
    
    for(int l = 0; l < 5; l++) {
        cout << "ON\n";
	    //*((unsigned *) gpio1_dataout) = (*(unsigned *) gpio1_dataout) | (1 << 7);
	    *((unsigned *) gpio1_dataout) = ((*(unsigned *) gpio1_dataout) | 0x180);
	    //*((unsigned *) gpio1_oe) = 0xFFFFFE7F;
	     
	    //for(unsigned int j = 0; j < 0xffffffff; j++) 
	    //    for(unsigned int i = 0; i < 0xff; i++)
	    //            sum1 = j + i;
	    
	    Alarm::delay(100000);
	                
	    //*(unsigned *) gpio1_dataout = (*(unsigned *) gpio1_dataout) & ~(1 << 7);
	    *(unsigned *) gpio1_dataout = ((*(unsigned *) gpio1_dataout) & ~(0x180));
	    //*((unsigned *) gpio1_oe) = 0xFFFFFE7F;
	    
	    cout << "OFF" << endl;
	    
	    //for(unsigned int j = 0; j < 0xffffffff; j++) 
	    //    for(unsigned int i = 0; i < 0xff; i++)
	    //            sum2 = j + i;
	    
	    Alarm::delay(100000);
    }
    
    cout << "sum1 = " << sum1 << " sum2 = " << sum2<< "\n";
    
    while(1) ;

    
    return 0;
	*/
	while(1);
}
