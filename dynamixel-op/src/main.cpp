\#include <stdio.h>
#include <vector>
#include "dynamixel_sdk.h"

#define PROTOCOL_VERSION      2.0
#define BAUDRATE              57600
#define DEVICE_NAME           "/dev/ttyUSB0"

#define ADDR_PRESENT_POSITION 132
#define ADDR_GOAL_POSITION    116
#define ADDR_MOVING_SPEED     112
#define ADDR_TORQUE_ENABLE    64

#define COMM_SUCCESS          0
#define COMM_TX_FAIL          -1001

class DynamixelManager
{
private:
    dynamixel::PortHandler* portHandler;
    dynamixel::PacketHandler* packetHandler;

public:
    DynamixelManager();
    ~DynamixelManager();

    bool connect();
    void disconnect();
    int ping(uint8_t dxl_id, uint8_t* error = 0);
    int readByte(uint8_t dxl_id, uint16_t address, uint8_t* data, uint8_t* error = 0);
    int readWord(uint8_t dxl_id, uint16_t address, uint16_t* data, uint8_t* error = 0);
    int readDWord(uint8_t dxl_id, uint16_t address, uint32_t* data, uint8_t* error = 0);
    int writeByte(uint8_t dxl_id, uint16_t address, uint8_t data, uint8_t* error = 0);
    int writeWord(uint8_t dxl_id, uint16_t address, uint16_t data, uint8_t* error = 0);
    int writeDWord(uint8_t dxl_id, uint16_t address, uint32_t data, uint8_t* error = 0);
    int syncWrite(uint16_t address, uint16_t data_length, std::vector<uint8_t>& param);
    int bulkRead(std::vector<uint8_t>& param);
};

DynamixelManager::DynamixelManager()
{
    portHandler = dynamixel::PortHandler::getPortHandler(DEVICE_NAME);
    packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
}

DynamixelManager::~DynamixelManager()
{
    disconnect();
}

bool DynamixelManager::connect()
{
    if (portHandler->openPort())
    {
        printf("Succeeded to open the port!\n");

        if (portHandler->setBaudRate(BAUDRATE))
        {
            printf("Succeeded to change the baudrate!\n");
            return true;
        }
        else
        {
            printf("Failed to change the baudrate!\n");
            return false;
        }
    }
    else
    {
        printf("Failed to open the port!\n");
        return false;
    }
}

void DynamixelManager::disconnect()
{
    portHandler->closePort();
}

int DynamixelManager::ping(uint8_t dxl_id, uint8_t* error)
{
    return packetHandler->ping(portHandler, dxl_id, error);
}

int DynamixelManager::readByte(uint8_t dxl_id, uint16_t address, uint8_t* data, uint8_t* error)
{
    return packetHandler->read1ByteTxRx(portHandler, dxl_id, address, data, error);
}

int DynamixelManager::readWord(uint8_t dxl_id, uint16_t address, uint16_t* data, uint8_t* error)
{
    return packetHandler->read2ByteTxRx(portHandler, dxl_id, address, data, error);
}

int DynamixelManager::readDWord(uint8_t dxl_id, uint16_t address, uint32_t* data, uint8_t* error)
{
    return packetHandler->read4ByteTxRx(portHandler, dxl_id, address, data, error);
}

int DynamixelManager::writeByte(uint8_t dxl_id, uint16_t address, uint8_t data, uint8_t* error)
{
    return packetHandler->write1ByteTxRx(portHandler, dxl_id, address, data, error);
}

int DynamixelManager::writeWord(uint8_t dxl_id, uint16_t address, uint16_t data, uint8_t* error)
{
    return packetHandler->write2ByteTxRx(portHandler, dxl_id, address, data, error);
}

int DynamixelManager::writeDWord(uint8_t dxl_id, uint16_t address, uint32_t data, uint8_t* error)
{
    return packetHandler->write4ByteTxRx(portHandler, dxl_id, address, data, error);
}

int DynamixelManager::syncWrite(uint16_t address, uint16_t data_length, std::vector<uint8_t>& param)
{
    return packetHandler->syncWriteTxOnly(portHandler, address, data_length, param.data(), param.size());
}

int DynamixelManager::bulkRead(std::vector<uint8_t>& param)
{
    return packetHandler->bulkReadTx(portHandler, param.data(), param.size());
}

int main()
{
    DynamixelManager dxlManager;

    if (dxlManager.connect())
    {
        printf("Dynamixel manager connected.\n");

        uint8_t dxl_error = 0;
        uint32_t presentPosition = 0;
        uint32_t goalPosition = 512;

        if (dxlManager.ping(1, &dxl_error) == COMM_SUCCESS)
        {
            printf("Dynamixel ID 1 is connected.\n");

            dxlManager.writeByte(1, ADDR_TORQUE_ENABLE, 1, &dxl_error);

            dxlManager.readDWord(1, ADDR_PRESENT_POSITION, &presentPosition, &dxl_error);
            printf("Current position: %d\n", presentPosition);

            dxlManager.writeDWord(1, ADDR_GOAL_POSITION, goalPosition, &dxl_error);
            printf("Goal position set to: %d\n", goalPosition);

            std::vector<uint8_t> syncWriteParam;
            syncWriteParam.push_back(1);
            syncWriteParam.push_back(DXL_LOBYTE(DXL_LOWORD(goalPosition)));
            syncWriteParam.push_back(DXL_HIBYTE(DXL_LOWORD(goalPosition)));
            syncWriteParam.push_back(DXL_LOBYTE(DXL_HIWORD(goalPosition)));
            syncWriteParam.push_back(DXL_HIBYTE(DXL_HIWORD(goalPosition)));

            dxlManager.syncWrite(ADDR_GOAL_POSITION, 4, syncWriteParam);

            std::vector<uint8_t> bulkReadParam;
            bulkReadParam.push_back(1);
            bulkReadParam.push_back(2);
            bulkReadParam.push_back(1);
            bulkReadParam.push_back(ADDR_PRESENT_POSITION);

            dxlManager.bulkRead(bulkReadParam);

            dxlManager.writeByte(1, ADDR_TORQUE_ENABLE, 0, &dxl_error);
        }
        else
        {
            printf("Failed to ping Dynamixel ID 1.\n");
        }

        dxlManager.disconnect();
    }
    else
    {
        printf("Failed to connect to Dynamixel manager.\n");
    }

    return 0;
}