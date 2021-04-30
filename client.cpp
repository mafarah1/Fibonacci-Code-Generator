#include <pthread.h>
#include <iomanip>
#include <stack>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <unordered_map>

using namespace std;

//Functions earlier than the main function need this.

struct alphabet {
    char name;
    int frequency;
    int rank;
    int port;
    char hostName[100];
    string fibcode;
};

bool orderFrequencies(alphabet a, alphabet b) {
  if (a.frequency == b.frequency) {
    return ((int)a.name > (int)b.name);
  }
  return (a.frequency > b.frequency);
}

char* stoa(string input){
    int n = input.length();
    char char_array[n+1];
    strcpy(char_array,input.c_str());
    return char_array;
}

void error(char *msg) {
 perror(msg);
 exit(0);
}

string decodeFile(string file_to_read, unordered_map<string, char> fibs){
  string code = ""; //one at a time
  string answer = "";
  cout << "Decompressed message = ";
  for(int i = 0; i < file_to_read.length(); i++) {
    if(file_to_read[i] == '1' && file_to_read[i+1] == '1') {
      answer += fibs[code + "11"];
      code = "";
      i++;
    } else {
      code += file_to_read[i];
    }
  }
  return answer;
}

void *fibPush(void *pos_void_ptr) {
  struct alphabet *pos_ptr = (struct alphabet *)pos_void_ptr;

  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char buffer[256]; //If I were passing a string. Don't know what it does, but Prof. Rincon used it so I think it should be important.
  portno = pos_ptr->port;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error(stoa("ERROR opening socket"));
  server = gethostbyname(pos_ptr->hostName);
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
  (char *)&serv_addr.sin_addr.s_addr,
  server->h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    error(stoa("ERROR connecting"));

  //Start fib

  //Send Rank
  string temp = to_string(pos_ptr->rank);
  strcpy(buffer, temp.c_str());
  n = write(sockfd,buffer,strlen(buffer)); //strlen is trying to find how big the buffer is
  //^From here, the server will produce the fib and send it back

  if (n < 0) error(stoa("ERROR writing to socket"));
  bzero(buffer,256);
  
  n = read(sockfd,buffer,255);

  string temp_final(buffer);

  pos_ptr->fibcode = temp_final;

  if (n < 0) error(stoa("ERROR reading from socket"));
  
  return NULL;
}
int main(int argc, char *argv[]) {

  int total = 0;
  cin >> total;
  cin.ignore(); //straight into the next input

  alphabet fib[total];
  for(int i = 0; i<total; i++){
    string temp_string;
    getline(cin, temp_string);
    fib[i].name = temp_string[0];
    fib[i].frequency = (int)temp_string[2] - '0';
//    cout << fib[i].name << " " << fib[i].frequency << endl;
    fib[i].port = atoi(argv[2]); //this is the portnumber
    strcpy(fib[i].hostName, argv[1]);
  }
  
  //sort from largest to smallest
  sort(fib, fib+total, orderFrequencies);

  //populate the ranks
  for(int i = 0; i<total; i++){
    fib[i].rank = i+1;
  }
  /*
  for(int i = 0; i<total; i++) cout<<fib[i].name<<fib[i].rank<<fib[i].frequency<<endl;
  */

  pthread_t tid[total];

  for(int i=0;i<total;i++) {
    if(pthread_create(&tid[i], NULL, fibPush, &fib[i])) {
      fprintf(stderr, "Error creating thread\n");
      return 1;
    }
  }

  for (int i = 0; i < total; i++) {
    pthread_join(tid[i], NULL);
  }

  ifstream files;
  string file_to_read;
  cin >> file_to_read;
  files.open(file_to_read);
  files >> file_to_read;

  unordered_map<string, char> fibs;

  for (int i = 0; i < total; i++) {
    fibs[fib[i].fibcode] = fib[i].name;
  }

  cout << decodeFile(file_to_read, fibs);
  
  fibs["011"];

/*
  for(int i = 0; i<total; i++){
      cout<<"code: " << fib[i].fibcode << ", ";
      cout<<endl;
  }
*/

  return 0;
}