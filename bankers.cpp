/********************
Name: Harsh Patel
N10425496
Lab 3
Banker's Algorithm
**********************/

#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<queue>
#include<fstream>

using namespace std;

class Task{
public:
  string state; //Ready;Blocked;Computing;Terminated;
  int id;
  int compute_time; 
  int finish_time;
  int wait_time;
  vector<int> resource_hold; //How many instances of each resource type does the task currently hold?
  void task_output();
};

class Data{
public:
  string activity;
  int number_requested,number_released,number_cycles;
  int task_number,resource_type,initial_claim;
  int current_cycle;
  bool done;
  int compute_time;
  void set_values(string activity, int number_requested, int number_released, int number_cycles,int task_number, int resource_type, int initial_claim, int current_cycle, bool done, int compute_time);
  
  //Display Functions.
  void output();

}; 

//Globals
vector<int> Available; //LENGTH M for M Resources.
vector<int> Max[1000];
vector<int> Allocation[1000];
vector<int> Need[1000];
vector<Data> store[100]; // Structure that stores the input.
vector<Data>::iterator it;
vector<bool> finished_fifo,finished_bankers;
vector<bool> state;//Tracks the state of the system-- for deadlocks.
queue<int> fifo_Q; 
vector<Task> T; //Vector of type Task. 
// Need == Max - Allocation

void Data::set_values(string activity, int number_requested, int number_released, int number_cycles, int task_number, int resource_type, int initial_claim, int current_cycle, bool done, int compute_time){
  this->activity = activity;
  this->number_requested = number_requested;
  this->number_released = number_released;
  this->number_cycles = number_cycles;
  this->task_number = task_number;
  this->resource_type = resource_type;
  this->initial_claim = initial_claim;
  this->current_cycle = current_cycle;
  this->done = done;
  this->compute_time = compute_time;
}

void Data::output(){
  cout<<this->activity<<" ";
  cout<<this->number_requested<<" "<<this->number_released<<" "<<this->number_cycles<<" "<<this->task_number<<" "<<this->resource_type<<" "<<this->initial_claim<<" "<<this->current_cycle<<" "<<this->done<<endl;
}

void Task::task_output(){
  cout<<this->id<<" "<<this->state<<" "<<this->finish_time<<" "<<this->wait_time<<endl;
  
}

void Deadlock(int tasks, int resource_type){ //Modifies the Available and the resource hold vectors. 
  bool flag = false;
  cout<<"Deadlock Detected! Resolving..."<<endl;
  for(int i = 0; i < tasks; i++){
    if(flag)
      break;
    if(T[i].state == "Blocked"){
      T[i].state = "Ready";
      for(int d = 0; d < resource_type; d++){
	if(T[i].resource_hold[i] > 0){
	Available[resource_type] += T[i].resource_hold[i];
	T[i].resource_hold[i] = 0;
	flag = true;
	}
      }
    }
  }
}

//Tasks run in parallel. Task state: On Hold/Computing/Terminated/Ready. 
//IF a run is successful done set to true. 
//1 Initiate 2 Initiate 3 Initiate 1 Request 2 Request 3 Request. 1 Release 2 Release 
void simulate_fifo(int tasks, int resource_type){ //Simulate the task run.
  bool fifo_finished = false;
  int counter = 0,count = 0;
  while(!fifo_finished && counter < 10){
    for(int i = 0; i < tasks; i++){
      for(it = store[i].begin(); it != store[i].begin() + 1; it++){
	counter++;
	it->output();
	if(T[it->task_number - 1].state == "Computing"){
	  T[it->task_number - 1].compute_time -= 1;
	  if(T[it->task_number - 1].compute_time == 0)
	    T[it->task_number - 1].state = "Ready";
	}
	else if(it->activity == "initiate"){
	  //Do Nothing since this is fifo.
	}
	else if(it->activity == "request"){
	  // If resources are available grant the request else push into the wait Queue. 
	  if(Available[it->resource_type] > it->number_requested){
	    Available[it->resource_type] -= it->number_requested;
	    T[it->task_number - 1].resource_hold[it->resource_type] += it->number_requested; //Update Tasks resource hold list.
	  }
	  else{
	    // Q.push_back(Available[it->task_number]); //Push the blocked task onto the queue. 
	    T[it->task_number - 1].state = "Blocked";
	    T[it->task_number - 1].wait_time++;
	  }
	}
	else if(it->activity =="release"){
	  if(T[it->task_number - 1].state != "Computing")
	    Available[it->resource_type] += it->number_released;
	  T[it->task_number - 1].resource_hold[it->resource_type] -= it->number_released; //Update Tasks resource hold list. 
	}
	else if(it->activity == "compute"){
	  //Set the state to computing; Set the clock.
	  if(T[it->task_number - 1].state != "Computing"){
	    T[it->task_number - 1].state = "Computing";
	    T[it->task_number - 1].compute_time = it->number_cycles;
	  }
	}
	else if(it->activity == "terminate"){
	  if(T[it->task_number - 1].state != "Computing"){
	    T[it->task_number -1 ].state = "Terminated";
	    T[it->task_number - 1].finish_time = counter - 1;
	  }
	}
      }
    }
    count = 0;
  for(int i = 0; i < tasks; i++){
    if(T[i].state == "blocked"){
      count++;
    }
  }
  if(count == tasks)
    Deadlock(tasks,resource_type);
  
  count = 0;
  for(int i = 0; i < tasks; i++){
    if(T[i].state == "Terminated")
      count++;
  }
  if(count == tasks)
    fifo_finished = true;
  store[i].erase(0);
  
  }
}


int main()
{
  freopen("bankers","r",stdin);
  int tasks,units,task_number,resource_type,initial_claim;
  int number_requested,number_released,number_cycles;
  string activity; //Types:Initiate;Request;Release;Compute:Terminate;
  int counter = 0; 
  int prev_task = 1;
  Data current;
  Task run;
  while(cin>>tasks>>resource_type){
    
    Available.push_back(0);
    for(int i = 0; i < tasks; i++){ //Initialize each Task.
      run.id = i+1;
      run.state ="Ready";
      run.compute_time = 0;
      run.finish_time = 0;
      run.wait_time = 0;
      for(int d = 0; d < resource_type; d++){
	run.resource_hold.push_back(0);
      }
      T.push_back(run);              
    }
    for(int i = 0; i < resource_type; i++){
      cin>>units;
      Available.push_back(units);
    }
    cout<<tasks<<" "<<resource_type<<" ";
    for(int i = 0; i <= resource_type; i++)
      cout<<Available[i]<<" ";
    cout<<endl;
    for(int i = 0; i <= tasks; i++){
      finished_fifo.push_back(false);
      finished_bankers.push_back(false);
    }
    while(cin>>activity){
      counter++; //Counter Increments for each cycle and resets itself on termination.
      if(activity == "initiate"){
	cin>>task_number>>resource_type>>initial_claim;
	current.set_values(activity,0,0,0,task_number,resource_type,initial_claim,counter,false,0);
	store[task_number].push_back(current);
	//Process
      }
      else if(activity == "request"){
	cin>>task_number>>resource_type>>number_requested;
	//Process
	current.set_values(activity,number_requested,0,0,task_number,resource_type,0,counter,false,0);
	store[task_number].push_back(current);
      }	
      else if(activity == "release"){
	cin>>task_number>>resource_type>>number_released;
	//Process
	current.set_values(activity,0,number_released,0,task_number,resource_type,0,counter,false,0);
	store[task_number].push_back(current);
      }
      else if(activity == "compute"){
	cin>>task_number>>number_cycles;
 	//Process
	current.set_values(activity,0,0,number_cycles,task_number,0,0,counter,false,0);
	store[task_number].push_back(current);

      }
      else if(activity == "terminate"){
	cin>>task_number;
	//Process
	current.set_values(activity,0,0,0,task_number,0,0,counter,false,0);
	store[task_number].push_back(current);
	counter = 0;
      }
    }
  }

  for(int i = 0; i < tasks; i++){
    fifo_Q.push(i+1);
  }
  simulate_fifo(tasks,resource_type); //
  for(int i = 0; i < tasks; i++)
    T[i].task_output();
  /* for(int i = 0; i < tasks; i++){
    for(it = store[i].begin(); it != store[i].end(); it++){
      it->output();
    }
  }
  */
  
  return 0;
}


