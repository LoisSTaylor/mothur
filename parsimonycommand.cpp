/*
 *  parsimonycommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 1/26/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "parsimonycommand.h"

/***********************************************************/
ParsimonyCommand::ParsimonyCommand(string option) {
	try {
		globaldata = GlobalData::getInstance();
		abort = false;
		Groups.clear();
		
		//allow user to run help
		if(option == "help") { help(); abort = true; }
		
		else {
			//valid paramters for this command
			string Array[] =  {"random","groups","iters"};
			vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
			
			parser = new OptionParser();
			parser->parse(option, parameters);  delete parser;
			
			ValidParameters* validParameter = new ValidParameters();
		
			//check to make sure all parameters are valid for command
			for (it4 = parameters.begin(); it4 != parameters.end(); it4++) { 
				if (validParameter->isValidParameter(it4->first, myArray, it4->second) != true) {  abort = true;  }
			}
			
			randomtree = validParameter->validFile(parameters, "random", false);		if (randomtree == "not found") { randomtree = ""; }
			
			//are you trying to use parsimony without reading a tree or saying you want random distribution
			if (randomtree == "")  {
				if (globaldata->gTree.size() == 0) {
					cout << "You must read a treefile and a groupfile or set the randomtree parameter to the output filename you wish, before you may execute the parsimony command." << endl; abort = true;  }
			}
						
			//check for optional parameter and set defaults
			// ...at some point should added some additional type checking...
			groups = validParameter->validFile(parameters, "groups", false);			
			if (groups == "not found") { groups = ""; }
			else { 
				splitAtDash(groups, Groups);
				globaldata->Groups = Groups;
			}
				
			itersString = validParameter->validFile(parameters, "iters", false);			if (itersString == "not found") { itersString = "1000"; }
			convert(itersString, iters); 
			
			delete validParameter;
			
			if (abort == false) {
				//randomtree will tell us if user had their own treefile or if they just want the random distribution
				//user has entered their own tree
				if (randomtree == "") { 
					T = globaldata->gTree;
					tmap = globaldata->gTreemap;
					output = new ColumnFile(globaldata->getTreeFile()  +  ".parsimony", itersString);
					sumFile = globaldata->getTreeFile() + ".psummary";
					openOutputFile(sumFile, outSum);
				}else { //user wants random distribution
					savetmap = globaldata->gTreemap;
					getUserInput();
					output = new ColumnFile(randomtree, itersString);
				}
				
				//set users groups to analyze
				util = new SharedUtil();
				util->setGroups(globaldata->Groups, tmap->namesOfGroups, allGroups, numGroups, "unweighted");	//sets the groups the user wants to analyze
				util->getCombos(groupComb, globaldata->Groups, numComp);
				
				if (numGroups == 1) { numComp++; groupComb.push_back(allGroups); }
				
				pars = new Parsimony(tmap);
				counter = 0;
				
			}
			
		}

	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the ParsimonyCommand class Function ParsimonyCommand. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the ParsimonyCommand class function ParsimonyCommand. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
}

//**********************************************************************************************************************

void ParsimonyCommand::help(){
	try {
		cout << "The parsimony command can only be executed after a successful read.tree command, unless you use the random parameter." << "\n";
		cout << "The parsimony command parameters are random, groups and iters.  No parameters are required." << "\n";
		cout << "The groups parameter allows you to specify which of the groups in your groupfile you would like analyzed.  You must enter at least 1 valid group." << "\n";
		cout << "The group names are separated by dashes.  The iters parameter allows you to specify how many random trees you would like compared to your tree." << "\n";
		cout << "The parsimony command should be in the following format: parsimony(random=yourOutputFilename, groups=yourGroups, iters=yourIters)." << "\n";
		cout << "Example parsimony(random=out, iters=500)." << "\n";
		cout << "The default value for random is "" (meaning you want to use the trees in your inputfile, randomtree=out means you just want the random distribution of trees outputted to out.rd_parsimony)," << "\n";
		cout << "and iters is 1000.  The parsimony command output two files: .parsimony and .psummary their descriptions are in the manual." << "\n";
		cout << "Note: No spaces between parameter labels (i.e. random), '=' and parameters (i.e.yourOutputFilename)." << "\n" << "\n";
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the ParsimonyCommand class Function help. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the ParsimonyCommand class function help. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}	
}


/***********************************************************/
int ParsimonyCommand::execute() {
	try {
	
		if (abort == true) { return 0; }
	
		Progress* reading;
		reading = new Progress("Comparing to random:", iters);
		
		//get pscore for users tree
		userData.resize(numComp,0);  //data = AB, AC, BC, ABC.
		randomData.resize(numComp,0);  //data = AB, AC, BC, ABC.
		rscoreFreq.resize(numComp);  
		uscoreFreq.resize(numComp);  
		rCumul.resize(numComp);  
		uCumul.resize(numComp);  
		userTreeScores.resize(numComp);  
		UScoreSig.resize(numComp); 
				
		if (randomtree == "") {
			//get pscores for users trees
			for (int i = 0; i < T.size(); i++) {
				userData = pars->getValues(T[i]);  //data = AB, AC, BC, ABC.

				//output scores for each combination
				for(int k = 0; k < numComp; k++) {

					//update uscoreFreq
					it = uscoreFreq[k].find(userData[k]);
					if (it == uscoreFreq[k].end()) {//new score
						uscoreFreq[k][userData[k]] = 1;
					}else{ uscoreFreq[k][userData[k]]++; }
					
					//add users score to valid scores
					validScores[userData[k]] = userData[k];
					
					//save score for summary file
					userTreeScores[k].push_back(userData[k]);
				}
			}
			
			//get pscores for random trees
			for (int j = 0; j < iters; j++) {
				//create new tree with same num nodes and leaves as users
				randT = new Tree();

				//create random relationships between nodes
				randT->assembleRandomTree();

				//get pscore of random tree
				randomData = pars->getValues(randT);
					
				for(int r = 0; r < numComp; r++) {
					//add trees pscore to map of scores
					it2 = rscoreFreq[r].find(randomData[r]);
					if (it2 != rscoreFreq[r].end()) {//already have that score
						rscoreFreq[r][randomData[r]]++;
					}else{//first time we have seen this score
						rscoreFreq[r][randomData[r]] = 1;
					}
			
					//add randoms score to validscores
					validScores[randomData[r]] = randomData[r];
				}
				
				//update progress bar
				reading->update(j);
				
				delete randT;
			}

		}else {
			//get pscores for random trees
			for (int j = 0; j < iters; j++) {
				//create new tree with same num nodes and leaves as users
				randT = new Tree();
				//create random relationships between nodes

				randT->assembleRandomTree();

				//get pscore of random tree
				randomData = pars->getValues(randT);
			
				for(int r = 0; r < numComp; r++) {
					//add trees pscore to map of scores
					it2 = rscoreFreq[r].find(randomData[r]);
					if (it2 != rscoreFreq[r].end()) {//already have that score
						rscoreFreq[r][randomData[r]]++;
					}else{//first time we have seen this score
						rscoreFreq[r][randomData[r]] = 1;
					}
			
					//add randoms score to validscores
					validScores[randomData[r]] = randomData[r];
				}
				
				//update progress bar
				reading->update(j);
				
				delete randT;
			}
		}

		for(int a = 0; a < numComp; a++) {
			float rcumul = 0.0000;
			float ucumul = 0.0000;
			//this loop fills the cumulative maps and put 0.0000 in the score freq map to make it easier to print.
			for (it = validScores.begin(); it != validScores.end(); it++) { 
				if (randomtree == "") {
					it2 = uscoreFreq[a].find(it->first);
					//user data has that score 
					if (it2 != uscoreFreq[a].end()) { uscoreFreq[a][it->first] /= T.size(); ucumul+= it2->second;  }
					else { uscoreFreq[a][it->first] = 0.0000; } //no user trees with that score
					//make uCumul map
					uCumul[a][it->first] = ucumul;
				}
			
				//make rscoreFreq map and rCumul
				it2 = rscoreFreq[a].find(it->first);
				//get percentage of random trees with that info
				if (it2 != rscoreFreq[a].end()) {  rscoreFreq[a][it->first] /= iters; rcumul+= it2->second;  }
				else { rscoreFreq[a][it->first] = 0.0000; } //no random trees with that score
				rCumul[a][it->first] = rcumul;
			}
			
			//find the signifigance of each user trees score when compared to the random trees and save for printing the summary file
			for (int h = 0; h < userTreeScores[a].size(); h++) {
				UScoreSig[a].push_back(rCumul[a][userTreeScores[a][h]]);
			}
		}
		
		//finish progress bar
		reading->finish();
		delete reading;

		
		printParsimonyFile();
		if (randomtree == "") { printUSummaryFile(); }
		
		//reset globaldata's treemap if you just did random distrib
		if (randomtree != "") {
			//memory leak prevention
			//if (globaldata->gTreemap != NULL) { delete globaldata->gTreemap;  }
			globaldata->gTreemap = savetmap;
		}
		
		//reset groups parameter
		globaldata->Groups.clear(); 
		
		return 0;
		
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the ParsimonyCommand class Function execute. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the ParsimonyCommand class function execute. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
}

/***********************************************************/
void ParsimonyCommand::printParsimonyFile() {
	try {
		vector<double> data;
		vector<string> tags;
		
		if (randomtree == "") {
			tags.push_back("Score"); tags.push_back("UserFreq"); tags.push_back("UserCumul"); tags.push_back("RandFreq"); tags.push_back("RandCumul");
		}else {
			tags.push_back("Score"); tags.push_back("RandFreq"); tags.push_back("RandCumul");
		}

		for(int a = 0; a < numComp; a++) {
			output->initFile(groupComb[a], tags);
			//print each line
			for (it = validScores.begin(); it != validScores.end(); it++) { 
				if (randomtree == "") {
					data.push_back(it->first);  data.push_back(uscoreFreq[a][it->first]); data.push_back(uCumul[a][it->first]); data.push_back(rscoreFreq[a][it->first]); data.push_back(rCumul[a][it->first]); 
				}else{
					data.push_back(it->first);  data.push_back(rscoreFreq[a][it->first]); data.push_back(rCumul[a][it->first]); 
				}
				output->output(data);
				data.clear();
			} 
			output->resetFile();
		}
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the ParsimonyCommand class Function printParsimonyFile. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the ParsimonyCommand class function printParsimonyFile. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
}
/***********************************************************/
void ParsimonyCommand::printUSummaryFile() {
	try {
		//column headers
		outSum << "Tree#" << '\t' << "Groups" << '\t'  <<  "ParsScore" << '\t' << "ParsSig" <<  endl;
		cout << "Tree#" << '\t' << "Groups" << '\t'  <<  "ParsScore" << '\t' << "ParsSig" <<  endl;
		
		//format output
		outSum.setf(ios::fixed, ios::floatfield); outSum.setf(ios::showpoint);
		
		
		//print each line
		for (int i = 0; i< T.size(); i++) {
			for(int a = 0; a < numComp; a++) {
				if (UScoreSig[a][i] > (1/(float)iters)) {
					outSum << setprecision(6) << i+1 << '\t' << groupComb[a]  << '\t' << userTreeScores[a][i] << setprecision(itersString.length()) << '\t' << UScoreSig[a][i] << endl;
					cout << setprecision(6) << i+1 << '\t' << groupComb[a]  << '\t' << userTreeScores[a][i] << setprecision(itersString.length()) << '\t' << UScoreSig[a][i] << endl;
				}else {
					outSum << setprecision(6) << i+1 << '\t' << groupComb[a] << '\t' << userTreeScores[a][i] << setprecision(itersString.length())  << '\t' << "<" << (1/float(iters)) << endl;
					cout << setprecision(6) << i+1 << '\t' << groupComb[a] << '\t' << userTreeScores[a][i] << setprecision(itersString.length()) << '\t' << "<" << (1/float(iters)) << endl;
				}
			}
		}
		
		outSum.close();
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the ParsimonyCommand class Function printUSummaryFile. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the ParsimonyCommand class function printUSummaryFile. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
}

/***********************************************************/
void ParsimonyCommand::getUserInput() {
	try {
	
		//create treemap
		tmap = new TreeMap();

		cout << "Please enter the number of groups you would like to analyze: ";
		cin >> numGroups;
			
		int num, count;
		count = 1;
		numEachGroup.resize(numGroups, 0);  
		
		for (int i = 1; i <= numGroups; i++) {
			cout << "Please enter the number of sequences in group " << i <<  ": ";
			cin >> num;
				
			//set tmaps seqsPerGroup
			tmap->seqsPerGroup[toString(i)] = num;
			tmap->namesOfGroups.push_back(toString(i));
			
			//set tmaps namesOfSeqs
			for (int j = 0; j < num; j++) {
				tmap->namesOfSeqs.push_back(toString(count));
				tmap->treemap[toString(count)].groupname = toString(i);
				count++;
			}
		}
		
		//clears buffer so next command doesn't have error
		string s;	
		getline(cin, s);
		
		//save tmap for later
		//memory leak prevention
		//if (globaldata->gTreemap != NULL) { delete globaldata->gTreemap;  }
		globaldata->gTreemap = tmap;
		globaldata->Treenames = tmap->namesOfSeqs; 
		
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the ParsimonyCommand class Function getUserInput. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the ParsimonyCommand class function getUserInput. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
}

/***********************************************************/


