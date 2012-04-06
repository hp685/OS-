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
  int in_IO_time,finish_time,wait_time,in_CPU;
  string state;  // ready,blocked,running,terminated
  int block_time; //The amount of time a process stays in blocked state i.e, doing I/O. 
  float P_Ratio;	 
};

//Globals 
int current_random;
int timer,IO_time,CPU_time;    //Total IO time and CPU time 
vector<long long> U;
vector<process> P,CP; //CP is a copy of P where state is unchanged.
deque<process> Q;  //Process Queue
vector<int> Arrival;
//Functions 

bool sortbyArrival(const process &A, const process &B)
  {
    return(A.A < B.A);
  }
bool sortbyPR(const process &A, const process &B)
{
  return(A.P_Ratio < B.P_Ratio);
}
void schedule()
{
 
  int num_finished = 0;
  //First Come First Serve
  //Populate the queue based on arrival times
  int min = 1000,proc_num = 0;
 
  //cout<<"Proc "<<proc_num<<endl;

  int cpu_run,IO_run;
  timer = 0; //Set the Clock
  process current = P[0];
  current.state = "running";
  bool flag = true;
  while(flag)
    {
   
      //   cout<<"Here"<<endl;
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

      //cout<<current.id<<" "<<current.state<<" "<<timer<<endl;
    
	 
      while(current.state=="blocked")
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
		      for(int  j= 0; j < CP.size(); j++)
			if(CP[j].id == P[k].id)
			  CP[j] = P[k];
		    }
		}
	      else if(P[k].state == "unstarted" && timer == P[k].A)
		{
		  
		  P[k].state = "ready";
		  for(int  j= 0; j < CP.size(); j++)
		    if(CP[j].id == P[k].id)
		      CP[j] = P[k];
		}
	    }
      	  sort(CP.begin(),CP.end(),sortbyPR);
	  int tracker = 0;
	  float min = 0.0;
	  for(int i = 0; i < CP.size(); i++)
	    {
	      if(CP[i].state == "ready")
	      {
		if(min < CP[i].P_Ratio)
		  {
		    min = CP[i].P_Ratio;
		    tracker = i;
		  }
	       
	      }
	    }
	  for(int i = 0; i < CP.size(); i++)
	    if(CP[tracker].P_Ratio == P[i].P_Ratio)
	      if(P[i].state == "ready")
		{
		  current = P[i];
		  break;
		}
	}
      
      if(current.state == "ready")
	current.state = "running";
      cpu_run = 1 + (U[current_random] % current.B);
      current_random++;
      
      current.in_CPU = max(1,current.in_CPU);
      current.in_CPU += cpu_run;
          //Update the in_CPU.
      if(cpu_run > current.C)
	cpu_run = current.C;
      // cout<<"cpu: "<<cpu_run<<endl;
      CPU_time += cpu_run;
      for(int i = 1; i  <= cpu_run; i++)
	{
	  timer++;
	  cout<<"Current: "<<timer<<" ";
	if(verbose)	  
	for(int k = 0; k  < P.size(); k++)
	    {
	      if(P[k].id == current.id)
		cout<<P[k].id<<" "<<current.state<<" ";
	      else
		cout<<P[k].id<<" "<<P[k].state<<" ";
	      
	    }
	  cout<<endl;
	  //cout<<"Timer: "<<timer<<" "<<current.state<<endl;
	  if(current.state == "running")
	    current.C = current.C - 1;
	
	  for(int j = 0; j < P.size(); j++)
	    {
	     
	      if(P[j].id != current.id)
		{
		  if(P[j].state == "blocked")  //Decrease block_time for each Blocked Process.
		    {
		 
		      P[j].block_time -= 1;
		      P[j].in_IO_time += 1;
		      if(P[j].block_time == 0)
			{
			  P[j].state = "ready";
			  for(int k = 0; k < CP.size(); k++)
			    if(CP[k].id == P[j].id)
			      CP[k] = P[j];
			}
		    }
		  else if(P[j].state == "ready") //If the Process is in ready state and not executing, it is waiting.
		    {
		      P[j].wait_time++;
		      for(int k = 0; k < CP.size(); k++)
			if(CP[k].id == P[j].id)
			  CP[k] = P[j];
		    }
		  else if(P[j].state == "unstarted" && P[j].A <= timer)
		    {
		     
		      P[j].state ="ready";
		      for(int k = 0; k < CP.size(); k++)
			if(CP[k].id == P[j].id)
			  CP[k] = P[j];
		    }

		}//P[j].id != current.id
	    }//j ends
	}//i ends
      
      if(current.C == 0)
	{
	  current.finish_time = timer; 
	  current.state = "terminated";
	  current.P_Ratio = 0.00;
	  for(int k = 0; k < P.size(); k++)
	    if(P[k].id == current.id)
	      {
		P[k] = current;
		//	cout<<P[k].P_Ratio;
		for(int j = 0; j < CP.size(); j++)
		  if(CP[j].id == current.id)
		    CP[j] = current;
	      }
	  	  
	  //  float min = 0.0;
	      

	      for(int i = 0 ; i < P.size(); i++)
		{
		  //if(P[i].in_CPU <= 1)
		  P[i].in_CPU = max(P[i].in_CPU, 1);
		  if(P[i].state != "terminated")
		    {
		      P[i].P_Ratio = (float)((float)(float)(timer - P[i].A)/(float) P[i].in_CPU);
		      for(int j = 0; j < CP.size(); j++)
			if(P[i].id == CP[j].id)
			  CP[j].P_Ratio = P[i].P_Ratio;
		    }
		}
	      
	      /*for(int i = 0; i < P.size(); i++)
		{
		 
		  if(P[i].state == "ready" || (P[i].state =="blocked" && P[i].block_time == 1))
		    if(P[i].P_Ratio > min)
		      {
			min = P[i].P_Ratio;
			tracker = i;
			if(P[i].state == "blocked")
			  P[i].state = "ready";
		      }
		      }*/
	      float min = 0.0;
		  sort(CP.begin(),CP.end(),sortbyPR);
		  int tracker = 0;
	 	  for(int i = 0; i < CP.size(); i++)
		    {
		      if(CP[i].state == "ready")
		      {
			if(CP[i].P_Ratio > min)
			  {
			    tracker = i;
			    min = CP[i].P_Ratio;
			  }
		      }
		    }
		  for(int i = 0; i < CP.size(); i++)
		    if(CP[tracker].P_Ratio == P[i].P_Ratio)
		      {
			if(P[i].state =="ready")
			  {
			    current = P[i];
			    break;
			  }
		      }
	      	  
		  /*	  if(current.state == "terminated")
	    {
	      for(int i = 0; i < P.size(); i++)
		{
		  if(P[i].state == "blocked")
		    current = P[i];
		}
		}*/
      
      //break;
	}
      else
	{
	  current.state = "blocked";
	  // cout<<"rand"<<U[current_random]<<endl;
	  IO_run = 1 + (U[current_random] % current.IO); //IO run for the current process.
	  current_random++;
	  //cout<<"IOR "<<IO_run<<endl;
	  current.block_time = IO_run;
	  IO_time += IO_run;
	  for(int k = 0; k < P.size(); k++)
	    if(P[k].id == current.id)
	      {
		P[k] = current; //State saved in the Process.
		for(int j = 0; j < CP.size(); j++)
		  if(CP[j].id == current.id)
		    CP[j]= current;
	      }
	  
	  for(int i = 0 ; i < P.size(); i++)
	    {
	     
	      P[i].in_CPU = max(1,P[i].in_CPU);
	      if(P[i].state != "terminated")
		{
		  P[i].P_Ratio = (float)((float)(float)(timer  - P[i].A )/(float) P[i].in_CPU);
		  for(int j = 0; j < CP.size(); j++)
		    if(P[i].id == CP[j].id)
		      CP[j].P_Ratio = P[i].P_Ratio;
		}
	        cout<<P[i].P_Ratio<<" "<<P.size()<<endl;
	    }
	  //float min = 0.0; int tracker = 0;
	  /*for(int i = 0; i < P.size(); i++)
	    {
	      cout<<P[i].P_Ratio<<" "<<P[i].id<<" "<<P[i].in_CPU<<endl;
	        if(P[i].state == "ready" || (P[i].state =="blocked" && P[i].block_time == 1))
		if(P[i].P_Ratio > min)
		  {		  min = P[i].P_Ratio;
		  tracker = i;
		  if(P[i].state == "blocked")
		    P[i].state ="ready";
		  }
		  }*/
	  int tracker = 0;float min = 0.0;
	  sort(CP.begin(),CP.end(),sortbyPR);
	  for(int i = 0; i < CP.size(); i++)
	    {
	      if(CP[i].state == "ready")
	      {
		if( CP[i].P_Ratio > min)
		  {
		    min = CP[i].P_Ratio;
		    tracker = i;
		  }
	      }
	    }
	  for(int i = 0; i < CP.size(); i++ )
	    if(CP[tracker].P_Ratio == P[i].P_Ratio)
	      {
		if(P[i].state =="ready")
		  {
		    //   cout<<i<<endl;
		    current = P[i];
		    break;
		  }
	      }
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
   }
 
 current_random = 0;
   sort(P.begin(),P.end(),sortbyArrival);
  sort(CP.begin(),CP.end(),sortbyArrival);
 
 schedule(); //Run the Scheduler
  return 0;
}
