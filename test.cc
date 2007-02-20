/*********************************************************************

  EVG-THIN v1.1: A Thinning Approximation to the Extended Voronoi Graph
  
  Copyright (C) 2006 - Patrick Beeson  (pbeeson@cs.utexas.edu)


  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
  USA

*********************************************************************/


#include <iostream>
#include <float.h>
#include <math.h>
#include "datatypes.hh"
#include "fileio.hh"
#include "evg-thin.hh"
#include "utils.hh"

using namespace std;


char* imagefile=NULL;
char* outfile=NULL;
unsigned int unknown_min=127;
unsigned int unknown_max=128;
bool pruning = true;
bool robot_close = true;
float distance_min=0.0;
float distance_max=FLT_MAX;
int robot_locx=-1;
int robot_locy=-1;

void parse_params(int argc, char *argv[]) {
  if (argc < 2) {
    cout << "Please run with -? for runtime options.\n";
    exit (0);
  }
  
  for (int i=1;i<argc;i++) {
    if (0==strcmp(argv[i],"-?")) {
      cout << "\nTo run : ./test -image-file FILENAME1 <options>\n\n";
      cout << "options (see README for details):\n";
      cout << "-output-file FILENAME2\n";
      cout << "-min-unknown N\n";
      cout << "-max-unknown M\n";
      cout << "-pruning [0|1]\n";
      cout << "-min-distance R\n";
      cout << "-max-distance S\n";
      cout << "-robot_loc X Y\n";
      cout << "-robot-close [0|1]\n\n";
      exit (0);
    }
    else 
      if (i+2 < argc &&
	  0==strcmp(argv[i],"-robot-loc")) {
	robot_locx=atoi(argv[++i]);
	robot_locy=atoi(argv[++i]);
      }
      else
	
	if (i+1 < argc) {
	  if (0==strcmp(argv[i],"-image-file"))
	    imagefile=argv[++i];
	  else
	    if (0==strcmp(argv[i],"-output-file"))
	      outfile=argv[++i];
	    else
	    if (0==strcmp(argv[i],"-min-unknown"))
	      unknown_min=atoi(argv[++i]);
	    else
	      if (0==strcmp(argv[i],"-max-unknown"))
		unknown_max=atoi(argv[++i]);
	      else
		if (0==strcmp(argv[i],"-pruning"))
		  pruning=atoi(argv[++i]);
		else
		  if (0==strcmp(argv[i],"-robot-close"))
		    robot_close=atoi(argv[++i]);
		  else 	      
		    if (0==strcmp(argv[i],"-min-distance"))
		      distance_min=fmaxf(0,atof(argv[++i]));
		    else 	      
		      if (0==strcmp(argv[i],"-max-distance"))
			distance_max=atof(argv[++i]);
		      else {
			cout << "Incorrect parameter list.\n";
			cout << "Please run with -? for runtime options.\n";
			exit(0);
		      }
	}
	else {
	  cout << "Incorrect parameter lists.\n";
	  cout << "Please run with -? for runtime options.\n";
	  exit(0);
	}
  }
}


int main(int argc, char *argv[]) {
  parse_params(argc,argv);

  if (imagefile==NULL) {
    cout << "No image given.\n";
    cout << "Please run with -? for runtime options.\n";
    exit (0);
  }

  if (unknown_min > unknown_max) {
    cout << "min_unknown value must be <= max_unknown value\n";
    cout << "Please run with -? for runtime options.\n";
    exit (0);
  }

  if (distance_min > distance_max) {
    cout << "min_distance value must be <= max_distance value\n";
    cout << "Please run with -? for runtime options.\n";
    exit (0);
  }

  if (unknown_min < 1 || unknown_max > 254) {
    cout << "unknown values can be between 1 and 254.\n";
    cout << "Please run with -? for runtime options.\n";
    exit (0);
  }

  fileio IO;

  grid_type curr_grid=IO.read_file(imagefile,unknown_min,unknown_max);

  if (curr_grid.empty() || curr_grid[0].empty()) {
    cout << "Read grid has no dimensions\n";
    exit(-1);
  }

  if (robot_locx < 0 || robot_locx >= int(curr_grid.size()) ||
      robot_locy < 0 || robot_locy >= int(curr_grid[0].size())) {
    robot_locx=curr_grid.size()/2;
    robot_locy=curr_grid[0].size()/2;
  }



  if (outfile==NULL) {
    int delim=strcspn(imagefile,".");
    outfile=new char[delim+14];
    strncpy(outfile,imagefile,delim);
    outfile[delim]='\0';
    strcat(outfile,"_skeleton.ppm");
  }

  cout << endl<< "Input file: " << imagefile << endl;
  cout << "Output file: " << outfile << endl;
  cout << "OCCUPIED grayscale range: 0-" << unknown_min-1 << endl;
  cout << "UNKNOWN grayscale range: " << unknown_min << "-" << unknown_max << endl;
  cout << "FREESPACE grayscale range: " << unknown_max+1 << "-255" << endl;
  cout << "Pruning: ";
  if (pruning)
    cout << "On\n";
  else cout << "Off\n";
  cout << "Robot close: ";
  if (robot_close)
    cout << "On\n";
  else cout << "Off\n";
  cout << "Minimum distance: " << distance_min << endl;
  cout << "Maximum distance: " << distance_max << endl;
  cout << "Robot location: "<<robot_locx<<", "<<robot_locy<<endl<<endl;

  evg_thin thin(curr_grid,distance_min,distance_max,pruning,robot_close,robot_locx,robot_locy);

  Time t;
  skeleton_type skel=thin.generate_skeleton();
  cout << "Skeleton created in "<<t.get_since()<<" seconds\n\n";

  IO.save_file(skel,outfile);

  return 1;
}
