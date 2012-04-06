#include<iostream>
#include<vector>
#include<queue>
#include<deque>
#include<algorithm>
#include<fstream>
using namespace std;

//Classes
class process
{
public:
  int id,A,B,C,IO;
  int in_IO_time,finish_time,wait_time;
  string state;  // ready,blocked,running 
  int block_time; //The amount of time a process stays in blocked state i.e, doing I/O. 
  
};

//Globals

const int RR_Q = 2; //Quantum
vector<process> P,CP;
deque<process> Q;
vector<long long> U;
int current_random;
int timer,IO_time,CPU_time;
//Functions
bool sortbyArrival(const process &A, const process &B)
  {
    return(A.A < B.A);
  }
void schedule()
{
  int cpu_run = 0, IO_run = 0, timer = 0;
  process current = Q.front();

  if(current.state == "unstarted" && current.A <= timer )
    {
      current.state = "ready";
    }
  while(!Q.empty() && timer < 1800)
    {
      int counter = 0;
      
      //Edge cases
      for(int i = 0; i < Q.size(); i++) //All processes unstarted or blocked.
	if(Q[i].state == "unstarted" || Q[i].state == "blocked")
	  counter++;
      if(counter == Q.size())
	{
	  bool flag = true;
	  while(flag)
	    {
	      timer++;
	      for(int i = 0; i < Q.size(); i++)
		{
		  if(Q[i].state == "unstarted")
		    {
		      if(Q[i].A == timer)
			{
			  Q[i].state="ready";
			  current = Q[i];
			  Q.erase(Q.begin() + i);
			  Q.push_front(current);
			  flag = false;
			}
		    }
		  else if(Q[i].state == "blocked")
		    {
		      Q[i].block_time--;
		      Q[i].in_IO_time++;
		      if(Q[i].block_time == 0)
			{
			  Q[i].state ="ready";
			  current = Q[i];
			  Q.erase(Q.begin() + i);
			  Q.push_front(current);
			  flag = false;
			}
		    }
		}
	    }
	}
      //End Edge Cases
      if(current.state == "unstarted" && current.A > timer) // If process is unstarted place it at the back of the queue.
	{
	  process temp = current;
	  Q.pop_front();
	  Q.push_back(temp);
	  current = Q.front(); // Initialize the current process. 
	}

      if(current.state == "ready")
	{
	  current.state = "running";
	}
      
      cpu_run = 1 + (U[current_random] % current.B);
      current_random++;
      cout<<current.id<<" "<<current.state<<" "<<timer<<endl;
      //Update CPU_time
      
	
      for(int i = 0; i < RR_Q; i++)
	{
	  timer++;
	  //I/O Utilization of other Processes. 
	  for(int j = 0; j < Q.size(); j++)
	    {
	      if(current.id != Q[j].id)
		{
		  if(Q[j].state == "blocked")
		    {
		      Q[j].in_IO_time++;
		      Q[j].block_time--;
		      if(Q[j].block_time == 0)
			Q[j].state = "ready";
		    }
		  else if(Q[j].state == "ready")
		    {
		      Q[j].wait_time++;
		    }
		  else if(Q[j].state == "unstarted" && Q[j].A <= timer)
		    {
		      // cout<<Q[j].id<<" "<<timer<<endl;
		      Q[j].state = "ready";
		    }
		} //current != Q[j].
	    }//end for j.
	  
	  if(current.state == "running")
	    {
	      current.C = current.C - 1; //Decrement the CPU requirement.
	      cpu_run--;
	      CPU_time++;
	      
	      if(cpu_run == 0)
		{
		  current.state = "blocked"; //Run ends before the quantum.
		  break;
		}
	    }
	  else if(current.state == "blocked")
	    {
	      current.in_IO_time++;
	      current.block_time--;
	      if(current.block_time == 0)
		current.state = "ready";
	      //Process is Blocked. 
	      break;
	    }
	}//end for i
      
      if(current.C == 0) //Process has finished executing.
	{
	  current.state = "terminated";
	  current.finish_time = timer; 
	  for(int k = 0; k < P.size(); k++) //When Process finished copy the state.
	    if(current.id == P[k].id)
	      P[k] = current;
	  Q.pop_front(); //Process not pushed back. Done!.
	  if(Q.size() > 0)
	  current = Q.front();
	}
      else //Process does its I/O. 
 	{
	  if(current.state == "running") //Process used up its quantum
	    current.state = "blocked"; //Else the process has already been blocked
	  IO_run = 1 + (U[current_random] % current.IO);
	  current_random++;
	  current.block_time = IO_run;
	  IO_time += IO_run;
	  
	  //Now put the process at the end of the queue. 
	  process temp = current;
	  Q.pop_front();
	  Q.push_back(temp);
	  current = Q.front(); // Initialize the current process. 
	  //
	  
	}
    }
    for(int i = 0; i < CP.size(); i++)
    {
      
      cout<<"Process "<<CP[i].id<<" : "<<P[i].state<<endl;
      cout<<"(A,B,C,IO) : ( "<<CP[i].A<<", "<<CP[i].B<<", "<<CP[i].C<<", "<<CP[i].IO<<" )"<<endl;
      cout<<"Finishing Time: "<< P[i].finish_time<<endl;
      cout<<"Turnaround Time: "<<P[i].finish_time - P[i].A<<endl;
      cout<<"I/O Time: "<<P[i].in_IO_time<<endl;
      cout<<"Waiting Time: "<<P[i].wait_time<<endl;
    }
  cout<<"Summary Data:"<<endl;
  cout<<"Finishing Time: "<<timer<<endl;
  cout<<"CPU Utilization: "<<(float)CPU_time/(timer)<<endl;
  cout<<"I/O Utilization: "<<(float)IO_time/timer<<endl;
  //Fill in these later
  cout<<"Throughput: "<<"" <<" processes per hundred cycles"<<endl; 
  cout<<"Average Turnaround time: "<<endl;
  cout<<"Average Waiting Time: "<<endl;
}


int main()
{
  int rand;
  ifstream input("random.txt");  
  // freopen("random.txt","r",stdin);
  while(input>>rand)   
    {
      U.push_back(rand);
    }
  
  ifstream input1("fcfs");
  // freopen("fcfs","r",stdin);
  
  int number_processes,A,B,C,IO;
  char dummy;
  process pin;
  input1>>number_processes;
  
  for(int i = 0; i < number_processes; i++)
    {
     input1>>dummy; // Read '('
     input1>>A>>B>>C>>IO;
     input1>>dummy;  //Read ')'

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
     P.push_back(pin);
     CP.push_back(pin);
     Q.push_back(pin); //Processes Queued.
   }
  sort(P.begin(),P.end(),sortbyArrival);
  sort(CP.begin(),CP.end(),sortbyArrival);
  for(int i = 0; i < P.size(); i++)
    {
      P[i].id = i;
      CP[i].id = i;
    }
  current_random = 0;
 
  schedule(); //Run the Scheduler
  return 0;
}
