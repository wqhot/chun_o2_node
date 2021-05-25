#include <Arduino.h>
#include <string>
#define LED_PIN PC13

/**
 *定义串口对象
 * */
HardwareSerial serial_report(PA10, PA9);
HardwareSerial serial_o2(PA3, PA2);
const uint8_t head[4] = {0x1, 0x2, 0x3, 0x4};

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    serial_o2.begin(115200);
    serial_report.begin(115200);
}

void getId(uint8_t *id)
{
    memcpy(id, (const void *)(0x1FFFF7E8), 12);
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
    // union transfer
    // {
    //     float f;
    //     uint8_t c[4];
    // } t;
    // t.f = num;
    // memcpy(buffer, t.c, 4);
    buffer[0] = static_cast<int>(num / 10) + '0';
    buffer[1] = static_cast<int>(num) % 10 + '0';
    buffer[2] = static_cast<int>(num * 10) % 10 + '0';
}

bool recv(uint8_t *buffer)
{
    const size_t len = 4;
    serial_o2.write("read O2 Data");
    int c = serial_o2.read();
    while (c >= 0)
    {
        if (c=='=')
        {
            break;
        }
        else
        {
            c = serial_o2.read();
        }
    }
    int length = 0;
    c = serial_o2.read();
    while (c > 0 && (length < len))
    {
        buffer[length] = c;
        ++length;
        c = serial_o2.read();
    }
    if (length != len)
    {
        return false;
    }
    return true;
}

bool recv(float &o2)
{
    const size_t len = 4;
    uint8_t buffer[len] = {0};
    serial_o2.write("read O2 Data");
    int c = serial_o2.read();
    while (c >= 0)
    {
        if (c=='=')
        {
            break;
        }
        else
        {
            c = serial_o2.read();
        }
    }
    int length = 0;
    c = serial_o2.read();
    while (c > 0 && (length < len))
    {
        buffer[length] = c;
        ++length;
        c = serial_o2.read();
    }
    if (length != len)
    {
        o2 = -1;
        return false;
    }
    o2 = 10.0f * (buffer[0] - '0') + 1.0f * (buffer[1] - '0') + 0.1f * (buffer[3] - '0');
    return true;
}

void loop()
{
    uint8_t buffer[17];
    uint8_t sum;
    float o2;
    digitalWrite(LED_PIN, LOW); //小灯亮
    delay(500);
    if (!recv(o2))
    {
        serial_report.write("No sensor\n");
    }
    else
    {
        getId(buffer);
        setNum(buffer + 12, o2);
        sum = getSum(buffer, 16);
        buffer[16] = sum;
        serial_report.write(head, 4);
        serial_report.write(buffer, 17);
    }
    digitalWrite(LED_PIN, HIGH); //小灯灭
    delay(1000);
}