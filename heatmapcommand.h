#ifndef HEATMAPCOMMAND_H
#define HEATMAPCOMMAND_H

/*
 *  heatmapcommand.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 3/25/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */


#include "command.hpp"
#include "inputdata.h"
#include "readotu.h"
#include "sharedlistvector.h"
#include "heatmap.h"
#include "rabundvector.hpp"

class GlobalData;

class HeatMapCommand : public Command {

public:
	HeatMapCommand(string);
	~HeatMapCommand();
	int execute();
	void help();
	
private:
	GlobalData* globaldata;
	ReadOTUFile* read;
	InputData* input;
	SharedListVector* SharedList;
	RAbundVector* rabund;
	vector<SharedRAbundVector*> lookup;
	HeatMap* heatmap;
	OptionParser* parser;
	map<string, string> parameters;
	map<string, string>::iterator it;
	bool abort, allLines;
	set<int> lines; //hold lines to be used
	set<string> labels; //holds labels to be used
	string format, groups, sorted, scale, line, label;
	vector<string> Groups;


};

#endif

