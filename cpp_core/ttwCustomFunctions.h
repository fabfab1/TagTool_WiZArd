#pragma once
#ifndef TTWCUSTOMFUNCTIONS_H
#define TTWCUSTOMFUNCTIONS_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream> 
#include <vector>
#include <regex>

#include "ttwClasses.h"
#include "ttwDeclarations.h"
#include "ttwFunctions.h"


/*
For reasons of clearity this file collects functions 
that don´t belong to the core resp. are needed for special purposes,
e. g. in context of a single project or for special feautues
*/

void apply_citation_style_hyphens(vector<string> &articleFile, struct documentSectionsClass &documentSections){
	
	string citationSearch, citationReplace;
	string toReplace;
	
	//find citations like 1993, 3-46 oder 2006b, 300-201... (only in the footnote section)
	
	for(size_t i=documentSections.lineNrFootnotesBegin_; i<articleFile.size(); i++){
		// TODO: FAB (from CoPilot) The regular expression string currently has an unknown escape sequence warning. 
		// This is because in C++, backslashes in strings are used to introduce escape sequences. 
		// If we want to include an actual backslash in the string, we need to escape it by using two backslashes.
		// In this case, we're trying to use a backslash to escape a special character in a regular expression. 
		// However, because this is a string literal, the C++ compiler is trying to interpret the backslash as starting an escape sequence.
		// To fix this, we need to use four backslashes to represent a single backslash in the regular expression. 
		// So the corrected line of code should be:
		//            std::regex r1{"[0-9]{4}[a-z]?,\\\\s[0-9]{1,4}[Γ\\-]{1,3}[0-9]{1,4}"};
		// instead of std::regex r1{"[0-9]{4}[a-z]?,\\s[0-9]{1,4}[Γ\\-]{1,3}[0-9]{1,4}"};
	std::regex r1{"[0-9]{4}[a-z]?,\\s[0-9]{1,4}[–|\-]{1,3}[0-9]{1,4}"};
	std::smatch m1;
	std::sregex_iterator begin{ articleFile[i].begin(), articleFile[i].end(), r1};
    std::sregex_iterator end;
    
    for (std::sregex_iterator it = begin; it != end; ++it) {
        m1=*it;
        citationSearch=m1.str();
        
		//now prepare a replace string with the correct hyphen 
		//and set a green marking to highlight the alteration for the editing process
        std::regex r2{"[^.\\w^(^)\\s]{2,4}"};  
		std::stringstream result;
		
		if(htmlSelected==true){
			toReplace = "<span style=\"background-color:green;\">–</span>";
		}
		else{
			toReplace = "–";
		}
		
		result << std::regex_replace(citationSearch, r2, toReplace);
	
		citationReplace = result.str();
		
		search_replace(articleFile[i], citationSearch, citationReplace);	
           
        citationSearch.clear();
        citationReplace.clear();
        }
	}	
}


void xml_preparation_tagebuecher (vector<string> &articleFile, struct documentSectionsClass& documentSections){
	
	int pos1;
	int pos2;
	string toInsert;
	string pageNumberStr;
	
	
	//First detect page numbers to insert figure reference tags	
	for(int i=0; i<documentSections.lineNrBodyEnd_; i++){
		
		pos1=articleFile[i].find("[S.]");
		
		if(pos1==3){ //3 to be sure to get only the usual page numbers at the beginning ("<p>")
			std::regex pattern{"[0-9]{1,3}"};
			std::sregex_iterator begin{ articleFile[i].cbegin(), articleFile[i].cend(), pattern};
			std::sregex_iterator end;

			for (std::sregex_iterator i = begin; i != end; ++i) {
				std::smatch match = *i;
				pageNumberStr = match.str();
			}
			
			pos2=articleFile[i].find("</p>");
			
			if(pos2>=0){
				
			articleFile[i].insert(pos2, figureReferencesClass::figReferenceTagEndXML_); 	
			
			}
				
 	       	std::regex r1("#");
 	       	toInsert = std::regex_replace(figureReferencesClass::figReferenceTagBeginXML_, r1, pageNumberStr);
			articleFile[i].insert(pos1, toInsert);
		}
	}
	
	//Now detect notes at the end of the pages and put them into boxed text
	
	bool boxedTextTagOpen;
	int lineNrLastOpenTag;
	
	for(int i=0; i<documentSections.lineNrBodyEnd_; i++){
	
		pos1=articleFile[i].find("[Notizen außerhalb der Textkolumne]");
		
		if(pos1>=0){
			articleFile[i].insert(0, "<boxed-text>\n");
			boxedTextTagOpen=true;
			lineNrLastOpenTag=i;
		}
			
		for(int y=i; y<documentSections.lineNrBodyEnd_; y++){
			pos2=articleFile[i].find("<p><xref ref-type=\"fig\"");
			if(pos2>=0 && boxedTextTagOpen==true){
				articleFile[i].insert(0, "</boxed-text>\n");
				boxedTextTagOpen=false;
				y=documentSections.lineNrBodyEnd_;
			}
		}
	}
	
	if(boxedTextTagOpen==true){
		for(int i=lineNrLastOpenTag; i<documentSections.lineNrBodyEnd_; i++){
			
			pos2=articleFile[i].find("</sec>");
			
			if(pos2>=0 && boxedTextTagOpen==true){
				articleFile[i].insert(0, "</boxed-text>\n");
				boxedTextTagOpen=false;
				break;
			}
		}
	}
}

#endif // TTWCUSTOMFUNCTIONS_H