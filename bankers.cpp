/********************
Name: Harsh Patel
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
  string state; //Ready;Blocked;Computing;Terminated;Aborted;
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
vector<int> Collect;
vector<int> Available; //LENGTH M for M Resources.
vector<int> Max[1000];
vector<int> Allocation[1000];
vector<int> Need[1000];
vector<Data> store[100]; // Structure that stores the input.
vector<Data>::iterator it;
vector<bool> finished_fifo,finished_bankers;
vector<bool> state;//Tracks the state of the system-- for deadlocks.
deque<int> Q; 
vector<bool> visited;
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



bool Compute(Data current,int counter){
  bool flag = true;
////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(T[current.task_number - 1].state == "Computing"){
    T[current.task_number - 1].compute_time -= 1;
    if(T[current.task_number - 1].compute_time == 0)
      T[current.task_number - 1].state = "Ready";
    flag = false;
  }
  else if(current.activity == "initiate"){
    //Do Nothing since this is fifo.
  }
  else if(current.activity == "request"){
    // If resources are available grant the request else push into the wait Queue. 
    if(Available[current.resource_type] >= current.number_requested){
      Available[current.resource_type] -= current.number_requested;
      T[current.task_number - 1].resource_hold[current.resource_type] += current.number_requested; //Update Tasks resource hold list.
    }
    else{
      if(T[current.task_number -1].state != "Aborted" && T[current.task_number - 1].state != "Terminated")
      Q.push_back(current.task_number); //Push the blocked task onto the queue. 
      T[current.task_number - 1].state = "Blocked";
      T[current.task_number - 1].wait_time++;
      flag = false;
    }
  }
  else if(current.activity == "release"){
    if(T[current.task_number - 1].state != "Computing")
      Collect[current.resource_type] += current.number_released;
    T[current.task_number - 1].resource_hold[current.resource_type] -= current.number_released; //Update Tasks resource hold list. 
  }
  else if(current.activity == "compute"){
    //Set the state to computing; Set the clock.
    if(T[current.task_number - 1].state != "Computing"){
      T[current.task_number - 1].state = "Computing";
      T[current.task_number - 1].compute_time = current.number_cycles;
    }
  }
  else if(current.activity == "terminate"){
    if(T[current.task_number - 1].state != "Computing"){
      T[current.task_number -1 ].state = "Terminated";
      T[current.task_number - 1].finish_time = counter - 1;
    }
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  return flag;
}

int Deadlock(int tasks, int resource_type, int counter){
  bool flag = false;
  cout<<"Deadlock detected!, Resolving..."<<endl;
  for(int i = 0; i < tasks; i++){
    for(int d = 1; d <= resource_type; d++){
      Available[d] += T[i].resource_hold[d];
      T[i].state = "Aborted";
      store[i+1].erase(store[i+1].begin(),store[i+1].end());
      for(int j = 0; j < Q.size(); j++ )
	if(Q[j] == T[i].id)
	  Q.erase(Q.begin() + j);
      int n = Q.front();
      Data current = store[n].front();
      if(Compute(current,counter)){
	flag = true;
	store[n].erase(store[n].begin());
	counter++;
	break;
      }
    }
    if(flag == true)
      break;
  }
  for(int i = 0; i < tasks; i++)
    if(T[i].state =="Blocked")
      T[i].state="Ready";
  return counter;
}

//Tasks run in parallel. Task state: On Hold/Computing/Terminated/Ready. 
//IF a run is successful done set to true. 
//1 Initiate 2 Initiate 3 Initiate 1 Request 2 Request 3 Request. 1 Release 2 Release 
void simulate_fifo(int tasks, int resource_type){ //Simulate the task run.
  bool fifo_finished = false;
  int counter = 0,count = 0;
  int k = 0;
  Data current;
while(!fifo_finished && counter < 25){
  counter++;
  for(int i = 1; i <= tasks; i++){
    visited[i] = false;
  }
  
  // cout<<"Q-Size "<<Q.size()<<endl;
  while(!Q.empty()){
    int n = Q.front();
    Q.pop_front();
    if(visited[n]) 
      break;
    if(store[n].size() > 0){
      visited[n] = true;
      current = store[n].front();
      current.output();                            //DEBUG
      if(Compute(current,counter)) 
	store[n].erase(store[n].begin());
     
    }
  }
  for(int i = 1; i <= tasks; i++){
      if(store[i].size() > 0 && visited[i] == false){
	current = store[i].front(); 
	current.output();
	if(Compute(current,counter))
	  store[i].erase(store[i].begin());
	visited[i] = true;
    }
  }
  for(int i = 1; i <=resource_type; i++){
      Available[i] += Collect[i];
      Collect[i] = 0;
    }
 
      count = 0;
  for(int i = 0; i < tasks; i++){
    if(T[i].state == "Terminated" || T[i].state =="Aborted")
      count++;
  }
  if(count == tasks){
    fifo_finished = true;
    break;
  }
   count = 0;
  for(int i = 0; i < tasks; i++){
    if(T[i].state == "Blocked"||T[i].state=="Aborted"||T[i].state =="Terminated"){
      count++;
    }
  }
  if(count == tasks)
   counter =  Deadlock(tasks,resource_type,counter);
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
    Collect.push_back(0);
    for(int i = 0; i < tasks; i++){ //Initialize each Task.
      run.id = i+1;
      run.state ="Ready";
      run.compute_time = 0;
      run.finish_time = 0;
      run.wait_time = 0;
      for(int d = 0; d <= resource_type; d++){
	run.resource_hold.push_back(0);
      }
      T.push_back(run);              
    }
    for(int i = 0; i < resource_type; i++){
      cin>>units;
      Available.push_back(units);
      Collect.push_back(0);
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

  for(int i = 0; i <= tasks; i++)
    visited.push_back(false);
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


