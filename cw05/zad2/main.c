#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 256

void print_output(FILE* file){
    char line[MAX_SIZE];
    while(fgets(line, MAX_SIZE, file) != NULL){
        printf("%s\n", line);
    }
}

void print_ordered_by(char* mode){
    char* command;

    if (strcmp(mode,"date") == 0) {
        command = "echo | mail -f | tail +2 | head -n -1 | tac";
    }
    else if (strcmp(mode, "sender") == 0){
        command = "echo | mail -f | tail +2 | head -n -1 | sort -k 2";
    }
    else{
        printf("Wrong mode!\n");
        exit(1);
    }

    FILE* file = popen(command, "r");

    if (file == NULL){
        printf("popen() error!");
        exit(1);
    }

    printf("\nSORTED EMAILS: ");
    print_output(file);
}

void send_email(char* address, char* object, char* content){
    char command[MAX_SIZE];
    snprintf(command, sizeof(command), "echo %s | mail -s %s %s", content, object, address);
    printf("%s\n", command);
    FILE* file = popen(command, "r");

    if (file == NULL){
        printf("popen() error!");
        exit(1);
    }

    printf("Email sent to: %s\nObject: %s\nText: %s\n", address, object, content);
}

int main(int argc, char* argv[]){

    if (argc != 2 && argc != 4){
        printf("Zla liczba argumentow!\n");
        exit(1);
    }
    else if (argc == 2){
        print_ordered_by(argv[1]);
    }
    else {
        char* address=argv[1], subject=argv[2], content=argv[3];

        send_email(address, subject, content);
    }

}