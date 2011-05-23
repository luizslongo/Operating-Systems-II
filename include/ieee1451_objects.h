#ifndef __ieee1451_objects_h
#define __ieee1451_objects_h

#include <machine.h>

__BEGIN_SYS

struct Command
{
    unsigned short _channel_number;
    unsigned short _command;
    unsigned short _length;
};

struct Reply
{
    bool _success;
    unsigned short _length;
};

/*struct TEDS_Query_Reply
{
     Reply _header;
     unsigned char _atributes;
     unsigned char _status;
     unsigned long _size;
     unsigned short _checksum;
     unsigned long _maxSize;
};*/

struct TEDS_Read_Reply
{
    Reply _header;
    unsigned long _offset;
};

struct Data_Set_Read_Reply
{
    Reply _header;
    unsigned long _offset;
};

enum CommandClass //IEEE 1451.0 (2007) -> Chapter 7
{
    //COMMAND_CLASS_QUERY_TEDS								= 0x0101, //Query TEDS
    //COMMAND_CLASS_UPDATE_TEDS								= 0x0104, //Update TEDS
    COMMAND_CLASS_READ_TEDS_SEGMENT                         = 0x0102, //Read TEDS segment
    COMMAND_CLASS_READ_TRANSDUCER_CHANNEL_DATA_SET_SEGMENT  = 0x0301, //Read TransducerChannel data-set segment
    COMMAND_CLASS_TRANSDUCER_CHANNEL_OPERATE                = 0x0401, //TransducerChannel Operate
    COMMAND_CLASS_TRANSDUCER_CHANNEL_IDLE                   = 0x0402  //TransducerChannel Idle
};

enum AddressClass //IEEE 1451.0 (2007) -> Chapter 5.3
{
    ADDRESS_CLASS_TIM                       = 0x0000,
    ADDRESS_CLASS_TRANSDUCER_CHANNEL_FIRST  = 0x0001,
    ADDRESS_CLASS_TRANSDUCER_CHANNEL_LAST   = 0x7FFF,
    ADDRESS_CLASS_GROUP_FIRST               = 0x8000,
    ADDRESS_CLASS_GROUP_LAST                = 0xFFFE,
    ADDRESS_CLASS_GLOBAL                    = 0xFFFF
};

#define IEEE1451_PORT 55667

struct IEEE1451_Packet
{
    unsigned short _trans_id;
    unsigned int _length;
};

__END_SYS

#endif
