#include<iostream>
#include<fstream>
#include<vector>
#include<queue>
#include<algorithm>
using namespace std;

//Classes
class process
{
public:
  int id,A,B,C,IO;
  int in_IO_time,finish_time,wait_time,run_left;
  string state;  // ready,blocked,running,terminated
  int block_time; //The amount of time a process stays in blocked state i.e, doing I/O. 
 
};

//Globals 
int current_random;
int timer,IO_time,CPU_time;    //Total IO time and CPU time 
vector<long long> U;
vector<process> P,CP; //CP is a copy of P where state is unchanged.
deque<process> Q;  //Process Queue
vector<int> Arrival;
const int RR_Q = 2;
//Functions 

bool sortbyArrival(const process &A, const process &B)
  {
    return(A.A < B.A);
  }
void schedule()
{
 
  int num_finished = 0;
  //First Come First Serve
  //Populate the queue based on arrival times
  int min = 1000,proc_num = 0,max = 0;
 
  //cout<<"Proc "<<proc_num<<endl;
  Q.push_back(P[0]);
  int cpu_run,IO_run,cpu_r;
  timer = 0; //Set the Clock
  process current = Q.front();
  current.state = "running";
  bool flag = true;
  max = P[P.size()-1].A;
  while(!Q.empty() || timer <= max || flag)
    {
   

      int counter = 0;
      for(int i = 0; i < P.size(); i++)  // All processes Terminated?
	{
	  if(P[i].state =="terminated")
	  counter++;
	}
     
      if(counter == P.size())
	{
	  flag = false;
	  break;
	}
      
    
      bool no_IO, race = false; 
      while(Q.empty() && flag )
	{ 
	  timer++;
	  for(int k = 0; k < P.size(); k++)
	    {
	      if(P[k].state == "blocked")
		{
		  P[k].block_time--;
		  P[k].in_IO_time++;
		  if(P[k].block_time == 0)
		    {
		      P[k].state ="ready";
		      Q.push_back(P[k]);
		    }
		}
	    }
	  
	  if(Q.size() > 0)
	    current = Q.front();
	}
      
      if(current.state == "ready")
	current.state = "running";
     
      if(current.run_left == 0)
	{
	  cpu_run = 1 + (U[current_random] % current.B);
	  cout<<"CPU Burst: "<<U[current_random]<<endl;
	  current_random++;

	  no_IO = false;
	  if(cpu_run > RR_Q)
	    {
	      no_IO = true;
	      current.run_left = cpu_run - RR_Q;
	      cpu_run = RR_Q;
	    }
	}
      else if(current.run_left > RR_Q)
	    {
	      no_IO = true;
	      current.run_left -= RR_Q;
	      cpu_run = RR_Q;
	    }
      else if(current.run_left <= RR_Q)
	{
	  no_IO = false;
	  cpu_run = current.run_left;
	  current.run_left = 0;
	}
	   
       if(cpu_run > current.C)
	 cpu_run = current.C;
      // cout<<"cpu: "<<cpu_run<<endl;
      cpu_r = cpu_run;
      CPU_time += cpu_run;
      bool done = false;
      for(int i = 1; i  <= cpu_run; i++)
	{
	  timer++;
	if(verbose)
{
 	 cout<<"Current: "<<timer<<" ";		  
	for(int k = 0; k  < P.size(); k++)
	    {
	      if(P[k].id == current.id)
		cout<<P[k].id<<" "<<current.state<<" ";
	      else
		cout<<P[k].id<<" "<<P[k].state<<" ";
	      
	    }
	  cout<<endl;
}
	  //current.state = "running";
	  if(current.state == "running")
	    current.C = current.C - 1;

	  for(int j = 0; j < P.size(); j++)
	    {
	      //  cout<<"Timer: "<<timer<<" "<<current.state<<" "<<P[j].state<<endl;
	      if(P[j].id != current.id)
		{
		  if(P[j].state == "blocked")  //Decrease block_time for each Blocked Process.
		    {
		      P[j].block_time -= 1;
		      P[j].in_IO_time += 1;
		      if(P[j].block_time == 0)
			{
			  P[j].state = "ready";
			  Q.push_back(P[j]); //Process Ready to run now.
			  cout<<"Here :L"<<timer<<" "<<P[j].id<<endl; 
			}
		    }
		  else if(P[j].state == "ready") //If the Process is in ready state and not executing, it is waiting.
		    {
		      for(int k = 0; k < Q.size(); k++)	
			if(Q[k].id == P[j].id)
			  {
			    Q[k].wait_time += 1;
			    //  Q[k].state = "ready"; 
			  }
		    }
		  else if(P[j].state == "unstarted" && P[j].A < timer)
		    {
		      P[j].state = "ready";
		      P[j].wait_time += 1;
		      Q.push_back(P[j]);
		    }
		  if(P[j].A == timer && P[j].state == "unstarted")
		    {
		      P[j].state ="ready";
		      Q.push_back(P[j]);
		    }
		}//P[j].id != current.id
	      else
		{
		  if(no_IO == true && i == cpu_run && current.C != 0)
		    {
		      current.state = "ready";
		      current.block_time = 0;
		      Q.pop_front();
		      Q.push_back(current);
		      done = true;
		    }
		}
	    }//j ends
	}//i ends
      
      if(current.C == 0)
	{
	  current.finish_time = timer; 
	  current.state = "terminated";
	  for(int k = 0; k < P.size(); k++)
	    {
	      if(P[k].id == current.id)
	      P[k] = current;
	    }
	
	  Q.pop_front();
	  if(Q.size() > 0)
	    current = Q.front();
	}
      else
	{
	  if(no_IO == false)
	    {
	      current.state = "blocked";
	      // cout<<"rand"<<U[current_random]<<endl;
	      IO_run = 1 + (U[current_random] % current.IO); //IO run for the current process.
	      cout<<"IO Burst: "<<U[current_random]<<endl;
	      current_random++;
	      //cout<<"IOR "<<IO_run<<endl;
	      current.block_time = IO_run;
	      IO_time += IO_run;
	      Q.pop_front();
	      
	      
	    }
	  else
	    {
	      /*for(int k = 0; k < P.size(); k++)
		if(P[k].state == "blocked" && P[k].block_time == 1)
		  {
		    P[k].block_time = 0;
		    P[k].state = "ready";
		    P[k].run_left = 0;
		    Q.push_back(P[k]);
		    }
	      */
	       if(done == false)
		{
		  current.state = "ready";
		  current.block_time = 0;
		  Q.pop_front();
		  Q.push_back(current);
		}
	    }

	  for(int k = 0; k < P.size(); k++)
	    if(P[k].id == current.id)
	      P[k] = current; //State saved in the Process.
	   // Process is blocked hence popped off the queue. 
	  

	  if(Q.size() > 0)
	    current = Q.front();
	}
    }
  
   for(int i = 0; i < CP.size(); i++)
    {
      
      cout<<"Process "<<P[i].id<<" : "<<P[i].state<<endl;
      cout<<"(A,B,C,IO) : ( "<<CPZ[i].A<<", "<<CPZ[i].B<<", "<<CPZ[i].C<<", "<<CPZ[i].IO<<" )"<<endl;
      cout<<"Finishing Time: "<< P[i].finish_time<<endl;
      cout<<"Turnaround Time: "<<P[i].finish_time - P[i].A<<endl;
      cout<<"I/O Time: "<<P[i].in_IO_time<<endl;
      cout<<"Waiting Time: "<<P[i].wait_time<<endl;
    }
  cout<<"Summary Data:"<<endl;
  cout<<"Finishing Time: "<<timer<<endl;
  cout<<"CPU Utilization: "<<(float)CPU_time/(timer)<<endl;
    cout<<"I/O Utilization: "<<(float)IO_time/timer<<endl;
  
    cout<<"Throughput: "<<"" <<" processes per hundred cycles "<<(float)((float)P.size()/(float)timer) * (float)100<<endl; 
  float turn = 0.0; int sum = 0;  
for(int i = 0; i < P.size(); i++)
    {
      sum += (P[i].finish_time - P[i].A);
    }
 turn = (float)sum/P.size();
 cout<<"Average Turnaround time: "<<turn<<endl;
 sum = 0; 
for(int i = 0; i < P.size(); i++)
  sum+= P[i].wait_time;
 turn = (float)sum/P.size();
 cout<<"Average Waiting Time: "<<turn<<endl;
}


int main( int argc, char *argv[])
{

  int rand;
  ifstream input1;
  string file;

  if(argc == 3)
    { 
     ifstream input(argv[1]);
     //cout<<argv[0]<<endl;  
     //input.open(argv[0]);
      
     while(input>>rand)   
	{
	  U.push_back(rand);
	}

      int number_processes,A,B,C,IO;
      char dummy;
      process pin;      
      ifstream input1(argv[2]);
      //input1.open(argv[1]);

      input1>>number_processes;
      
      for(int i = 0; i < number_processes; i++)
	{
	  input1>>dummy; // Read '('
	  input1>>A>>B>>C>>IO;
	  input1>>dummy;  //Read ')'
	  Arrival.push_back(A);
	  pin.id = i;
	  pin.A = A;
	  pin.B = B;
	  pin.C = C;
	  pin.IO = IO;
	  pin.state = "unstarted"; //Initialize the process as Ready. 
	  pin.in_IO_time = 0;
	  pin.finish_time = 0;
	  pin.block_time = 0;
	  pin.wait_time = 0;
	  pin.P_Ratio = 0.0;
	  pin.in_CPU = 0;
	  P.push_back(pin);
	  CP.push_back(pin);
	  CPZ.push_back(pin);
	}
      
      current_random = 0;
      sort(P.begin(),P.end(),sortbyArrival);
      sort(CP.begin(),CP.end(),sortbyArrival);
      sort(CPZ.begin(),CPZ.end(),sortbyArrival);
      schedule(); //Run the Scheduler
    }
  else if(argc == 4)
    {
      verbose = true;
      ifstream input(argv[2]);  
      //input.open(argv[0]);
      while(input>>rand)   
	{
	  U.push_back(rand);
	  //cout<<"here"<<endl;
	}
      
      ifstream input1(argv[3]);
      //input1.open(argv[1]);
      
      int number_processes,A,B,C,IO;
      char dummy;
      process pin;
      input1>>number_processes;
      
      for(int i = 0; i < number_processes; i++)
	{
	  input1>>dummy; // Read '('
	  input1>>A>>B>>C>>IO;
	  input1>>dummy;  //Read ')'
	  Arrival.push_back(A);
	  pin.id = i;
	  pin.A = A;
	  pin.B = B;
	  pin.C = C;
	  pin.IO = IO;
	  pin.state = "unstarted"; //Initialize the process as Ready. 
	  pin.in_IO_time = 0;
	  pin.finish_time = 0;
	  pin.block_time = 0;
	  pin.wait_time = 0;
	  pin.P_Ratio = 0.0;
	  pin.in_CPU = 0;
	  P.push_back(pin);
	  CP.push_back(pin);
	  CPZ.push_back(pin);
	}
      
      current_random = 0;
      sort(P.begin(),P.end(),sortbyArrival);
      sort(CP.begin(),CP.end(),sortbyArrival);
      sort(CPZ.begin(),CPZ.end(),sortbyArrival);
      schedule(); //Run the Scheduler
    }
  
 
  return 0;
}
