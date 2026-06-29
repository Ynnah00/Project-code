#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ITEMS 50
#define LOST_FILE "lost.txt"
#define FOUND_FILE "found.txt"

typedef struct {
    int id;
    char type[16];
    char name[50];
    char place[50];
    char date[20];
} Item;

Item lostItems[MAX_ITEMS];
Item foundItems[MAX_ITEMS];
int lostCount = 0;
int foundCount = 0;
int nextId = 1;

void readLine(char *buffer, int size) {
    if (fgets(buffer, size, stdin)) {
        size_t len = strlen(buffer);
        if (len && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

int caseInsensitiveEquals(const char *a, const char *b) {
    while (*a && *b) {
        char ca = (*a >= 'A' && *a <= 'Z') ? *a + 32 : *a;
        char cb = (*b >= 'A' && *b <= 'Z') ? *b + 32 : *b;
        if (ca != cb) return 0;
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

void saveItem(const Item *item, const char *filename) {
    FILE *fp = fopen(filename, "a");
    if (fp == NULL) {
        printf("Error: could not save to file.\n");
        return;
    }
    fprintf(fp, "%d|%s|%s|%s|%s\n", item->id, item->type, item->name, item->place, item->date);
    fclose(fp);
}

void loadItems(Item items[], int *count, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return;
    }

    char line[200];
    while (fgets(line, sizeof(line), fp) != NULL && *count < MAX_ITEMS) {
        Item item;
        char *token;

        token = strtok(line, "|");
        if (token) item.id = atoi(token);

        token = strtok(NULL, "|");
        if (token) strcpy(item.type, token);

        token = strtok(NULL, "|");
        if (token) strcpy(item.name, token);

        token = strtok(NULL, "|");
        if (token) strcpy(item.place, token);

        token = strtok(NULL, "|");
        if (token) {
            size_t len = strlen(token);
            if (len && token[len - 1] == '\n') token[len - 1] = '\0';
            strcpy(item.date, token);
        }

        items[*count] = item;
        (*count)++;

        if (item.id >= nextId) {
            nextId = item.id + 1;
        }
    }

    fclose(fp);
}

void checkMatches(const Item *newFound) {
    int matches = 0;
    printf("\nChecking for possible matches...\n");
    for (int i = 0; i < lostCount; i++) {
        if (caseInsensitiveEquals(newFound->name, lostItems[i].name) ||
            caseInsensitiveEquals(newFound->place, lostItems[i].place)) {
            printf("Possible match! Found item '%s' may match Lost ID %d ('%s' at %s on %s)\n",
                   newFound->name, lostItems[i].id, lostItems[i].name, lostItems[i].place, lostItems[i].date);
            matches++;
        }
    }
    if (matches == 0) {
        printf("No possible matches found yet.\n");
    }
    printf("\n");
}

void addItem(Item items[], int *count, const char *kind, const char *filename) {
    if (*count >= MAX_ITEMS) {
        printf("%s list is full.\n", kind);
        return;
    }
    Item item;
    item.id = nextId++;
    strcpy(item.type, kind);
    printf("Enter item name: ");
    readLine(item.name, sizeof(item.name));
    printf("Enter location (where lost/found): ");
    readLine(item.place, sizeof(item.place));
    printf("Enter date (e.g. 2026-05-13): ");
    readLine(item.date, sizeof(item.date));
    items[*count] = item;
    (*count)++;
    saveItem(&item, filename);
    printf("%s record added with ID %d.\n\n", kind, item.id);

    if (strcmp(kind, "Found") == 0) {
        checkMatches(&item);
    }
}

void listItems(Item items[], int count, const char *title) {
    printf("\n=== %s ===\n", title);
    if (count == 0) {
        printf("No records found.\n\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        printf("ID: %d\n", items[i].id);
        printf("Name: %s\n", items[i].name);
        printf("Location: %s\n", items[i].place);
        printf("Date: %s\n", items[i].date);
        printf("-------------------------\n");
    }
    printf("\n");
}

void searchItems(const char *keyword) {
    int found = 0;
    printf("\nSearch results for '%s':\n", keyword);
    for (int i = 0; i < lostCount; i++) {
        if (strstr(lostItems[i].name, keyword) || strstr(lostItems[i].place, keyword)) {
            printf("[Lost] ID %d: %s at %s on %s\n", lostItems[i].id, lostItems[i].name, lostItems[i].place, lostItems[i].date);
            found = 1;
        }
    }
    for (int i = 0; i < foundCount; i++) {
        if (strstr(foundItems[i].name, keyword) || strstr(foundItems[i].place, keyword)) {
            printf("[Found] ID %d: %s at %s on %s\n", foundItems[i].id, foundItems[i].name, foundItems[i].place, foundItems[i].date);
            found = 1;
        }
    }
    if (!found) {
        printf("No matching records found.\n");
    }
    printf("\n");
}

int main(void) {
    int option;
    char keyword[50];

    loadItems(lostItems, &lostCount, LOST_FILE);
    loadItems(foundItems, &foundCount, FOUND_FILE);

    printf("Lost and Found System\n");
    printf("=======================\n\n");
    while (1) {
        printf("1. Add lost item\n");
        printf("2. Add found item\n");
        printf("3. List lost items\n");
        printf("4. List found items\n");
        printf("5. Search items\n");
        printf("6. Exit\n");
        printf("Choose an option: ");
        if (scanf("%d", &option) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');
        switch (option) {
            case 1:
                addItem(lostItems, &lostCount, "Lost", LOST_FILE);
                break;
            case 2:
                addItem(foundItems, &foundCount, "Found", FOUND_FILE);
                break;
            case 3:
                listItems(lostItems, lostCount, "Lost Items");
                break;
            case 4:
                listItems(foundItems, foundCount, "Found Items");
                break;
            case 5:
                printf("Enter search keyword: ");
                readLine(keyword, sizeof(keyword));
                searchItems(keyword);
                break;
            case 6:
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Please choose a valid option.\n\n");
        }
    }
}
