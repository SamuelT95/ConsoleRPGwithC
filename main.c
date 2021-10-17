#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/unistd.h>
#include <time.h>

//Notes: Pointers to structures must use the arrow '->'

// Structure
enum itemList {
    HEALTH_POTION,
    MANA_POTION
};
typedef struct ItemStructure {
    char name[50];
    char description[100];
    int health;
    int mana;
    int quantity;
    int usesLeft;
    int id;
} item;
typedef struct itemNodeStructure {
    item *current;
    struct itemNodeStructure *next;
    struct itemNodeStructure *previous;
} itemNode;
typedef enum classEnum {
    WARRIOR,
    RANGER,
    MAGE,
    GOBLIN
} class;
typedef struct playerStructure {
    char name[50];
    class class;
    int health;
    int attack;
    int defense;
    float criticalRate;
    itemNode *inventory;
    bool autopilot;
} player;

// Global Functions
void SetName(player *target, char name[50]) {
    strcpy(target->name, name);
}

player *NewPlayer(class class, char name[50]) {
    player *tempPlayer = malloc(sizeof(player));
    SetName(tempPlayer, name);
    static int playersCreated = 0; // To keep track of the number of players added to the game.
    if (playersCreated > 0) {
        tempPlayer->autopilot = true;
    } else {
        tempPlayer->autopilot = false;
    }
    playersCreated++;

    switch (class) {
        case WARRIOR:
            tempPlayer->health = 30;
            tempPlayer->defense = 3;
            tempPlayer->attack = 5;
            tempPlayer->criticalRate = 0.5;
            tempPlayer->class = WARRIOR;
            break;
        case RANGER:
            tempPlayer->health = 25;
            tempPlayer->attack = 5;
            tempPlayer->defense = 3;
            tempPlayer->class = RANGER;
            break;
        case MAGE:
            tempPlayer->health = 20;
            tempPlayer->defense = 0;
            tempPlayer->attack = 10;
            tempPlayer->class = MAGE;
            break;
        case GOBLIN:
            tempPlayer->health = 10;
            tempPlayer->defense = 1;
            tempPlayer->attack = 3;
            tempPlayer->criticalRate = 0.5;
            tempPlayer->class = GOBLIN;
            break;
        default:
            tempPlayer->health = 10;
            tempPlayer->attack = 2;
            tempPlayer->defense = 0;
            break;
    }
    return tempPlayer;
}

void DisplayMenu() {
    printf("(0) Exit\t"
           "(1) Stats\t"
           "(2) Inventory\t"
           "(3) Rest\t"
           "(4) Explore\t"
           "(5) Shop\n");
}

void DisplayStats(player *target) {
    printf("%s\nHealth: %d\tMana: %d\n", target->name, target->health);
}

void DisplayAttackMenu() {
    printf("(1) Attack\t(2) Run\t\t(3) Use Hp Pot\n");
}

itemNode *findItem(itemNode *node, enum itemList item) {
    // Checking if the list is empty. If the current node is NULL, then the entire list is empty.
    if (node == NULL) {
        return NULL;
    }

    // While an item exists in our current node
    while (node->current != NULL) {
        // Compare that item to our enumerated itemList item example HEALTH_POTION
        // If it is a match, return the node's address and increment the quantity to 2.
        // Back at AddItem function where assigning object ID to enum itemList,
        // if the id matches, it will return successful and will increase the item's quantity.
        if (node->current->id == item) {
            return node;
        }
        // If there isn't a match, look at the next node. If the next node isn't null,
        // move to the next node and compare again. Else, return null.
        if (node->next != NULL) {
            node = node->next;
        } else {
            return NULL;
        }
    }
    return NULL; // Possible when node exist but node->current or node->next or node->previous are null.

}


int AddItem(itemNode *node, enum itemList newItem) {
    // Example: AddItem(Hero->inventory, HEALTH_POTION);
    itemNode *previousNode;
    itemNode *searchResult;

    // See if newItem already exists so that we can increase
    // the item's quantity rather making the linked list longer
    searchResult = findItem(node, newItem);
    if (searchResult != NULL) {
        searchResult->current->quantity += 1; // Matching item found inside existing player's inventory.
        return 0;
    }

    // Generate an item if it doesn't exist inside the player's inventory.
    // Requires allocating memory and increasing the size of the linked list.
    item *object = malloc(sizeof(item));


    switch (newItem) {
        case HEALTH_POTION:
            strcpy(object->name, "Health Potion");
            strcpy(object->description, "A consumable that restores 20 HP.");
            object->health = 20;
            object->mana = 0;
            object->usesLeft = 1;
            object->quantity = 1;
            object->id = newItem;
            break;
        case MANA_POTION:
            strcpy(object->name, "Mana Potion");
            strcpy(object->description, "A consumable that restores 20 mana.");
            object->health = 0;
            object->mana = 20;
            object->usesLeft = 1;
            object->quantity = 1;
            object->id = newItem;
            break;
    }
    // The newItem object is created.

    // Finding free space to put the new object.
    // Checking if the current node is NULL, if so, insert the object inside.
    if (node->current == NULL) {
        node->current = malloc(sizeof(itemNode)); // Allocating memory before adding the object.
        node->current = object; // Storing object address inside current node address.
        node->next = NULL; // Assigning the next node NULL in case of a never ending creation of nodes
        node->previous = NULL;
    } else if (node->next == NULL) {
        node->next = malloc(sizeof(itemNode)); // Allocating memory for the next node.
        previousNode = node; // Storing the locations of the current node.
        node = node->next; // Moving to the next node.
        node->previous = previousNode; // Linking the previous node location with the current.
        node->current = object; // Finally, assigning the object into the new fresh node.
        node->next = NULL;
    } else {
        // If current node and next node are occupied, search for the last node. The last node
        // has an empty spot (NULL) next to it.
        while (node->next != NULL) {
            node = node->next;
        }
        // Once a non-null empty spot is found, create space for the object and set it up for the future nodes.
        node->next = malloc(sizeof(itemNode));
        previousNode = node;
        node = node->next;
        node->previous = previousNode;
        node->current = object;
        node->next = NULL;
    }
}

void DisplayInventory(itemNode *node) {
    // While an item exists in the current node, print out said item, and move to the next node if not null.
    while (node != NULL && node->current != NULL) {
        printf("-------------------\n");
        printf("Name: %s\n", node->current->name);
        printf("Description: %s\n", node->current->description);
        printf("Health restore: %d\tMana restore: %d\nQuantity: %d\t\tUses: %d\n",
               node->current->health, node->current->mana, node->current->quantity, node->current->usesLeft);
        if (node->next != NULL) {
            node = node->next;
        } else {
            return;
        }
    }
    printf("Your inventory is empty.\n");
    return;
}

int RemoveItem(itemNode *node, enum itemList removeItem) {
    itemNode *searchResult;
    itemNode *previous;
    itemNode *next;

    // See if the item already exists
    searchResult = findItem(node, removeItem);

    // If item exists, reduce quantity by 1
    if (searchResult != NULL) {
        searchResult->current->quantity -= 1;

        // If reduction results in 0, delete item.
        if (searchResult->current->quantity <= 0) {
            previous = searchResult->previous;
            next = searchResult->next;

            // Switch linked list together
            // Cannot assign the next/previous members if the itemNode is null.
            if (previous != NULL) {
                searchResult = previous;
                searchResult->next = next;
            }
            if (next != NULL) {
                searchResult = next;
                searchResult->previous = previous;
            }

            // If inventory is empty
            if (previous == NULL && next == NULL) {
                node->current = NULL;
            }
        }
    }
    return 0;
}

int Fight(player *attacker, player *defender) {
    int choose;
    int criticalStrikeRoll;
    int effectiveAttack;
    int *searchResult;
    while (defender->health > 0) {
        // Takes the attacker's critical strike rate and rolls the probability of a critical strike.
        // if attacker's crit is 20%, 1/0.20 = 5 and then rand() rolls a probability of 5 (number range 0 - 4)
        // and if it is 0, it is a critical strike.
        if (attacker->criticalRate == 0) {
            perror("Attacker critical rate not found.");
        }
        criticalStrikeRoll = rand() % (int) (1 / attacker->criticalRate);
        if (criticalStrikeRoll == 0) {
            effectiveAttack = attacker->attack * 2 - defender->defense;
        } else {
            effectiveAttack = attacker->attack - defender->defense;
        }

        // Health restoration for players

        // In case of negative effectiveAttack numbers.
        if (effectiveAttack < 0) {
            effectiveAttack = 0;
        }
        if (attacker->autopilot == 0) {
            DisplayAttackMenu();
            scanf(" %d", &choose);
            printf(">> ");
            switch (choose) {
                case 1:
                    defender->health -= effectiveAttack;
                    if (effectiveAttack == 0) {
                        printf("%s has blocked %s's attack.\n", defender->name, attacker->name);
                    } else {
                        if (criticalStrikeRoll == 0) {
                            printf("***Critical strike!*** ");
                        }
                        printf("%s has inflicted %d damage to %s\n", attacker->name, effectiveAttack, defender->name);
                    }
                    if (defender->health > 0) {
                        printf("%s has %d health left.\n", defender->name, defender->health);
                    } else {
                        printf("%s has 0 health left.\n", defender->name);
                    }
                    break;
                case 2:
                    printf("You run away\n");
                    return 0;
                case 3:
                    searchResult = findItem(attacker->inventory, HEALTH_POTION);
                    if (searchResult != NULL) {
                        int healCalculation = 30 - attacker->health;
                        if (healCalculation > 20) {
                            healCalculation = 20;
                        }
                        attacker->health += healCalculation;
                        printf("You use a health potion. It restored %d health!\n", healCalculation);
                        RemoveItem(attacker->inventory, HEALTH_POTION);
                        if (attacker->health > 30) {

                        }
                    } else {
                        printf("You didn't have a HP pot. Your turn is skipped.\n");
                    }
                    break;
                default:
                    printf("Input invalid\n"
                           "Your turn has been skipped\n");
                    break;
            }
        } else {
            defender->health -= effectiveAttack;
            sleep(1);
            if (effectiveAttack == 0) {
                printf("%s has blocked %s's attack.\n", defender->name, attacker->name);
            } else {
                if (criticalStrikeRoll == 0) {
                    printf("***Critical strike!*** ");
                }
                printf("%s has inflicted %d damage to %s\n", attacker->name, effectiveAttack, defender->name);
            }
            sleep(1);
            if (defender->health > 0) {
                printf("%s has %d health left.\n", defender->name, defender->health);
            } else {
                printf("%s has 0 health left.\n", defender->name);
            }
        }

        if (defender->health > 0) {
            // Switch the attacker and defender after each turn.
            player *tmp = attacker;
            attacker = defender;
            defender = tmp;
        } else {
            printf("%s has bested %s in combat\n", attacker->name, defender->name);
            break; //redundant
        }
    }

    return 0;
}

int main() {
    srand(time(NULL));
    player *Hero = NewPlayer(WARRIOR, "Samuel");
    Hero->inventory = malloc(sizeof(itemNode));
    player *Goblin = NewPlayer(GOBLIN, "Goblin Scout");
    player *Ogre = NewPlayer(WARRIOR, "Ogre Warrior");
    int choice = 1;
    printf("Welcome to ConsoleRPG\n");

    //Game Loop
    while (choice > 0) {
        DisplayMenu();
        printf(">> ");
        while (!scanf("%d", &choice)) {
            scanf("%*[^\n]"); //discard that line up to the newline
            printf("Could not read an integer value!\n"
                   "Please enter a number.\n");
        }

        switch (choice) {
            case 0:
                printf("Exiting");
                break;
            case 1:
                printf("Character stats\n");
                DisplayStats(Hero);
                break;
            case 2:
                printf("Inventory\n");
                DisplayInventory(Hero->inventory);
                break;
            case 3:
                Hero->health = 30;
                printf("You restore your health to %d\n", Hero->health);
                break;
            case 4:
                printf("Explore\n");

                int event = rand() % 3;

                switch (event) {
                    case 0:
                        printf("You encounter a goblin along the road.\n");
                        Goblin->health = 10;
                        Fight(Hero, Goblin);
                        break;
                    case 1:
                        printf("You encounter a huge ogre with a rock club.\n");
                        Ogre->health = 40;
                        Ogre->attack = 5;
                        Ogre->criticalRate = 0.20;
                        Ogre->defense = 1;
                        Fight(Hero, Ogre);
                        break;
                    case 2:
                        printf("You find nothing\n");
                        break;
                }
                break;
            case 5:
                printf("Free potion day! Get your free potions!\n"
                       "(1) Health Potion\t(2) Mana Potion\n");
                printf(">> ");
                while (!scanf(" %d", &choice)) {
                    scanf("%*[^\n]"); // Discard that line up to the newline
                    printf("Could not read an integer value!\n"
                           "Please enter a number.\n");
                }

                switch (choice) {
                    case 1:
                        printf("***A health potion has been added to your inventory!***\n");
                        AddItem(Hero->inventory, HEALTH_POTION);
                        break;
                    case 2:
                        printf("***A mana potion has been added to your inventory!***\n");
                        AddItem(Hero->inventory, MANA_POTION);
                        break;
                    default:
                        printf("Bad input\n");
                        break;
                }
                break;
            default:
                printf("No such command!\n");
                break;
        }
    }
    free(Hero); // Free up space
    return 0;
}

