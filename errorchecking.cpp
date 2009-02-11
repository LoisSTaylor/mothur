/*
 *  errorchecking.cpp
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/2/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "errorchecking.h"

/*******************************************************/

/******************************************************/

ErrorCheck::ErrorCheck() {
	globaldata = GlobalData::getInstance();
	validCommand = new ValidCommands();
	validParameter = new ValidParameters();
	validCalculator = new ValidCalculators();
}
/*******************************************************/

/******************************************************/

void ErrorCheck::refresh() {
	columnfile = globaldata->getColumnFile();
	phylipfile = globaldata->getPhylipFile();
	listfile = globaldata->getListFile();
	rabundfile = globaldata->getRabundFile();
	sabundfile = globaldata->getSabundFile();
	namefile = globaldata->getNameFile();
	groupfile = globaldata->getGroupFile();
	orderfile = globaldata->getOrderFile();
	fastafile = globaldata->getFastaFile();
	treefile = globaldata->getTreeFile();
	cutoff = globaldata->getCutOff();
	format = globaldata->getFormat();
	method = globaldata->getMethod();
	randomtree = globaldata->getRandomTree();
}

/*******************************************************/

/******************************************************/

ErrorCheck::~ErrorCheck() {}

/*******************************************************/

/******************************************************/

bool ErrorCheck::checkInput(string input) {
		errorFree = true;
		clear();
		
		//refresh variable
		refresh();
		
		//get command name and parameters
		int openParen = input.find_first_of('(');
		int closeParen = input.find_last_of(')');

		if(openParen != -1 && closeParen != -1){			
			commandName = input.substr(0, openParen);   //commandName contains everything before "("
			optionText = input.substr(openParen+1, closeParen-openParen-1); //optionString contains everything between "(" and ")".
		}else if (openParen == -1) { //there is no parenthesis
			cout << input << " is not a valid command. You are missing the ()." << endl;
			return false;
		}
		
		//is it a valid command
		if (validCommand->isValidCommand(commandName) != true) { return false; }
		
		string parameter, value;
		//reads in parameters and values
		if((optionText != "") && (commandName != "help")){
			while((optionText.find_first_of(',') != -1) && (errorFree)) {  //while there are parameters
				globaldata->splitAtComma(value, optionText);
				globaldata->splitAtEquals(parameter, value);
				
				//is it a valid parameter
				if (validParameter->isValidParameter(parameter) != true) { return false; }
				
				if (parameter == "phylip" )		{ phylipfile = value; }
				if (parameter == "column" )		{ columnfile = value; }
				if (parameter == "list" )		{ listfile = value; }
				if (parameter == "rabund" )		{ rabundfile = value; }
				if (parameter == "sabund" )		{ sabundfile = value; }
				if (parameter == "name" )		{ namefile = value; }
				if (parameter == "order" )		{ orderfile = value; }
				if (parameter == "fasta" )		{ fastafile = value; }
				if (parameter == "treefile" )		{ treefile = value; }
				if (parameter == "group" )		{ groupfile = value; }
				if (parameter == "cutoff" )			{ cutoff = value; }
				if (parameter == "precision" )		{ precision = value; }
				if (parameter == "iters" )			{ iters = value; }
				if (parameter == "jumble" )			{ jumble = value; }
				if (parameter == "freq" )			{ freq = value; }
				if (parameter == "method" )			{ method = value; }
				if (parameter == "fileroot" )		{ fileroot = value; }
				if (parameter == "line" )			{ line = value; }
				if (parameter == "label" )			{ label = value; }
				if (parameter == "randomtree" )		{ randomtree = value;	}

				if (parameter == "single") {//stores estimators in a vector
					singleEsimators.clear(); //clears out old values
					globaldata->splitAtDash(value, singleEsimators);
					for (int i = 0; i < singleEsimators.size(); i++) { //loop through estimators
						//is it a valid calculator
						if (validCalculator->isValidCalculator(parameter, singleEsimators[i]) != true) { return false; }
					}
				}
				if (parameter == "rarefaction") {//stores estimators in a vector
					rareEstimators.clear(); //clears out old values
					globaldata->splitAtDash(value, rareEstimators);
					for (int i = 0; i < rareEstimators.size(); i++) { //loop through estimators
						//is it a valid calculator
						if (validCalculator->isValidCalculator(parameter, rareEstimators[i]) != true) { return false; }
					}
				}
				if (parameter == "shared") {//stores estimators in a vector
					sharedEstimators.clear(); //clears out old values
					globaldata->splitAtDash(value, sharedEstimators);
					for (int i = 0; i < sharedEstimators.size(); i++) { //loop through estimators
						//is it a valid calculator
						if (validCalculator->isValidCalculator(parameter, sharedEstimators[i]) != true) { return false; }
					}
				}
				if (parameter == "summary") { //stores summaries to be used in a vector
					summaryEstimators.clear(); //clears out old values
					globaldata->splitAtDash(value, summaryEstimators);
					for (int i = 0; i < summaryEstimators.size(); i++) { //loop through estimators
						//is it a valid calculator
						if (validCalculator->isValidCalculator(parameter, summaryEstimators[i]) != true) { return false; }
					}
				}
				if (parameter == "sharedrarefaction") { //stores summaries to be used in a vector
					sharedRareEstimators.clear(); //clears out old values
					globaldata->splitAtDash(value, sharedRareEstimators);
					for (int i = 0; i < sharedRareEstimators.size(); i++) { //loop through estimators
						//is it a valid calculator
						if (validCalculator->isValidCalculator(parameter, sharedRareEstimators[i]) != true) { return false; }
					}
				}
			}
			
			//gets the last parameter and value
			if (errorFree)  { //gets the last parameter and value
				value = optionText;
				globaldata->splitAtEquals(parameter, value);
				//is it a valid parameter
				if (validParameter->isValidParameter(parameter) != true) { return false; }
				
				if (parameter == "phylip" )		{ phylipfile = value; }
				if (parameter == "column" )		{ columnfile = value; }				
				if (parameter == "list" )		{ listfile = value; }
				if (parameter == "rabund" )		{ rabundfile = value; }
				if (parameter == "sabund" )		{ sabundfile = value; }
				if (parameter == "name" )		{ namefile = value; }
				if (parameter == "order" )		{ orderfile = value; }
				if (parameter == "group" )		{ groupfile = value; }
				if (parameter == "fasta" )		{ fastafile = value; }
				if (parameter == "treefile" )		{ treefile = value; }
				if (parameter == "cutoff" )			{ cutoff = value; }
				if (parameter == "precision" )		{ precision = value; }
				if (parameter == "iters" )			{ iters = value; }
				if (parameter == "jumble" )			{ jumble = value; }
				if (parameter == "freq" )			{ freq = value; }
				if (parameter == "method" )			{ method = value; }
				if (parameter == "fileroot" )		{ fileroot = value; }
				if (parameter == "line" )			{ line = value; }
				if (parameter == "label" )			{ label = value; }
				if (parameter == "randomtree" )		{ randomtree = value;	}

				if (parameter == "single") {//stores estimators in a vector
					singleEsimators.clear(); //clears out old values
					globaldata->splitAtDash(value, singleEsimators);
					for (int i = 0; i < singleEsimators.size(); i++) { //loop through estimators
						//is it a valid calculator
						if (validCalculator->isValidCalculator(parameter, singleEsimators[i]) != true) { return false; }
					}
				}
				if (parameter == "rarefaction") {//stores estimators in a vector
					rareEstimators.clear(); //clears out old values
					globaldata->splitAtDash(value, rareEstimators);
					for (int i = 0; i < rareEstimators.size(); i++) { //loop through estimators
						//is it a valid calculator
						if (validCalculator->isValidCalculator(parameter, rareEstimators[i]) != true) { return false; }
					}
				}
				if (parameter == "shared") {//stores estimators in a vector
					sharedEstimators.clear(); //clears out old values
					globaldata->splitAtDash(value, sharedEstimators);
					for (int i = 0; i < sharedEstimators.size(); i++) { //loop through estimators
						//is it a valid calculator
						if (validCalculator->isValidCalculator(parameter, sharedEstimators[i]) != true) { return false; }
					}
				}
				if (parameter == "summary") { //stores summaries to be used in a vector
					summaryEstimators.clear(); //clears out old values
					globaldata->splitAtDash(value, summaryEstimators);
					for (int i = 0; i < summaryEstimators.size(); i++) { //loop through estimators
						//is it a valid calculator
						if (validCalculator->isValidCalculator(parameter, summaryEstimators[i]) != true) { return false; }
					}
				}
				if (parameter == "sharedrarefaction") { //stores summaries to be used in a vector
					sharedRareEstimators.clear(); //clears out old values
					globaldata->splitAtDash(value, sharedRareEstimators);
					for (int i = 0; i < sharedRareEstimators.size(); i++) { //loop through estimators
						//is it a valid calculator
						if (validCalculator->isValidCalculator(parameter, sharedRareEstimators[i]) != true) { return false; }
					}
				}

			}
		}
		
		//make sure the user does not use both the line and label parameters
		if ((line != "") && (label != "")) { cout << "You may use either the line or label parameters, but not both." << endl; return false; }
	
		
		if (commandName == "read.dist") { 
			validateReadFiles();
			validateReadDist();
		}else if (commandName == "read.otu") { 
			//you want to do shared commands
			if ((listfile != "") && (groupfile != ""))	{
				validateParseFiles(); //checks the listfile and groupfile parameters
			}else { //you want to do single commands
				validateReadFiles();
				validateReadPhil();
			}
		}else if (commandName == "read.tree") { 
			validateTreeFiles(); //checks the treefile and groupfile parameters
		}else if (commandName == "deconvolute") {
			if (fastafile == "") { cout << "You must enter a fastafile with the deconvolute() command." << endl; return false; }
			validateReadFiles();
		}
		
		//are you trying to cluster before you have read something			
		if ((commandName == "cluster") && (globaldata->getSparseMatrix() == NULL) ||
			(commandName == "cluster") && (globaldata->getListVector() == NULL)) {
				cout << "Before you use the cluster command, you first need to read in a distance matrix." << endl; 
				errorFree = false;
		} 
		
		if (commandName == "parsimony") {
			//are you trying to use parsimony without reading a tree or saying you want random distribution
			if (randomtree == "0")  {
				if ((globaldata->getTreeFile() == "") || (globaldata->getGroupFile() == "")) {
					cout << "You must read a treefile and a groupfile or set the randomtree parameter to 1, before you may execute the parsimony command." << endl; return false;  }
			}
		}
		
		//check for valid method
		if (commandName == "cluster") {
			if ((method == "furthest") || (method == "nearest") || (method == "average")) { }
			else {cout << "Not a valid clustering method.  Valid clustering algorithms are furthest, nearest or average." << endl; return false; }
		}
		
		if ((commandName == "collect.single") || (commandName == "rarefaction.single") || (commandName == "summary.single") ){ 
			if ((globaldata->getListFile() == "") && (globaldata->getRabundFile() == "") && (globaldata->getSabundFile() == "")) { cout << "You must read a list, sabund or rabund before you can use the collect.single, rarefaction.single or summary.single commands." << endl; return false; }
		}
		
		if ((commandName == "collect.shared") || (commandName == "rarefaction.shared") || (commandName == "summary.shared") ){ 
			if (globaldata->getListFile() == "") { cout << "You must read a list and a group before you can use the collect.shared, rarefaction.shared or summary.shared commands." << endl; return false; }
			else if (globaldata->getGroupFile() == "") { cout << "You must read a list and a group before you can use the collect.shared, rarefaction.shared or summary.shared commands." << endl; return false; }
		}
 
		
		return errorFree;
}

/*******************************************************/

/******************************************************/
//This function checks to make sure the user entered a file to 
// read and that the file exists and can be opened.
void ErrorCheck::validateReadFiles() {
	try {
		//Validating files for read
		ifstream filehandle;
		int ableToOpen;
	
		//are we reading a phylipfile
		if (phylipfile != "") {
			ableToOpen = openInputFile(phylipfile, filehandle);
			filehandle.close();
			//unable to open
			if (ableToOpen == 1) { errorFree = false; }
			else { globaldata->inputFileName = phylipfile; }
		//are we reading a phylipfile
		}else if (columnfile != "") {
			ableToOpen = openInputFile(columnfile, filehandle);
			filehandle.close();
			//unable to open
			if (ableToOpen == 1) { errorFree = false; }
			else { globaldata->inputFileName = columnfile; }
		//are we reading a listfile
		}else if (listfile!= "") {
			ableToOpen = openInputFile(listfile, filehandle);
			filehandle.close();
			//unable to open
			if (ableToOpen == 1) {  errorFree = false; }
			else { globaldata->inputFileName = listfile; }
		//are we reading a rabundfile
		}else if (rabundfile != "") {
			ableToOpen = openInputFile(rabundfile, filehandle);
			filehandle.close();
			//unable to open
			if (ableToOpen == 1) {  errorFree = false; }
			else { globaldata->inputFileName = rabundfile; }
		//are we reading a sabundfile
		}else if (sabundfile != "") {
			ableToOpen = openInputFile(sabundfile, filehandle);
			filehandle.close();
			//unable to open
			if (ableToOpen == 1) {  errorFree = false; }
			else { globaldata->inputFileName = sabundfile; }
		}else if (fastafile != "") {
			ableToOpen = openInputFile(fastafile, filehandle);
			filehandle.close();
			//unable to open
			if (ableToOpen == 1) {  errorFree = false; }
			else { globaldata->inputFileName = fastafile; }
		}else{ //no file given
			errorFree = false;
		}
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the ErrorCheck class Function validateReadFiles. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the ErrorCheck class function validateReadFiles. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	
}
/*******************************************************/

/******************************************************/
//This function checks to make sure the user entered appropriate
// format parameters on a distfile read
void ErrorCheck::validateReadDist() {
	try {
		ifstream filehandle;
		int ableToOpen;
		
		if ((phylipfile == "") && (columnfile == "")) { cout << "When executing a read.dist you must enter a phylip or a column." << endl; errorFree = false; }
		else if ((phylipfile != "") && (columnfile != "")) { cout << "When executing a read.dist you must enter ONLY ONE of the following: phylip or column." << endl; errorFree = false; }
		
		if (columnfile != "") {
			if (namefile == "") {
				cout << "You need to provide a namefile if you are going to use the column format." << endl;
				errorFree = false; 
			}else {
				ableToOpen = openInputFile(namefile, filehandle);
				filehandle.close();
				//unable to open
				if (ableToOpen == 1) { errorFree = false; }
			}
		}
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the ErrorCheck class Function validateReadDist. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the ErrorCheck class function validateReadDist. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
}
/*******************************************************/

/******************************************************/
//This function checks to make sure the user entered appropriate
// format parameters on a parselistcommand
void ErrorCheck::validateParseFiles() {
	try {
		ifstream filehandle;
		int ableToOpen;
		
		//checks for valid files
	
		if (listfile == "") { cout << "When executing a read.otu for groups you must enter a list and a group." << endl; errorFree = false; }
		else if (groupfile == "") { cout << "When executing a read.otu for groups you must enter a list and a group." << endl; errorFree = false; }
	
		//checks parameters on the read command
		if (listfile != "") {
			ableToOpen = openInputFile(listfile, filehandle);
			filehandle.close();
			if (ableToOpen == 1) { //unable to open
				errorFree = false;
			}
			if (groupfile != "") {
				ableToOpen = openInputFile(groupfile, filehandle);
				filehandle.close();
				if (ableToOpen == 1) { //unable to open
					errorFree = false;;
				}
			}
		}
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the ErrorCheck class Function validateReadPhil. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the ErrorCheck class function validateReadPhil. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
}
/*******************************************************/

/******************************************************/
//This function checks to make sure the user entered appropriate
// format parameters on a parselistcommand
void ErrorCheck::validateTreeFiles() {
	try {
		ifstream filehandle;
		int ableToOpen;
		
		//checks for valid files
	
		if (treefile == "") { cout << "When executing a read.tree you must enter a treefile and a groupfile." << endl; errorFree = false; }
		else if (groupfile == "") { cout << "When executing a read.tree you must enter a treefile and a groupfile." << endl; errorFree = false; }
	
		//checks parameters on the read command
		if (treefile != "") {
			ableToOpen = openInputFile(treefile, filehandle);
			filehandle.close();
			if (ableToOpen == 1) { //unable to open
				errorFree = false;
			}
			if (groupfile != "") {
				ableToOpen = openInputFile(groupfile, filehandle);
				filehandle.close();
				if (ableToOpen == 1) { //unable to open
					errorFree = false;;
				}
			}
		}
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the ErrorCheck class Function validateTreeFiles. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the ErrorCheck class function validateTreeFiles. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
}

/*******************************************************/

/******************************************************/
//This function checks to make sure the user entered appropriate
// format parameters on a distfile read
void ErrorCheck::validateReadPhil() {
	try {
		ifstream filehandle;
		int ableToOpen;
		
		//checks to make sure only one file type is given
		if (listfile != "") { 
			if ((rabundfile != "") || (sabundfile != "")) { 
				cout << "When executing a read.otu you must enter ONLY ONE of the following: list, rabund or sabund." << endl; errorFree = false; }
		}else if (rabundfile != "") { 
			if ((listfile != "") || (sabundfile != "")) { 
				cout << "When executing a read.otu you must enter ONLY ONE of the following: list, rabund or sabund." << endl; errorFree = false; }
		}else if (sabundfile != "") { 
			if ((listfile != "") || (rabundfile != "")) { 
				cout << "When executing a read.otu you must enter ONLY ONE of the following: list, rabund or sabund." << endl; errorFree = false; }
		}else if ((listfile == "") && (rabundfile == "") && (sabundfile == "")) {
			    cout << "When executing a read.otu you must enter one of the following: list, rabund or sabund." << endl; errorFree = false; 
		}
		
		//checks parameters on the read command
		if (orderfile != "") {
			ableToOpen = openInputFile(orderfile, filehandle);
			filehandle.close();
			if (ableToOpen == 1) { //unable to open
				errorFree = false;
			}
		}	
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the ErrorCheck class Function validateReadPhil. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the ErrorCheck class function validateReadPhil. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
}
/*******************************************************/

/******************************************************/

void ErrorCheck::clear() {
	//option definitions should go here...
	phylipfile		=	"";
	columnfile		=	"";
	listfile		=	"";
	rabundfile		=	"";
	sabundfile		=	"";
	namefile		=	"";
	groupfile		=	""; 
	orderfile		=	"";
	line			=	"";
	label			=	"";
	method			=   "furthest";
}
/*******************************************************/

/******************************************************/

