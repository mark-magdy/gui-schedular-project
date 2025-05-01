#include "header.h"


// Process class implementation

process::process(string name, float arrival_time, float burst_time, int priority) :
	name(name), arrival_time(arrival_time), burst_time(burst_time), remaining_time(burst_time), priority(priority) {
}
void process::setRemainingTime(float remaining_time) { // new 
    this->remaining_time = remaining_time;
}

void process::setFinished(bool TF) { // new 
    this->finished = TF;
}

void process::setStartTime(float start_time) { // new 
    this->start_time = start_time;
}
void process::setTurnAroundTime(float turnaround_time) {
    this->turnaround_time = turnaround_time;
}
void process::addIntervalOfTime(float start_time, float end_time) {
    tuple<float, float> interval = make_tuple(start_time, end_time);
    schedule.push_back(interval);
}

void process::showIntervals() {
    if (!schedule.empty()) {
        for (int i = 0; i < schedule.size(); i++) {
            cout << "Start: " << get<0>(schedule[i]) << endl;
            cout << "End: " << get<1>(schedule[i]) << endl;
        }
    }
}


// Non-const version - returns a copy
//vector<tuple<float, float>> myprocess::getSchedule() {
//    return schedule;
//}

// Const version - returns a const reference (more efficient)
const vector<tuple<float, float>>& process::getSchedule() const {
    return schedule;
}

float process::getArrivalTime() const {
    return arrival_time;
}

float process::getBurstTime()const {
    return burst_time;
}

float process::getRemainingTime() const {
    return remaining_time;
}

float process::getStartTime()const {
    return start_time;
}

bool process::getFinished() const {
    return finished;
}

string process::getName() const {
    return name;
}

int process::getPriority()const {
    return priority;
}
float process::getTurnAroundTime()const {
    return turnaround_time;
}


// Scheduler class implementation
void scheduler::inProgress(algorithm& algo) {
	int time = 0;
	SingletonCounter& glbCounter = SingletonCounter::getInstance();
	{
		std::lock_guard<std::mutex> lock(processMutex);
		for (int i = 0; i < incoming.size(); i++) {           //push the arrived process in ready queue
			process* p = incoming[i];
			if (p->getArrivalTime() <= time) {
				ready_queue.push_back(p);
				incoming.erase(incoming.begin() + i);
				i--;
			}
		}
		
		algo.initReadyQ(ready_queue);
		if (!ready_queue.empty()) {
			ready_queue[0]->setStartTime(0);
		}
	}

	if (live) {
		int curVal = glbCounter.getValue() + 1;
		while (glbCounter.getValue() < (double)curVal) {}
	}

	time++;

	while (time < 20000) {
		std::cout << "Time is: " << time << std::endl;
		
		{
			std::lock_guard<std::mutex> lock(processMutex);
			
				
			if (!ready_queue.empty()) {
				algo.updateProcesses(ready_queue[0], time);
				std::cout << ready_queue[0]->getName() << std::endl;
				algo.updateReadyQ(ready_queue, time);
			}
				for (int i = 0; i < incoming.size(); i++) {           //push the arrived process in ready queue

					process* p = incoming[i];
					if (p->getArrivalTime() <= time) {
						ready_queue.push_back(p);
						if (ready_queue.size() == 1) {
							ready_queue[0]->setStartTime(time);
						}
						incoming.erase(incoming.begin() + i);
						i--;
					}
				}
		}

		if (live) {
			int curVal = glbCounter.getValue()+1 ;
			while (glbCounter.getValue() < (double)curVal) {} 
		}

		time++;
	}
}
float scheduler::avgTurnAround() {
    float sum = 0;
    float turn = 0;
    int size = processes.size();
    for (int i = 0; i < size; i++) {
		if (!processes[i]->getSchedule().empty()) 
        turn = get<1>(processes[i]->schedule.back()) - processes[i]->getArrivalTime();
        sum += turn;
        processes[i]->setTurnAroundTime(turn);
    }
    return sum / size;
	return 200; 
}
float scheduler::avgWaiting() {
    float sum = 0;
    float wait = 0;
    int size = processes.size();
    for (int i = 0; i < size; i++) {
        wait = processes[i]->getTurnAroundTime() - processes[i]->getBurstTime();
        cout << wait << endl;
        sum += wait;
    }
    return sum / size;
}

process* scheduler::getRunningProcess() {
    return ready_queue[0];
}

void scheduler::addProcess(process* p) {
	std::lock_guard<std::mutex> lock(processMutex);
	processes.push_back(p);
	incoming.push_back(p);
}

//void scheduler::addProcess(process* p) {
//    processes.push_back(p);
//    ready_queue.push_back(p);
//}
// FCFS algorithm implementation
void FCFS::updateProcesses(process* proc, int time) {
	float start = proc->getStartTime();
	float burst = proc->getBurstTime();
	proc->setRemainingTime(proc->getRemainingTime() - 1);

	// cout << "rem: " << proc->getRemainingTime() << endl;
	if (proc->getRemainingTime() == 0) {
		proc->setFinished(true);
	}
}

void FCFS::updateReadyQ(vector<process*>& ready_vec, int time) {

	float start = ready_vec[0]->getStartTime();
	float burst = ready_vec[0]->getBurstTime();
	if (!ready_vec.empty()) {

		if (ready_vec[0]->getFinished()) {
			ready_vec[0]->addIntervalOfTime(start, start + burst);
			//  ready_vec[0]->showIntervals();
			ready_vec.erase(ready_vec.begin());

			if (!ready_vec.empty()) {
				sort(ready_vec.begin(), ready_vec.end(), [](process* a, process* b) {
					return a->getArrivalTime() < b->getArrivalTime();
					});
				ready_vec[0]->setStartTime(time);
			}
		}
	}
}

void FCFS::initReadyQ(vector<process*>& ready_vec) {
	sort(ready_vec.begin(), ready_vec.end(), [](process* a, process* b) {
		return a->getArrivalTime() < b->getArrivalTime();
		});
}

//void FCFS::sortReadyQ(vector<myprocess>& ready_vec, int time) {
//    myprocess* before = &ready_vec[0];
//    sort(ready_vec.begin(), ready_vec.end(), [](myprocess& a, myprocess& b) {
//        return a.getArrivalTime() < b.getArrivalTime();
//        });
//    myprocess* after = &ready_vec[0];
//    if (before != after) {
//    }
//    else {
//        cout << "Not Sorted" << endl;
//    }
//}

// SJF algorithm implementation
void SJF::initReadyQ(vector<process*>& ready_vec) {
	sort(ready_vec.begin(), ready_vec.end(), [](process* a, process* b) {



		return a->getRemainingTime() < b->getRemainingTime();
		}
	);

}

//void SJF::sortReadyQ(vector<myprocess>& ready_vec, int time) {
//    // Sorting logic can be added here if needed
//}

void SJF::updateProcesses(process* proc, int time) {
	float start = proc->getStartTime();
	float burst = proc->getBurstTime();
	proc->setRemainingTime(proc->getRemainingTime() - 1);
	//cout << "rem: " << proc.getRemainingTime() << endl;
	if (proc->getRemainingTime() == 0) {
		//cout << "Finished"<<endl;
		proc->setFinished(true);
		//cout <<"Finished ;"<<proc.getFinished() << endl;
	}
}

void SJF::updateReadyQ(vector<process*>& ready_vec, int time) {
	//checking finished or not
	float start = ready_vec[0]->getStartTime();
	float burst = ready_vec[0]->getBurstTime();
	bool erased = false;
	if (ready_vec[0]->getFinished()) {
		ready_vec[0]->addIntervalOfTime(start, time);
		ready_vec.erase(ready_vec.begin());
		erased = true;

		if (ready_vec.empty()) {
			return;
		}

	}
	if (this->preemptive == true) {
		process* before = ready_vec[0];

		sort(ready_vec.begin(), ready_vec.end(), [](process* a, process* b) {


			return a->getRemainingTime() < b->getRemainingTime();
			});

		process* after = ready_vec[0];
		if ((before != after) && erased == false) {

			before->addIntervalOfTime(start, time);
			after->setStartTime(time);



		}
		else
		{
			if (erased)
			{
				if (!ready_vec.empty()) {
					sort(ready_vec.begin(), ready_vec.end(), [](process* a, process* b) {


						return a->getRemainingTime() < b->getRemainingTime();
						});

					ready_vec[0]->setStartTime(time);
				}
			}
		}
	}
	else {
		if (erased)
		{
			if (!ready_vec.empty()) {
				sort(ready_vec.begin(), ready_vec.end(), [](process* a, process* b) {


					return a->getRemainingTime() < b->getRemainingTime();
					});

				ready_vec[0]->setStartTime(time);
			}
		}

	}
}
void RR::initReadyQ(vector<process*>& ready_vec) {
	sort(ready_vec.begin(), ready_vec.end(), [](process* a, process* b) {
		return a->getArrivalTime() < b->getArrivalTime();
		});
}
void RR::updateProcesses(process* proc, int time) {
	float start = proc->getStartTime();
	float burst = proc->getBurstTime();
	proc->setRemainingTime(proc->getRemainingTime() - 1);
	this->rem_q--;

	// cout << "rem: " << proc->getRemainingTime() << endl;
	if (proc->getRemainingTime() == 0) {
		//cout << "Finished"<<endl;
		proc->setFinished(true);
		//cout <<"Finished ;"<<proc.getFinished() << endl;
	}
}
RR::RR(int quantum)  {
	this->quantum = quantum;
	rem_q = quantum;
}
void RR::updateReadyQ(vector<process*>& ready_vec, int time) {
	if (ready_vec.empty()) return;

	float start = ready_vec[0]->getStartTime();
	float burst = ready_vec[0]->getBurstTime();

	if (rem_q == 0 && ready_vec[0]->getFinished()) {
		ready_vec[0]->addIntervalOfTime(start, time);
		//ready_vec[0]->showIntervals();
		ready_vec.erase(ready_vec.begin());
		rem_q = quantum;
		if (!ready_vec.empty()) {
			ready_vec[0]->setStartTime(time);
		}
	}
	else if (rem_q == 0 && !ready_vec[0]->getFinished()) {
		ready_vec[0]->addIntervalOfTime(start, time);

		ready_vec.push_back(ready_vec[0]);
		ready_vec.erase(ready_vec.begin());
		if (!ready_vec.empty()) {
			ready_vec[0]->setStartTime(time);
		}
		rem_q = quantum;
	}
	else if (ready_vec[0]->getFinished()) {
		ready_vec[0]->addIntervalOfTime(start, time);
		//ready_vec[0]->showIntervals();
		ready_vec.erase(ready_vec.begin());
		rem_q = quantum;

		if (!ready_vec.empty()) {
			ready_vec[0]->setStartTime(time);
		}
	}

	else {
		return;
	}
}


void Priority::initReadyQ(vector<process*>& ready_vec) {
	sort(ready_vec.begin(), ready_vec.end(), [](process* a, process* b) {



		return a->getPriority() < b->getPriority();
		}
	);
}

void Priority::updateProcesses(process* proc, int time) {
	float start = proc->getStartTime();
	float burst = proc->getBurstTime();
	proc->setRemainingTime(proc->getRemainingTime() - 1);
	//cout << "rem: " << proc.getRemainingTime() << endl;
	if (proc->getRemainingTime() == 0) {
		//cout << "Finished"<<endl;
		proc->setFinished(true);
		//cout <<"Finished ;"<<proc.getFinished() << endl;
	}
}
void Priority::updateReadyQ(vector<process*>& ready_vec, int time) {
	//checking finished or not
	float start = ready_vec[0]->getStartTime();
	float burst = ready_vec[0]->getBurstTime();
	bool erased = false;
	if (ready_vec[0]->getFinished()) {
		ready_vec[0]->addIntervalOfTime(start, time);
		ready_vec.erase(ready_vec.begin());
		erased = true;

		if (ready_vec.empty()) {
			return;
		}

	}
	if (this->preemptive == true) {
		process* before = ready_vec[0];

		sort(ready_vec.begin(), ready_vec.end(), [](process* a, process* b) {


			return a->getPriority() < b->getPriority();
			});

		process* after = ready_vec[0];
		if ((before != after) && erased == false) {

			before->addIntervalOfTime(start, time);
			after->setStartTime(time);



		}
		else
		{
			if (erased)
			{
				if (!ready_vec.empty()) {
					sort(ready_vec.begin(), ready_vec.end(), [](process* a, process* b) {


						return a->getPriority() < b->getPriority();
						});

					ready_vec[0]->setStartTime(time);
				}
			}
		}
	}
	else {
		if (erased)
		{
			if (!ready_vec.empty()) {
				sort(ready_vec.begin(), ready_vec.end(), [](process* a, process* b) {


					return a->getPriority() < b->getPriority();
					});

				ready_vec[0]->setStartTime(time);
			}
		}

	}
}
