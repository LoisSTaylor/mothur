/*
 *  readcluster.cpp
 *  Mothur
 *
 *  Created by westcott on 10/28/09.
 *  Copyright 2009 Schloss Lab. All rights reserved.
 *
 */

#include "readcluster.h"

/***********************************************************************/

ReadCluster::ReadCluster(string distfile, float c, string o, bool s){
		m = MothurOut::getInstance();
        distFile = distfile;
		cutoff = c;
		outputDir = o;
		sortWanted = s;
		list = NULL;
}

/***********************************************************************/

int ReadCluster::read(NameAssignment*& nameMap){
	try {
        
		if (format == "phylip") { convertPhylip2Column(nameMap); }
		else { list = new ListVector(nameMap->getListVector());  }
		
		if (m->control_pressed) { return 0; }
		
		if (sortWanted) {  OutPutFile = m->sortFile(distFile, outputDir);  }
		else {  OutPutFile = distFile;   } //for use by clusters splitMatrix to convert a phylip matrix to column
		
		return 0;
			
	}
	catch(exception& e) {
		m->errorOut(e, "ReadCluster", "read");
		exit(1);
	}
}
/***********************************************************************/
int ReadCluster::read(CountTable*& ct){
	try {
        
		if (format == "phylip") { convertPhylip2Column(ct); }
		else { list = new ListVector(ct->getListVector());  }
		
		if (m->control_pressed) { return 0; }
		
		if (sortWanted) {  OutPutFile = m->sortFile(distFile, outputDir);  }
		else {  OutPutFile = distFile;   } //for use by clusters splitMatrix to convert a phylip matrix to column
		
		return 0;
        
	}
	catch(exception& e) {
		m->errorOut(e, "ReadCluster", "read");
		exit(1);
	}
}
/***********************************************************************/

int ReadCluster::convertPhylip2Column(NameAssignment*& nameMap){
	try {	
		//convert phylip file to column file
		map<int, string> rowToName;
		map<int, string>::iterator it;
		
		ifstream in;
		ofstream out;
		string tempFile = distFile + ".column.temp";
		
		m->openInputFile(distFile, in);  m->gobble(in);
		m->openOutputFile(tempFile, out);
		
		float distance;
		int square, nseqs;
		string name;
		vector<string> matrixNames;
		
		string numTest;
		in >> numTest >> name;
		
		if (!m->isContainingOnlyDigits(numTest)) { m->mothurOut("[ERROR]: expected a number and got " + numTest + ", quitting."); m->mothurOutEndLine(); exit(1); }
		else { convert(numTest, nseqs); }
		
		rowToName[0] = name;
		matrixNames.push_back(name);
		
		if(nameMap == NULL){
			list = new ListVector(nseqs);
			list->set(0, name);
		}
		else{
			list = new ListVector(nameMap->getListVector());
			if(nameMap->count(name)==0){        m->mothurOut("Error: Sequence '" + name + "' was not found in the names file, please correct"); m->mothurOutEndLine(); }
		}
        
		char d;
		while((d=in.get()) != EOF){
			
			if(isalnum(d)){
				square = 1;
				in.putback(d);
				for(int i=0;i<nseqs;i++){
					in >> distance;
				}
				break;
			}
			if(d == '\n'){
				square = 0;
				break;
			}
		}
        
		if(square == 0){
					
			for(int i=1;i<nseqs;i++){
				in >> name;
				rowToName[i] = name;
				matrixNames.push_back(name);
				
				//there's A LOT of repeated code throughout this method...
				if(nameMap == NULL){
					list->set(i, name);
					
					for(int j=0;j<i;j++){
					
						if (m->control_pressed) { in.close(); out.close(); m->mothurRemove(tempFile); return 0; }
						
						in >> distance;
						
						if (distance == -1) { distance = 1000000; }
						
						if(distance < cutoff){
							out << i << '\t' << j << '\t' << distance << endl;
						}
					}
					
				}
				else{
					if(nameMap->count(name)==0){        m->mothurOut("Error: Sequence '" + name + "' was not found in the names file, please correct"); m->mothurOutEndLine(); }
					
					for(int j=0;j<i;j++){
						
						if (m->control_pressed) { in.close(); out.close(); m->mothurRemove(tempFile); return 0; }
						
						in >> distance;
						
						if (distance == -1) { distance = 1000000; }
						
						if(distance < cutoff){
							out << i << '\t' << j << '\t' << distance << endl;
						}
					}
				}
			}
		}
		else{
			for(int i=1;i<nseqs;i++){
				in >> name;                
				rowToName[i] = name;
				matrixNames.push_back(name);
		
				if(nameMap == NULL){
					list->set(i, name);
					for(int j=0;j<nseqs;j++){
						if (m->control_pressed) { in.close(); out.close(); m->mothurRemove(tempFile); return 0; }
						
						in >> distance;
					
						if (distance == -1) { distance = 1000000; }
						
						if(distance < cutoff && j < i){
							out << i << '\t' << j << '\t' << distance << endl;
						}
					}
				}
				else{
					if(nameMap->count(name)==0){        m->mothurOut("Error: Sequence '" + name + "' was not found in the names file, please correct"); m->mothurOutEndLine(); }
					
					for(int j=0;j<nseqs;j++){
						if (m->control_pressed) { in.close(); out.close(); m->mothurRemove(tempFile); return 0; }
						
						in >> distance;
                        
						if (distance == -1) { distance = 1000000; }
						
						if(distance < cutoff && j < i){
							out << i << '\t' << j << '\t' << distance << endl;
						}
						
					}
				}
			}
		}
		
		list->setLabel("0");
		in.close();
		out.close();
	
		if(nameMap == NULL){
			nameMap = new NameAssignment();
			for(int i=0;i<matrixNames.size();i++){
				nameMap->push_back(matrixNames[i]);
			}
		}
		
	
		ifstream in2;
		ofstream out2;
		
		string outputFile = m->getRootName(distFile) + "column.dist";
		m->openInputFile(tempFile, in2);
		m->openOutputFile(outputFile, out2);
		
		int first, second;
		float dist;
		
		while (in2) {
			if (m->control_pressed) { in2.close(); out2.close(); m->mothurRemove(tempFile); m->mothurRemove(outputFile); return 0; }
			
			in2 >> first >> second >> dist;
			out2 << rowToName[first] << '\t' << rowToName[second] << '\t' << dist << endl;
			m->gobble(in2);
		}
		in2.close();
		out2.close();
		
		m->mothurRemove(tempFile);
		distFile = outputFile;
	
		if (m->control_pressed) {  m->mothurRemove(outputFile);  }

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "ReadCluster", "convertPhylip2Column");
		exit(1);
	}
}
/***********************************************************************/

int ReadCluster::convertPhylip2Column(CountTable*& ct){
	try {	
		//convert phylip file to column file
		map<int, string> rowToName;
		map<int, string>::iterator it;
		
		ifstream in;
		ofstream out;
		string tempFile = distFile + ".column.temp";
		
		m->openInputFile(distFile, in);  m->gobble(in);
		m->openOutputFile(tempFile, out);
		
		float distance;
		int square, nseqs;
		string name;
		vector<string> matrixNames;
		
		string numTest;
		in >> numTest >> name;
		
		if (!m->isContainingOnlyDigits(numTest)) { m->mothurOut("[ERROR]: expected a number and got " + numTest + ", quitting."); m->mothurOutEndLine(); exit(1); }
		else { convert(numTest, nseqs); }
		
		rowToName[0] = name;
		matrixNames.push_back(name);
		
		if(ct == NULL){
			list = new ListVector(nseqs);
			list->set(0, name);
		}
		else{  list = new ListVector(ct->getListVector()); }
        
		char d;
		while((d=in.get()) != EOF){
			
			if(isalnum(d)){
				square = 1;
				in.putback(d);
				for(int i=0;i<nseqs;i++){
					in >> distance;
				}
				break;
			}
			if(d == '\n'){
				square = 0;
				break;
			}
		}
        
		if(square == 0){
            
			for(int i=1;i<nseqs;i++){
				in >> name;
				rowToName[i] = name;
				matrixNames.push_back(name);
				
				//there's A LOT of repeated code throughout this method...
				if(ct == NULL){
					list->set(i, name);
					
					for(int j=0;j<i;j++){
                        
						if (m->control_pressed) { in.close(); out.close(); m->mothurRemove(tempFile); return 0; }
						
						in >> distance;
						
						if (distance == -1) { distance = 1000000; }
						
						if(distance < cutoff){
							out << i << '\t' << j << '\t' << distance << endl;
						}
					}
					
				}
				else{
					
					for(int j=0;j<i;j++){
						
						if (m->control_pressed) { in.close(); out.close(); m->mothurRemove(tempFile); return 0; }
						
						in >> distance;
						
						if (distance == -1) { distance = 1000000; }
						
						if(distance < cutoff){
							out << i << '\t' << j << '\t' << distance << endl;
						}
					}
				}
			}
		}
		else{
			for(int i=1;i<nseqs;i++){
				in >> name;                
				rowToName[i] = name;
				matrixNames.push_back(name);
                
				if(ct == NULL){
					list->set(i, name);
					for(int j=0;j<nseqs;j++){
						if (m->control_pressed) { in.close(); out.close(); m->mothurRemove(tempFile); return 0; }
						
						in >> distance;
                        
						if (distance == -1) { distance = 1000000; }
						
						if(distance < cutoff && j < i){
							out << i << '\t' << j << '\t' << distance << endl;
						}
					}
				}
				else{
					for(int j=0;j<nseqs;j++){
						if (m->control_pressed) { in.close(); out.close(); m->mothurRemove(tempFile); return 0; }
						
						in >> distance;
                        
						if (distance == -1) { distance = 1000000; }
						
						if(distance < cutoff && j < i){
							out << i << '\t' << j << '\t' << distance << endl;
						}
						
					}
				}
			}
		}
		
		list->setLabel("0");
		in.close();
		out.close();
        
		if(ct == NULL){
			ct = new CountTable();
			for(int i=0;i<matrixNames.size();i++){
				ct->push_back(matrixNames[i]);
			}
		}
		
        
		ifstream in2;
		ofstream out2;
		
		string outputFile = m->getRootName(distFile) + "column.dist";
		m->openInputFile(tempFile, in2);
		m->openOutputFile(outputFile, out2);
		
		int first, second;
		float dist;
		
		while (in2) {
			if (m->control_pressed) { in2.close(); out2.close(); m->mothurRemove(tempFile); m->mothurRemove(outputFile); return 0; }
			
			in2 >> first >> second >> dist;
			out2 << rowToName[first] << '\t' << rowToName[second] << '\t' << dist << endl;
			m->gobble(in2);
		}
		in2.close();
		out2.close();
		
		m->mothurRemove(tempFile);
		distFile = outputFile;
        
		if (m->control_pressed) {  m->mothurRemove(outputFile);  }
        
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "ReadCluster", "convertPhylip2Column");
		exit(1);
	}
}
/***********************************************************************/

ReadCluster::~ReadCluster(){}
/***********************************************************************/

