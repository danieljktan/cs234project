#include <stdio.h>
#include <string.h>

int main(int argc, const char **argv) {
  char buffer[500];
  if(argc==2) {
    FILE *file = fopen(argv[1], "r");
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
        /*int serial_number;
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
        fscanf(file, "%d", &length);        // length of helix*/

      } else if(strcmp(buffer, "SHEET")==0) {

      } else if(strcmp(buffer, "SSBOND")==0) {

      }
    }
    fclose(file);
    return 0;
  }

  return 1;
}

