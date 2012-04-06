/***************************
Name : Harsh Patel
Lab  : Linker
E-Mail : harsh@nyu.edu
****************************/

#include<iostream>
#include<vector>
#include<map>
#include<fstream>
#include<string>
#include<cstring>
using namespace std;

map<string,int> symbol_table;
map<int,string> symbol_use[10000];
map<int,string> symbol_check[10000];
vector<pair<char,int > > address[10000];
vector<pair<char,int> > :: iterator it_add;
map<string,int>::iterator it;
map<int,string> :: iterator it1;
vector<pair<string,int> > use[10000];
vector<pair<string,int> >::iterator it2;
map<string,int> symbol_base;
map<string,bool> used;
map<string,bool>::iterator it3;


int main()
{
 // freopen("linker.txt","r",stdin); //Uncomment this line for File Input Note: The name of the input file should be linker.txt and it should be in the same working directory
  int base[10000],n_def,n_use,n_mod,def_val,num_modules = 0,locations,mem_add,op_code = 0,module_size[10000];
  bool locations_used[10000];
  string def,symbol;
  char add_type;
  cout<<"Output Begins here-------------------------------------------------"<<endl;
  //Pass 1 --Generating the Symbol Table
  while(cin >> n_def)
    {
      for(int i = 0; i < n_def; i++)
	{
	  cin>>def>>def_val;
	  if(symbol_table[def])
	    cout<<"Multiple Definitions of a Symbol:First Definition Used"<<endl;
	  else
	    symbol_table[def] = def_val + base[num_modules] ;
	  
	  symbol_base[def] = base[num_modules];
	  used[def] = true;
	}
      
      memset(locations_used,false,sizeof(locations_used));
      //Store info from the Use List
      cin>>n_use;
      for(int i = 0; i < n_use; i++)
	{
	  cin>>symbol;
	  //cout<<"Debug: "<<symbol_table[symbol]<<endl;
	 
	  while(cin>>locations)
	    {
	      if(locations == -1)
		break;
	      
	      if(locations_used[locations] == true)
		cout<<"Error: Multiple variables used in instruction; all but first ignored."<<endl;
	      else
		{
		  symbol_use[num_modules][locations] = symbol;
		  use[num_modules].push_back(make_pair(symbol,locations));
		  locations_used[locations] = true;
		}
	      //symbol_use[num_modules][i] = symbol;
	    }
	}
      
      //Store info from the Module List
      cin>>n_mod;
      module_size[num_modules] = n_mod; 
      if(!num_modules)
	base[num_modules + 1] = n_mod;
      else
	base[num_modules + 1] = base[num_modules] +  n_mod;
     
      for(int i = 0; i < n_mod; i++)
	{
	  cin>>add_type>>mem_add;
	  address[num_modules].push_back(make_pair(add_type,mem_add));
	}
      num_modules++; //Keeps track of the number of modules
    }
  cout<<"*****************************************************************"<<endl;
  cout<<"Here's the Symbol Table after First Pass:"<<endl;
  for(it = symbol_table.begin(); it!= symbol_table.end(); it++)
    {
      if(it->second > base[num_modules])
	{
	  it->second = symbol_base[it->first];
	  cout<<it->first<<" "<<it->second<<" Error:Definition exceeds module size: Zero used"<<endl;
	}
      else
      cout<<it->first<<" "<<it->second<<endl;
    }
  cout<<"*****************************************************************"<<endl;

  cout<<"Memory Map:"<<endl;
  //Pass 2:Relocating Relative Address

  for(int i = 0; i < num_modules; i++)
    {
      for(it_add = address[i].begin(); it_add != address[i].end(); it_add++)
	{
	  if(it_add->first == 'R')
	    {
	      it_add->second = base[i] + it_add->second; //Relocate
	    }
	 
	}
    }
  //Resolving External Addresses
  int temp;

  for(int i = 0; i < num_modules; i++)
    {
      temp = 0;
      for(it_add = address[i].begin(); it_add != address[i].end(); it_add++)
	{
	  if(it_add->first == 'E')
	    {
	      it_add->second = it_add->second - it_add->second % 1000;
	      if(!symbol_table[symbol_use[i][temp]])
		 cout<<"Error:"<<symbol_use[i][temp]<<" is not defined; Zero used in each instance"<<endl;
	      used[symbol_use[i][temp]] = false;
	      it_add->second += symbol_table[symbol_use[i][temp]];
	      
	    }
	  temp++;
	}
    }
 
  //Error-Checking Conditions and Output
  int count = 0;
  for(int i = 0; i < num_modules; i++)
    {
      for(it_add = address[i].begin(); it_add != address[i].end(); it_add++)
	{
	  op_code = it_add->second % 1000;
	  
	  if(it_add->first =='R' && op_code > base[num_modules])
	    {
	      it_add->second = it_add->second - it_add->second % 1000;
	      cout<<count<<": "<<it_add->second<<" Relative Address Exceeds the Module Size: Zero Used"<<endl;
	    }
	  else if(it_add->first == 'A' && op_code > 200)
	    {
	      it_add->second -= it_add->second % 1000;
	      cout<<count<<": "<<it_add->second<<" Absolute Address Exceeds the Machine size"<<endl;
	    }
	  else
	    cout<<count<<": "<<it_add->second<<endl;
	  count++;
	}
    }

  for(int i = 0; i < num_modules; i++)
    {
      for(it2 = use[i].begin(); it2 != use[i].end(); it2++)
	{
	  if (it2->second > module_size[i])
	    cout<<"Error:Use of"<<it2->first<<" exceeds module size: Use ignored"<<endl;
	}
    }
  for(it3 = used.begin(); it3 != used.end(); it3++)
    {
      if(it3->second == true)
	cout<<"Warning:"<<it3->first<<" was defined but never used"<<endl;
    }
  return 0;
}
