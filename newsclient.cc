/* myclient.cc: sample client program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "MessageHandler.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

Connection init(int argc, char *argv[]);
int app(Connection &conn);
void list_NG(MessageHandler &msg);
void create_NG(MessageHandler &msg);
void delete_NG(MessageHandler &msg);
void list_AinNG(MessageHandler &msg);
void list_NG(MessageHandler &msg);
void list_NG(MessageHandler &msg);
void list_NG(MessageHandler &msg);
