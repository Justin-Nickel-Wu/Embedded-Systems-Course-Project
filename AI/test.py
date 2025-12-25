import serial
import time
import serial.tools.list_ports

ser = serial.Serial(
    port='COM4',        # 改成你的
    baudrate=115200,    # 必须和 STM32 一致
    bytesize=8,
    parity='N',
    stopbits=1,
    timeout=0.005
)

# 计算 CRC-16 
def crc16_modbus(data: bytes) -> int:
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            if crc & 0x0001:
                crc >>= 1
                crc ^= 0xA001
            else:
                crc >>= 1
    return crc & 0xFFFF

def recvUartFrame(ser):
    frame = bytearray()
    while True:
        chunk = ser.read(256)   # 读当前缓冲
        if chunk:
            frame += chunk
        else:
            # timeout 触发：说明总线空闲了一段时间
            if frame:
                return frame

def main():
    msg = 'Hello'
    crc = crc16_modbus(msg.encode('utf-8'))
    msg_with_crc = msg.encode('utf-8') + crc.to_bytes(2, byteorder='little')
    ser.write(msg_with_crc)

    ports = serial.tools.list_ports.comports()
    for p in ports:
        print(p.device, p.description)

    while 1:
        frame = recvUartFrame(ser)
        if frame:
            print("Received frame:", frame)
            if len(frame) >= 3:  # 至少要有数据和 CRC
                data = frame[:-2]
                received_crc = int.from_bytes(frame[-2:], byteorder='little')
                calculated_crc = crc16_modbus(data)
                if received_crc == calculated_crc:
                    print("CRC check passed. Data:", data)
                else:
                    print("CRC check failed. Received:", received_crc, "Calculated:", calculated_crc)



if __name__ == "__main__":
    main()