#pragma once
#include <iostream>
#include <windows.h>
#include <vector>
#include <tuple>
#include <algorithm>
#include <thread>
#include <chrono>
#include <mutex>  
#include "SingletonCounter.h"
using namespace std;
class process {
protected:
    string name;
    bool finished = false;
    int priority;
    float arrival_time;
    float start_time;
    float burst_time;
    float remaining_time;
    float turnaround_time;

public:
    vector<tuple<float, float>> schedule; // each tuple has a start and an endtime
    process(string name, float arrival_time, float burst_time, int priority);

    void setRemainingTime(float remaining_time);
    void setFinished(bool TF);
    void setStartTime(float start_time);
    void setTurnAroundTime(float turnaround_time);
    void addIntervalOfTime(float start_time, float end_time); // Remove qualified name
    void showIntervals();
    float getArrivalTime() const;
    float getBurstTime() const;
    float getRemainingTime() const;
    float getStartTime()const;
    float getTurnAroundTime()const;
    bool getFinished() const;
    string getName()const;
    int getPriority()const;  // New getter for priority
    //vector<tuple<float, float>> getSchedule();
    const vector<tuple<float, float>>& getSchedule() const;  // Add this line};
};

class algorithm {

public:
    bool preemptive;

    virtual void updateProcesses(process* proc, int time) = 0; //to be overridden
    virtual void updateReadyQ(vector<process*>& ready_vec, int time) = 0;  //to be overridden
    virtual void initReadyQ(vector<process*>& ready_vec) = 0; //to be overridden
};

class scheduler {
private:
    static inline std::mutex instanceMutex; // For thread-safe singleton
    static inline scheduler* instance = nullptr;

    // Make constructor private
    scheduler() = default;

public:
    std::mutex processMutex; // Now the only mutex for process access
    std::vector<process*> processes;
    std::vector<process*> ready_queue;
    vector<process*> incoming;
    bool live = false;

    static scheduler* getInstance() {
        std::lock_guard<std::mutex> lock(instanceMutex);
        if (!instance) {
            instance = new scheduler();
        }
        return instance;
    }

    void inProgress(algorithm& algo);
    process* getRunningProcess();
    void addProcess(process* p);
    float avgTurnAround();
    float avgWaiting();
};


class FCFS : public algorithm {
public:
    void updateProcesses(process* proc, int time);
    void updateReadyQ(vector<process* >& ready_vec, int time);
    void initReadyQ(vector<process*>& ready_vec);


};

class SJF : public algorithm {
public:
    void updateProcesses(process* proc, int time);
    void updateReadyQ(vector<process* >& ready_vec, int time);
    void initReadyQ(vector<process*>& ready_vec);

};

class Priority : public algorithm {
public:
    void updateProcesses(process* proc, int time);
    void updateReadyQ(vector<process* >& ready_vec, int time);
    void initReadyQ(vector<process*>& ready_vec);
};

class RR : public algorithm {
public:
    RR(int quantum);
    int quantum ;
    int rem_q ;
    void updateProcesses(process* proc, int time);
    void updateReadyQ(vector<process* >& ready_vec, int time);
    void initReadyQ(vector<process*>& ready_vec);
};