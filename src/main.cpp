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

int recvData(uint8_t *buffer, int maxLength)
{
    int c = serial_o2.read();
    int length = 0;
    while (c > 0 && length < maxLength)
    {
        buffer[length] = c;
        ++length;
        c = serial_o2.read();
    }
    return length;
}

bool recv(uint8_t *buffer)
{
    const size_t len = 10;
    serial_o2.write("read O2 Data");
    size_t l = serial_o2.readBytes(buffer, len);
    if (l != len)
    {
        return false;
    }
    return true;
}

bool recv(float &o2)
{
    const size_t len = 10;
    uint8_t buffer[len] = {0};
    serial_o2.write("read O2 Data");
    size_t l = serial_o2.readBytes(buffer, len);
    if (l != len)
    {
        return false;
    }
    o2 = 10.0f * (buffer[3] - '0') + 1.0f * (buffer[4] - '0') + 0.1f * (buffer[6] - '0');
    return true;
}

void loop()
{
    uint8_t buffer[23];
    uint8_t bufferNew[10];
    uint8_t sum;
    float o2;
    digitalWrite(LED_PIN, LOW); //小灯亮
    delay(500);
    if (!recv(bufferNew))
    {
        serial_report.write("No sensor\n");
    }
    else
    {
        // getId(buffer);
        // // setNum(buffer + 12, o2);
        // sum = getSum(buffer, 22);
        // buffer[22] = sum;
        serial_report.write(bufferNew, 10);
        // serial_report.write(buffer, 23);
    }
    digitalWrite(LED_PIN, HIGH); //小灯灭
    delay(1000);
}