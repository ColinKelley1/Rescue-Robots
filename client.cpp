//*****************************************
//  File name: main.cpp
//  Project Name: hw4
//  Author: Colin Kelley
//  Date: 4/04/2019
//
//*****************************************



#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <queue>
#include "pthread.h"
#include "semaphore.h"
#include "message.h"
#include "SafeQueue.h"


using namespace std;

pthread_t TIDS[6];			        //Array of pthread IDs

SafeQueue<message> SafeQueues [6];  //Array of SafeQueues

int numActiveThreads;			    //Number of active threads
int bots = 0;            //Number of bots
int xMax = 0;            //X grid value
int yMax = 0;            //Y grid value
int tempPortnum;
char port[81];
char hostname[81];

struct addrinfo *myinfo; // Address record
int sockdesc;
int connection;

//Strings hold setup file data
message R;
message X;
message Y;
fstream inFile;

bool checkInputFile(string filename);
void* robotFunctionOne(void*);
void* robotFunctionTwo(void*);
void* robotFunctionThree(void*);
void* robotFunctionFour(void*);
void* robotFunctionFive(void*);
bool parseSetup(string line);
void validateCommands(string cfilename);
void readParentQueue();

int main(int argc, char* argv[])
{
    //String variables to hold filenames
    string lFileName;
    string sfilename;
    string cfilename;
    string line;       //Holds input
    bool cflag = false; //bool for cflag
    bool lflag = false; //Bool for logfile

/////////////////////////////////////////////////////////////////////////////////////////////////////
//                          Getting input from log and setup file
/////////////////////////////////////////////////////////////////////////////////////////////////////

    int remainder = argc % 2;   //See if number of arguments is odd

    //If there is an argument present, looks through the arguments to see if -l command is there
    if (remainder != 0)
    {
        //Checks each item to see if flags is present
        for (int i = 0; i < argc; ++i)
        {
            //If -l flag present then
            if (strcmp( argv[i], "-l") == 0)
            {
                lflag = true;
                if ((strcmp(argv[i+1], "-s") == 0) || (strcmp(argv[i+1], "-c") == 0))
                {
                    cout << "ERROR: Invalid command arguments." << endl;
                    return 0;
                }

                lFileName = argv[i + 1];       //set lfilename equal if present
            }
                //If -c flag is present
            else if (strcmp( argv[i], "-c") == 0)
            {
                cflag = true;   //Set flag to true
                if ((strcmp(argv[i+1], "-l") == 0) || (strcmp(argv[i+1], "-s") == 0))
                {
                    cout << "ERROR: Invalid command arguments." << endl;
                    return 0;
                }
                cfilename = argv[i + 1];       //set cfilename equal if it is
            }
                //if -s file is present
            else if (strcmp( argv[i], "-s") == 0)
            {
                if ((strcmp(argv[i+1], "-l") == 0) || (strcmp(argv[i+1], "-c") == 0))
                {
                    cout << "ERROR: Invalid command arguments." << endl;
                    return 0;
                }
                sfilename = argv[i + 1];       //set sfilename equal if it is
            }
        }
    }
    else
    {
        cout << "ERROR: Invalid command arguments." << endl;
        return 0;
    }
    //If flag not set then send default
    if(lflag == false)
    {
        lFileName = "log.txt";
    }

    message tempMessage;

    //Check to see if file was not able to open
    if(!checkInputFile(sfilename))
    {
        exit(0);
    }

    inFile.open(sfilename);         //Open the command

    //Parse the setup file
    //While the file does not end
    while (getline(inFile, line))
    {

        if(!parseSetup(line))     //Send the line to function for parsing
        {
            //Write line to console and log
            line = "ERROR: Invalid line in setup file input.";
            cerr << line << endl;

            //Close setup and exit
            inFile.close();

            exit(0);   //Exit program
        }

    }

    inFile.close();                 //Close the setup file

    //The following 3 if statements create a connection
    sockdesc = socket(AF_INET, SOCK_STREAM, 0);
    if ( sockdesc < 0 )
    {
        cout << "Error creating socket" << endl;
        exit(0);
    }

    if ( getaddrinfo(hostname, port, NULL, &myinfo) != 0 )
    {
        cout << "Error getting address" << endl;
        exit(0);
    }

    connection = connect(sockdesc, myinfo->ai_addr, myinfo->ai_addrlen);
    if ( connection < 0 )
    {
        cout << "Error in connect" << endl;
        exit(0);
    }


    //debug
    cout << "Connection with server established successfully." << endl;

    //Temporary message tp hold log name
    message logName;

    //Append filename to N
    string fileName = "N ";
    fileName.append(lFileName);
    strcpy(logName.payload, fileName.c_str());

    //Send the files down the socket
    write(sockdesc,(char*)&logName, sizeof(message));
    write(sockdesc,(char*)&R, sizeof(message));
    write(sockdesc,(char*)&X, sizeof(message));
    write(sockdesc,(char*)&Y, sizeof(message));

    //Set up the temp message
    tempMessage.from = 1;
    strcpy(tempMessage.payload,hostname);

    //Send the hostname
    write(sockdesc,(char*)&tempMessage, sizeof(message));

    //Set up the temp message
    tempMessage.from = 1;
    strcpy(tempMessage.payload,port);

    //Send the port
    write(sockdesc,(char*)&tempMessage, sizeof(message));

    numActiveThreads = bots;        //Set the number of thread

    //Create the threads based on number of bots
    if(bots == 1)
    {
        pthread_create(&TIDS[1], NULL, robotFunctionOne, NULL);
    }
    else if(bots == 2)
    {
        pthread_create(&TIDS[1], NULL, robotFunctionOne, NULL);
        pthread_create(&TIDS[2], NULL, robotFunctionTwo, NULL);
    }
    else if(bots == 3)
    {
        pthread_create(&TIDS[1], NULL, robotFunctionOne, NULL);
        pthread_create(&TIDS[2], NULL, robotFunctionTwo, NULL);
        pthread_create(&TIDS[3], NULL, robotFunctionThree, NULL);
    }
    else if(bots == 4)
    {
        pthread_create(&TIDS[1], NULL, robotFunctionOne, NULL);
        pthread_create(&TIDS[2], NULL, robotFunctionTwo, NULL);
        pthread_create(&TIDS[3], NULL, robotFunctionThree, NULL);
        pthread_create(&TIDS[4], NULL, robotFunctionFour, NULL);
    }
    else if(bots == 5)
    {
        pthread_create(&TIDS[1], NULL, robotFunctionOne, NULL);
        pthread_create(&TIDS[2], NULL, robotFunctionTwo, NULL);
        pthread_create(&TIDS[3], NULL, robotFunctionThree, NULL);
        pthread_create(&TIDS[4], NULL, robotFunctionFour, NULL);
        pthread_create(&TIDS[5], NULL, robotFunctionFive, NULL);
    }


    //Attempt to open the command file
    if (cflag)
    {
        if (!checkInputFile(cfilename))
        {
            exit(0);
        }
    }


    //This function will parse the command file, and send commands to the bots
    validateCommands(cfilename);

    //This function will read commands from the parent queue and send to log
    readParentQueue();


    //This code block creates a message, and writes a Q to the log file
    tempMessage.from = 0;
    line = "Q";
    stpcpy(tempMessage.payload, line.c_str());				//Copy the message
    write(sockdesc,(char*)&tempMessage, sizeof(message));	//Send the Q message

    cout << "Results written to log file." << endl;

    // Close the socket
    close(sockdesc);
    return 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//                               Supporting Functions
/////////////////////////////////////////////////////////////////////////////////////////////////////

//The next 5 functions are essentially repeats of each other, and only differ in their bot number. The reason
//I used 5 startup functions is because I could not find a way to reliably get bots to handle their own coordinates
//since they would share the data from one function

//Handles robot 1
void* robotFunctionOne(void *)
{
    int x=0,y=0;        //XY coords for the robot
    char direction;
    message temporaryMessage;
    string messageString;
    int botNum = 1;        //Retrieves the int value

    while(true)
    {

        //Dequeue a message from
        temporaryMessage = SafeQueues[botNum].dequeue();

        //Checks for quit
        if(temporaryMessage.payload[0] == 'Q')
        {

            //Subtract active threads and exit
            numActiveThreads--;

            //Send dead message to parent and kill itself
            SafeQueues[0].enqueue(temporaryMessage);
            pthread_exit(NULL);
        }
        else if(temporaryMessage.payload[0] == 'M')
        {

            //Get direction
            direction = temporaryMessage.payload[4];

            if(direction == 'N')
            {
                //If move is valid
                if(y-1 >= 0)
                {
                    y--;
                }
            }
            if(direction == 'S')
            {
                if(y+1 <= yMax)
                {
                    y++;
                }
            }
            if(direction == 'W')
            {
                if(x-1 >= 0)
                {
                    x--;
                }
            }
            if(direction == 'E')
            {
                if(x+1 <= xMax)
                {
                    x++;
                }
            }

            //Create the message
            messageString = "P " + to_string(botNum) + " " + to_string(x) + " " + to_string(y);     //Construct the message
            temporaryMessage.from = botNum;
            strcpy(temporaryMessage.payload, messageString.c_str());

            SafeQueues[0].enqueue(temporaryMessage);

        }


    }
}

//Handles robot 2
void* robotFunctionTwo(void *)
{
    int x=0,y=0;        //XY coords for the robot
    char direction;
    message temporaryMessage;
    string messageString;
    int botNum = 2;        //Retrieves the int value

    //While the bot is receiving signals
    while(true)
    {

        //Dequeue from bot queue
        temporaryMessage = SafeQueues[botNum].dequeue();

        //Checks for quit
        if(temporaryMessage.payload[0] == 'Q')
        {
            //Subtract active threads and exit
            numActiveThreads--;

            //Send dead message to parent and kill itself
            SafeQueues[0].enqueue(temporaryMessage);
            pthread_exit(NULL);
        }
        else if(temporaryMessage.payload[0] == 'M')
        {
            //Get direction
            direction = temporaryMessage.payload[4];

            if(direction == 'N')
            {
                //If move is valid
                if(y-1 >= 0)
                {
                    y--;
                }
            }
            if(direction == 'S')
            {
                if(y+1 <= yMax)
                {
                    y++;
                }
            }
            if(direction == 'W')
            {
                if(x-1 >= 0)
                {
                    x--;
                }
            }
            if(direction == 'E')
            {
                if(x+1 <= xMax)
                {
                    x++;
                }
            }

            //Create the message
            messageString = "P " + to_string(botNum) + " " + to_string(x) + " " + to_string(y);     //Construct the message
            temporaryMessage.from = botNum;
            strcpy(temporaryMessage.payload, messageString.c_str());

            //Enqueue to parent
            SafeQueues[0].enqueue(temporaryMessage);

        }

    }
}

//Handles robot 3
void* robotFunctionThree(void *)
{
    int x=0,y=0;        //XY coords for the robot
    char direction;
    message temporaryMessage;
    string messageString;
    int botNum = 3;        //Retrieves the int value

    //While the bot is receiving signals
    while(true)
    {

        //Dequeue from bot queue
        temporaryMessage = SafeQueues[botNum].dequeue();

        //Checks for quit
        if(temporaryMessage.payload[0] == 'Q')
        {
            //Subtract active threads and exit
            numActiveThreads--;

            //Send dead message to parent and kill itself
            SafeQueues[0].enqueue(temporaryMessage);
            pthread_exit(NULL);
        }
        else if(temporaryMessage.payload[0] == 'M')
        {
            //Get direction
            direction = temporaryMessage.payload[4];

            if(direction == 'N')
            {
                //If move is valid
                if(y-1 >= 0)
                {
                    y--;
                }
            }
            if(direction == 'S')
            {
                if(y+1 <= yMax)
                {
                    y++;
                }
            }
            if(direction == 'W')
            {
                if(x-1 >= 0)
                {
                    x--;
                }
            }
            if(direction == 'E')
            {
                if(x+1 <= xMax)
                {
                    x++;
                }
            }

            //Create the message
            messageString = "P " + to_string(botNum) + " " + to_string(x) + " " + to_string(y);     //Construct the message
            temporaryMessage.from = botNum;
            strcpy(temporaryMessage.payload, messageString.c_str());

            SafeQueues[0].enqueue(temporaryMessage);

        }

    }
}

//Handles robot 4
void* robotFunctionFour(void *)
{
    int x=0,y=0;        //XY coords for the robot
    char direction;
    message temporaryMessage;
    string messageString;
    int botNum = 4;        //Retrieves the int value

    //While the bot is receiving signals
    while(true)
    {
        //Dequeue from bot queue
        temporaryMessage = SafeQueues[botNum].dequeue();

        //Checks for quit
        if(temporaryMessage.payload[0] == 'Q')
        {
            //Subtract active threads and exit
            numActiveThreads--;

            //Send dead message to parent and kill itself
            SafeQueues[0].enqueue(temporaryMessage);
            pthread_exit(NULL);

        }
        else if(temporaryMessage.payload[0] == 'M')
        {
            //Get direction
            direction = temporaryMessage.payload[4];

            if(direction == 'N')
            {
                //If move is valid
                if(y-1 >= 0)
                {
                    y--;
                }
            }
            if(direction == 'S')
            {
                if(y+1 <= yMax)
                {
                    y++;
                }
            }
            if(direction == 'W')
            {
                if(x-1 >= 0)
                {
                    x--;
                }
            }
            if(direction == 'E')
            {
                if(x+1 <= xMax)
                {
                    x++;
                }
            }

            //Create the message
            messageString = "P " + to_string(botNum) + " " + to_string(x) + " " + to_string(y);     //Construct the message
            temporaryMessage.from = botNum;
            strcpy(temporaryMessage.payload, messageString.c_str());

            SafeQueues[0].enqueue(temporaryMessage);

        }

    }
}

//Handles robot 5
void* robotFunctionFive(void *)
{
    int x=0,y=0;        //XY coords for the robot
    char direction;
    message temporaryMessage;
    string messageString;
    int botNum = 5;        //Retrieves the int value

    //While the bot is receiving signals
    while(true)
    {

        //Dequeue from bot queue
        temporaryMessage = SafeQueues[botNum].dequeue();

        //Checks for quit
        if(temporaryMessage.payload[0] == 'Q')
        {
            //Subtract active threads and exit
            numActiveThreads--;

            //Send dead message to parent and kill itself
            SafeQueues[0].enqueue(temporaryMessage);
            pthread_exit(NULL);
        }
        else if(temporaryMessage.payload[0] == 'M')
        {
            //Get direction
            direction = temporaryMessage.payload[4];

            //Move validation
            if(direction == 'N')
            {
                //If move is valid
                if(y-1 >= 0)
                {
                    y--;
                }
            }
            if(direction == 'S')
            {
                if(y+1 <= yMax)
                {
                    y++;
                }
            }
            if(direction == 'W')
            {
                if(x-1 >= 0)
                {
                    x--;
                }
            }
            if(direction == 'E')
            {
                if(x+1 <= xMax)
                {
                    x++;
                }
            }

            //Create the message
            messageString = "P " + to_string(botNum) + " " + to_string(x) + " " + to_string(y);     //Construct the message
            temporaryMessage.from = botNum;
            strcpy(temporaryMessage.payload, messageString.c_str());


            SafeQueues[0].enqueue(temporaryMessage);

        }
    }
}

//Attempts to open file and returns false if it cant. It will also close the log file
bool checkInputFile(string filename)
{
    string line;               //String holds error line
    inFile.open(filename);     //Attempt to open setup file
    message temp;


    //Check to see if file was not able to open
    if(!inFile.is_open())
    {
        line = "File " + filename + " does not exist";     //Write record to log file

        cout << line << endl;

        return false;
    }

    inFile.close();
    return true;
}

//Parses a line from the setup file
bool parseSetup(string line)
{
    string temp;
    vector<string> tokens;      //Vector holds tokenized commands

    istringstream iss(line);    //Create sstream object

    //This will copy the tokens into the vector container
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(tokens));
    

    //If item is R then set number
    if (tokens[0] == "R")
    {
        bots = stoi(tokens[1]);     //Convert string to int and set

        //Validate the bot input
        if (bots >= 1 && bots <= 5)
        {
            strcpy(R.payload, line.c_str());

            //debug
            cout << "Number of bots is: " << bots << endl;

            return true;
        }
        //Otherwise return false
        return false;
    }
    else if (tokens[0] == "X")
    {
        xMax = stoi(tokens[1]);     //Convert string to int and set

        //Validate the size of X
        if (xMax >= 1 && xMax <= 20)
        {
            strcpy(X.payload,line.c_str());

            //debug
            cout << "X max is: " << xMax << endl;

            return true;
        }

        //Otherwise return false
        return false;
    }
    else if (tokens[0] == "Y")
    {
        yMax = stoi(tokens[1]);     //Convert string to int and set

        //Validate the size of Y
        if (yMax >= 1 && yMax <= 20)
        {
            strcpy(Y.payload,line.c_str());

            //debug
            cout << "Y max is: " << yMax << endl;

            return true;
        }

        //Otherwise return false
        return false;
    }
    else if (tokens[0] == "127.0.0.1")
    {
            strcpy(hostname, line.c_str());

            //debug
            cout << "Server location retrieved" << endl;

            return true;

    }     //If all the above fail then the input is invalid
    else
    {
        tempPortnum = stoi(tokens[0]);  //Convert port number to int

        //Check portnumber validity
        if(tempPortnum < 1024 || tempPortnum > 65535)
        {
            return false;
        }

        temp = to_string(tempPortnum);      //Store the int
        strcpy(port, line.c_str());     //Store into hostname

        cout << "Port number retrieved: " << port << endl;
        return true;
    }
}

//Validates the command file command
void validateCommands(string cfilename)
{
    string line;                //Holds input
    bool processing = true;     //Is true while processing is true
    message command;            //Creates a new message object
    int currentBot;


    inFile.open(cfilename);     //open the inFile

    istream &input = (cfilename.length() > 0) ? inFile : cin;      //Choose between file stream


    //Go while processing
    while(processing)
    {
        getline(input, line);          //Get first line
        currentBot = line[2] - '0';

        //If line is a Q then write to queues
        if(line[0] == 'Q')
        {
            command.from = 0;                       //Set "from"
            strcpy(command.payload, line.c_str());  //Create into the message

            //Write Qs to bot queues
            for(int i = 1; i <= bots; i++)
            {
                SafeQueues[i].enqueue(command);

            }

            inFile.close();
            break;
        }
            //If its M
        else if(line[0] == 'M')
        {
            //If the bot num is valid
            if(line[2] - '0' > 0 && line[2] - '0' < 6)
            {

                if(line[4] == 'N' || line[4] == 'S' || line[4] == 'E' || line[4] == 'W')
                {
                    command.from = 0;                           //Set from
                    strcpy(command.payload, line.c_str());      //Copy string

                    SafeQueues[currentBot].enqueue(command);
                }
            }
        }
            //Otherwise wrong command
        else
        {
            cout << "Command Invalid." << endl;
        }
    }

}

//Parent reads its queue and displays to log
void readParentQueue()
{
    message m;

    //Busy wait on threads to finish
    while(numActiveThreads != 0)
    {}

    int deadbots = 0;

    //Get everything from the queue
    while(deadbots <= bots)
    {

        m = SafeQueues[0].dequeue();

        //If bot is dead then increment
        if(m.payload[0] == 'Q')
        {
            deadbots++;
        }

        //Write to the log
        write(sockdesc,(char*)&m, sizeof(message));	//Send the Q message

    }

}