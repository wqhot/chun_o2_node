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

bool recv(float &o2)
{
    const size_t len = 8;
    uint8_t buffer[len] = {0};
    serial_o2.write("read O2 Data");
    size_t l = serial_o2.readBytes(buffer, len);
    if (l != len)
    {
        o2 = -1;
        return false;
    }
    o2 = 10.0f * (buffer[3] - '0') + 1.0f * (buffer[4] - '0') + 0.1f * (buffer[6] - '0');
    return true;
}

void loop()
{
    uint8_t buffer[17];
    uint8_t sum;
    float o2;
    if (!recv(o2))
    {
        delay(1000);
        return;
    }
    getId(buffer);
    setNum(buffer + 12, o2);
    sum = getSum(buffer, 16);
    buffer[16] = sum;
    serial_report.write(head, 4);
    serial_report.write(buffer, 17);
    delay(1000);
}