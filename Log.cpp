//*****************************************
//  File name: Log.cpp
//  Project Name: hw4
//  Author: Colin Kelley
//  Date: 1/17/2019
//
//*****************************************

#include "Log.h"

using namespace std;

//Constant that is the default log file name
const string Log::DEFAULT_LOG_FILE_NAME = "log.txt";

// Get the timestamp value
string Log::getTimeStamp()
{
    string strTime;     //String holds time

    time_t curr_time;        //Create time_t object
    curr_time = time(NULL);  //get time

    char *tm = ctime(&curr_time);   //Call ctime to get exact time in characters

    strTime = string(tm);        //Cast tm to a string

    return strTime;
}

//Default log constructor
Log::Log()
{
    setLogfileName(DEFAULT_LOG_FILE_NAME);      //Set the log file name to default
}

// Overloaded char* constructor
Log::Log(char* lname)
{
    string name = string(lname);        //Cast lname to a string
    setLogfileName(name);               //Set log name equal to name
}

// Overloaded string class constructor
Log::Log(string lname)
{
    setLogfileName(lname);              //Call the setter for the file name
}

//********************************************
// Setters
//********************************************

// set the log file name
void Log::setLogfileName(string cname)
{
    logfilename = cname;    //Set logfilename to cname
}

//*******************************************
// Getters
//*******************************************


// Get the log file name
string Log::getLogfileName()
{
    string name;            //String holds name

    name = logfilename;     //Set the string equal to logfilename
    return name;
}

//*******************************************
// Log functions
//*******************************************

// Return SUCCESS or FAILURE
//
// open the log with timestamp
int Log::open( )
{
    string name = getLogfileName();     //Get file name and store in string

    logF.open(name, ios::out | ios::app);        //Attempt to open the file

    if(!logF.good())
    {
        return FAILURE;
    }
    //If it was able to open return success
    if (logF.is_open())
    {
        writeLogRecord("BEGIN");    //Write begin record
        return SUCCESS;
    }
        //Otherwise return failure
    else
    {
        return FAILURE;
    }
}

// close the log with timestamp
int Log::close( )
{
    writeLogRecord("END");     //Write done record
    logF.close();              //Close the log file

    //See if file is open
    if (logF.is_open())
    {
        return FAILURE;
    }
        //Otherwise return success
    else
    {
        return SUCCESS;
    }
}

// Write a string to the log
int Log::writeLogRecord(string s)
{
    logF << getTimeStamp(); //Write a time stamp to the file
    logF << s << endl;      //Append the string to file

    //Check to see if write was successful
    if (logF.bad())
    {
        return FAILURE;
    }
        //Otherwise return success
    else
    {
        return SUCCESS;
    }

}


