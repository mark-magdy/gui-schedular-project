//#pragma once
//#ifndef PROCESS_H
//#define PROCESS_H
//
//#include <string>
//#include <vector>
//#include <tuple>
//
//class myprocess {
//protected:
//    std::string name;
//    bool finished;
//    int priority;
//    float arrival_time;
//    float start_time;
//    float burst_time;
//    float remaining_time;
//public:
//    std::vector<std::tuple<float, float>> schedule;
//
//    myprocess(std::string n, int p, float at, float bt);
//
//    std::string getName() const;
//    bool isFinished() const;
//    int getPriority() const;
//    float getArrivalTime() const;
//    float getBurstTime() const;
//    float getRemainingTime() const;
//    const std::vector<std::tuple<float, float>>& getSchedule() const;
//
//    void addScheduleTime(float start, float end);
//};
//
//#endif // PROCESS_H

//#pragma once
//#include <iostream>
//#include <windows.h>
//#include <vector>
//#include <tuple>
//#include <algorithm>
//using namespace std;
//class myprocess {
//protected:
//    string name;
//    bool finished = false;
//    int priority;
//    float arrival_time;
//    float start_time;
//    float burst_time;
//    float remaining_time;
//public:
//    vector<tuple<float, float>> schedule; // each tuple has a start and an endtime
//    myprocess(string name, float arrival_time, float burst_time, int priority);
//
//    void setRemainingTime(float remaining_time);
//    void setFinished(bool TF);
//    void setStartTime(float start_time);
//    void addIntervalOfTime(float start_time, float end_time); // Remove qualified name
//    void showIntervals();
//    float getArrivalTime() const;
//    float getBurstTime() const;
//    float getRemainingTime() const;
//    float getStartTime()const;
//    bool getFinished() const;
//    string getName()const;
//    int getPriority()const;  // New getter for priority
//    //vector<tuple<float, float>> getSchedule();
//    const vector<tuple<float, float>>& getSchedule() const;  // Add this line};
//};