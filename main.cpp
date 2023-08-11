#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <boost/asio.hpp>

class Message {
public:
    Message(const std::string& str) : data(str) {}
    
    std::vector<unsigned char> encrypt() const {
        std::vector<unsigned char> encrypted;
        encrypted.reserve(5 + data.size());
        
        // Добавление заголовка "HELLO"
        encrypted.push_back('H');
        encrypted.push_back('E');
        encrypted.push_back('L');
        encrypted.push_back('L');
        encrypted.push_back('O');
        
        // Шифрование символов строки
        for (char c : data) {
            encrypted.push_back(c + 1);
        }
        
        return encrypted;
    }
    
private:
    std::string data;
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " COM_PORT UDP_PORT" << std::endl;
        return 1;
    }
    
    std::string comPortName = argv[1];
    std::string udpPortName = argv[2];
    
    // Открытие COM-порта
    boost::asio::io_context io_context;
    boost::asio::serial_port comPort(io_context, comPortName);
    comPort.set_option(boost::asio::serial_port_base::baud_rate(9600));
    
    // Создание UDP-сокета
    boost::asio::ip::udp::socket socket(io_context);
    socket.open(boost::asio::ip::udp::v4());

    // Установка опции для разрешения броадкаста
    boost::asio::socket_base::broadcast option(true);
    socket.set_option(option);
    
    // Чтение 10 строк из COM-порта
    for (int i = 0; i < 10; ++i) {
        std::string input;
        
        // Установка таймера на 5 секунд
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point end = start + std::chrono::seconds(5);
        
        // Чтение строки с учетом таймера
        while (std::getline(std::cin, input)) {
            // Проверка на пустую строку
            if (input.empty()) {
                continue;
            }
            
            // Шифрование сообщения
            Message message(input);
            std::vector<unsigned char> encrypted = message.encrypt();
            
            // Отправка сообщения по UDP-протоколу броадкастом
            boost::asio::ip::udp::endpoint broadcastEndpoint(boost::asio::ip::address_v4::broadcast(), std::stoi(udpPortName));
            socket.send_to(boost::asio::buffer(encrypted), broadcastEndpoint);
            
            // Проверка на истечение таймера
            std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
            if (current >= end) {
                break;
            }
        }
        
        // Пауза в 1 секунду перед чтением следующей строки
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // Закрытие COM-порта
    comPort.close();
    
    return 0;
}