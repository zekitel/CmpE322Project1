#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <queue>
#include <cstdlib>
using namespace std;
class Process {
    int arrivalTime;
    string processId;
    int processType;
public:
    queue<int> instructionQueue;
    Process(string processId){
        this->processId=processId;
    }
    int getArrivalTime() const {
        return arrivalTime;
    }
    void setArrivalTime(int arrivalTime) {
        Process::arrivalTime = arrivalTime;
    }
    const string &getProcessId() const {
        return processId;
    }
    void setProcessId(const string &processId) {
        Process::processId = processId;
    }
    int getProcessType() const {
        return processType;
    }
    void setProcessType(int processType) {
        Process::processType = processType;
    }
    Process operator=( Process rhs)  {
        this->setArrivalTime(rhs.arrivalTime);
        this->setProcessId(rhs.processId);
        this->instructionQueue=rhs.instructionQueue;
        this->setProcessId(rhs.processId);
        return *this;
    }
};
//the array that keeps the all processes
vector<Process> processArray;
//includes all instructions in 2-dimensional array
vector<vector<int> > instructionCodes;
//reads the file of instruction code
void instrCodeReading(char  codeName[]){
    ifstream codes(codeName);

    string line11;
    vector<string> tempArray; //ifstream reads the lines as strings, the array keeps the words of lines
    vector<int> tempArray2;
    //reads the line with the getline function and puts the value in the 2-dimensional array
    while(getline(codes,line11)){
        istringstream iss(line11);//input string stream
        while(iss){
            string word;
            iss>>word;
            if(word.compare("")){
                tempArray.push_back(word);
            }
        }
    }

    //returns the strings that is read from the file to integer and push them in to the integer vector
    for (int i = 0; i <tempArray.size() ; ++i) {
        if(i%2==1){
            int value = atoi(tempArray[i].c_str());
            tempArray2.push_back(value);
        }
    }
    instructionCodes.push_back(tempArray2);
}
//reads the definition file and fill in the process array with value of the processes
void fileReading(char  fileName[]){
    ifstream processDefinition;
    processDefinition.open(fileName);
    string line;
    vector<string> temp1;
    while(getline(processDefinition,line)){
        istringstream iss(line);
        while(iss){
            string word;
            iss>>word;
            if(word.compare("")){
                temp1.push_back(word);
            }
        }
    }
    //creates the object of Process and fill the fields of that  object with the info from the file
    for (int i = 0; i <(temp1.size()/3)+temp1.size()%3 ; i++) {
        //first column is about the name
        string processName=temp1[3*i];
        //second column is  about the set of instruction
        int instrType=atoi(temp1[(3*i)+1].substr(0,1).c_str());
        //third column indicates the arrival time
        int arrivalTime=atoi(temp1[(3*i)+2].c_str());
        Process *a=new Process(processName);
        a->setArrivalTime(arrivalTime);
        a->setProcessId(processName);
        a->setProcessType(instrType);
        // puts the set  of instructions into the queue of process
        for(int j=0;j<instructionCodes[instrType-1].size();j++){
            a->instructionQueue.push(instructionCodes[instrType-1][j]);
        }
        processArray.push_back(*a);
    }
    processDefinition.close();
}
//checks whether  the time is arrival or scheduled  time
bool isArrival(int time){
    if(time%10==1){
        return true;
    }
    else{
        return false;
    }
}
//prints the status of the queue in any time
void printStatusOfQueue(int time, queue<Process> temp){
    queue<Process> temp2;
    if(temp.empty()){
        cout<<time<<"::HEAD--TAIL"<<endl;
        return;
    }
    cout<<time<<"::HEAD-";
    while(!temp.empty()){
        Process a=temp.front();
        cout<<a.getProcessId()<<"-";
        temp2.push(a);
        temp.pop();
    }
    cout<<"TAIL"<<endl;
    temp=temp2;
}
//calculates  the process time ,  if it is zero,it shows that  process is ended
int getProcessedTime(Process &a){
    int  temp=0;
    int quantum=100;

    if(a.instructionQueue.size()==0){
        return 0;
    }
    //this loop  works for that sums the instructions until quantum is reached or excessed by one unit
    do {
        if(a.instructionQueue.size()==0){
            return temp;
        }
        temp=temp+a.instructionQueue.front();
        a.instructionQueue.pop();
    }while(temp<quantum);
    return temp;

}
//checks whether the  process  is ended  or not
//looks at the instr queue of  that process
bool isProcessEnded(Process a){
    if(a.instructionQueue.size()==0){
        return true;
    }
    return false;
}
//scheduler function
void processScheduler(){
    priority_queue<int,vector<int>,greater<int> > time; //min-priority queue
    queue<Process> readyQueue; //keeps  the processes in the queue
    //At the beginning, pushes the all arival times into the time queue as  the format "arrivaltime1"
    //the  format  indicates that last  digit  means it is an arrival  time,  If  the last digit is 2, It means it is scheduled time.
    for(int i=0;i<processArray.size();i++){
        time.push((10*processArray[i].getArrivalTime())+1);
    }
    //For the first element, triggered part
    int time2=time.top();
    time.pop();
    readyQueue.push(processArray[0]);
    int timeCurrent=10*(getProcessedTime(readyQueue.front()))+2;
    Process temp=readyQueue.front();
    readyQueue.pop();
    readyQueue.push(temp);
    time.push(timeCurrent+time2-1);
    printStatusOfQueue(time2/10,readyQueue);
    //After the first process is rescheduled, Then it continues in the loop until the time loop is empty
    while(time.size()!=0){
        //when the time is arrival, then the  process is pushed back to the queue then time-queue is poped
        if(isArrival(time.top())){
            int tempTime=time.top()/10;
            for(int i=0;i<processArray.size();i++){
                if((time.top()/10)==processArray[i].getArrivalTime()){
                    readyQueue.push(processArray[i]);
                }
            }
            time.pop();
            printStatusOfQueue(tempTime,readyQueue);//prints the queue
        }
        //when the time is scheduled-time, pushes the process back into the ready-queue unless the process is finished
        else{
            int tempTime=time.top()/10;
            //when the process is ended, the queue is pop out, nothing will be repushed.
            if(isProcessEnded(readyQueue.front())){
                time.pop();
                readyQueue.pop();
                if(!readyQueue.empty()){
                    Process a=readyQueue.front();
                    int temp=10*getProcessedTime(readyQueue.front())+2;
                    time.push(temp+(tempTime*10));
                }
            }
                //schedules the process and repushes it the queue
            else{
                time.pop();
                Process a=readyQueue.front();
                readyQueue.pop();
                readyQueue.push(a);
                int temp=10*getProcessedTime(readyQueue.front())+2;
                time.push(temp+(tempTime*10));
            }
            printStatusOfQueue(tempTime,readyQueue);
        }
    }
}
int main() {
    //reads the instr files
    /*
    instrCodeReading("C:\\Users\\ZekiTel\\CLionProjects\\CmpE322Project1\\1.code.txt");
    instrCodeReading("C:\\Users\\ZekiTel\\CLionProjects\\CmpE322Project1\\2.code.txt");
    instrCodeReading("C:\\Users\\ZekiTel\\CLionProjects\\CmpE322Project1\\3.code.txt");
    instrCodeReading("C:\\Users\\ZekiTel\\CLionProjects\\CmpE322Project1\\4.code.txt");
    //reads the definition file
    fileReading("C:\\Users\\ZekiTel\\CLionProjects\\CmpE322Project1\\definition.txt");
    //scheduler function
    processScheduler();
     */

    //reads the instr files
    /*
    instrCodeReading("1.code.txt");
    instrCodeReading("2.code.txt");
    instrCodeReading("3.code.txt");
    instrCodeReading("4.code.txt");
    //reads the definition file
    fileReading("definition.txt");
    //scheduler function
    processScheduler();
     */
    return 0;
}





