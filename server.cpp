#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>
#include<iostream>
#include <sstream>
#include <sys/wait.h>
#include <vector>

using namespace std;
void fireman(int) {
    while (waitpid(-1, NULL, WNOHANG) > 0){
    //std::cout << "A child process ended" << std::endl;
      int x=1;
}

}
struct alphabet {
    char name;
    int frequency;
    int port;
    char hostName[100];
    int rank; // Consider adding this if you need a placeholder for rank
};
char* stoa(string input){
    int n = input.length();
    char char_array[n+1];
    strcpy(char_array,input.c_str());
    return char_array;
}
void error(char *msg) {
    perror(msg);
    exit(1);
}

string fib(int rank) {
    vector<int> fib_stack;
    
    fib_stack.push_back(0);
    fib_stack.push_back(1);
    
    while (fib_stack.back() <= rank) {
        if ((fib_stack[fib_stack.size()-1] + fib_stack[fib_stack.size()-2]) > rank) {
            break;
        }
        fib_stack.push_back(fib_stack[fib_stack.size()-1] + fib_stack[fib_stack.size()-2]); //It's not just fib_stack.size() - 1, because we need the actual value not the size of the stack
    }
    
    vector<int> fib_numbers_to_add;
    int i = (fib_stack.size() - 1);
    
    while (i >= 0 && rank > 0) {
        if (fib_stack[i] <= rank) {
            rank -= fib_stack[i];
            fib_numbers_to_add.push_back(fib_stack[i]);
        } else {
            i--;
        }
    }
    
    vector<int> fib_index;
    
    //Find the indexes of the numbers to add
    for(int i = 0; i < fib_numbers_to_add.size(); i++) {
        for (int j = 2; j < fib_stack.size(); j++) {
            if(fib_numbers_to_add[i] == fib_stack[j]) {
                fib_index.push_back(j);
            }
        }
    }
    
    //Replace those indexes with one
    for (int j = 0; j < fib_index.size(); j++) {
        for(int i = 0; i < fib_stack.size(); i++) {
            if(i == fib_index[j]) {
                fib_stack[fib_index[j]] = 1;
            }
        }
    }
    
    //Replace the rest with zero
    for (int i = 0; i < fib_stack.size(); i++) {
        if(fib_stack[i] != 1) {
            fib_stack[i] = 0;
        }
    }
    
    //Case were 1 at the beginning is not a combination for the larger number
    //Find the indexes of the numbers to add
    
    for (int j = 2; j < 3; j++) {
        for(int i = 0; i < fib_numbers_to_add.size(); i++) {
            if (fib_numbers_to_add[i] == 1) { // If any of the numbers is equal to 1
                fib_stack[j] = 1;
                break;
            } else {
                fib_stack[j] = 0;
            }
        }
    }
    
    string temp = "";
    
    //starting from after 0 and 1
    for (int k = 2 ; k < fib_stack.size(); k++) {
        temp += to_string (fib_stack[k]);
    }
    
    //Pass the delimeter [1] at the end
    temp += to_string (1);
    
    //return the fib string as an int
    return temp;
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error(stoa("ERROR opening socket"));
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error(stoa("ERROR on binding"));
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    
    signal(SIGCHLD, fireman);
    while(1){ //Perhaps
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        if (newsockfd < 0) error(stoa("ERROR on accept"));
        if(fork()==0){
            //get a buffer, create a structure and populate it, then send it back
            bzero(buffer,256);
            n = read(newsockfd,buffer,255);
            if (n < 0) error(stoa("ERROR reading from socket"));
            
            //Calculations
            
            string answer = fib(atoi(buffer));
            
            strcpy(buffer, answer.c_str());
            
            n = write(newsockfd,buffer,strlen(buffer));
            
            if (n < 0)
            error(stoa("ERROR writing to socket"));
            _exit(0);
            close(newsockfd);
        }
        close(newsockfd);
    }
    _exit(0);
    close(sockfd);
    
    return 0;
}