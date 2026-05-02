#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

struct Student {
    int id;
    char name[50];
} students[MAX];

int count = 0;

struct Node {
    int id;
    char status[10];
    struct Node* next;
};

struct Node* head = NULL;

void trimNewline(char text[]) {
    size_t len = strlen(text);

    if (len > 0 && text[len - 1] == '\n') {
        text[len - 1] = '\0';
    }
}

int readLine(char text[], int size) {
    if (fgets(text, size, stdin) == NULL) {
        return 0;
    }

    trimNewline(text);
    return 1;
}

int readInt(const char prompt[], int *value) {
    char input[50];
    char extra;

    printf("%s", prompt);
    if (!readLine(input, sizeof(input))) {
        return 0;
    }

    if (sscanf(input, " %d %c", value, &extra) != 1) {
        printf("Please enter a valid number.\n");
        return -1;
    }

    return 1;
}

int isValidStatus(const char status[]) {
    return strcmp(status, "Present") == 0 || strcmp(status, "Absent") == 0;
}

int studentExists(int id) {
    int savedId;
    char savedName[50];
    FILE *f = fopen("students.txt", "r");

    if (f == NULL) {
        return 0;
    }

    while (fscanf(f, "%d %49[^\n]", &savedId, savedName) == 2) {
        if (savedId == id) {
            fclose(f);
            return 1;
        }
        fgetc(f);
    }

    fclose(f);
    return 0;
}

int addStudent(int id, char name[]) {
    FILE *f;

    if (id <= 0 || name[0] == '\0') {
        printf("Valid student ID and name are required\n");
        return 0;
    }

    if (studentExists(id)) {
        printf("Student ID already exists\n");
        return 0;
    }

    if (count >= MAX) {
        printf("Student limit reached\n");
        return 0;
    }

    students[count].id = id;
    strncpy(students[count].name, name, sizeof(students[count].name) - 1);
    students[count].name[sizeof(students[count].name) - 1] = '\0';
    count++;

    f = fopen("students.txt", "a");
    if (f == NULL) {
        printf("Could not open students.txt\n");
        return 0;
    }
    fprintf(f, "%d %s\n", id, name);
    fclose(f);

    printf("Student added successfully\n");
    return 1;
}


int markAttendance(int id, char status[]) {
    FILE *f;
    struct Node* newNode;

    if (id <= 0) {
        printf("Valid student ID is required\n");
        return 0;
    }

    if (!isValidStatus(status)) {
        printf("Status must be Present or Absent\n");
        return 0;
    }

    if (!studentExists(id)) {
        printf("Student not found\n");
        return 0;
    }

    newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        return 0;
    }

    f = fopen("attendance.txt", "a");
    if (f == NULL) {
        printf("Could not open attendance.txt\n");
        free(newNode);
        return 0;
    }

    newNode->id = id;
    strncpy(newNode->status, status, sizeof(newNode->status) - 1);
    newNode->status[sizeof(newNode->status) - 1] = '\0';
    newNode->next = head;
    head = newNode;

    fprintf(f, "%d %s\n", id, status);
    fclose(f);

    printf("Attendance marked successfully\n");
    return 1;
}

void display() {
    int id;
    int found = 0;
    char status[10];
    FILE *f = fopen("attendance.txt", "r");

    if (f == NULL) {
        printf("No attendance records found\n");
        return;
    }

    while(fscanf(f, "%d %9s", &id, status) == 2) {
        printf("ID: %d Status: %s\n", id, status);
        found = 1;
    }

    fclose(f);

    if (!found) {
        printf("No attendance records found\n");
    }
}

void showUsage() {
    printf("Usage:\n");
    printf("  backend.exe add <id> <name>\n");
    printf("  backend.exe mark <id> <Present|Absent>\n");
    printf("  backend.exe display\n");
}

int main(int argc, char *argv[]) {
    int choice, id;
    char name[50], status[10];
    int inputResult;

    if (argc > 1) {
        if (strcmp(argv[1], "add") == 0 && argc == 4) {
            id = atoi(argv[2]);
            return addStudent(id, argv[3]) ? 0 : 1;
        }

        if (strcmp(argv[1], "mark") == 0 && argc == 4) {
            id = atoi(argv[2]);
            return markAttendance(id, argv[3]) ? 0 : 1;
        }

        if (strcmp(argv[1], "display") == 0 && argc == 2) {
            display();
            return 0;
        }

        showUsage();
        return 1;
    }

    do {
        printf("\n1.Add Student\n2.Mark Attendance\n3.Display\n4.Exit\n");
        inputResult = readInt("Enter your choice: ", &choice);

        if (inputResult == 0) {
            printf("\nExiting...\n");
            return 0;
        }

        if (inputResult == -1) {
            continue;
        }

        switch(choice) {
            case 1:
                inputResult = readInt("Enter ID: ", &id);
                if (inputResult == 0) {
                    printf("\nExiting...\n");
                    return 0;
                }
                if (inputResult == -1) {
                    break;
                }

                printf("Enter Name: ");
                if (!readLine(name, sizeof(name))) {
                    printf("\nExiting...\n");
                    return 0;
                }
                addStudent(id, name);
                break;

            case 2:
                inputResult = readInt("Enter ID: ", &id);
                if (inputResult == 0) {
                    printf("\nExiting...\n");
                    return 0;
                }
                if (inputResult == -1) {
                    break;
                }

                printf("Enter Status (Present/Absent): ");
                if (!readLine(status, sizeof(status))) {
                    printf("\nExiting...\n");
                    return 0;
                }
                markAttendance(id, status);
                break;

            case 3:
                display();
                break;

            case 4:
                printf("Exiting...\n");
                return 0;

            default:
                printf("Invalid choice. Please choose 1 to 4.\n");
        }
    } while (1);
}
