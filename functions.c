#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "header.h"

/*
=================================================================================
Functions
=================================================================================
*/

/*
This function generates a linklist with each node having
a) filename with extension
b) filename without extension
c) qualified path to the storm file
*/
struct fileName* get_file_name_list(char *year) {

    // Windows filnames are restrcted to 255 Characters + 1 for / and 1 for delimiter
    char *qualified_file_path_of_the_data_file = malloc(strlen(year) + 257);
    strcpy(qualified_file_path_of_the_data_file, year);
    strcat(qualified_file_path_of_the_data_file, "/data.txt");

    char *relative_path_to_the_storm_files = malloc(strlen(year) + 257);
    strcpy(relative_path_to_the_storm_files, year);
    strcat(relative_path_to_the_storm_files, "/");

    struct fileName *current_head = NULL;

    //Variables for the getline function
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE *data_file_ptr;
    data_file_ptr = fopen(qualified_file_path_of_the_data_file, "r");
    if (data_file_ptr == NULL) {
        perror("Error: ");
    }

    while ((read = getline(&line, &len, data_file_ptr)) != -1) {
        if (strlen(line) == 2) {
            continue;
        }
        else {
            char *temp_filename_without_extension = strtok(line, ".");
            char *temp_filename_with_extension = (char *)malloc(strlen(temp_filename_without_extension) + 6);
            strcpy(temp_filename_with_extension, temp_filename_without_extension);
            strcat(temp_filename_with_extension, ".txt");
            char *path_to_file =  (char *)malloc(strlen(relative_path_to_the_storm_files) + 260);
            strcpy(path_to_file, relative_path_to_the_storm_files);
            strcat(path_to_file, temp_filename_with_extension);
            current_head = add_storm_file_names_to_list(temp_filename_with_extension, temp_filename_without_extension, path_to_file, current_head);
            free(temp_filename_with_extension);
            free(path_to_file);
        }
    }
    if( line ) {
        free(line);
    }
    fclose(data_file_ptr);
    return current_head;
}


/*
Appends node to the beginning of the list. This is for generating the file name list which is a Singly linked list
*/
struct fileName* add_storm_file_names_to_list(char *with_extension, char *without_extension, char *path_to_storm_file, struct fileName *current_head) {
    // Creating a Link
    struct fileName *link = (struct fileName*) malloc(sizeof(struct fileName));

    link -> filename_with_extension = malloc(strlen(with_extension) + 1);
    link -> filename_without_extension = malloc(strlen(without_extension) + 1);
    link -> file_path = malloc(strlen(path_to_storm_file) + 1);

    strcpy(link -> filename_with_extension, with_extension);
    strcpy(link -> filename_without_extension, without_extension);
    strcpy(link -> file_path, path_to_storm_file);
    // Pointing it to the old first node
    link -> next = current_head;
    // point first to new first node
    current_head = link;
    return current_head;
}

/*
Prints the filelist LinkedList
*/
void print_filename_list(struct fileName* head_of_the_list) {
    struct fileName *ptr = head_of_the_list;
    // Starting from the beginning

    printf("================================================================\n");
    printf("FILE LIST CONTAINING WTHOUT EXT, WITH EXT AND RELATIVE PATH \n");
    printf("================================================================\n");
    while (ptr != NULL) {
        printf("%s, %s, %s \n", ptr->filename_with_extension, ptr-> filename_without_extension, ptr-> file_path);
        ptr = ptr->next;
    }
    ptr = NULL;
}

/*
Displays no of records in each storm file.
*/
void get_number_of_records_for_each_storm(struct fileName* list_of_storms) {
    struct fileName *iter = list_of_storms;
    int no_of_lines = 0;
    printf("================================================================\n");
    printf("NO OF RECORDS IN EACH FILE \n");
    printf("================================================================\n");
    while (iter != NULL) {
        no_of_lines = get_no_of_lines(iter -> file_path);
        printf("%s has %d records \n", iter -> filename_without_extension, no_of_lines);
        iter = iter -> next;
    }
    iter = NULL;
}

/*
Helper function to retrieve no of lines in a file
*/
int get_no_of_lines(char *file_path) {
    FILE *file;
    char ch;
    int lines = 1;
    file = fopen(file_path, "r");
    if ( file == NULL) {
        perror("Error:  ");
        return (-1);
    }
    while (!feof(file)) {
        ch = fgetc(file);
        if(ch == '\n') {
            lines++;
        }
    }
    fclose(file);
    return lines;
}

/*
This function is responsible for generating the whole structure from which the data is parsed.
*/
struct fileName* generate_data_links(struct fileName *storms_list) {

    struct fileName *list_head = storms_list;
    struct fileName *iterator = storms_list;

    int x = 0;

    while(iterator != NULL) {
        //while (x < 1) {
        struct stormDataCell *storm_info_headnode = NULL;

        //Variables for the getline function
        char *row = NULL;
        size_t length = 0;
        ssize_t no;

        FILE *storm_data_file;
        storm_data_file = fopen(iterator->file_path, "r");
        if (storm_data_file == NULL) {
            perror("Error: ");
        }

        while ((no = getline(&row, &length, storm_data_file)) != -1) {
            if (strlen(row) == 2) {
                continue;
            }
            else {
                char *data_row = malloc(strlen(row) + 1);
                strcpy(data_row, row);
                char *trash = strtok(data_row, " ");
                char *lati = strtok(NULL, " ");
                char *longi = strtok(NULL, " ");
                char *dateT = strtok(NULL, " ");
                char *win_spd = strtok(NULL, " ");
                char *pressu = strtok(NULL, " ");
                char *strm_typ = strtok(NULL, " ");
                char *mm = strtok(dateT, "/");
                char *dd = strtok(NULL, "/");
                char *tt = strtok(NULL, "Z");
                struct dateTime dat_tim = {.month = atoi(mm), .day = atoi(dd), .time = atoi(tt)};
                storm_info_headnode = append_storm_data(atof(lati), atof(longi), dat_tim ,atoi(win_spd), atoi(pressu), strm_typ, storm_info_headnode);
                free(data_row);
            }
        }

        if (row) {
            free (row);
        }

        iterator -> stormData = storm_info_headnode;
        fclose(storm_data_file);
        iterator = iterator->next;
    }

    iterator = NULL;
    return list_head;

}

/*
Function is responsible for appending data nodes to the front of a list. This creates a doubly linked list
*/
struct stormDataCell* append_storm_data(float lat, float lon, struct dateTime ts, int ws, int pres, char* st, struct stormDataCell *storm_info_headnode) {

    // Creating a Link
    struct stormDataCell *storm_data = (struct stormDataCell*) malloc(sizeof(struct stormDataCell));
    struct stormDataCell *last_node = NULL;

    storm_data -> storm_type = (char *)malloc(strlen(st) + 3);

    storm_data -> latitude = lat;
    storm_data -> longitude = lon;
    storm_data -> timestamp = ts;
    storm_data -> wind_speed = ws;
    storm_data -> pressure = pres;
    strcpy(storm_data -> storm_type, st);


    if(storm_info_headnode == NULL) {
        // make it the last link
        last_node = storm_data;
    }
    else {
        // update first prev link
        storm_info_headnode -> prev_data = storm_data;
    }

    // point it to the old first link
    storm_data -> next_data = storm_info_headnode;

    // point first to new first link
    storm_info_headnode = storm_data;

    return storm_info_headnode;
}

/*
This is a complex huge function that in one traversal gathers all the information need it also calls the respective print functions to print
the data
*/
void process_data(struct fileName *storms_list) {

    printf("==========================================================================================\n");
    printf("Summary of the Storms\n");
    printf("===========================================================================================\n");

    struct fileName *head_copy = storms_list;
    struct fileName *head_copy_2 = storms_list;
    struct fileName *storms_looper = storms_list;
    int monthly_storm_table[6][31];
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 31; j++)
        {
            monthly_storm_table[i][j] = 0;
        }
    }
    int no_storms = no_of_storms(head_copy);
    int wind_speed_record[no_storms][20];
    for (int i = 0; i < no_storms; i++) {
        for (int j = 0; j < 21; j++)
        {
            wind_speed_record[i][j] = 0;
        }
    }

    int lat_vs_long[70][30];
    for (int i = 0; i < 70; i++) {
        for (int j = 0; j < 30; j++)
        {
            lat_vs_long[i][j] = 0;
        }
    }


    int record_row = 1;

    while (storms_looper != NULL) {
        char *storm_name = storms_looper -> filename_without_extension;
        int storm_lvl = 0;
        int start_month = 0;
        int start_day = 0;
        int start_time = 0;
        int end_month = 0;
        int end_day = 0;
        int end_time = 0;
        float min_lat = storms_looper -> stormData -> latitude;
        float max_lat = storms_looper -> stormData -> latitude;
        float min_lon = storms_looper -> stormData -> longitude;
        float max_lon = storms_looper -> stormData -> longitude;
        int max_wind_speed = 0;
        int max_wind_speed_month = 0;
        int max_wind_speed_day = 0;
        int max_wind_speed_time = 0;
        char *storm_type_words = "";
        struct stormDataCell *records_looper = storms_looper -> stormData;

        while(records_looper != NULL) {
            storm_lvl = storm_level(records_looper -> storm_type);
            if (records_looper -> next_data == NULL) {
                start_month = (records_looper -> timestamp).month;
                start_day = (records_looper -> timestamp).day;
                start_time = (records_looper -> timestamp).time;
            }
            if (records_looper -> prev_data == NULL) {
                end_month = (records_looper -> timestamp).month;
                end_day = (records_looper -> timestamp).day;
                end_time = (records_looper -> timestamp).time;
            }
            if (min_lat > records_looper -> latitude) {
                min_lat = records_looper -> latitude;
            }
            if (max_lat < records_looper -> latitude) {
                max_lat = records_looper -> latitude;
            }
            if (min_lon > records_looper -> longitude) {
                min_lon = records_looper -> longitude;
            }
            if (max_lon < records_looper -> longitude) {
                max_lon = records_looper -> longitude;
            }
            if (max_wind_speed < records_looper -> wind_speed) {
                max_wind_speed = records_looper -> wind_speed;
                max_wind_speed_month = (records_looper -> timestamp).month;
                max_wind_speed_day = (records_looper -> timestamp).day;
                max_wind_speed_time = (records_looper -> timestamp).time;
            }

            if ((records_looper -> timestamp).month > 5 && (records_looper -> timestamp).month < 12) {
                int temp_mm =  return_month_index((records_looper -> timestamp).month) - 1;
                int temp_dd =  (records_looper -> timestamp).day - 1;
                monthly_storm_table[temp_mm][temp_dd]++;
            }
            wind_speed_record[record_row - 1][storm_wind_speed_level(records_looper -> wind_speed) -1]++;

            //printf("%d, %d\n", (int)(records_looper -> longitude), (int)(records_looper -> latitude));
            if (abs((int)(records_looper -> longitude)) > 30 && abs((int)(records_looper -> longitude)) < 101) {
                if (abs((int)(records_looper -> latitude)) > 10 && abs((int)(records_looper -> latitude)) < 41) {
                    int temp_lat = abs(((int)(records_looper -> latitude))) - 10 - 1;
                    int temp_long = abs(((int)(records_looper -> longitude))) - 30 - 1;
                    //printf("%d, %d\n", temp_long, temp_lat);
                    lat_vs_long[temp_long][temp_lat]++;
                }
            }

            records_looper = records_looper -> next_data;
        }
        if (storm_lvl == 1) {
            storm_type_words = "Tropical Depression";
        }
        else if (storm_lvl == 2) {
            storm_type_words = "Tropical Storm";
        }
        else {
            storm_type_words = "Hurricane";
        }

        print_summary_to_file(storm_name, storm_type_words, start_month, start_day, start_time, end_month, end_day, end_time, min_lat, max_lat,
                              min_lon, max_lon, max_wind_speed, max_wind_speed_month, max_wind_speed_day, max_wind_speed_time);
        record_row++;
        storms_looper = storms_looper -> next;
    }
    printf("\n");
    print_statistics_table(monthly_storm_table, wind_speed_record, head_copy_2, lat_vs_long);

}


/*
This function is responsible for printing the summary of each storm file
*/
void print_summary_to_file(char* storm_name, char* storm_type, int start_mm, int start_dd, int start_time, int end_mm, int end_dd, int end_time, float min_lat,
                           float max_lat, float min_lon, float max_lon, int max_wind_spd, int max_wind_spd_mm, int max_wind_spd_dd, int max_wind_spd_time) {

    printf("*****************************************************************************************\n");
    printf("Name: %s \n", storm_name);
    printf("Type: %s \n", storm_type);
    printf("Start: %d/%d at %d:00 hours, End: %d/%d at %d:00 hours \n", start_mm, start_dd, start_time, end_mm, end_dd, end_time);
    printf("Range: Latitude: %f to %f; Longitude: %f to %f \n", min_lat, max_lat, min_lon, max_lon);
    printf("Maximum sustained winds: %d MPH on %d/%d at %d:00 hours \n", max_wind_spd, max_wind_spd_mm, max_wind_spd_dd, max_wind_spd_time);
    printf("*****************************************************************************************\n");
}


/*
This giant function performs pointer arithematic to print statistics
*/
void print_statistics_table(int monthly_storm_table[6][31], int wind_speed_record[][20], struct fileName* list_of_storms, int lat_vs_long[70][30]) {

    struct fileName* copy_of_list = list_of_storms;
    struct fileName* copy_of_list_2 = list_of_storms;

    printf("==========================================================================================\n");
    printf("Monthly Totals for typical Hurricane season from June 1 to November 30 (Month Vs Day) \n");
    printf("===========================================================================================\n");


    // Define a pointer to walk the rows of the 2D array.
    int (*p1)[31] = monthly_storm_table;

    // Define a pointer to walk the columns of each row of the 2D array.
    int *p2 = NULL;

    printf("         |");
    for (int i = 1; i <=31; i++) {
        printf("%d\t", i);
    }
    printf("\n");
    // There are three rows in the 2D array.
    // p1 has been initialized to point to the first row of the 2D array.
    // Make sure the iteration stops after the third row of the 2D array.
    for (int iter = 1; p1 != monthly_storm_table+6; ++p1, iter++) {
        printf("----------------------------------------------------------------");
        printf("----------------------------------------------------------------");
        printf("----------------------------------------------------------------");
        printf("-------------------------------------------------------------\n");
        if (iter == 1) {
            printf("June     |");
        }
        else if (iter == 2) {
            printf("July     |");
        }
        else if (iter == 3) {
            printf("August   |");
        }
        else if (iter == 4) {
            printf("September|");
        }
        else if (iter == 5) {
            printf("October  |");
        }
        else {
            printf("November |");
        }
        // Iterate over each column of the arrays.
        // p2 is initialized to *p1, which points to the first column.
        // Iteration must stop after two columns. Hence, the breaking
        // condition of the loop is when p2 == *p1+2
        for (p2 = *p1; p2 != *p1+31; ++p2) {
            printf("%d\t", *p2);
        }
        printf("\n");
    }
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");

    printf("================================================================\n");
    printf("Sustained Winds (Storm Name Vs Windspeed) \n");
    printf("================================================================\n");

    // Define a pointer to walk the rows of the 2D array.
    int (*ptr1)[20] = wind_speed_record;

    // Define a pointer to walk the columns of each row of the 2D array.
    int *ptr2 = NULL;

    //printf("         |");
    for (int i = 1; i <=20; i++) {
        printf("%d\t", i);
    }
    printf("\n");
    // There are three rows in the 2D array.
    // p1 has been initialized to point to the first row of the 2D array.
    // Make sure the iteration stops after the third row of the 2D array.
    for (; ptr1 != wind_speed_record+no_of_storms(copy_of_list_2); ++ptr1) {
        printf("----------------------------------------------------------------");
        printf("----------------------------------------------------------------");
        printf("-------------------------------------------------------------\n");

        // Iterate over each column of the arrays.
        // p2 is initialized to *p1, which points to the first column.
        // Iteration must stop after two columns. Hence, the breaking
        // condition of the loop is when p2 == *p1+2
        for (ptr2 = *ptr1; ptr2 != *ptr1+20; ++ptr2) {
            printf("%d\t", *ptr2);
        }
        printf("%s", copy_of_list -> filename_without_extension);

        printf("\n");
        copy_of_list = copy_of_list -> next;
    }

    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");


    printf("================================================================\n");
    printf("Latitude Longitude Totals (Longitude Vs Latitude) \n");
    printf("================================================================\n");


    // Define a pointer to walk the rows of the 2D array.
    int (*ptr11)[30] = lat_vs_long;

    // Define a pointer to walk the columns of each row of the 2D array.
    int *ptr22 = NULL;

    printf("         |");
    for (int i = 11; i <=40; i++) {
        printf("%d\t", i);
    }
    printf("\n");
    // There are three rows in the 2D array.
    // p1 has been initialized to point to the first row of the 2D array.
    // Make sure the iteration stops after the third row of the 2D array.
    for (int iter=31; ptr11 != lat_vs_long+70; ++ptr11, iter++) {
        printf("----------------------------------------------------------------");
        printf("----------------------------------------------------------------");
        printf("----------------------------------------------------------------");
        printf("-------------------------------------------------------------\n");
        printf("%d     |", iter);
        // Iterate over each column of the arrays.
        // p2 is initialized to *p1, which points to the first column.
        // Iteration must stop after two columns. Hence, the breaking
        // condition of the loop is when p2 == *p1+2
        for (ptr22 = *ptr11; ptr22 != *ptr11+30; ++ptr22) {
            printf("%d\t", *ptr22);
        }
        printf("\n");
    }
}


/*
Helper Function
*/

int storm_level(char* storm_type_in_words) {
    int x = 0;
    if (strcmp(storm_type_in_words, "TROPICAL-DEPRESSION")) {
        x = 1;
    }
    else if (strcmp(storm_type_in_words, "TROPICAL-STORM")) {
        x = 2;
    }
    else {
        x = 3;
    }
    return x;
}

/*
Helper Function
*/
int no_of_storms(struct fileName* list_of_storms) {
    int i = 0;
    while (list_of_storms != NULL) {
        i++;
        list_of_storms = list_of_storms -> next;
    }
    return i;
}

/*
Helper Function
*/
int storm_wind_speed_level(int wind_speed) {
    int var = 0;
    if (wind_speed >= 0 && wind_speed <=9) {
        var = 1;
    }
    else if (wind_speed >= 10 && wind_speed <= 19) {
        var = 2;
    }
    else if (wind_speed >= 20 && wind_speed <= 29) {
        var = 3;
    }
    else if (wind_speed >= 30 && wind_speed <= 39) {
        var = 4;
    }
    else if (wind_speed >= 40 && wind_speed <= 49) {
        var = 5;
    }
    else if (wind_speed >= 50 && wind_speed <= 59) {
        var = 6;
    }
    else if (wind_speed >= 60 && wind_speed <= 69) {
        var = 7;
    }
    else if (wind_speed >= 70 && wind_speed <= 79) {
        var = 8;
    }
    else if (wind_speed >= 80 && wind_speed <= 89) {
        var = 9;
    }
    else if (wind_speed >= 90 && wind_speed <= 99) {
        var = 10;
    }
    else if (wind_speed >= 100 && wind_speed <= 109) {
        var = 11;
    }
    else if (wind_speed >= 110 && wind_speed <= 119) {
        var = 12;
    }
    else if (wind_speed >= 120 && wind_speed <= 129) {
        var = 13;
    }
    else if (wind_speed >= 130 && wind_speed <= 139) {
        var = 14;
    }
    else if (wind_speed >= 140 && wind_speed <= 149) {
        var = 15;
    }
    else if (wind_speed >= 150 && wind_speed <= 159) {
        var = 16;
    }
    else if (wind_speed >= 160 && wind_speed <= 169) {
        var = 17;
    }
    else if (wind_speed >= 170 && wind_speed <= 179) {
        var = 18;
    }
    else if (wind_speed >= 180 && wind_speed <= 189) {
        var = 19;
    }
    else {
        var = 20;
    }
    return var;
}

/*
Helper Function
*/
int return_month_index(int month) {
    int var = 0;
    if (month == 6) {
        var = 1;
    }
    else if(month == 7) {
        var = 2;
    }
    else if (month == 8) {
        var = 3;
    }
    else if (month == 9) {
        var = 4;
    }
    else if (month == 10) {
        var = 5;
    }
    else {
        var = 6;
    }
    return var;
}


/*
Cleans up the list and frees the memory
*/
void clean_up(struct fileName* list) {

    struct fileName* head = list;

    while (list !=  NULL) {
        deleteList_sub(list -> stormData);
        list = list -> next;
    }

    deleteList_main(head);

}

/* Function to delete the entire linked list */
void deleteList_sub(struct stormDataCell* head)
{

    struct stormDataCell* current = head;
    struct stormDataCell* next_data;

    while (current != NULL)
    {
        next_data = current->next_data;
        free(current);
        current = next_data;
    }

    head = NULL;
}

/* Function to delete the entire linked list */
void deleteList_main(struct fileName* head)
{

    struct fileName* current = head;
    struct fileName* next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    head = NULL;
}