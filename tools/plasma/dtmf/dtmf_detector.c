/*
 * Software/Hardware Integration Laboratory
 * Federal University of Santa Catarina - Brazil
 * Author: João Paulo Pizani Flor <joaopizani@gmail.com>
 */
#include "parameters.h"

#define MAX_UCHAR 255
#define MAX_SHORT 32768
#define INVAL 255


int putchar(int value);
int getch(void);


static float samples[FRAMESIZE];
static unsigned char /*boolean*/ tone_responses[TONES];


unsigned int /*boolean*/ goertzel(int tone_index) {
    float cosine = cosines[tone_index];
    float sine = sines[tone_index];
    float coeff = 2.0 * cosine;

    float real = 0.0;
    float imag = 0.0;
    float magnitude_square = 0.0;

    float q0 = 0.0, q1 = 0.0, q2 = 0.0;

    float* frame = samples;
    float* frame_end = (samples + FRAMESIZE);
    for(; frame != frame_end; ++frame) {
        q0 = (coeff * q1) - (q2 + *frame);
        q2 = q1;
        q1 = q0;
    }

    real = q1 - q2 * cosine;
    imag = q2 * sine;
    magnitude_square = (real*real) + (imag*imag);

    return (magnitude_square > THRESHOLD);
}

float collect_sample(void) {
    static char float_buf[4];

    // read from UART and normalize from [MIN_SHORT,MAX_SHORT] to [-1,1])
    // behold, this is C poetry :)  MIPS (plasma in particular) works with Big-endian
    float value_read = 0.0;
    float_buf[3] = getch();
    float_buf[2] = getch();
    float_buf[1] = getch();
    float_buf[0] = getch();
    value_read = *((float*) float_buf);

    float value_normalized = value_read * MAX_SHORT;

    return value_normalized;
}

unsigned char analyze_responses(unsigned char* responses) {
    // sparse array, indexed with 8 bits indices, provides a 0(1) lookup to the valid tone combinations
    static unsigned char response_lookup[MAX_UCHAR + 1] = {
        INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,   13,   15,INVAL,    0,INVAL,INVAL,INVAL,   14,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,   12,    9,INVAL,    8,INVAL,INVAL,INVAL,    7,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,   11,    6,INVAL,    5,INVAL,INVAL,INVAL,    4,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,   10,    3,INVAL,    2,INVAL,INVAL,INVAL,    1,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL, 
        INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL,INVAL
    };
    
    unsigned char index = 0;
    index |= (responses[0] << 0);
    index |= (responses[1] << 1);
    index |= (responses[2] << 2);
    index |= (responses[3] << 3);
    index |= (responses[4] << 4);
    index |= (responses[5] << 5);
    index |= (responses[6] << 6);
    index |= (responses[7] << 7);

    return response_lookup[index];
}

void write_signal(unsigned char signal) {
    static char button_names[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', '*', '#',
    };
    // convert from [0, 15] to [1,2,3,..,A,B,..,F] using a lookup table
    // and write the char to UART

    // 'I' for invalid signal
    char button_name = 0;
    if(signal == INVAL) {
        button_name = 'I';
    } else {
        button_name = button_names[signal];
    }

    // chamge for MIPS MemoryWrite(button_name, UART_ADDR)
    putchar(button_name);
}

int main(void) {
    int i, j;
    unsigned int detected_signal = 0;

    while(1) {
        for(i = 0; i < FRAMESIZE; ++i) {   
            samples[i] = collect_sample();
        }

        for(j = 0; j < TONES; ++j) {
            tone_responses[j] = goertzel(j);
        }

        detected_signal = analyze_responses(tone_responses);
        write_signal(detected_signal);
    }

    return 0;
}

