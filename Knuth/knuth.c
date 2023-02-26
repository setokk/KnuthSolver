#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fenv.h>


#define N 4 
#define NUMBER_OF_CHILDREN 3
#define ACCEPT_INT 0.00000001
#define TIMEOUT 90 // seconds


struct TreeNode
{
    double num;
    char operation[10];
    struct TreeNode *parent;
    struct TreeNode *children[NUMBER_OF_CHILDREN]; // Root, Factorial, Floor

    // Overflow field
    int overflow_raised;
};

struct FrontierNode
{
    struct TreeNode *leaf;
    struct FrontierNode *next;
    struct FrontierNode *previous;
};


typedef struct TreeNode TreeNode;
typedef struct FrontierNode FrontierNode;


int isInteger(double n);
TreeNode* search(double input_num, char* algorithm);
void expandSearchTree(TreeNode* parent_node, char* algorithm, int input_num);
void initializeRoot(TreeNode* root);
int add_frontier_back(TreeNode *node);
int add_frontier_front(TreeNode *node);
int isSolution(TreeNode node, double input_num);
int isFrontierEmpty();
double factorial(double num);
int node_depth(TreeNode node);
int isCycle(TreeNode node);
void get_solution(TreeNode* solution);


FrontierNode* frontier_head = NULL;	// The one end of the frontier
FrontierNode* frontier_tail = NULL;


clock_t start_time;	 // Start time of the search algorithm
clock_t end_time;  // End time of the search algorithm


int depth = 0; // Depth for ids

int total_steps = 0;


int main(int argc, char* argv[])
{
    double input_num;
    char* algorithm = malloc(sizeof(30 * sizeof(char)));

    double state = N;

    // User Input
    do
    {
        printf("PLease insert the number you wish to reach: ");
        scanf("%lf", &input_num);

        if ((input_num <= 0) || !(isInteger(input_num)))
        {
            printf("Negative and non integer numbers not allowed\n\n");
        }

    } while ((input_num <= 0) || !(isInteger(input_num)));
    
    do
    { 
        printf("\nPlease insert the algorithm to be used [bfs/ids(iterative deepening search)]: ");
        scanf("%s", algorithm);

    } while (strcmp(algorithm, "bfs") != 0 && strcmp(algorithm, "ids") != 0);


    start_time = clock();

    // Begin searching based on algorithm chosen
    TreeNode* solution = search(input_num, algorithm);

    end_time = clock();
    
    get_solution(solution);
    
    return 0;
}


int isInteger(double n)
{
    double difference = n - (int) n;

    return (difference <= ACCEPT_INT);
}


TreeNode* search(double input_num, char* algorithm)
{
    clock_t t;

    // Initialize root
    TreeNode* root = (TreeNode*) malloc(sizeof(TreeNode));
    initializeRoot(root);

    // Check if it's the solution
    if (isSolution(*root, input_num))
        return root;

    TreeNode* current_node;
    FrontierNode* temp_frontier_node; 

    if (strcmp(algorithm, "bfs") == 0)
    {
        while (!isFrontierEmpty())
        {
            t = clock();
            if (t - start_time > CLOCKS_PER_SEC * TIMEOUT)
            {
                printf("\nTime is out! Exiting...");
                exit(1);
            }

            current_node = frontier_head->leaf;

            if (isSolution(*current_node, input_num))
                return current_node;

            // Move to the next node in frontier
            temp_frontier_node = frontier_head;
            frontier_head = frontier_head->next;
            free(temp_frontier_node);

            if (frontier_head == NULL)
                frontier_tail = NULL;
            else
                frontier_head->previous = NULL;

            expandSearchTree(current_node, algorithm, input_num);
        }
    }
    else if (strcmp(algorithm, "ids") == 0)
    {
        while (1)
        {
            if (isFrontierEmpty())
            {
                TreeNode* root = (TreeNode*) malloc(sizeof(TreeNode));
                initializeRoot(root);
            }

            while (!isFrontierEmpty())
            {
                t = clock();
                if (t - start_time > CLOCKS_PER_SEC * TIMEOUT)
                {
                    printf("\nTime is out! Exiting...");
                    exit(1);
                }

                current_node = frontier_head->leaf;

                if (isSolution(*current_node, input_num))
                    return current_node;

                // Move to the next node in frontier
                temp_frontier_node = frontier_head;
                frontier_head = frontier_head->next;
                free(temp_frontier_node);

                if (frontier_head == NULL)
                    frontier_tail = NULL;
                else
                    frontier_head->previous = NULL;

                if (!isCycle(*current_node))
                    expandSearchTree(current_node, algorithm, input_num);
            }

            ++depth;
        }
    }

    return NULL;
}


// Called by BFS algorithm (breadth_first_search)
int add_frontier_back(TreeNode *node)
{
	// Creating the new frontier node
    FrontierNode *new_frontier_node = (FrontierNode*) malloc(sizeof(FrontierNode));
	if (new_frontier_node == NULL)
		return -1;

	new_frontier_node->leaf = node;
	new_frontier_node->next = NULL;
	new_frontier_node->previous = frontier_tail;

	if (frontier_tail == NULL)
	{
		frontier_head = new_frontier_node;
		frontier_tail = new_frontier_node;
	}
	else
	{
		frontier_tail->next = new_frontier_node;
		frontier_tail = new_frontier_node;
	}
}


// Called by repeated DFS algorithm (repeated_dfs)
int add_frontier_front(TreeNode *node)
{
	// Creating the new frontier node
	FrontierNode *new_frontier_node = (FrontierNode*) malloc(sizeof(FrontierNode));
	if (new_frontier_node == NULL)
		return -1;

	new_frontier_node->leaf = node;
	new_frontier_node->previous = NULL;
	new_frontier_node->next = frontier_head;

	if (frontier_head == NULL)
	{
		frontier_head = new_frontier_node;
		frontier_tail = new_frontier_node;
	}
	else
	{
		frontier_head->previous = new_frontier_node;
		frontier_head = new_frontier_node;
	}
}


void initializeRoot(TreeNode* root)
{
    root->num = N;
    root->parent = NULL;
    root->overflow_raised = 0;
    strcpy(root->operation, ""); // No operation

    add_frontier_front(root);
}


void expandSearchTree(TreeNode* parent_node, char* algorithm, int input_num)
{
	++total_steps;
	
    double fact; // Result for factorial

    // Check depth for ids
    if (strcmp(algorithm, "ids") == 0)
    {
        if (node_depth(*parent_node) > depth)
            return;
    }
    
    // No loops 
    if (isInteger((parent_node->num)) && (((int) parent_node->num == N)) && (parent_node->parent != NULL))
        return;
    
    if (input_num != 1 && input_num != 2)
    {
        if ((int) parent_node->num == 1)
            return;
        
        if ((int) parent_node->num == 2)
            return;
    }

    // Allocate memory for all three children
    for (int i = 0; i < NUMBER_OF_CHILDREN; i++)
    {
        parent_node->children[i] = (TreeNode*) malloc(sizeof(TreeNode));
        parent_node->children[i]->parent = parent_node;
    }

    // Root operation always makes sense to be used
    parent_node->children[2]->num = sqrt(parent_node->num);
    parent_node->children[2]->overflow_raised = 0;
    strcpy(parent_node->children[2]->operation, "root");


    // Operation optimization
    if (!isInteger(parent_node->num) || parent_node->overflow_raised) // Can't do factorial
    {
        parent_node->children[1]->num = floor(parent_node->num);
        parent_node->children[1]->overflow_raised = 0;
        strcpy(parent_node->children[1]->operation, "floor");

        // De-allocate third child 
        free(parent_node->children[0]);
        parent_node->children[0] = NULL;
    }
    else if (strcmp(parent_node->operation, "floor") == 0 || isInteger(parent_node->num)) // No reason to use floor again if floor was previously used
    {
        fact = factorial(parent_node->num);
        parent_node->children[0]->overflow_raised = (fact == -1);

        if (!parent_node->children[0]->overflow_raised) 
        {
            parent_node->children[0]->num = fact;
            strcpy(parent_node->children[0]->operation, "factorial");
        }
        else
        {
            free(parent_node->children[0]);
            parent_node->children[0] = NULL;
        }

        // De-allocate third child 
        free(parent_node->children[1]);
        parent_node->children[1] = NULL;
    }
    else // All operations make sense
    {
        parent_node->children[1]->num = floor(parent_node->num);
        parent_node->children[1]->overflow_raised = 0;
        strcpy(parent_node->children[1]->operation, "floor");
        
        // Factorial
        fact = factorial(parent_node->num);
        parent_node->children[0]->overflow_raised = (fact == -1);

        if (!parent_node->children[0]->overflow_raised) 
        {
            parent_node->children[0]->num = fact;
            strcpy(parent_node->children[0]->operation, "factorial");
        }
        else
        {
            free(parent_node->children[0]);
            parent_node->children[0] = NULL;
        }
    }
	

    for (int i = 0; i < NUMBER_OF_CHILDREN; i++)
    {
        if (parent_node->children[i] != NULL)
        {
            if (strcmp(algorithm, "bfs") == 0)
            {
                add_frontier_back(parent_node->children[i]);
            }
            else if (strcmp(algorithm, "ids") == 0)
            {
                add_frontier_front(parent_node->children[i]);
            }
        }
    }
}


int isSolution(TreeNode node, double input_num)
{
    return (isInteger(node.num) && 
            (int) node.num == (int) input_num);
}


int isFrontierEmpty()
{
    return (frontier_head == NULL);
}


int isCycle(TreeNode node)
{
    TreeNode* curr_node = node.parent;

    while (curr_node != NULL)
    {
        if (curr_node->num == node.num)
            return 1; // True
        
        curr_node = curr_node->parent;
    }
    
    return 0;
}


// Returns -1 if overflow
double factorial(double num)
{
    double fact = 1;

    for (int i = 2; i <= (int) num; i++)
    {
        fact *= i;

        /* Check overflow
        if (fetestexcept(FE_OVERFLOW)) 
            return -1.0; 
        */
    }

    return fact;
}


int node_depth(TreeNode node)
{
    int d = 0;

    while (node.parent != NULL)
    {
        ++d;
        node = *node.parent;
    }

    return d;
}


void get_solution(TreeNode* solution)
{
    if (solution == NULL)
    {
        printf("No Solution was found");
        return;
    }
    
    printf("\n\nSolution:\n");
    TreeNode* curr_node = solution;

    int length = 12;
    int list_size = 1;
    char** steps = 0;
    
    while (curr_node != NULL)
    {
        steps = (char**) realloc(steps, list_size * (sizeof(char*)));
        steps[list_size - 1] = (char*) malloc(length * sizeof(char));

        strcpy(steps[list_size - 1], curr_node->operation);
        ++list_size;

        curr_node = curr_node->parent;
    }

    for (int i = list_size - 2; i >= 0; i--)
        printf("%s\n", steps[i]);

    printf("\nTotal Steps: %d\n", total_steps);
    printf("Time Spent: %fs", ((float) end_time - start_time) / CLOCKS_PER_SEC);

}
