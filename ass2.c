/* Program to find the shortest range from one point to other point in 
   gridsville. Where gridsville is a grid based town. It takes
   a text file as input.

   Written by Maurice Bernardo Edwin, 10 October 2017.
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Maximum column in gridsville ('a' to 'z')
#define MAX_COLUMN 26

// Value if the road can't be crossed
#define IMPOSSIBLE 999

// Initial value of each node (Not accessible at all)
#define INITIAL_VALUE 9999

// Initial value of each node (Doesn't come from anywhere)
#define NO_DIRECTION -2

// Minimal coordinate given for starting and ending point
#define MINIMAL_TARGET 2

// Direction value if it's the starting point
#define BASE -1

// Initial value for the node if it's the starting point
#define BASE_VALUE 0

/* The rank for value of the direction is RIGHT > DOWN > UP > LEFT 
   as coming from right is highest in "lexicographic ordering" 
   e.g ("0a" to "1a") for RIGHT , ("1a" to "1b") for DOWN 
   ("0c" to "0b") for UP , ("1a" to "0a") for LEFT */

// Direction value for going right
#define RIGHT 3

// Direction value for going up
#define UP 1

// Direction value for going left
#define LEFT 0

// Direction value for going down
#define DOWN 2

// Number of line which shows the up and down directions
#define NO_CHARACTER_LINE 2

// Number of directions
#define NUMBER_OF_DIRECTION 4

// The ASCII representation of 'a'
#define ASCII_VAL 97

/* Type definition ---------------------------------------------------------- */

/* Structure that retains the coordinate (row and column) for a point */
typedef struct {
    char row;
    int column;
}coordinate_t;

/* Structure that retains the coordinate and the value of each direction 
   for a point */
typedef struct {
    coordinate_t coordinate;
    int direction[NUMBER_OF_DIRECTION];
}intersection_t;

/* Structure that retains the coordinate, value of the point, and the direction 
   where the value came from */
typedef struct{
    coordinate_t coordinate;
    int value;
    int direction;
}node_t;

/* Structure that retain the pointer to the variable that is used in this 
   program */
typedef struct {
    intersection_t *data;
    node_t *nodes;
    coordinate_t *target_list;
}ptrstorage_t;

/* Function Prototypes--------------------------------------------------------*/

/* Function Prototypes for stage 1 */
void city_details(intersection_t *data, coordinate_t *target_list, node_t *nodes
                ,coordinate_t *city_size, ptrstorage_t *storage, int *n_target);
intersection_t initialize_and_calc(int *block, int *totalcost, node_t *nodes);
int calc_block(int *direction_value);
int calc_valid_cost(int *direction_value);

/* Function Prototypes for stage 2 except fill_nodes where it's also 
   used in stage 3 */
void pathtracing(intersection_t *data, coordinate_t *target_list, node_t *nodes, 
                coordinate_t city_size, int n_target);
void fill_nodes(intersection_t *data,node_t *nodes,int total_column, 
                int num_intersection);
node_t* tracing(node_t *trace, node_t *nodes, int column, char row, 
                int total_column, int *trace_length);
void retrace_and_print(int column, char row, node_t *nodes, int total_column);
void print_trace(node_t *trace, int trace_length);

/* Function Prototypes for stage 3 */
void generate_map(intersection_t *data, coordinate_t *target_list, node_t *nodes
                ,coordinate_t city_size, int n_target);
void map_printer(node_t *nodes, coordinate_t city_size);
void free_memory(intersection_t *data, coordinate_t *target_list, node_t *nodes, 
                 ptrstorage_t *storage);

/* The heart of the program---------------------------------------------------*/

int main(int argc, char *argv[]){
    intersection_t *data = NULL;
    coordinate_t *target_list = NULL;
    node_t *nodes = NULL;
    ptrstorage_t storage;
    coordinate_t city_size;
    int n_target;

    /* Do the stage 1 of the project */
    city_details(data,target_list,nodes,&city_size,&storage,&n_target);

    /* Update the pointer to data, target_list, and nodes after initialising 
       them in stage 1 */
    data = storage.data;
    target_list = storage.target_list;
    nodes = storage.nodes;

    /* Do the stage 2 of the project */
    pathtracing(data,target_list,nodes,city_size, n_target);

    /* Do the stage 3 of the project */
    generate_map(data,target_list,nodes,city_size, n_target);

    /* Free the space that are claimed and put all the pointer to NULL*/
    free_memory(data,target_list,nodes,&storage);

    return 0;
}

/* The other component of the program-----------------------------------------*/

/* Function to execute stage 1 of the project and initialise the variable 
   needed for the next stage */
void city_details(intersection_t *data, coordinate_t *target_list, node_t *nodes
                ,coordinate_t *city_size, ptrstorage_t *storage, int *n_target){
    
    /* Get the number of column and row of the city then print */
    int col, row, i;
    scanf("%d %d", &col, &row);
    int num_intersection = col*row;
    city_size->column = col;
    city_size->row = row;
    printf("S1: grid is %d x %d, and has %d intersections\n",col,row,
            num_intersection);
    
    /* allocate memory space for data and nodes and store it in storage */
    data = malloc(sizeof(*data)*num_intersection);
    assert(data);
    nodes = malloc(sizeof(*nodes)*num_intersection);
    assert(nodes);
    storage->data = data;
    storage->nodes = nodes;  

    /* Initialise data by initialize_and_calc function then print the next
       line in stage 1 */
    int block = 0, totalcost = 0;
    int total = num_intersection*4;

    for (i = 0; i < num_intersection; i++){
        data[i] = initialize_and_calc(&block,&totalcost,&nodes[i]);
    }

    printf("S1: of %d possibilities, %d of them cannot be used\n",total,block);
    printf("S1: total cost of remaining possibilities is %d seconds\n",
            totalcost);
    
    /* Initialise target_list */
    int count = 0;
    int size = MINIMAL_TARGET;
    coordinate_t dummy;
    target_list = malloc(sizeof(*target_list)*size);
    assert(target_list);

    while(scanf("%d%c",&dummy.column,&dummy.row) == 2){

        // realloc if the space allocated is not enough
        if (count == size){
            size *= 2;
            target_list = realloc(target_list, sizeof(*target_list)*size);
        }

        target_list[count] = dummy;
        count++;
    }

    /* Assign n_target as the number of target and put pointer of target_list
       to storage */
    *n_target = count;
    storage->target_list = target_list;
 
    /* Print the last part of stage 1 */
    printf("S1: %d grid locations supplied, first one is %d%c, ", count,
            target_list[0].column,target_list[0].row);
    printf("last one is %d%c\n", target_list[count-1].column,
            target_list[count-1].row);
    printf("\n");
}

/* Calculate the number of the road that cannot be accessed by detecting 
   how many direction value is IMPOSSIBLE */
int calc_block(int *direction_value){
    int i;
    int blocks = 0;
    for (i=0; i < NUMBER_OF_DIRECTION; i++){
        if (direction_value[i] == IMPOSSIBLE){
            blocks++;
        }
    }
    return blocks;
}

/* Calculate the total cost of possible costs */
int calc_valid_cost(int *direction_value){
    int i;
    int sum = 0;
    for (i=0; i < NUMBER_OF_DIRECTION; i++){
        if (direction_value[i] != IMPOSSIBLE){
            sum = sum + direction_value[i];
        }
    }
    return sum;
}

/* Function to initialise each data of the point by returning it, it also 
   calculate the number of total cost and the number of blocked road, and
   initialise value for each node in nodes */
intersection_t initialize_and_calc(int *block, int *totalcost, node_t *nodes){
    int column;
    int direction_value[NUMBER_OF_DIRECTION];
    char row;
    intersection_t duplicate;
    scanf("%d%c %d %d %d %d",&column, &row, &direction_value[RIGHT],
          &direction_value[UP], &direction_value[LEFT], &direction_value[DOWN]);
    
    /* Assign the data to duplicate to be returned later */
    duplicate.coordinate.column = column;
    duplicate.coordinate.row = row;
    duplicate.direction[RIGHT] = direction_value[RIGHT];
    duplicate.direction[UP] = direction_value[UP];
    duplicate.direction[LEFT] = direction_value[LEFT];
    duplicate.direction[DOWN] = direction_value[DOWN];
    
    /* Initialise each node with it's coordinate, and 
       its initial value and direction */
    nodes->coordinate.column= column;
    nodes->coordinate.row = row;
    nodes->value = INITIAL_VALUE;
    nodes->direction = NO_DIRECTION;

    /* Calculate the road that can't be accessed and the total cost of possible 
       passage */
    *block = *block + calc_block(direction_value);
    *totalcost = *totalcost + calc_valid_cost(direction_value);

    return duplicate;
}

/* Function to execute stage 2 of the project. It prints the path history from
   the starting point to each target point */
void pathtracing(intersection_t *data, coordinate_t *target_list, node_t *nodes, 
             coordinate_t city_size, int n_target){
    
    /* Initialise the starting position as the base */
    int base_column = target_list[0].column;
    char base_row = target_list[0].row;
    
    /* Extract the information about the size of the city from city_size */
    int total_column = city_size.column;
    int total_row = city_size.row;
    int num_intersection = total_row*total_column;
    
    /* Calculate the index of the base and initialise it in nodes as a base */
    int base_index;
    base_index = ((base_row - ASCII_VAL)*total_column)+base_column;
    nodes[base_index].value = BASE_VALUE;
    nodes[base_index].direction = BASE;

    /* Fill in the nodes with informations about them */
    fill_nodes(data,nodes,total_column,num_intersection);

    /* Retrace the nodes and print path for each target_point */
    int i;
    for (i = 1; i < n_target; i++){
        retrace_and_print(target_list[i].column,target_list[i].row,nodes,
                        total_column);
    }
    printf("\n");
}

/* Function to fill in each nodes with its value and where it came from */
void fill_nodes(intersection_t *data, node_t *nodes, int total_column, 
                int num_intersection){
    int i,j;

    /* Initialise the change of index for each direction */
    int move[NUMBER_OF_DIRECTION];
    move[RIGHT] = 1;
    move[LEFT] = -1;
    move[UP] = -total_column;
    move[DOWN] = total_column;

    /* Variable to detect if the nodes already stable or not stable */
    int not_stable = 1;
    int dir_val, node_val;

    /* Keep looping if there's change happening in the nodes */
    while (not_stable){
        not_stable = 0;
        /* Check for each node in nodes */
        for (i = 0; i < num_intersection; i++){
            /* Check for each direction */
            for (j = 0; j < NUMBER_OF_DIRECTION; j++){
                dir_val = data[i].direction[j];
                node_val = nodes[i].value;
                
                /* Continue if the road is accessible and the target 
                   nodes is not a base, then initialise for the node if the 
                   value is smaller or the same */
                if(dir_val < IMPOSSIBLE && nodes[i+move[j]].direction != BASE){
                    if (dir_val + node_val < nodes[i+move[j]].value){
                        nodes[i+move[j]].value = node_val + dir_val;
                        nodes[i+move[j]].direction = j;
                        not_stable = 1;
                    } else if (dir_val + node_val == nodes[i+move[j]].value){
                        /* if the value is the same, compare the direction it 
                           comes from */
                        if(nodes[i+move[j]].direction < j){
                            nodes[i+move[j]].value = node_val + dir_val;
                            nodes[i+move[j]].direction = j;
                            not_stable = 1;
                        }
                    }
                }
            }
        }
    }
}

/* Retrace tha path taken from target to the starting point and print the
   path */
void retrace_and_print(int column, char row, node_t *nodes, int total_column){
    node_t *trace = NULL;
    int trace_length;

    /* Update the pointer of trace after initialising it */
    trace = tracing(trace, nodes, column, row, total_column, &trace_length);

    /* Print the path from the trace */
    print_trace(trace, trace_length);

    /* Free memory allocated to the trace */
    free(trace);
    trace = NULL;
}  

/* Function that return the path that is accessed to get from base till the
   target point and update how many step it needs to trace_length */
node_t* tracing(node_t *trace, node_t *nodes, int column, char row, 
                int total_column, int *trace_length){
    
    /* Initialise the index of the base (target point) and make the target 
       node as base_node */
    int base_index;
    node_t base_node;
    base_index = ((row - ASCII_VAL)*total_column)+column;
    base_node = nodes[base_index];

    /* Initialise the change of index for each direction */
    int move[NUMBER_OF_DIRECTION];
    move[RIGHT] = 1;
    move[LEFT] = -1;
    move[UP] = -total_column;
    move[DOWN] = total_column;

    /* Allocate memory to trace */
    int size = MINIMAL_TARGET;
    trace = malloc(sizeof(*trace)*size);
    
    /* Initialise the first trace as the target node */
    trace[0] = base_node;
    int i = 1;

    /* Keep looping until it reach the starting node */
    while (base_node.direction != -1){
        /* realloc if the memory allocated is not enough */
        if(i == size){
            size *= 2;
            trace = realloc(trace,sizeof(*trace)*size);
        }

        /* Change the base_index according with the direction it comes from */
        base_index = base_index - move[base_node.direction];
       
        /* Put the next node into the trace and make the next node to base */
        trace[i] = nodes[base_index];
        base_node = nodes[base_index];
        i++;
    }

    /* Update the length of the trace */
    *trace_length = i;

    return trace;
}

/* Function that do the printing for stage 2 based on the path trace given */
void print_trace(node_t *trace, int trace_length){
    int i;
    /* Print from behind to front as the trace start from the target point 
       to the starting point */
    for (i = trace_length - 1; i > -1; i--){
        if(i == trace_length - 1){
            printf("S2: start at grid %d%c, cost of %d\n", 
                    trace[i].coordinate.column, trace[i].coordinate.row,
                    trace[i].value);
        } else {
            printf("S2:       then to %d%c, cost of %d\n", 
                    trace[i].coordinate.column, trace[i].coordinate.row,
                    trace[i].value);
        }
    }
}

/* Function to execute stage 3 of the project it prints the map when each point
   in target_list is starting point */
void generate_map(intersection_t *data, coordinate_t *target_list, node_t *nodes
                  , coordinate_t city_size, int n_target){
    
    int i;
    int base_column, base_index;
    char base_row;

    /* Extract the information about the size of the city from city_size */
    int total_column = city_size.column;
    int total_row = city_size.row;
    int num_intersection = total_row*total_column;

    /* Initialise the nodes with point in target_list as base */
    for (i = 0; i < n_target; i++){
        base_column = target_list[i].column;
        base_row = target_list[i].row;
        base_index = ((base_row - ASCII_VAL)*total_column)+base_column;
        nodes[base_index].value = 0;
        nodes[base_index].direction = -1;
    }

    /* Update the nodes after initialising the node with point in target_list 
       as starting point */
    fill_nodes(data,nodes,total_column,num_intersection);

    /* Print the map with its formatting required in stage 3 */
    map_printer(nodes,city_size);

    printf("\n");
}

/* Function to print the map with format of stage 3 */
void map_printer(node_t *nodes, coordinate_t city_size){
    /* Extract the information about the size of the city from city_size */
    int total_column = city_size.column;
    int total_row = city_size.row;
    int num_intersection = total_column*total_row;

    int i,j,z;

    /* Print the first line of the map containing the number of column */
    printf("S3:");
    for (i = 0; i < total_column; i++){
        printf("%9d",i);
    }
    printf("\n");

    /* Print the second line of the map which is the upper border */
    printf("S3:   +----+");
    for (i = 1; i < total_column; i++){
        printf("--------+");
    }
    printf("\n");

    int row = 0;
    /* Loop for the number of row times */
    for (i = 0; i < num_intersection ; i = i + total_column){
        printf("S3: %c |  ",(97 + row));
        row++;

        /* Print mechanism for the line with row character in front of it-----*/
        
        /* Loop for the number of column times */
        for (j = 0; j < total_column; j++){
            /* If it's the last value in the row */
            if((i+j)%total_column == total_column-1){
                printf("%3d",nodes[i+j].value);
            } else {
                /* Check if the node in the right of this node come from this 
                   node or this node come from left */
                if(nodes[i+j+1].direction == RIGHT){
                    printf("%3d >>>> ",nodes[i+j].value);
                } else if (nodes[i+j].direction == LEFT) {
                    printf("%3d <<<< ",nodes[i+j].value);
                } else {
                    printf("%3d      ",nodes[i+j].value);
                }
            }
        }
        printf("\n");

        /* Print mechanism for the line without row character in front of it--*/

        /* Do this part as long as it's not the last row */
        if(i != (num_intersection)-total_column){

            /* Loop for 2 times because there's 2 line without row character 
               for each row character */
            for (z = 0; z < NO_CHARACTER_LINE; z++){

                printf("S3:   |    ");

                /* Loop for the number of column times */
                for (j = 0; j < total_column; j++){
                    /* If it's the last column */
                    if((i+j)%total_column == (total_column - 1)){
                        /* Check if the node now come from bottom or the node
                           in the bottom of this node come from this node */
                        if(nodes[i+j].direction == UP){
                            printf("^");
                        }else if(nodes[i+j+total_column].direction == DOWN){
                            printf("v");
                        }else{
                            printf(" ");
                        }
                    } else {
                        /* Check if the node now come from bottom or the node
                           in the bottom of this node come from this node */
                        if(nodes[i+j].direction == UP){
                            printf("^        ");
                        }else if(nodes[i+j+total_column].direction == DOWN){
                            printf("v        ");
                        }else{
                            printf("         ");
                        }
                    }
                }
                printf("\n");
            }
        }
    }
}

/* Function to Free the space that are claimed and put all the pointer 
   to NULL */
void free_memory(intersection_t *data, coordinate_t *target_list, node_t *nodes, 
                 ptrstorage_t *storage){
    free(nodes);
    free(target_list);
    free(data);
    nodes = NULL;
    data = NULL;
    target_list = NULL;
    storage->data = NULL;
    storage->target_list = NULL;
    storage->nodes = NULL;
}

// Algorithm is fun
