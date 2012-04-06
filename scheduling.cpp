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
  int id,arrival_time,cpu_interval,cpu_burst,IO_interval;
  int in_IO_time,finish_time;
  bool state;  // false => Blocked and True => Ready 
};

//Globals
int current_random;
int timer,IO_time,CPU_time;    //Total IO time and CPU time 
vector<long long> U;
vector<process> P,CP; //CP is a copy of P where state is unchanged.
queue<process> Q;  //Process Queue
vector<int> Arrival;
//Functions 
void schedule()
{
  //First Come First Serve
  //Populate the queue based on arrival times
  for(int i = 0; i < P.size(); i++)
    Q.push(P[i]);
  
  int cpu_run,IO_run;
  
 while(!Q.empty())
    {
      process current = Q.front();
      	 	  
	  cpu_run = 1 + (U[current_random] % current.cpu_interval); //Read in (0,B]
	  cout<<"Random number used :"<<U[current_random]<<endl;
	  current_random++;
	  if(cpu_run > current.cpu_burst)
	    {
	      cpu_run = current.cpu_burst;
	      current.cpu_burst = 0;
	      CPU_time += cpu_run;
	      timer += cpu_run;
	    }
	  else
	    {
	      current.cpu_burst -= cpu_run;
	      CPU_time += cpu_run;
	      timer += cpu_run;
	    }
	  //if(current.cpu_burst == 0) //All processes have finished. Why continue scheduling!? 
	  IO_run = 1 + (U[current_random] % current.IO_interval);  //Read in (0,IO]	  
	  cout<<"Random number used :"<<U[current_random]<<endl;
	  current_random++;
	   current.in_IO_time += IO_run;
	   IO_time += IO_run;
	   timer += IO_run;
	
      //Process done executing.
	   if(current.cpu_burst == 0)
	     {
	       current.finish_time = timer; 
	       for(int i = 0; i < CP.size(); i++)
		 {
		   if(CP[i].id == current.id)  P[i] = current;
		 }
	     }
      Q.pop();
      Q.push(current);
    }
  for(int i = 0; i < CP.size(); i++)
    {
      cout<<"Process "<<CP[i].id<<" : "<<endl;
      cout<<"(A,B,C,IO) : ( "<<CP[i].arrival_time<<", "<<CP[i].cpu_interval<<", "<<CP[i].cpu_burst<<", "<<CP[i].IO_interval<<" )"<<endl;
      cout<<"Finishing Time: "<< CP[i].finish_time<<endl;
      cout<<"Turnaround Time: "<<P[i].finish_time - P[i].arrival_time<<endl;
      cout<<"I/O Time: "<<P[i].in_IO_time<<endl;
      cout<<"Waiting Time:"<<P[i].in_IO_time<<endl;
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

  int rand,dum;
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
     Arrival.push_back(A);
     pin.id = i;
     pin.arrival_time = A;
     pin.cpu_interval = B;
     pin.cpu_burst = C;
     pin.IO_interval = IO;
     pin.state = true; //Initialize the process as Ready. 
     pin.in_IO_time = 0;
     pin.finish_time = 0;
     P.push_back(pin);
     CP.push_back(pin);
   }
 current_random = 0;
 sort(Arrival.begin(),Arrival.end());
 
 schedule(); //Run the Scheduler
  return 0;
}
