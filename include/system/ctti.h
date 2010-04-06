// EPOS-- Compile-time Type Information

#ifndef __ctti_h
#define __ctti_h

#include <system/types.h>

__BEGIN_SYS

// Type -> Id
template<typename T>struct Type2Id	{ enum { ID = UNKNOWN_TYPE_ID }; };

template<> struct Type2Id<CPU>		{ enum { ID = CPU_ID }; };
template<> struct Type2Id<TSC>		{ enum { ID = TSC_ID }; };
template<> struct Type2Id<MMU>		{ enum { ID = MMU_ID }; };


template<> struct Type2Id<Machine>	{ enum { ID = MACHINE_ID }; };
template<> struct Type2Id<PCI>		{ enum { ID = PCI_ID }; };
template<> struct Type2Id<IC>		{ enum { ID = IC_ID }; };
template<> struct Type2Id<Timer>	{ enum { ID = TIMER_ID }; };
template<> struct Type2Id<RTC>		{ enum { ID = RTC_ID }; };
template<> struct Type2Id<EEPROM>	{ enum { ID = EEPROM_ID }; };
template<> struct Type2Id<Flash>	{ enum { ID = FLASH_ID }; };
template<> struct Type2Id<UART>		{ enum { ID = UART_ID }; };
template<> struct Type2Id<SPI>		{ enum { ID = SPI_ID }; };
template<> struct Type2Id<Display>	{ enum { ID = DISPLAY_ID }; };
template<> struct Type2Id<NIC>		{ enum { ID = NIC_ID }; };
template<> struct Type2Id<ADC>		{ enum { ID = ADC_ID }; };
template<> struct Type2Id<Temperature_Sensor>{ enum { ID = TEMPERATURE_SENSOR_ID }; };
template<> struct Type2Id<Photo_Sensor>	{ enum { ID = PHOTO_SENSOR_ID }; };


template<> struct Type2Id<Thread>	{ enum { ID = THREAD_ID }; };
template<> struct Type2Id<Task>		{ enum { ID = TASK_ID }; };
template<> struct Type2Id<Active>	{ enum { ID = ACTIVE_ID }; };

template<> struct Type2Id<Segment>	{ enum { ID = SEGMENT_ID }; };
template<> struct Type2Id<Address_Space>{ enum { ID = ADDRESS_SPACE_ID }; };

template<> struct Type2Id<Mutex>	{ enum { ID = MUTEX_ID }; };
template<> struct Type2Id<Semaphore>	{ enum { ID = SEMAPHORE_ID }; };
template<> struct Type2Id<Condition>	{ enum { ID = CONDITION_ID }; };

template<> struct Type2Id<Clock>	{ enum { ID = CLOCK_ID }; };
template<> struct Type2Id<Alarm>	{ enum { ID = ALARM_ID }; };
template<> struct Type2Id<Chronometer>	{ enum { ID = CHRONOMETER_ID }; };

template<> struct Type2Id<Network>	{ enum { ID = NETWORK_ID }; };
template<> struct Type2Id<IP>		{ enum { ID = IP_ID }; };
//template<> struct Type2Id<ARP>		{ enum { ID = ARP_ID }; };
template<> struct Type2Id<UDP>		{ enum { ID = UDP_ID }; };
template<> struct Type2Id<CAN>      { enum { ID = CAN_ID }; };

// template<> struct Type2Id<Temperature_Sentient>{ enum { ID = TEMPERATURE_SENTIENT_ID }; };
// template<> struct Type2Id<Photo_Sentient>{ enum { ID = PHOTO_SENTIENT_ID }; };


// Id -> Type
template<Type_Id id>
struct Id2Type { typedef Dummy<0> TYPE; };

template<> struct Id2Type<CPU_ID>	{ typedef CPU TYPE; };
template<> struct Id2Type<TSC_ID>	{ typedef TSC TYPE; };
template<> struct Id2Type<MMU_ID>	{ typedef MMU TYPE; };


template<> struct Id2Type<MACHINE_ID>	{ typedef Machine TYPE; };
template<> struct Id2Type<PCI_ID>	{ typedef PCI TYPE; };
template<> struct Id2Type<IC_ID>	{ typedef IC TYPE; };
template<> struct Id2Type<TIMER_ID>	{ typedef Timer TYPE; };
template<> struct Id2Type<RTC_ID>	{ typedef RTC TYPE; };
template<> struct Id2Type<EEPROM_ID>	{ typedef EEPROM TYPE; };
template<> struct Id2Type<FLASH_ID>	{ typedef Flash TYPE; };
template<> struct Id2Type<UART_ID>	{ typedef UART TYPE; };
template<> struct Id2Type<SPI_ID>	{ typedef SPI TYPE; };
template<> struct Id2Type<DISPLAY_ID>	{ typedef Display TYPE; };
template<> struct Id2Type<NIC_ID>	{ typedef NIC TYPE; };
template<> struct Id2Type<ADC_ID>	{ typedef ADC TYPE; };
template<> struct Id2Type<TEMPERATURE_SENSOR_ID> { typedef Temperature_Sensor TYPE; };
template<> struct Id2Type<PHOTO_SENSOR_ID> { typedef Photo_Sensor TYPE; };
template<> struct Id2Type<ACCELEROMETER_ID> { typedef Accelerometer TYPE; };


template<> struct Id2Type<THREAD_ID>	{ typedef Thread TYPE; };
template<> struct Id2Type<TASK_ID>	{ typedef Task TYPE; };
template<> struct Id2Type<ACTIVE_ID>	{ typedef Active TYPE; };

template<> struct Id2Type<ADDRESS_SPACE_ID>{ typedef Address_Space TYPE; };
template<> struct Id2Type<SEGMENT_ID>	{ typedef Segment TYPE; };

template<> struct Id2Type<MUTEX_ID>	{ typedef Mutex TYPE; };
template<> struct Id2Type<SEMAPHORE_ID>	{ typedef Semaphore TYPE; };
template<> struct Id2Type<CONDITION_ID>	{ typedef Condition TYPE; };

template<> struct Id2Type<CLOCK_ID>	{ typedef Clock TYPE; };
template<> struct Id2Type<ALARM_ID>	{ typedef Alarm TYPE; };
template<> struct Id2Type<CHRONOMETER_ID>{ typedef Chronometer TYPE; };

template<> struct Id2Type<NETWORK_ID>	{ typedef Network TYPE; };
template<> struct Id2Type<IP_ID>	{ typedef IP TYPE; };
//template<> struct Id2Type<ARP_ID>	{ typedef ARP TYPE; };
template<> struct Id2Type<UDP_ID>	{ typedef UDP TYPE; };
template<> struct Id2Type<CAN_ID>   { typedef CAN TYPE; };

__END_SYS

#endif
