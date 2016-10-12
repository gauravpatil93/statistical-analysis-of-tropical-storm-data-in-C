/*
=================================================================================
Structures
=================================================================================
*/

struct dateTime {
    int month;
    int day;
    int time;
};

struct stormDataCell {
    float latitude;
    float longitude;
    struct dateTime timestamp;
    int wind_speed;
    int pressure;
    char* storm_type;

    struct stormDataCell *next_data;
    struct stormDataCell *prev_data;
};

struct fileName {
    char *filename_with_extension;
    char *filename_without_extension;
    char *file_path;

    struct fileName *next;
    struct stormDataCell *stormData;
};

/*
=================================================================================
Function Prototypes
=================================================================================
*/

struct fileName* get_file_name_list(char *year);
struct fileName* add_storm_file_names_to_list(char *with_extension, char *without_extension, char *path_to_storm_file, struct fileName *current_head);
void print_filename_list(struct fileName* head_of_the_list);
void get_number_of_records_for_each_storm(struct fileName *list_of_storms);
int get_no_of_lines(char *file_path);
struct fileName* generate_data_links(struct fileName *storms_list);
struct stormDataCell* append_storm_data(float lat, float lon, struct dateTime ts, int ws, int pres, char* st, struct stormDataCell *storm_info_headnode);
void process_data(struct fileName *storms_list);
void print_summary_to_file(char* storm_name, char* storm_type, int start_mm, int start_dd, int start_time, int end_mm, int end_dd, int end_time, float min_lat,
                           float max_lat, float min_lon, float max_lon, int max_wind_spd, int max_wind_spd_mm, int max_wind_spd_dd, int max_wind_spd_time);
int storm_level(char* storm_type_in_words);
int no_of_storms(struct fileName* list_of_storms);
int storm_wind_speed_level(int wind_speed);
int return_month_index(int month);
void print_statistics_table(int monthly_storm_table[6][31], int wind_speed_record[][20], struct fileName* list_of_storms, int lat_vs_long[70][30]);
void clean_up(struct fileName* list);
void deleteList_sub(struct stormDataCell* head);
void deleteList_main(struct fileName* head);