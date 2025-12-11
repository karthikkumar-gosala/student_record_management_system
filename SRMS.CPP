#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STUDENT_FILE "students.txt"
#define CREDENTIAL_FILE "credentials.txt"

struct Student {
    char roll[64];
    char name[256];
    float marks;
};

char currentRole[32];
char currentUser[64];

/* ---------- Prototypes ---------- */
void clearInput(void);
int loginSystem(void);
void mainMenu(void);
void adminMenu(void);
void studentMenu(void);
void staffMenu(void);
void guestMenu(void);

void addStudent(void);
void displayStudents(void);
void searchStudent(void);
void updateMarksOnly(void);
void deleteStudent(void);
void getGradeAndPassStatus(float marks, char *grade, char *status);
static void trim_newline(char *s);

/* ---------- Utilities ---------- */
void clearInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static void trim_newline(char *s) {
    size_t n = strlen(s);
    if (n == 0) return;
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r')) { s[--n] = '\0'; }
}

/* ---------- Grade ---------- */
void getGradeAndPassStatus(float marks, char *grade, char *status) {
    if (marks >= 90) strcpy(grade, "O");
    else if (marks >= 80) strcpy(grade, "A+");
    else if (marks >= 70) strcpy(grade, "B+");
    else if (marks >= 60) strcpy(grade, "C");
    else if (marks >= 45) strcpy(grade, "D");
    else strcpy(grade, "F");

    if (marks < 45) strcpy(status, "FAIL");
    else strcpy(status, "PASS");
}

/* ---------- Login ---------- */
int loginSystem(void) {
    char username[128], password[128];
    char fileUser[128], filePass[128], fileRole[64];

    printf("===== Login =====\n");
    printf("Username: ");
    if (scanf("%127s", username) != 1) return 0;
    printf("Password: ");
    if (scanf("%127s", password) != 1) return 0;
    clearInput();

    FILE *fp = fopen(CREDENTIAL_FILE, "r");
    if (!fp) {
        printf("Error: Missing %s\n", CREDENTIAL_FILE);
        return 0;
    }

    while (fscanf(fp, "%127s %127s %63s", fileUser, filePass, fileRole) == 3) {
        if (strcmp(username, fileUser) == 0 && strcmp(password, filePass) == 0) {
            strncpy(currentRole, fileRole, sizeof(currentRole)-1);
            currentRole[sizeof(currentRole)-1] = '\0';
            strncpy(currentUser, fileUser, sizeof(currentUser)-1);
            currentUser[sizeof(currentUser)-1] = '\0';
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

/* ---------- Menus ---------- */
void mainMenu(void) {
    if (strcmp(currentRole, "ADMIN") == 0) adminMenu();
    else if (strcmp(currentRole, "STUDENT") == 0) studentMenu();
    else if (strcmp(currentRole, "STAFF") == 0) staffMenu();
    else guestMenu();
}

void adminMenu(void) {
    int choice;
    do {
        printf("\n===== ADMIN MENU =====\n");
        printf("1. Add Student\n2. Display Students\n3. Search Student\n4. Update Marks\n5. Delete Student\n6. Logout\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { clearInput(); continue; }
        clearInput();
        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudent(); break;
            case 4: updateMarksOnly(); break;
            case 5: deleteStudent(); break;
            case 6: return;
            default: printf("Invalid choice\n");
        }
    } while (1);
}

void studentMenu(void) {
    int choice;
    do {
        printf("\n===== STUDENT MENU =====\n1. Display Students\n2. Search Student\n3. Logout\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { clearInput(); continue; }
        clearInput();
        if (choice == 1) displayStudents();
        else if (choice == 2) searchStudent();
        else if (choice == 3) return;
        else printf("Invalid choice\n");
    } while (1);
}

void staffMenu(void) {
    int choice;
    do {
        printf("\n===== STAFF MENU =====\n1. Display Students\n2. Search Student\n3. Update Marks\n4. Logout\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { clearInput(); continue; }
        clearInput();
        if (choice == 1) displayStudents();
        else if (choice == 2) searchStudent();
        else if (choice == 3) updateMarksOnly();
        else if (choice == 4) return;
        else printf("Invalid choice\n");
    } while (1);
}

void guestMenu(void) {
    int choice;
    do {
        printf("\n===== GUEST MENU =====\n1. Display Students\n2. Logout\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { clearInput(); continue; }
        clearInput();
        if (choice == 1) displayStudents();
        else if (choice == 2) return;
        else printf("Invalid choice\n");
    } while (1);
}

/* ---------- Add Student (writes ROLL|NAME|MARKS) ---------- */
void addStudent(void) {
    struct Student s;
    FILE *fp = fopen(STUDENT_FILE, "a");
    if (!fp) { printf("Unable to open %s for append\n", STUDENT_FILE); return; }

    printf("Enter Roll: ");
    if (scanf("%63s", s.roll) != 1) { clearInput(); fclose(fp); return; }
    clearInput();

    printf("Enter Name: ");
    if (!fgets(s.name, sizeof(s.name), stdin)) { fclose(fp); return; }
    trim_newline(s.name);

    printf("Enter Marks: ");
    if (scanf("%f", &s.marks) != 1) { clearInput(); fclose(fp); return; }
    clearInput();

    fprintf(fp, "%s|%s|%.2f\n", s.roll, s.name, s.marks);
    fclose(fp);
    printf("Student added successfully.\n");
}

/* ---------- Display Students (auto column width) ---------- */
void displayStudents(void) {
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) { printf("No students data found (%s missing or empty).\n", STUDENT_FILE); return; }

    char line[512];
    struct Student s;
    int maxRoll = (int)strlen("ROLL");
    int maxName = (int)strlen("NAME");

    /* 1st pass: compute max widths */
    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        /* parse safely */
        char rollbuf[64] = "", namebuf[256] = "";
        float marks = 0.0f;
        if (sscanf(line, "%63[^|]|%255[^|]|%f", rollbuf, namebuf, &marks) >= 2) {
            if ((int)strlen(rollbuf) > maxRoll) maxRoll = (int)strlen(rollbuf);
            if ((int)strlen(namebuf) > maxName) maxName = (int)strlen(namebuf);
        }
    }

    rewind(fp);

    /* print header */
    printf("\n%-*s | %-*s | MARKS | CGPA | GRADE | STATUS\n", maxRoll, "ROLL", maxName, "NAME");

    /* 2nd pass: print rows */
    char grade[16], status[16];
    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        char rollbuf[64] = "", namebuf[256] = "";
        float marks = 0.0f;
        if (sscanf(line, "%63[^|]|%255[^|]|%f", rollbuf, namebuf, &marks) >= 2) {
            getGradeAndPassStatus(marks, grade, status);
            float cgpa = marks / 10.0f;
            printf("%-*s | %-*s | %5.2f | %4.2f | %-5s | %-6s\n",
                   maxRoll, rollbuf,
                   maxName, namebuf,
                   marks, cgpa, grade, status);
        }
    }

    fclose(fp);
}

/* ---------- Search Student ---------- */
void searchStudent(void) {
    char roll[64];
    printf("Enter roll: ");
    if (scanf("%63s", roll) != 1) { clearInput(); return; }
    clearInput();

    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) { printf("No data file.\n"); return; }

    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        struct Student s;
        if (sscanf(line, "%63[^|]|%255[^|]|%f", s.roll, s.name, &s.marks) >= 2) {
            if (strcmp(s.roll, roll) == 0) {
                char grade[16], status[16];
                getGradeAndPassStatus(s.marks, grade, status);
                printf("\nROLL  : %s\nNAME  : %s\nMARKS : %.2f\nCGPA  : %.2f\nGRADE : %s\nSTATUS: %s\n",
                       s.roll, s.name, s.marks, s.marks/10.0f, grade, status);
                found = 1;
                break;
            }
        }
    }
    if (!found) printf("Student not found.\n");
    fclose(fp);
}

/* ---------- Update Marks ---------- */
void updateMarksOnly(void) {
    char roll[64];
    printf("Enter roll to update: ");
    if (scanf("%63s", roll) != 1) { clearInput(); return; }
    clearInput();

    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) { printf("No data file.\n"); return; }
    FILE *tmp = fopen("temp.txt", "w");
    if (!tmp) { printf("Unable to open temporary file.\n"); fclose(fp); return; }

    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        struct Student s;
        if (sscanf(line, "%63[^|]|%255[^|]|%f", s.roll, s.name, &s.marks) >= 2) {
            if (strcmp(s.roll, roll) == 0) {
                printf("Current marks: %.2f\nEnter new marks: ", s.marks);
                if (scanf("%f", &s.marks) != 1) { clearInput(); }
                clearInput();
                found = 1;
            }
            fprintf(tmp, "%s|%s|%.2f\n", s.roll, s.name, s.marks);
        }
    }

    fclose(fp);
    fclose(tmp);

    if (remove(STUDENT_FILE) != 0) {
        /* if remove fails but temp exists, still try rename */
    }
    if (rename("temp.txt", STUDENT_FILE) != 0) {
        printf("Error replacing data file.\n");
    }

    if (found) printf("Marks updated.\n");
    else printf("Roll not found.\n");
}

/* ---------- Delete Student ---------- */
void deleteStudent(void) {
    char roll[64];
    printf("Enter roll to delete: ");
    if (scanf("%63s", roll) != 1) { clearInput(); return; }
    clearInput();

    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) { printf("No data file.\n"); return; }
    FILE *tmp = fopen("temp.txt", "w");
    if (!tmp) { printf("Unable to open temporary file.\n"); fclose(fp); return; }

    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        struct Student s;
        if (sscanf(line, "%63[^|]|%255[^|]|%f", s.roll, s.name, &s.marks) >= 2) {
            if (strcmp(s.roll, roll) == 0) {
                found = 1;
                continue; /* skip writing this record */
            }
            fprintf(tmp, "%s|%s|%.2f\n", s.roll, s.name, s.marks);
        }
    }

    fclose(fp);
    fclose(tmp);

    if (remove(STUDENT_FILE) != 0) {
        /* ignore */
    }
    if (rename("temp.txt", STUDENT_FILE) != 0) {
        printf("Error replacing data file.\n");
    }

    if (found) printf("Deleted student %s\n", roll);
    else printf("Roll not found.\n");
}

/* ---------- main ---------- */
int main(void) {
    if (loginSystem()) {
        mainMenu();
    } else {
        printf("Access denied.\n");
    }
    return 0;
}
