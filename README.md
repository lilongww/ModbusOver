# ModbusOver

## 1. 项目简介

​	ModbusOver基于标准Modbus协议编写，主要目的是为了解决实践中遇到的与各种Modbus设备进行通信的问题。

​	在项目实践中，遇到许多设备并非是像标准Modbus协议那样的，在串行线路使用ModbusRTU，在LAN线路中使用ModbusTCP，也有扩展到UDP通信的设备，多样而复杂，因此在本项目中，Modbus协议构建与通信链路接口设计是相互分开的，可以自由的使用 *ModbusRTU Over TCP* 或者反过来使用。

## 2. 项目进度

目前仅实现Master端。

### 2.1 Modbus协议

- [x] Modbus RTU
- [x] Modbus TCP
- [x] Modbus ASCII

### 2.2 通信接口

- [x] TCP
- [x] Serial Port
- [ ] UDP

### 2.3 协议接口

- [x] 读线圈 0x01
- [x] 读离散量输入 0x02
- [x] 读保持寄存器 0x03
- [x] 读输入寄存器 0x04
- [x] 写单个线圈 0x05
- [x] 写单个寄存器 0x06
- [ ] 读异常状态 0x07
- [ ] 诊断 0x08
- [ ] 获得事件计数器 0x0B
- [ ] 获得事件记录 0x0C
- [x] 写多个线圈 0x0F
- [x] 写多个寄存器 0x10
- [ ] 报告从站ID 0x11
- [ ] 读文件记录 0x14
- [ ] 写文件记录 0x15
- [ ] 屏蔽写寄存器 0x16
- [ ] 读写多个寄存器 0x17
- [ ] 读FIFO队列 0x18
- [ ] 读设备标识码 0x2B
- [ ] 封装接口传输 0x2B
- [ ] CANopen 通用引用 0x2B

## 3. 编译

本项目基于C++23标准编写，在VS2022下编译通过。

### 3.1 第三方库

* boost

### 3.2 环境变量设置

### 2. 环境变量设置

* 添加 ```BOOST_ROOT``` 变量，并设置值为 BOOST库根目录，比如：C:\local\boost_1_82_0
* 添加 ```BOOST_LIBRARYDIR``` 变量，并设置值为 BOOST lib库所在目录，比如：%BOOST_ROOT%\lib64-msvc-14.3
* 添加 ```BOOST_INCLUDEDIR``` 变量，并设置值为 BOOST 头文件目录，比如 %BOOST_ROOT%

## 4. 使用

* *注意：ModbusTCP要求的默认从机ID为0xFF，因为在串口连接中，数据路由由从机ID实现，而在TCP连接中，由TCPIP协议实现，在TCP连接中应设置为0xFF，所以本项目的默认从机ID也为0xFF，所以在连接时，应当自行设置从机ID，即调用setSlave函数，否则在进行串口连接时可能失败*

* *setSlave函数也可以在连接前调用，connect函数并不修改从机ID*

* *在串口通信中可以使用广播，广播使用的从机ID为0，也可使用**ModbusOver::Broadcast***

### 4.1 连接设备

#### 4.1.1 连接TCP设备

``` cpp
ModbusOver::Master master;
master.connect(ModbusOver::Address<ModbusOver::AddressType::TCP>("192.168.1.150"));
master.setSlave(1); // 设置从机ID
// Do something ...
master.close();
```

按照标准Modbus协议，在TCP上实现的Modbus使用的端口默认为502，如果需要设置端口号，则：

```cpp
master.connect(ModbusOver::Address<ModbusOver::AddressType::TCP>("192.168.1.150", 5000));
```

在TCP链路中，默认使用的时ModbusTCP协议，如果需要使用ModbusRTU Over TCP，则：

```cpp
master.connect(ModbusOver::Address<ModbusOver::AddressType::TCP>("192.168.1.150", 5000, ModbusOver::ModbusProtocol::ModbusRTU));
```

#### 4.1.2 连接串口设备

```cpp
ModbusOver::Master master;
master.connect(ModbusOver::Address<ModbusOver::AddressType::SerialPort>("COM5"));
master.setSlave(1);// 设置从机ID
// Do something ...
master.close();
```

按照标准Modbus协议，在串口上实现的Modbus默认连接参数为：

* 波特率：9600
* 数据位：8
* 停止位：1
* 校验：Even，**注意：如果使用 None 则需要2个停止位**

* 流控制：None，**协议中并未提到流控制，默认使用None**

如果需要以自定义的参数连接，则：

```cpp
master.connect(ModbusOver::Address<ModbusOver::AddressType::SerialPort>("COM5", 9600, ModbusOver::DataBits::Data8, ModbusOver::FlowControl::None, ModbusOver::Parity::Even, ModbusOver::StopBits::One));
```

如果需要在串口连接中使用其他协议，则：

```cpp
master.connect(ModbusOver::Address<ModbusOver::AddressType::SerialPort>("COM5", ModbusOver::ModbusProtocol::ModbusRTU);
// 或
master.connect(ModbusOver::Address<ModbusOver::AddressType::SerialPort>("COM5", 9600, ModbusOver::DataBits::Data8, ModbusOver::FlowControl::None, ModbusOver::Parity::Even, ModbusOver::StopBits::One, ModbusOver::ModbusProtocol::ModbusRTU));
```

