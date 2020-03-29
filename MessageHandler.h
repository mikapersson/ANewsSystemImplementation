#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"

#include <string>
#include <memory>

class MessageHandler { //message handling between client and server
	public:
	MessageHandler(std::shared_ptr<Connection>&); 
	void send_anscode(Protocol);
	void send_int(int);
	void send_int_parameter(int);
	void send_string_parameter(std::string);
	Protocol rec_cmd();
	int rec_int();
	int rec_int_parameter();
	std::string rec_string_parameter();
private:
	std::shared_ptr<Connection> conn;  //anslutingen    
	unsigned char rec_byte();  //mottagen byte
  	void send_byte(const char code);  //skicka byte
};