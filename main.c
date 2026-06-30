#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_ITEMS 500
#define LOST_FILE "lost.txt"
#define FOUND_FILE "found.txt"
#define MAX_CATEGORY 30
#define MAX_CONTACT 50
#define MAX_DESCRIPTION 100

typedef struct {
    int id;
    char type[16];
    char name[50];
    char category[MAX_CATEGORY];
    char place[50];
    char date[20];
    char contact[MAX_CONTACT];
    char description[MAX_DESCRIPTION];
    int resolved;
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

char* toLower(const char *str, char *result) {
    int i = 0;
    while (str[i]) {
        result[i] = tolower((unsigned char)str[i]);
        i++;
    }
    result[i] = '\0';
    return result;
}

int levenshteinDistance(const char *a, const char *b) {
    int len_a = strlen(a);
    int len_b = strlen(b);
    int **dp = malloc((len_a + 1) * sizeof(int *));
    for (int i = 0; i <= len_a; i++) {
        dp[i] = malloc((len_b + 1) * sizeof(int));
    }

    for (int i = 0; i <= len_a; i++) dp[i][0] = i;
    for (int j = 0; j <= len_b; j++) dp[0][j] = j;

    for (int i = 1; i <= len_a; i++) {
        for (int j = 1; j <= len_b; j++) {
            int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            dp[i][j] = (dp[i - 1][j] + 1 < dp[i][j - 1] + 1) 
                        ? (dp[i - 1][j] + 1 < dp[i - 1][j - 1] + cost ? dp[i - 1][j] + 1 : dp[i - 1][j - 1] + cost)
                        : (dp[i][j - 1] + 1 < dp[i - 1][j - 1] + cost ? dp[i][j - 1] + 1 : dp[i - 1][j - 1] + cost);
        }
    }

    int distance = dp[len_a][len_b];
    for (int i = 0; i <= len_a; i++) {
        free(dp[i]);
    }
    free(dp);
    return distance;
}

int fuzzyMatch(const char *str1, const char *str2) {
    char lower1[100], lower2[100];
    toLower(str1, lower1);
    toLower(str2, lower2);
    
    int distance = levenshteinDistance(lower1, lower2);
    int maxLen = strlen(lower1) > strlen(lower2) ? strlen(lower1) : strlen(lower2);
    
    return distance <= maxLen / 3;
}

void saveItem(const Item *item, const char *filename) {
    FILE *fp = fopen(filename, "a");
    if (fp == NULL) {
        printf("Error: could not save to file.\n");
        return;
    }
    fprintf(fp, "%d|%s|%s|%s|%s|%s|%s|%s|%d\n", 
            item->id, item->type, item->name, item->category, 
            item->place, item->date, item->contact, item->description, item->resolved);
    fclose(fp);
}

void loadItems(Item items[], int *count, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return;
    }

    char line[500];
    while (fgets(line, sizeof(line), fp) != NULL && *count < MAX_ITEMS) {
        Item item;
        char *token;
        int field = 0;

        token = strtok(line, "|");
        while (token && field < 9) {
            switch (field) {
                case 0: item.id = atoi(token); break;
                case 1: strcpy(item.type, token); break;
                case 2: strcpy(item.name, token); break;
                case 3: strcpy(item.category, token); break;
                case 4: strcpy(item.place, token); break;
                case 5: strcpy(item.date, token); break;
                case 6: strcpy(item.contact, token); break;
                case 7: strcpy(item.description, token); break;
                case 8: item.resolved = atoi(token); break;
            }
            token = strtok(NULL, "|");
            field++;
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
    printf("\n=== Checking for possible matches ===\n");
    
    for (int i = 0; i < lostCount; i++) {
        if (lostItems[i].resolved) continue;
        
        int nameMatch = fuzzyMatch(newFound->name, lostItems[i].name);
        int placeMatch = fuzzyMatch(newFound->place, lostItems[i].place);
        int categoryMatch = strcmp(newFound->category, lostItems[i].category) == 0;
        
        if (nameMatch || placeMatch || categoryMatch) {
            printf("\n⚠️  POSSIBLE MATCH!\n");
            printf("Found item: ID %d - %s (Category: %s)\n", newFound->id, newFound->name, newFound->category);
            printf("Lost item:  ID %d - %s (Category: %s)\n", lostItems[i].id, lostItems[i].name, lostItems[i].category);
            printf("Location match: %s | Name match: %s\n", 
                   placeMatch ? "Yes" : "No", nameMatch ? "Yes" : "No");
            printf("Contact for lost item: %s\n", lostItems[i].contact);
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
    item.resolved = 0;
    
    printf("\n=== Add %s Item ===\n", kind);
    printf("Enter item name: ");
    readLine(item.name, sizeof(item.name));
    
    printf("Enter category (e.g., Electronics, Jewelry, Documents, Clothing): ");
    readLine(item.category, sizeof(item.category));
    
    printf("Enter location (where %s): ", kind == "Lost" ? "lost" : "found");
    readLine(item.place, sizeof(item.place));
    
    printf("Enter date (e.g. 2026-05-13): ");
    readLine(item.date, sizeof(item.date));
    
    printf("Enter contact (name/phone/email): ");
    readLine(item.contact, sizeof(item.contact));
    
    printf("Enter description: ");
    readLine(item.description, sizeof(item.description));
    
    items[*count] = item;
    (*count)++;
    saveItem(&item, filename);
    printf("\n✓ %s record added with ID %d.\n\n", kind, item.id);

    if (strcmp(kind, "Found") == 0) {
        checkMatches(&item);
    }
}

void listItems(Item items[], int count, const char *title, int showResolved) {
    printf("\n=== %s ===\n", title);
    if (count == 0) {
        printf("No records found.\n\n");
        return;
    }
    
    int displayed = 0;
    for (int i = 0; i < count; i++) {
        if (!showResolved && items[i].resolved) continue;
        
        printf("\nID: %d | Status: %s\n", items[i].id, items[i].resolved ? "RESOLVED" : "ACTIVE");
        printf("Name: %s\n", items[i].name);
        printf("Category: %s\n", items[i].category);
        printf("Location: %s\n", items[i].place);
        printf("Date: %s\n", items[i].date);
        printf("Contact: %s\n", items[i].contact);
        printf("Description: %s\n", items[i].description);
        printf("-------------------------\n");
        displayed++;
    }
    
    if (displayed == 0) {
        printf("No active records found.\n");
    }
    printf("\n");
}

void searchItems(const char *keyword) {
    int found = 0;
    printf("\n=== Search results for '%s' ===\n\n", keyword);
    
    for (int i = 0; i < lostCount; i++) {
        if (strstr(lostItems[i].name, keyword) || 
            strstr(lostItems[i].place, keyword) ||
            strstr(lostItems[i].category, keyword) ||
            strstr(lostItems[i].description, keyword)) {
            printf("[Lost] ID %d: %s (%s) at %s on %s\n", 
                   lostItems[i].id, lostItems[i].name, lostItems[i].category, 
                   lostItems[i].place, lostItems[i].date);
            printf("       Contact: %s | Status: %s\n\n", 
                   lostItems[i].contact, lostItems[i].resolved ? "RESOLVED" : "ACTIVE");
            found = 1;
        }
    }
    
    for (int i = 0; i < foundCount; i++) {
        if (strstr(foundItems[i].name, keyword) || 
            strstr(foundItems[i].place, keyword) ||
            strstr(foundItems[i].category, keyword) ||
            strstr(foundItems[i].description, keyword)) {
            printf("[Found] ID %d: %s (%s) at %s on %s\n", 
                   foundItems[i].id, foundItems[i].name, foundItems[i].category, 
                   foundItems[i].place, foundItems[i].date);
            printf("        Contact: %s | Status: %s\n\n", 
                   foundItems[i].contact, foundItems[i].resolved ? "RESOLVED" : "ACTIVE");
            found = 1;
        }
    }
    
    if (!found) {
        printf("No matching records found.\n");
    }
    printf("\n");
}

void markResolved(Item items[], int count, const char *kind, const char *filename) {
    int id;
    printf("Enter ID to mark as resolved: ");
    scanf("%d", &id);
    while (getchar() != '\n');
    
    for (int i = 0; i < count; i++) {
        if (items[i].id == id) {
            items[i].resolved = 1;
            
            FILE *fp = fopen(filename, "w");
            if (fp == NULL) {
                printf("Error saving changes.\n");
                return;
            }
            
            for (int j = 0; j < count; j++) {
                fprintf(fp, "%d|%s|%s|%s|%s|%s|%s|%s|%d\n",
                        items[j].id, items[j].type, items[j].name, items[j].category,
                        items[j].place, items[j].date, items[j].contact, items[j].description, items[j].resolved);
            }
            fclose(fp);
            
            printf("✓ Item ID %d marked as resolved.\n\n", id);
            return;
        }
    }
    
    printf("Item ID %d not found.\n\n", id);
}

int main(void) {
    int option;
    char keyword[100];

    loadItems(lostItems, &lostCount, LOST_FILE);
    loadItems(foundItems, &foundCount, FOUND_FILE);

    printf("╔════════════════════════════════╗\n");
    printf("║  Lost and Found System v2.0    ║\n");
    printf("╚════════════════════════════════╝\n\n");
    
    while (1) {
        printf("┌─ MAIN MENU ─────────────────┐\n");
        printf("│ 1. Add lost item             │\n");
        printf("│ 2. Add found item            │\n");
        printf("│ 3. List lost items           │\n");
        printf("│ 4. List found items          │\n");
        printf("│ 5. Search items              │\n");
        printf("│ 6. Mark item as resolved     │\n");
        printf("│ 7. View statistics           │\n");
        printf("│ 8. Exit                      │\n");
        printf("└──────────────────────────────┘\n");
        printf("Choose an option: ");
        
        if (scanf("%d", &option) != 1) {
            printf("Invalid input. Please enter a number.\n\n");
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
                listItems(lostItems, lostCount, "Lost Items", 0);
                break;
            case 4:
                listItems(foundItems, foundCount, "Found Items", 0);
                break;
            case 5:
                printf("Enter search keyword: ");
                readLine(keyword, sizeof(keyword));
                searchItems(keyword);
                break;
            case 6: {
                int resolveChoice;
                printf("1. Mark lost item as resolved\n");
                printf("2. Mark found item as resolved\n");
                printf("Choose: ");
                scanf("%d", &resolveChoice);
                while (getchar() != '\n');
                if (resolveChoice == 1) {
                    markResolved(lostItems, lostCount, "Lost", LOST_FILE);
                } else if (resolveChoice == 2) {
                    markResolved(foundItems, foundCount, "Found", FOUND_FILE);
                }
                break;
            }
            case 7: {
                int activeLost = 0, activeFound = 0;
                for (int i = 0; i < lostCount; i++) if (!lostItems[i].resolved) activeLost++;
                for (int i = 0; i < foundCount; i++) if (!foundItems[i].resolved) activeFound++;
                
                printf("\n=== System Statistics ===\n");
                printf("Total lost items: %d (Active: %d)\n", lostCount, activeLost);
                printf("Total found items: %d (Active: %d)\n", foundCount, activeFound);
                printf("Potential matches: Items awaiting review\n\n");
                break;
            }
            case 8:
                printf("Thank you for using Lost and Found System. Goodbye!\n");
                return 0;
            default:
                printf("Please choose a valid option.\n\n");
        }
    }
}
