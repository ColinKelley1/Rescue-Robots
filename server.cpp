#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <iterator>
#include <vector>
#include "message.h"
#include "Log.h"
using namespace std;



int main(int argc, char** argv ) {
    int sockdesc;            // Socket descriptor
    struct addrinfo *myinfo; // Address record
    char portnum[81];
    message mymessage;
    int connection;
    string logFileName;
    Log log1;               //Log object
    string line;            //String for message

    // Create the socket with domain, type, protocol as
    //    internet, stream, default
    sockdesc = socket(AF_INET, SOCK_STREAM, 0);

    if (sockdesc < 0)
    {
        cout << "Error creating socket" << endl;
        exit(0);
    }


    // Get the port number from the command line
    if (argc > 1) {
        strcpy(portnum, argv[2]);
    } else {
        cout << "Invalid command line parameters." << endl;
        exit(0);
    }

    //variable used to convert port to int
    int port;
    string temp;

    while (true)
    {
        // Set up the address record
        if (getaddrinfo("127.0.0.1", portnum, NULL, &myinfo) != 0)
        {

            cout << "Error getting address" << endl;
            exit(0);
        }
        // Bind the socket to an address
        else if (bind(sockdesc, myinfo->ai_addr, myinfo->ai_addrlen) < 0)
        {
            //Block incrememnts port number
            cout << "Error binding to socket. Trying again" << endl;
            port = atoi(portnum);
            port++;
            temp = to_string(port);
            strcpy(portnum,temp.c_str());

            //debug
            cout << "bound to " << portnum << endl;
        }
        else
        {
            break;
        }
    }
    // Now listen to the socket
    if ( listen(sockdesc, 1) < 0 )
    {
        cout << "Error in listen" << endl;
        exit(0);
    }

    cout << "Bound to port number: " << portnum << endl;

    // Accept a connect, check the returned descriptor
    connection = accept(sockdesc, NULL, NULL);

    if ( connection < 0 )
    {
        cout << "Error in accept" << endl;
        exit(0);
    }

    cout << "Connected to client." << endl;

    int value;
    //Wait for a message from the client program and
    //We iterate through the loop until we receive a Q
    while(true)
    {

        value = read(connection, (char*)&mymessage, sizeof(message));

        if(value > 0)
        {
            //debug
            cout << "Recieved: " << mymessage.payload << endl;

            //If it is a Q then quit
            if (mymessage.payload[0] == 'Q') {
                //write the log code and close
                log1.writeLogRecord(mymessage.payload);
                log1.close();
                break;
            }
                //If it is the filename command
            else if (mymessage.payload[0] == 'N') {
                line = mymessage.payload;

                vector<string> tokens;      //Vector holds tokenized commands

                istringstream iss(line);    //Create sstream object

                //This will copy the tokens into the vector container
                copy(istream_iterator<string>(iss),
                     istream_iterator<string>(),
                     back_inserter(tokens));

                logFileName = tokens[1];        //Set the logfile name

                log1.setLogfileName(logFileName);

                //debug
                cout << "Logfile name is: " << log1.getLogfileName() << endl;

                log1.open();
            }
            else
            {
                log1.writeLogRecord(mymessage.payload);              //Write the command to the log
            }
        }

    } // for

    // Close the connection
    close(connection);
    return 0;

} // main( )

