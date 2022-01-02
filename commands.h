

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>

#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO{
public:
	virtual string read()=0;
	virtual void write(string text)=0;
	virtual void write(float f)=0;
	virtual void read(float* f)=0;
	virtual ~DefaultIO(){}
};

class StandardIO  : public DefaultIO{

    // reads from the user
    virtual string read(){
        string input;
        cin >> input;
        return input;
    }

    // writes to screen
    virtual void write(string text){
        cout << text;
    }

    // writes to screen
    virtual void write(float f){
        cout << f;
    }

    // reads from the user
    virtual void read(float* f){
        cin >> *f;
    }
};


// you may edit this class
class Command{
	DefaultIO* dio;
    string description;
    HybridAnomalyDetector *detector;
public:
	Command(DefaultIO* dio, string des):dio(dio){
        description = des;
    }
	virtual void execute()=0;
	virtual ~Command(){}

    // returns the command's description
    string getDesc() { return description; }

    // returns the command's IO
    DefaultIO* getIO() { return dio; }

    // sets a detector
    void setDetector(HybridAnomalyDetector *det){ detector = det; }

    // returns the detector
    HybridAnomalyDetector* getDetector() { return detector; }
};

//     ~ Command Classes ~

// For command 1: used to upload a timeseries csv file
class Command1 : public Command {
public:
    Command1(DefaultIO* dio) : Command(dio,  "upload a time series csv file"){ }

    virtual void execute(){
        DefaultIO* io = getIO();
        string input = "";
        ofstream train;
        ofstream test;

        // uploading to anomalyTrain.csv
        train.open("anomalyTrain.csv");
        io->write("Please upload your local train CSV file.\n");
        input = io->read();
        while (input.compare("done") != 0) {
            train << input << "\n";
            input = io->read();
        }
        train.close();
        io->write("Upload complete.\n");

        // uploading to anomalyTest.csv
        test.open("anomalyTest.csv");
        io->write("Please upload your local test CSV file.\n");
        input = io->read();
        while (input.compare("done") != 0) {
            test << input << "\n";
            input = io->read();
        }
        test.close();
        io->write("Upload complete.\n");
    }
};

// For command 2: used to set algorithm settings
class Command2 : public Command{

    // check if the float is between 0 and 1
    bool checkThres(float t) {
        return (1 >= t && 0 <= t);
    }
public:
    Command2(DefaultIO* dio) : Command(dio,  "algorithm settings"){}

    // show the current threshold and changes it
    virtual void execute(){
        DefaultIO* io = getIO();
        HybridAnomalyDetector* detector = getDetector();

        // show the threshold
        io->write("The current correlation threshold is ");
        io->write(detector->getThreshold());
        io->write("\n");

        // change the threshold
        io->write("Type a new threshold\n");
        float newThres = stof(io->read());
        bool isValid = checkThres(newThres);
        while (!isValid) {
            io->write("please choose a value between 0 and 1.\n");
            newThres = stof(io->read());
            isValid = checkThres(newThres);
        }
        detector->setThreshold(newThres);
    }
};

// For command 3: used to detect anomalies
class Command3 : public Command{
    vector<AnomalyReport> *report; // "shared" with command4

public:
    Command3(DefaultIO* dio, vector<AnomalyReport>* rep) :
                             Command(dio,  "detect anomalies"){
    report = rep;
    }

    // run the Hybrid detector on the csv files
    virtual void execute(){
        HybridAnomalyDetector *detector = getDetector();
        DefaultIO *io = getIO();

        // run learnNormal for train input
        TimeSeries trainTs("anomalyTrain.csv");
        detector->learnNormal(trainTs);

        // run detect for test input
        TimeSeries TestTs("anomalyTest.csv");
        vector<AnomalyReport> rep = detector->detect(TestTs);

        // insert the detect result to the "shared" report
        for (auto c = rep.begin(); c != rep.end(); c++) {
            report->push_back(*c);
        }

        io->write("anomaly detection complete.\n");

    }
};

// For command 4: used to display results
class Command4 : public Command{
    vector<AnomalyReport> *report; // "shared" with command3

public:
    Command4(DefaultIO* dio, vector<AnomalyReport>* rep)
                             : Command(dio,  "display results"){
    report = rep;
    }

    virtual void execute(){
        DefaultIO* io = getIO();
        string line = "";

        // writing the report
        cout << report << endl;
        for (auto c = report->begin(); c != report->end(); c++) {
            io->write(c->timeStep);
            io->write("\t" + c->description + "\n");
        }
        io->write("Done.");
    }
};

// For command 5: used to upload anomalies and analyze results
class Command5 : public Command{
public:
    Command5(DefaultIO* dio) : Command(dio,  "upload anomalies and analyze results"){}

    virtual void execute(){}
};

// For command 6: used to exit
class Command6 : public Command{
public:
    Command6(DefaultIO* dio) : Command(dio,  "exit"){}

    virtual void execute(){

    }
};


#endif /* COMMANDS_H_ */
