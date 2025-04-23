//#include "process.h"
//
//myprocess::myprocess(std::string n, int p, float at, float bt) :
//    name(n), priority(p), arrival_time(at), burst_time(bt), remaining_time(bt), finished(false) {
//}
//
//std::string myprocess::getName() const { return name; }
//bool myprocess::isFinished() const { return finished; }
//int myprocess::getPriority() const { return priority; }
//float myprocess::getArrivalTime() const { return arrival_time; }
//float myprocess::getBurstTime() const { return burst_time; }
//float myprocess::getRemainingTime() const { return remaining_time; }
//const std::vector<std::tuple<float, float>>& myprocess::getSchedule() const { return schedule; }
//
//void myprocess::addScheduleTime(float start, float end) {
//    schedule.emplace_back(start, end);
//    remaining_time -= (end - start);
//    if (remaining_time <= 0) {
//        finished = true;
//        remaining_time = 0;
//    }
//}


//#include "process.h"
//
//// Process class implementation
//
//myprocess::myprocess(string name, float arrival_time, float burst_time, int priority) :
//    name(name), arrival_time(arrival_time), burst_time(burst_time), remaining_time(burst_time), priority(priority) {
//}
//void myprocess::setRemainingTime(float remaining_time) {
//    this->remaining_time = remaining_time;
//}
//
//void myprocess::setFinished(bool TF) {
//    this->finished = TF;
//}
//
//void myprocess::setStartTime(float start_time) {
//    this->start_time = start_time;
//}
//
//void myprocess::addIntervalOfTime(float start_time, float end_time) {
//    tuple<float, float> interval = make_tuple(start_time, end_time);
//    schedule.push_back(interval);
//}
//
//void myprocess::showIntervals() {
//    if (!schedule.empty()) {
//        for (int i = 0; i < schedule.size(); i++) {
//            cout << "Start: " << get<0>(schedule[i]) << endl;
//            cout << "End: " << get<1>(schedule[i]) << endl;
//        }
//    }
//}
//// Non-const version - returns a copy
////vector<tuple<float, float>> myprocess::getSchedule() {
////    return schedule;
////}
//
//// Const version - returns a const reference (more efficient)
//const vector<tuple<float, float>>& myprocess::getSchedule() const {
//    return schedule;
//}
//
//float myprocess::getArrivalTime() const {
//    return arrival_time;
//}
//
//float myprocess::getBurstTime()const {
//    return burst_time;
//}
//
//float myprocess::getRemainingTime() const {
//    return remaining_time;
//}
//
//float myprocess::getStartTime()const {
//    return start_time;
//}
//
//bool myprocess::getFinished() const {
//    return finished;
//}
//
//string myprocess::getName() const {
//    return name;
//}
//
//int myprocess::getPriority()const {
//    return priority;
//}
