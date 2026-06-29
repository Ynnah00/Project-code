// Online C compiler to run C program online
#include <stdio.h>
#include <string.h>

#define MAX_ITEMS 50

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

void addItem(Item items[], int *count, const char *kind) {
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
    printf("%s record added with ID %d.\n\n", kind, item.id);
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
                addItem(lostItems, &lostCount, "Lost");
                break;
            case 2:
                addItem(foundItems, &foundCount, "Found");
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