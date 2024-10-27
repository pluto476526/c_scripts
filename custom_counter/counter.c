#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILENAME "banana_log.csv"
#define REPORT_FILE "banana_report.txt"

// Record a new banana
void record_banana() {
	FILE *file = fopen(FILENAME, "a");
	if (file == NULL) {
		perror("Error opening file");
		return;
	}

	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	char date[11];     // Store date in "YYYY-MM-DD"
	char timeStr[9];   // Store time in "HH:MM:SS"
	strftime(date, sizeof(date), "%Y-%m-%d", t);
	strftime(timeStr, sizeof(timeStr), "%H:%M:%S", t);

	fprintf(file, "%s,%s\n", date, timeStr);
	fclose(file);
	printf("Banana recorded at %s on %s.\n", timeStr, date);
}

// Extract date from a CSV row
void get_date_from_row(const char *row, char *date) {
	strncpy(date, row, 10);
	date[10] = '\0';
}

// Display the summary based on the given period
void show_summary(const char *period, int grouping, FILE *output) {
	FILE *file = fopen(FILENAME, "r");
	if (file == NULL) {
		fprintf(output, "No records!\n");
		return;
	}

	char line[30];
	char prevKey[15] = "";
	int count = 0;

	// Skip header
	fgets(line, sizeof(line), file);

	while (fgets(line, sizeof(line), file)) {
		char date[11];
		get_date_from_row(line, date);

		struct tm tm_date;
		if (strptime(date, "%Y-%m-%d", &tm_date) == NULL) {
			perror("Error parsing date");
			continue;
		}

		char key[15];
		if (grouping == 1) {
			strncpy(key, date, 10);
			key[10] = '\0';
		} else if (grouping == 2) {
			strftime(key, sizeof(key), "%Y-W%U", &tm_date);
		} else if (grouping == 3) {
			strncpy(key, date, 7);
			key[7] = '\0';
		} else {
			strncpy(key, date, 4);
			key[4] = '\0';
		}

		if (strcmp(prevKey, key) == 0) {
			count++;
		} else {
			if (count > 0) {
				fprintf(output, "%s: %d bananas\n", prevKey, count);
			}
			strcpy(prevKey, key);
			count = 1;
		}
	}

	if (count > 0) {
		fprintf(output, "%s: %d bananas\n", prevKey, count);
	}

	fclose(file);
}

// Export report to a file
void export_report() {
	FILE *report_file = fopen(REPORT_FILE, "w");
	if (report_file == NULL) {
		perror("Error creating report file!");
		return;
	}

	fprintf(report_file, "Bananas Intake Report\n");
	fprintf(report_file, "=====================\n\n");

	fprintf(report_file, "Daily Intake:\n");
	show_summary("Daily Banana Intake", 1, report_file);
	fprintf(report_file, "\nMonthly Intake:\n");
	show_summary("Monthly Banana Intake", 3, report_file);
	fprintf(report_file, "\nYearly Intake:\n");
	show_summary("Yearly Banana Intake", 4, report_file);

	fclose(report_file);
	printf("Report exported as %s.\n", REPORT_FILE);
}

// Main menu
void menu() {
	int choice;
	do {
		printf("\n1. Record a new banana\n");
		printf("2. Show daily banana intake\n");
		printf("3. Show weekly banana intake\n");
		printf("4. Show monthly banana intake\n");
		printf("5. Show yearly banana intake\n");
		printf("6. Export report\n");
		printf("7. Exit\n");
		printf("Choose an option (1-7): ");
		scanf("%d", &choice);

		switch (choice) {
			case 1:
				record_banana();
				break;
			case 2:
				show_summary("Daily Banana Intake", 1, stdout);
				break;
			case 3:
				show_summary("Weekly Banana Intake", 2, stdout);
				break;
			case 4:
				show_summary("Monthly Banana Intake", 3, stdout);
				break;
			case 5:
				show_summary("Yearly Banana Intake", 4, stdout);
				break;
			case 6:
				export_report();
				break;
			case 7:
				printf("Goodbye! Nice banana.\n");
				break;
			default:
				printf("Invalid choice! Select a valid option.\n");
		}
	} while (choice != 7);
}

// Initialize the CSV file with a header if it doesn't exist
void initialize_file() {
	FILE *file = fopen(FILENAME, "a+");
	if (file == NULL) {
		perror("Error opening file");
		return;
	}

	// Check if header exists
	fseek(file, 0, SEEK_SET);
	char firstLine[30];
	if (fgets(firstLine, sizeof(firstLine), file) == NULL || strstr(firstLine, "Date, Time") == NULL) {
		fprintf(file, "Date,Time\n");
	}
	fclose(file);
}

// Main function
int main() {
	initialize_file();
	menu();
	return 0;
}
