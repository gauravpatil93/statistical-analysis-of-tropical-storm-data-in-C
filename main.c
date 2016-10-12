#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "header.h"
/*
=================================================================================
Main()
=================================================================================
*/

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Pass in a valid year\n");
    }
    else {
        for(int i=1; i<argc; i++) {
            struct fileName* list_of_filenames = get_file_name_list(argv[i]);
            get_number_of_records_for_each_storm(list_of_filenames);
            struct fileName* list = generate_data_links(list_of_filenames);
            print_filename_list(list);
            process_data(list);
            clean_up(list);
        }
    }
    return (0);
}