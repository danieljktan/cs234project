#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <bits/stdc++.h>
#include <vector>

void parse_file(const char *file_name) {
   std::string buffer;
   std::ifstream file(file_name); 

   std::vector<int> helix_begin;
   std::vector<int> helix_end;

   std::vector<int> sheet_begin;
   std::vector<int> sheet_end;


   if(!file.is_open()) return;
   while(std::getline(file, buffer)) {
     std::stringstream ss(buffer);
     std::string sub_string;
     ss >> sub_string;
     
     if(sub_string == "ATOM") {
       int resSeq = std::stoi(buffer.substr(22, 4));
       float x = std::stof(buffer.substr(30, 8));
       float y = std::stof(buffer.substr(38, 8));
       float z = std::stof(buffer.substr(46, 8));
       //printf("%d -> %f, %f, %f\n", resSeq, x, y, z);

       // Serial  // Integer
       // Atom    // Name
       // altLoc  // Character
       // resName // Residue Name
       // chainID // Character
       // resSeq  // Integer
       // iCode   // AChar
       // x       // float
       // y       // float
       // z       // float
       // occupancy // float
       // tempFactor // float
       // element    // LString(2)
       // charge     // LString(2)
       unsigned int helix_size = helix_begin.size();
       unsigned int sheet_size = sheet_begin.size();
       for(unsigned int i=0; i<helix_size; i++) {
         if(helix_begin[i] <= resSeq && resSeq <= helix_end[i]) {
           //add to the alpha helix.
           printf("ALPHA %d -> %f, %f, %f\n", resSeq, x, y, z);
           goto end_for_loop;
         }
       }

       for(unsigned int i=0; i<sheet_size; i++) {
         if(sheet_begin[i] <= resSeq && resSeq <= sheet_end[i]) {
           //add to the beta sheet.
           printf("BETA %d -> %f, %f, %f\n", resSeq, x, y, z);
           goto end_for_loop;
         }
       }

       // not either alpha or beta
       printf("DEFAULT %d -> %f, %f, %f\n", resSeq, x, y, z);


       end_for_loop:
       asm("nop");  // used to trick the compiler into accepting a label to nowhere.

     } else if(sub_string == "HELIX") {
       //std::cout << buffer.length() << std::endl;
       //int serNum;
       //std::string helixID;// LString(3) literal string with exactly 3 characters
       // initResName    // Residue Name
       // initChainID    // Character any non-control character or a space
       // initSeqNum     // Integer
       // initICode      // AChar  must be [A-Za-z]
       // endResName     // Residue Name
       // endChainID     // Character any non-control character or a space
       // endSeqNum      // Integer
       // endICode       // AChar
       // helixClass     // Integer
       // comment        // String
       // Length         // Integer

       int helixClass = std::stoi(buffer.substr(38,2));
       if(helixClass != 1) continue;
       int initSeqNum = std::stoi(buffer.substr(21,4));
       int endSeqNum  = std::stoi(buffer.substr(33,4));
       // helixClass == 1 is alpha
       //printf("HELIX %d->%d, %d\n", initSeqNum, endSeqNum, helixClass);
       helix_begin.push_back(initSeqNum);
       helix_end.push_back(endSeqNum);




     } else if(sub_string == "SHEET") {
       // strand   integer
       // sheetID  lstring(3)
       // numStrands integer
       // initResName residue name
       // initChainID character
       // initSeqNum  integer  23-26
       // initICode   achar
       // endResName
       // endChainID
       // endSeqNum            34-37
       // endICode achar
       // sense  integer
       // curAtom atom
       // curResName residue_name 
       // curChainID character
       // curResSeq Integer
       // curICode AChar
       // prevAtom atom
       // prevResName residue_name
       // prevChainID character
       // prevResSeq Integer
       // prevICode AChar
       std::string sheetID = buffer.substr(11,3);  // use the B id
       if(sheetID != "  B") continue;


       int initSeqNum = std::stoi(buffer.substr(23,4));
       int endSeqNum  = std::stoi(buffer.substr(34,4));
       
       sheet_begin.push_back(initSeqNum);
       sheet_end.push_back(endSeqNum);

       //printf("SHEETID: %s, %d, %d\n", sheetID.c_str(), initSeqNum, endSeqNum);
     } else if(sub_string == "SSBOND") {
       int seqNum1 = std::stoi(buffer.substr(17,4));
       int seqNum2 = std::stoi(buffer.substr(31,4));
       //printf("SSBOND: %d %d\n", seqNum1, seqNum2);
     } else if(sub_string == "LINK") {
       int resSeq1 = std::stoi(buffer.substr(22,4));
       int resSeq2 = std::stoi(buffer.substr(52,4));
       //printf("LINK: %d %d\n", resSeq1, resSeq2);
     } else if(sub_string == "CISPEP") {
       int seqNum1 = std::stoi(buffer.substr(17,4));
       int seqNum2 = std::stoi(buffer.substr(31,4));
       //printf("CISPEP: %d %d\n", seqNum1, seqNum2);
     } else if(sub_string == "SEQRES") {
       // consecutive...???
     }


     ss.clear();

   }

}




int main(int argc, const char **argv) {
  char buffer[500];
  if(argc==2) {
    
    parse_file(argv[1]);
    /*FILE *file = fopen(argv[1], "r");
    if(!file) {
      printf("Error. Invalid file\n");
      return 1;
    }
    while(!feof(file)) {
      fscanf(file, "%s", buffer);
      if(strcmp(buffer, "HEADER")==0) {

      } else if(strcmp(buffer, "ATOM")==0) {
        int number;
        float x; 
        float y; 
        float z; 
        float a;
        float aa;
        fscanf(file, "%d", &number);
        fscanf(file, "%s", buffer);
        fscanf(file, "%s", buffer);
        fscanf(file, "%s", buffer);
        fscanf(file, "%d", &number);
        fscanf(file, "%f %f %f", &x, &y, &z);
        fscanf(file, "%f", &a);
        fscanf(file, "%f", &aa);
        fscanf(file, "%s", buffer);
        printf("ATOM: x=%5f, y=%5f, z=%5f\n", x, y, z);
      } else if(strcmp(buffer, "HETATM")==0) {
        int number;
        float x; 
        float y; 
        float z; 
        float a;
        float aa;
        fscanf(file, "%d", &number);
        fscanf(file, "%s", buffer);
        fscanf(file, "%s", buffer);
        fscanf(file, "%s", buffer);
        fscanf(file, "%d", &number);
        fscanf(file, "%f %f %f", &x, &y, &z);
        fscanf(file, "%f", &a);
        fscanf(file, "%f", &aa);
        fscanf(file, "%s", buffer);
        printf("HETATM: x=%5f, y=%5f, z=%5f\n", x, y, z);

      } else if(strcmp(buffer, "TER")==0) {
        int serial_number;
        int residue_seq;

        fscanf(file, "%d", &serial_number); // serial number
        fscanf(file, "%s", buffer);         // residue name
        fscanf(file, "%s", buffer);         // chain identifier
        fscanf(file, "%d", &residue_seq);   // residue sequence number
        //fscanf(file, "%s", buffer);         // code for insertions of residues

      } else if(strcmp(buffer, "HELIX")==0) {
        int serial_number;
        int residue_seq;
        int residue_seq_n;
        int type_helix;
        int length;

        fscanf(file, "%d", &serial_number); // serial number
        fscanf(file, "%s", buffer);         // helix identifier
        fscanf(file, "%s", buffer);         // initial residue name
        fscanf(file, "%s", buffer);         // chain identifier
        fscanf(file, "%d", &residue_seq);   // residue sequence number
        fscanf(file, "%s", buffer);         // code for insertions of residues
        fscanf(file, "%s", buffer);         // terminal residue name
        fscanf(file, "%s", buffer);         // chain identifier
        fscanf(file, "%d", &residue_seq_n); // residue sequence number
        fscanf(file, "%s", buffer);         // code for insertions of residues
        fscanf(file, "%d", &type_helix);    // type of helix
        fscanf(file, "%s", buffer);         // comment
        fscanf(file, "%d", &length);        // length of helix

      } else if(strcmp(buffer, "SHEET")==0) {

      } else if(strcmp(buffer, "SSBOND")==0) {

      }
    }
    fclose(file);*/
    return 0;
  }

  return 1;
}

