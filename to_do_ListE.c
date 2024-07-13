#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <openssl/sha.h>

//Change to desired location
#define HASHFILE "hash"
#define FILENAME "list.txt"
#define TFILENAME "list.txt"
#define ENFILENAME "list.enc"

int loginR;
char password[200];
FILE *file;
void banner(char *string);
void printES(char *prompt, int status);
int checkUserExists();
void createUserPassword();
void encryptOrDecrypt(int eord);
void hashGenerator(char *password,char hash_str[2*SHA256_DIGEST_LENGTH+1]);
void arrayCleaner(char *string, int length);
int login();
void menu();
int menuChoice();
int printList();
void addToList();
void deleteFromList();
void updateItem();
void cleanUp();


int main(void)
{
	
	signal(SIGINT, cleanUp);	
    int choice = 0;
    bool mainLoop = true;
	//Check for hash file	
	if (checkUserExists()==1) {
		createUserPassword();
		cleanUp();
	}
	
	//Login check
	loginR = login();
	if (loginR != 0) {
		cleanUp();
	}else {
		banner("Logged In");
		encryptOrDecrypt(0);
	}

    if ((file = fopen(FILENAME,"r"))== NULL) {
        printf("[-] Creating File\n ");

        if ((file = fopen(FILENAME,"w"))== NULL) {
            perror("Error creating a file");
        }
    }
    fclose(file);
    while (mainLoop) {
        choice = menuChoice();
        switch (choice) {
            case 1:
                addToList();
                break;
            case 2:
                updateItem();
                break;
            case 3:
                deleteFromList();
                break;
            case 4:
                cleanUp();
                break;
            default:
                printf("Please Choose a valid option\n");
        }
    }
    return 0;
}

int checkUserExists(){
	FILE *fd;

	fd = fopen(HASHFILE, "r");
	if (fd == NULL) {
		return 1;	
	}else {
		return 0;
	}

}

void createUserPassword(){
	
	FILE *hashFile;
	char confirmPassword[200];
	char hash_str[2*SHA256_DIGEST_LENGTH+1];
	remove(HASHFILE);
	banner("New Password");
	printf("New Password> ");
	scanf("%200s", password);

	while (strcmp(password, confirmPassword)!=0) {
		
		printf("Confirm password> ");
		scanf("%200s", confirmPassword);
	}	
	hashGenerator(password, hash_str);

	hashFile = fopen(HASHFILE, "w");
	if (hashFile == NULL) {
		printf("[-] Failed to open hash file\n");
	}

	fprintf(hashFile, "%s", hash_str);
	fclose(hashFile);
	banner("New Password Set");

}


void hashGenerator(char *password,char hash_str[2*SHA256_DIGEST_LENGTH+1] ){
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256((unsigned char*)password, strlen(password),hash);

	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		sprintf(&hash_str[i*2], "%02x", hash[i]);
	}
	hash_str[2*SHA256_DIGEST_LENGTH] = '\0';
}


int login(){
	
	int count = 3;
	char buffer[2*SHA256_DIGEST_LENGTH+1];
	char hash_str[2*SHA256_DIGEST_LENGTH+1];

	banner("Login");

	FILE *fd = fopen(HASHFILE, "r");
	if (fd == NULL) {
		printES("Failed to open hash", 1);
		return 1;
	}

	fgets(buffer, sizeof(buffer), fd);


	for (int i = 0; i  < count ; i++) {
		printf("Password > ");
		scanf("%200s", password);
		hashGenerator(password, hash_str);
		if (strcmp(hash_str,buffer)==0) {
			fclose(fd);
			return 0;	
		}else{
			continue;
		}	
			
	}
	fclose(fd);
	return 1;

}

void encryptOrDecrypt(int eord){
	//1 = en, 0 = de
	char command[1000];

	if (eord == 1) {
		sprintf(command, "openssl enc -e -aes-256-cbc -in %s -out %s -k %s", FILENAME, ENFILENAME, password);
		system(command);
		printf("Encrypted\n");
	}else {
		sprintf(command, "openssl enc -d -aes-256-cbc -in %s -out %s -k %s", ENFILENAME, FILENAME, password);
		system(command);
		printf("Decrypted\n");

	}

}

void banner(char *string){
	printf("*****************************\n");
	printf("\t%s\t\t\n", string);
	printf("*****************************\n");
}

void printES(char *prompt, int status){
	
	if (status == 1) {
		printf("[-] %s\n", prompt);
	}else {
		printf("[+] %s\n", prompt);
	}

}

void menu(){
    printf("\n------Menu-------\n");
    printf("1. Add To List\n");
    printf("2. Update Item\n");
    printf("3. Delete From List\n");
    printf("4. Exit\n");
}

int menuChoice(){
    int choice = 0;
    bool loop = true;    
    char charChoice;

    while (loop) {
        printList();
        menu();
        printf("> ");
        scanf("%c", &charChoice);
        printf("---------------------\n");
        choice = atoi(&charChoice); 
        
        if (choice == 0) {
            system("cls");
            continue;
        }else if ((choice >= 1) && (choice <= 5)) {
            loop = false;
            system("cls");
        }else {
            system("cls");
            continue;
        }
    }
    return  choice;
}


int printList(){
    if ((file = fopen(FILENAME,"r"))== NULL) {
        printf("[-] Error opening file\n");
    }
    char buffer[100];
    int line = 0;
    printf("To Do list\n");
    printf("-----------\n");
    while ((fgets(buffer, sizeof(buffer),file ))!=NULL) {
        if ((buffer[0] != '\0'||buffer[1]!='\0') || (buffer[0] != '\n')) {
            printf("%d : %s", line+1, buffer);
            line++;
        }
    }
    rewind(file);
    fclose(file);
    return line;
}


void addToList(){
    if ((file = fopen(FILENAME,"a"))== NULL) {
        printf("Add Item To list\n");
    }
    printf("Add\n");
    char item[3000];
    printf("> ");
    while ((getchar()) != '\n');
    /* scanf("%[\n]s", item); */
    fgets(item, sizeof(item), stdin); // Read a line of text
   size_t len = strlen(item);
    if (len > 0 && item[len - 1] == '\n') {
        item[len - 1] = '\0';
    }
    fprintf(file,"%s\n",item);
    rewind(file);
    fclose(file);
    system("cls");
    /* printList(); */
}


void updateItem(){
    int updateChoice,  numberOfItems = 0;
    bool loop = true;    
	char buffer[500];
    char command[400];
    char finalbuffer[1000];
    printf("Update Item\n");
    printf("-----------\n");
    numberOfItems = printList();

    while (loop) {
        printf("Update Item> ");
        scanf("%d",&updateChoice);
        printf("---------------------\n");
        if ((updateChoice>= 1) && (updateChoice<= numberOfItems)) {
            loop = false;
        }else {
            printES("Choose an Item From The List", 1);
            continue;
        }
    }
    /* system("cls"); */ 
    printf("Update Item %d\n", updateChoice);
    
    printf("Update> ");
    
    while ((getchar()) != '\n');
    
	fgets(finalbuffer, sizeof(finalbuffer),stdin);
   
	int end = strlen(finalbuffer);
    
	if (end > 0 && finalbuffer[end - 1] == '\n') {
        finalbuffer[end - 1] = '\0';
    }
   
	FILE *tmpf;
	
	tmpf = fopen(TFILENAME, "w");
	if (tmpf == NULL) {
		printf("[-] Failed to open tmp file\n");	
	}
	
	file = fopen(FILENAME, "r");
	
	if (file == NULL) {
		printf("[-] Failed to open file\n");	
	}

	int i = 0;
	while (fgets(buffer,200, file) != NULL) {
		if ((updateChoice-1)== i) {
			fprintf(tmpf,"%s\n",finalbuffer);	
			i++;
		}else {
			fprintf(tmpf,"%s",buffer);	
			i++;
		}

	}

	fclose(tmpf);
	fclose(file);
	
	/* system("cp tmpList.txt toDoList.txt"); */

	tmpf = fopen(TFILENAME, "r");
	if (tmpf == NULL) {
		printf("[-] Failed to open tmp file\n");	
	}
	
	file = fopen(FILENAME, "w");
	
	if (file == NULL) {
		printf("[-] Failed to open file\n");	
	}

	while (fgets(buffer,200, tmpf) != NULL) {
			fprintf(file,"%s",buffer);	
	}



	fclose(tmpf);
	fclose(file);
	

	if (remove(TFILENAME)!=0) {
		printf("[-] Failed to remove tmp\n");
	}
    system("cls"); 
}


void arrayCleaner(char *string, int length){
	for (int i = 0; i < length; i++) {
		string[i] = '\0';
	}
}

void deleteFromList(){
    int deleteChoice,  numberOfItems = 0;
    bool loop = true;    
    char buffer[500];
	char command[400];
    char delCommand[400];
    char cleanCommand[400];
    printf("Delete Item\n");
    printf("-----------\n");
    numberOfItems = printList();

    while (loop) {
        printf("Delete Item> ");
        scanf("%d",&deleteChoice);
        printf("---------------------\n");
        if ((deleteChoice >= 1) && (deleteChoice<= numberOfItems)) {
            loop = false;
        }else {
            printES("Choose an Item From The List", 1);
            continue;
        }
    }
    /* system("cls"); */ 
	FILE *tmpf;
	
	tmpf = fopen(TFILENAME, "w");
	if (tmpf == NULL) {
		printf("[-] Failed to open tmp file\n");	
	}
	
	file = fopen(FILENAME, "r");
	
	if (file == NULL) {
		printf("[-] Failed to open file\n");	
	}

	int i = 0;
	while (fgets(buffer,200, file) != NULL) {
		if ((deleteChoice-1)== i) {
			i++;
		}else {
			fprintf(tmpf,"%s",buffer);	
			i++;
		}

	}

	fclose(tmpf);
	fclose(file);
	
	/* system("cp tmpList.txt toDoList.txt"); */

	tmpf = fopen(TFILENAME, "r");
	if (tmpf == NULL) {
		printf("[-] Failed to open tmp file\n");	
	}
	
	file = fopen(FILENAME, "w");
	
	if (file == NULL) {
		printf("[-] Failed to open file\n");	
	}

	while (fgets(buffer,200, tmpf) != NULL) {
			fprintf(file,"%s",buffer);	
	}



	fclose(tmpf);
	fclose(file);
	
    printf("Item %d Deleted\n", deleteChoice);
	if (remove(TFILENAME)!=0) {
		printf("[-] Failed to remove tmp\n");
	}
   
}


void cleanUp(){
	encryptOrDecrypt(1);
	system("cls");
	remove(FILENAME);
	arrayCleaner(password, 200); //Clean password array
	banner("Exiting");
	exit(0);
}
