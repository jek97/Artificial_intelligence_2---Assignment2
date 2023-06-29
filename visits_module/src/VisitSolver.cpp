    /*
     <one line to give the program's name and a brief idea of what it does.>
     Copyright (C) 2015  <copyright holder> <email>
     
     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.
     
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.
     */


#include "VisitSolver.h"
#include "ExternalSolver.h"
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "armadillo"
#include <initializer_list>

using namespace std;
using namespace arma;


//map <string, vector<double> > region_mapping;

extern "C" ExternalSolver* create_object(){
  return new VisitSolver();
}

extern "C" void destroy_object(ExternalSolver *externalSolver){
  delete externalSolver;
}

VisitSolver::VisitSolver(){

}

VisitSolver::~VisitSolver(){

}

void VisitSolver::loadSolver(string *parameters, int n){
  cout<<"a"<<endl;
  starting_position = "r0";
  string Paramers = parameters[0];

  char const *x[]={"dummy"};
  char const *y[]={"act-cost","triggered"};
  parseParameters(Paramers);
  affected = list<string>(x,x+1);
  dependencies = list<string>(y,y+2);

  string waypoint_file = "./waypoint.txt";   // change this to the correct path
  parseWaypointConnection(waypoint_file);

  string landmark_file = "visits_domain/landmark.txt";  // change this to the correct path
  parseLandmark(landmark_file);

  string region_file = "./region.txt";
  parseRegions(region_file);

  //startEKF();
}

map<string,double> VisitSolver::callExternalSolver(map<string,double> initialState,bool isHeuristic){
  cout<<"b"<<endl;
  map<string, double> toReturn;
  map<string, double>::iterator iSIt = initialState.begin();
  map<string, double>::iterator isEnd = initialState.end();
  double dummy;
  double act_cost;

  map<string, double> trigger;

  for(; iSIt != isEnd; ++iSIt){
    // 
    string parameter = iSIt->first;
    string function = iSIt->first;
    double value = iSIt->second;

    function.erase(0,1);
    function.erase(function.length()-1,function.length());
    int n = function.find(" ");

    if(n!=-1){
      string arg = function;
      string tmp = function.substr(n+1,5);
      

      function.erase(n,function.length()-1);
      arg.erase(0,n+1);
      if(function=="triggered"){
        trigger[arg] = value>0?1:0;
        if (value>0){

          string from = tmp.substr(0,2);
          string to = tmp.substr(3,2);
          pathfinder(from, to);

        }
      }
    }
    else{
      if(function=="dummy"){
        dummy = value;
      }
      else if(function=="act-cost"){
        act_cost = value;
      }
      //else if(function=="dummy1"){
      //duy = value;              
      ////cout << parameter << " " << value << endl;
      //}
    }
  }

  double results = calculateExtern(dummy, act_cost);      

  if (ExternalSolver::verbose){
    cout << "(dummy) " << results << endl;
  }

  toReturn["(dummy)"] = results;

  return toReturn;
}

list<string> VisitSolver::getParameters(){
  return affected;
}

list<string> VisitSolver::getDependencies(){
  return dependencies;
}


void VisitSolver::parseParameters(string parameters){
  cout<<"c"<<endl;
  int curr, next;
  string line;
  ifstream parametersFile(parameters.c_str());
  cout<< "param" <<endl;
  if (parametersFile.is_open()){
    while (getline(parametersFile,line)){
      cout<< "param_it" <<endl;
      curr=line.find(" ");
      string region_name = line.substr(0,curr).c_str();
      curr=curr+1;
      while(true ){
        next=line.find(" ",curr);
        region_mapping[region_name].push_back(line.substr(curr,next-curr).c_str());
        if (next ==-1)
          break;
        curr=next+1;
      }                
    }  
  }
}


double VisitSolver::calculateExtern(double external, double total_cost){

  //float random1 = static_cast <float> (rand())/static_cast <float>(RAND_MAX);
  //double cost = 5;//random1;
  return cost;
}


void VisitSolver::parseWaypointConnection(string waypoint_file){
  // let's enstablish a structure of the wayppoints file as follow: wp0[0,0,0,wp1,wp2,wp3,wp4,wp5,wp6] where then you have the connected waypoint to that one, fundamental all the waypoints must be connected to the same number of waypoints.
  cout<<"waypoint"<<endl;
  int curr, next, end, flag;
  flag = 0;
  string line, wp;
  double pose1, pose2, pose3;
  vector<string> wp_connected;
  ifstream parametersFile(waypoint_file.c_str());
  if (parametersFile.is_open()){
    while (getline(parametersFile,line)){
      cout<<"waypoint_it"<<endl;
      wp_connected.clear(); //clear the vector at initialization, look here *** to understan

      curr=line.find("[");
      string waypoint_name = line.substr(0,curr).c_str();

      curr=curr+1;
      next=line.find(",",curr);

      pose1 = (double)atof(line.substr(curr,next-curr).c_str());
      curr=next+1; 
      next=line.find(",",curr);

      pose2 = (double)atof(line.substr(curr,next-curr).c_str());
      curr=next+1; 
      next=line.find(",",curr);

      pose3 = (double)atof(line.substr(curr,next-curr).c_str());
      cout<<"waypoint_it2"<<endl;
      while(flag == 0){
        cout<<"waypoint_it3"<<endl;
        curr=next+1; 
        next=line.find(",",curr);
        end=line.find("]",curr);
        if(end<next){
          flag = 1;
        }
        else{
          wp = line.substr(curr,next-curr).c_str();
          wp_connected.push_back(wp); // ***
        }
      }

      waypoint[waypoint_name] = vector<double> {pose1, pose2, pose3};
      connection[waypoint_name] = wp_connected;
    }
  }
}


void VisitSolver::parseRegions(string region_file){
  cout << "mammt" <<endl;
  // let's enstablish a structure of the regions file as follow: r0=wp0.
  int curr, next;
  string line;
  ifstream parametersFile(region_file);
  cout << "mammt12" <<endl;
  if (parametersFile.is_open()){
    while (getline(parametersFile,line)){
      cout << "mammt1" <<endl;
      curr=line.find("=");
      string region_name = line.substr(0,curr).c_str();

      curr=curr+1;
      next=line.find(".",curr);

      string wp = line.substr(curr,next-curr).c_str();

      region[region_name] = wp;
      cout << region.size() << endl;
    }
  }
}


double VisitSolver::dist_euc(string wp_from, string wp_to){

  double dist;
  vector<double> from;
  vector<double> to;
  from = waypoint.at(wp_from);
  to = waypoint.at(wp_to);
  dist = sqrt(pow((from[0] - to[0]), 2) + pow((from[1] - to[1]), 2));
  return dist;
}


void VisitSolver::displayResult(string path_file){
  int i;
  ofstream output;
  output.open(path_file, std::ios_base::app);
  for(i = 0; i <= sizeof(path); ++i){
    output << i << ". " << path[i] << endl; // write on the file
    cout << i << ". " << path[i] << endl; // write on the terminal
  }
}

void VisitSolver::pathfinder(string reg_from, string reg_to){
  
  int i = 100000;
  int i_s;
  double f, g_s, h_s;
  string wp_init, wp_curr, wp_goal, node;
  vector<double> wp_curr_data, wp_succ_data;
  vector<string> successors; // contain all the successor waypoint (by name) of the current waypoint
  map<string, vector<double>> open, close; // both with the structure [wpn, {g(wp), h(wp), f(wp)=g(wp)+h(wp)}]
  map<string, string> parent; // parent of a given node
  cout << "bob" << endl;
  // translate the regions in waypoints
  wp_init = region.at(reg_from); // initial waypoint
  wp_goal = region.at(reg_to); // goal waypoint
  cout << "bob1" << endl;
  // use the A star algorithm to find a solution
  f = dist_euc(wp_init, wp_goal);
  open[wp_init]={0, f, f}; // put node_start (wp_init) in open with its heuristic (f)
  while(!open.empty()){ // while open is not empty
    for(auto i_wp = open.begin(); i_wp != open.end(); ++i_wp){ // from the open list take the node node_curr (wp_curr) with the lowest heuristic (f)
      if((i_wp->second[2]) <= i){ // even the < would be sufficient and if two valeus are equals would keep the oldest, with the <= i will keep the newest
        i = i_wp->second[2];
        wp_curr = i_wp->first;
      }
    }
    if(wp_curr == wp_goal){ // if node_current (wp_curr) = node_goal (wp_goal) we have finished
      cout << "path found" << endl;
      // recovering the path and its cost
      path.clear(); // clear all the vector from previous iterations
      cost = 0; // init the cost to 0 from previous iterations
      path.insert(path.begin(), wp_curr); // first insert the goal
      node = wp_curr; // initialize the node to the goal one
      while(node != wp_init){
        path.insert(path.begin(), parent[node]); // insert the parent of the actual node at the beginning of the path vector
        cost = cost + dist_euc(node, parent[node]); // add the step cost
        node = parent[node]; // set the new node as the parent of the actual one for the next iteration
      }
      displayResult("visits_domain/path.txt"); // write the result both on the terminal and on the text file
    }
    else{ // otherwise generate all the successors of the current node
      successors = connection[wp_curr]; // generate the successors nodes, that are all the one connected without the one where we come from                                                                                         // not sure, to check 
      for(i_s = 0; i_s <= successors.size(); ++i_s){ // for each successor
        wp_curr_data = open[wp_curr];
        g_s = wp_curr_data[0] + dist_euc(wp_curr, successors[i_s]); // set the successor_current_cost to g(wp_curr)+cost(wp_curr-wp_successor)
        cout << "b" << endl;
        try{ // if the successor is already in the open list
          wp_succ_data = open.at(successors[i_s]);
          if(wp_succ_data[0] <= g_s){
            goto done;
          }
        }
        catch(const std::out_of_range& oor){ // if the successor is not already in the open list
          cout << "c" <<endl;
          try{ // if the successor node is already in the close list and it's not in the open list
            wp_succ_data = close.at(successors[i_s]);
            if(wp_succ_data[0] <= g_s){
              goto done;
            }
            else{ 
              close.erase(successors[i_s]); // remove the waypoint from the close list
              open[successors[i_s]] = wp_succ_data; // put it in the open one
            }
          }
          catch(const std::out_of_range& oor){ // if the successor is not already neither in the open list nor in the close list
            cout << "d" <<endl;
            h_s = dist_euc(successors[i_s], wp_goal); // heuristic of the successor to the goal
            wp_succ_data = {0, h_s, h_s};
            open[successors[i_s]] = wp_succ_data; // add the successor in the open list
          }
        }
        wp_succ_data = open.at(successors[i_s]);                                 
        wp_succ_data[0] = g_s; // set the g(node successor)= g_s  
        wp_succ_data[2] = wp_succ_data[0] + wp_succ_data[1]; // evaluate f
        open[successors[i_s]] = wp_succ_data; // add the node successor to the open list
        parent.erase(successors[i_s]); // clear the parents of the successor
        parent[successors[i_s]] = wp_curr; // set the parent of the node successor to node current
        done:
        continue;
      }
      close[wp_curr] = wp_curr_data; // add current node to the close list
      open.erase(wp_curr);// remove the current node from the open list
    }
  }
  if (wp_curr != wp_goal){
    cout << "error, open is empty, not avaiable path" << endl;
  }
}

void VisitSolver::parseLandmark(string landmark_file){

  int curr, next;
  string line;
  double pose1, pose2, pose3;
  ifstream parametersFile(landmark_file);
  if (parametersFile.is_open()){
    while (getline(parametersFile,line)){
      curr=line.find("[");
      string landmark_name = line.substr(0,curr).c_str();
      
      curr=curr+1;
      next=line.find(",",curr);

      pose1 = (double)atof(line.substr(curr,next-curr).c_str());
      curr=next+1; next=line.find(",",curr);

      pose2 = (double)atof(line.substr(curr,next-curr).c_str());
      curr=next+1; next=line.find("]",curr);

      pose3 = (double)atof(line.substr(curr,next-curr).c_str());

      landmark[landmark_name] = vector<double> {pose1, pose2, pose3};
    }
  }
}







