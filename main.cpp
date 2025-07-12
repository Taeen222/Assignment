#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Encryption key for XOR Cipher

#define ENCRYPTION_KEY 0x4F

//structure for account

typedef struct {
	int accountNumber;
	char name[100];
} Account;

//File utilities

void getAccountInfoFilename(int accNo, char* filename) {
	sprintf(filename, "account_%d.txt", accNo);
}

//filename getting for encrypted balance
void getBalanceFilename(int accNo, char* filename) {

	sprintf(filename, "balance_%d.txt", accNo);

}

//filename getting for account password

void getPasswordFilename(int accNo, char* filename) {

	sprintf(filename, "password_%d.txt", accNo);

}

//store all account number

const char* account_list = "accounts.txt";

//Encryption or Decryption function

void encDec(char* data, char key) {
	for (int i = 0; data[i] != '\0'; ++i) {
		data[i] ^= key;
	}
}

//user password input

void promptPassword(char* password, int size) {

	printf("Enter your account password: ");

	fgets(password, size, stdin);

	password[strcspn(password, "\n")] = '\0';

}

//save password

void savePassword(int accNo, const char* password) {
	char filename[64];

	getPasswordFilename(accNo, filename);

	char passbuf[100];

	strcpy(passbuf, password);
	encDec(passbuf, ENCRYPTION_KEY);

	FILE* f = fopen(filename, "w");

	if (f) {

		fputs(passbuf, f);
		fclose(f);
	}

}


int checkPassword(int accNo, const char* password) {

	char filename[64];

	getPasswordFilename(accNo, filename);

	FILE* f = fopen(filename, "r");

	if (!f)
		return 0;

	char buf[100];

	if (!fgets(buf, sizeof(buf), f)) {
		fclose(f);
		return 0;
	}
	fclose(f);

	encDec(buf, ENCRYPTION_KEY);  // Decrypt the stored password
	return strcmp(buf, password) == 0;
}

int doesAccountExist(int accNo) {

	char filename[64];

	getAccountInfoFilename(accNo, filename);

	FILE* file = fopen(filename, "r");

	if (file) {
		fclose(file);
		return 1;

	}

	return 0;
}

// Function to add an account number
void addAcc(int accNo) {

	FILE* db = fopen(account_list, "a");

	if (db) {
		fprintf(db, "%d\n", accNo);
		fclose(db);
	}
}

// Function to remove an account number
void RemoveAcc(int accNo) {

	FILE* db = fopen(account_list, "r");

	FILE* tmp = fopen("temp_accounts.txt", "w");
	int n;

	if (db && tmp) {
		while (fscanf(db, "%d", &n) == 1) {
			if (n != accNo) fprintf(tmp, "%d\n", n);  
		}

		fclose(db);

		fclose(tmp);

		remove(account_list);

		rename("temp_accounts.txt", account_list);
	}
}

void BalanceSaveEnc(int accNo, float balance) {

	char filename[64];

	getBalanceFilename(accNo, filename);

	char balanceStr[32];

	sprintf(balanceStr, "%.2f", balance);

	encDec(balanceStr, ENCRYPTION_KEY);

	FILE* file = fopen(filename, "w");

	if (file) {
		fputs(balanceStr, file);

		fclose(file);
	}
}

// Function to load the balance
int balanceLoadEnc(int accNo, float* balance) {

	char filename[64];

	getBalanceFilename(accNo, filename);

	char buffer[32];

	FILE* file = fopen(filename, "r");

	if (!file) return 0;

	if (!fgets(buffer, sizeof(buffer), file)) {
		fclose(file);
		return 0;

	}

	fclose(file);

	encDec(buffer, ENCRYPTION_KEY);

	*balance = atof(buffer);

	return 1;
}



void createAccount() {

	Account acc;

	float balance;

	char password[100];


	printf("\n Create a New Account \n");

	printf("Enter account number: ");

	scanf("%d", &acc.accountNumber);

	getchar();

	printf("Enter account holder's name: ");

	fgets(acc.name, sizeof(acc.name), stdin);

	acc.name[strcspn(acc.name, "\n")] = '\0';

	printf("Set a password : ");

	fgets(password, sizeof(password), stdin);

	password[strcspn(password, "\n")] = '\0';

	if (doesAccountExist(acc.accountNumber)) {

		printf("Sorry, an account already exists with that number\n");

		return;
	}

	printf("Enter the initial deposit amount: ");

	scanf("%f", &balance);


	char infoFile[64];

	getAccountInfoFilename(acc.accountNumber, infoFile);

	FILE* file = fopen(infoFile, "w");

	if (!file) {
		printf("Error: Could not create the account file!\n");
		return;
	}

	fprintf(file, "%d\n%s\n", acc.accountNumber, acc.name);

	fclose(file);


	savePassword(acc.accountNumber, password);


	BalanceSaveEnc(acc.accountNumber, balance);

	addAcc(acc.accountNumber);

	printf("Congratulations! Account created for %s with balance %.2f\n", acc.name, balance);

}

int readAccountInfo(int accNo, Account* acc) {

	char filename[64];

	getAccountInfoFilename(accNo, filename);

	FILE* file = fopen(filename, "r");

	if (!file) return 0;

	fscanf(file, "%d\n", &acc->accountNumber);

	fgets(acc->name, sizeof(acc->name), file);

	acc->name[strcspn(acc->name, "\n")] = '\0';  // Remove newline

	fclose(file);

	return 1;
}

void viewAccount() {

	int accNo;

	Account acc;

	float balance;


	char password[100];

	printf("\nView Account Details \n");

	printf("Enter the account number: ");

	scanf("%d", &accNo);

	getchar();

	promptPassword(password, sizeof(password));

	if (!checkPassword(accNo, password)) {
		printf("The password is incorrect. Access denied.\n");
		return;
	}

	if (!readAccountInfo(accNo, &acc)) {
		printf("There exists no account with this number.\n");
		return;
	}

	if (!balanceLoadEnc(accNo, &balance)) {
		printf("The account balance could not be read.\n");
		return;
	}

	printf("Account Number: %d\nAccount Holder: %s\nCurrent Balance: %.2f\n", acc.accountNumber, acc.name, balance);
}

void depositMoney() {
	int accNo;
	float dep, balance;
	Account acc;
	char password[100];

	printf("\nDeposit Money \n");
	printf("Enter the account number: ");
	scanf("%d", &accNo);
	getchar();  // Clear newline

	promptPassword(password, sizeof(password));

	if (!checkPassword(accNo, password)) {
		printf("Incorrect password. Deposit denied.\n");
		return;
	}

	printf("Enter the amount to deposit: ");
	scanf("%f", &dep);

	if (!readAccountInfo(accNo, &acc)) {
		printf("There exists no account with this number.\n");
		return;
	}
	if (!balanceLoadEnc(accNo, &balance)) {
		printf("The balance could not be read.\n");
		return;
	}

	if (dep <= 0) {
		printf("You must depost a positive ammount .\n");
		return;
	}

	balance += dep;

	BalanceSaveEnc(accNo, balance);

	printf("Congratulations! New balance: %.2f\n", balance);
}

void withdrawMoney() {
	int accNo;
	float with, balance;
	Account acc;
	char password[100];

	printf("\n=== Withdraw Money ===\n");
	printf("Enter the account number: ");
	scanf("%d", &accNo);
	getchar();  // Clear newline

	promptPassword(password, sizeof(password));
	if (!checkPassword(accNo, password)) {
		printf("Incorrect password. Withdrawal denied.\n");
		return;
	}

	printf("Enter the amount to withdraw: ");
	scanf("%f", &with);

	if (!readAccountInfo(accNo, &acc)) {
		printf("There exists no aacount with this number.\n");
		return;
	}
	if (!balanceLoadEnc(accNo, &balance)) {
		printf("Account balance could not be read.\n");
		return;
	}
	if (with <= 0) {
		printf("You have to withdraw a positive ammount.\n");
		return;
	}
	if (with > balance) {
		printf("You don't have enough funds\n");
		return;
	}

	balance -= with;
	BalanceSaveEnc(accNo, balance);

	printf("Congratulations! New balance: %.2f\n", balance);
}

void deleteAccount() {
	int accNo;
	char infoFile[64], balFile[64], passFile[64];
	char password[100];

	printf("\nDelete an Account\n");
	printf("Enter the account number to delete: ");
	scanf("%d", &accNo);
	getchar();

	promptPassword(password, sizeof(password));
	if (!checkPassword(accNo, password)) {
		printf("Incorrect password.\n");
		return;
	}

	getAccountInfoFilename(accNo, infoFile);
	getBalanceFilename(accNo, balFile);
	getPasswordFilename(accNo, passFile);

	if (!doesAccountExist(accNo)) {
		printf("There exists no account with this number.\n");
		return;
	}

	if (remove(infoFile) != 0 || remove(balFile) != 0 || remove(passFile) != 0) {
		printf("Error deleting account \n");
	} else {
		RemoveAcc(accNo);
		printf("Account deleted successfully.\n");
	}
}


void listAllAccounts() {
	FILE *db = fopen(account_list, "r");
	int accNo;
	Account acc;
	float balance;
	printf("\nList of All Accounts\n");
	if (db) {
		while (fscanf(db, "%d", &accNo) == 1) {
			if (readAccountInfo(accNo, &acc) && balanceLoadEnc(accNo, &balance)) {
				printf("Account: %d | Name: %s | Balance: %.2f\n", acc.accountNumber, acc.name, balance);
			}
		}
		fclose(db);
	} else {
		printf("No accounts found yet.\n");
	}
}




int main() {

	int choice;

	printf("Welcome to the Console Banking System!\n");

	while (1) {

		printf("\n========== Main Menu ==========\n");
		printf("1. Create Account\n");
		printf("2. View Account\n");
		printf("3. Deposit Money\n");
		printf("4. Withdraw Money\n");
		printf("5. List All Accounts\n");
		printf("6. Delete Account\n");
		printf("7. Exit\n");
		printf("Enter your choice (1-7): ");
		scanf("%d", &choice);

		getchar();



		if (choice == 1)
			createAccount();

		else if (choice == 2)
			viewAccount();

		else if (choice == 3)
			depositMoney();

		else if (choice == 4)
			withdrawMoney();

		else if (choice == 5)
			listAllAccounts();

		else if (choice == 6)
			deleteAccount();

		else if (choice == 7) {
			printf("Thank you for using the system. Goodbye!\n");
			break;
		}

		else
			printf("Invalid choice! Please try again.\n");

	}
	return 0;
}




