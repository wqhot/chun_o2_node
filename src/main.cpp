#include <Arduino.h>
#include <HardwareSerial.h>
#include <string>

HardwareSerial serial_o2(2);
HardwareSerial serial_report(1);
const uint8_t head[4] = {0x1, 0x2, 0x3, 0x4};

void getId(uint8_t *id)
{
    memcpy(id, (const void*)(0x1FFFF7E8), 12);
}

uint8_t getSum(const uint8_t *buffer, size_t length)
{
    uint8_t sum = 0;
    for (size_t i = 0; i != length; ++i)
    {
        sum += *(buffer + i);
    }
    return sum;
}

void setNum(uint8_t *buffer, const float num)
{
    union transfer
    {
        float f;
        uint8_t c[4];
    } t;
    t.f = num;
    memcpy(buffer, t.c, 4);
}

void setup()
{
    serial_o2.begin(115200, SERIAL_8N1);
    serial_report.begin(115200, SERIAL_8N1);
}

void loop()
{
    uint8_t buffer[17];
    uint8_t sum;
    getId(buffer);
    setNum(buffer + 12, 1.0f);
    sum = getSum(buffer, 16);
    buffer[16] = sum;
    serial_report.write(head, 4);
    serial_report.write(buffer, 17);
    delay(1000);
}