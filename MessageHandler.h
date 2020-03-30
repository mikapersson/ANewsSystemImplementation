#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"

#include <string>
#include <memory>

class MessageHandler { //message handling between client and server
	public:
	MessageHandler(std::shared_ptr<Connection>&); 
	void send_anscode(Protocol);  //send answer code from server to client
	void send_int(int);  //send int (four bytes) from server to client
	void send_int_parameter(int);  //send int parameter from server to client
	void send_string_parameter(std::string);  //send string parameter from server to client
	Protocol rec_cmd();  //read command from client to server
	int rec_int();  //read int from client to server
	int rec_int_parameter();  //read int parameter from client to server
	std::string rec_string_parameter();  //read string parameter from client to server
private:
	std::shared_ptr<Connection> conn;  //anslutingen    
	unsigned char rec_byte();  //read byte from client to server
  	void send_byte(const char code);  //skicka byte
};