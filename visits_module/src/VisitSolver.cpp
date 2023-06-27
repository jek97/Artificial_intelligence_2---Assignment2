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
  cout << "a" << endl;
  starting_position = "r0";
  string Paramers = parameters[0];

  char const *x[]={"dummy"};
  char const *y[]={"act-cost","triggered"};
  parseParameters(Paramers);
  affected = list<string>(x,x+1);
  dependencies = list<string>(y,y+2);

  string waypoint_file = "visits_domain/waypoint.txt";   // change this to the correct path
  parseWaypoint(waypoint_file);

  string landmark_file = "visits_domain/landmark.txt";  // change this to the correct path
  parseLandmark(landmark_file);
  //startEKF();
}

map<string,double> VisitSolver::callExternalSolver(map<string,double> initialState,bool isHeuristic){
  cout << "b" << endl;
  // it takes initial cost and heuristic, in initial state seams that are implemented the initial state of the robot and the final one
  //
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

          string from = tmp.substr(0,2);   // from and to are regions
          string to = tmp.substr(3,2);
          // distance_euc(from, to);

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
  cout << "d" << endl;

  int curr, next;
  string line;
  ifstream parametersFile(parameters.c_str());
  if (parametersFile.is_open()){
    while (getline(parametersFile,line)){
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

//double VisitSolver::distance_euc(string from, string to){

//}

double VisitSolver::calculateExtern(double external, double total_cost){
  cout << "e" << endl;
  //float random1 = static_cast <float> (rand())/static_cast <float>(RAND_MAX);
  double cost = 5;//random1;
  return cost;
}


void VisitSolver::parseWaypointConnection(string waypoint_file){
  // let's enstablish a structure of the wayppoints file as follow: wp0[0,0,0,wp1,wp2,wp3,wp4,wp5,wp6] where then you have the connected waypoint to that one, fundamental all the waypoints must be connected to the same number of waypoints.
  int curr, next, end, flag;
  flag = 0;
  string line, wp;
  double pose1, pose2, pose3;
  vector<string> wp_connected;
  ifstream parametersFile(waypoint_file);
  if (parametersFile.is_open()){
    while (getline(parametersFile,line)){
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

      while(flag == 0){
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
  // let's enstablish a structure of the regions file as follow: r0=wp0.
  int curr, next;
  string line;
  ifstream parametersFile(region_file);
  if (parametersFile.is_open()){
    while (getline(parametersFile,line)){
      curr=line.find("=");
      string region_name = line.substr(0,curr).c_str();

      curr=curr+1;
      next=line.find(".",curr);

      string wp = line.substr(curr,next-curr).c_str();

      region[region_name] = wp;
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

void VisitSolver::pathfinder(string reg_from, string reg_to){

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







