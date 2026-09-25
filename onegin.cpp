#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

struct str_info_t {
    char* ptr_on_str;
    size_t len_str;
};

typedef enum {

    EXIT_REASON_FILE_READING_ERROR = 1,
    EXIT_REASON_EMPTY_FILE =         2,
    EXIT_REASON_NO_STR_IN_FILE =     3,
    EXIT_REASON_UNKNOWN_COMPORATOR = 4,       

} exit_reason_t;

#define PINK   "\033[95m"
#define RESET  "\033[37m"

//work with files
char* read_from_file(FILE* name_file, size_t* str_number, off_t size_of_file_in);
int print_strs_in_file(FILE* name_file, str_info_t* array_of_strs_info, size_t str_number, 
                       int (*compare_function)(void* str1, void* str2));
int print_strs_in_console(str_info_t* array_of_strs_info,  size_t str_number,
                          int (*compare_function)(void* str1, void* str2));
void print_sorting_mode(FILE* name_file, int (*compare_function)(void* str1, void* str2));
off_t size_of_file(const char* name_file);
str_info_t* create_array_of_structs(size_t str_number, char* buffer, off_t size_of_file_in);
void open_file_and_clean(const char* name_file);
//compare strs
int compare_two_str_from_begin(void* first_argument, void* second_argument);
int compare_two_str_from_end(void* first_argument, void* second_argument);
int compare_ptr_on_str_up(void* first_adress, void* second_adress);
void skip_not_letters_up(char* str, size_t* index_str, int* flag_end_string);
int skip_not_letters_down(char* str, size_t* index_str, int* flag_end_string);
//sort array
void quick_sort(void* left, void* right, size_t size,  int (*comp)(void* first_arg, void* second_arg));
void swap_numbers(char* value1, char* value2, size_t size);
size_t index_of_pivot(void* left, void* right, size_t size);

int main(int argc, char* argv[]) {

    const char* input_file =  NULL;
    const char* output_file = NULL;
    if (argc == 3) {
        input_file =  argv[1];  //"Onegin_text_in_english.txt"; 
        output_file = argv[2];  //"output_onegin_sorted_up.txt";
    }
    else {
        perror("Can not open the file: No such file");
        exit(EXIT_REASON_FILE_READING_ERROR);
    }

    FILE* fp_input = fopen(input_file, "rb");
    off_t size_of_file_in = size_of_file(input_file);
    size_t str_number = 0; 
    char* buffer = read_from_file(fp_input, &str_number, size_of_file_in);
    str_info_t* array_of_strs_info = create_array_of_structs(str_number, buffer, size_of_file_in);
     
    open_file_and_clean(output_file);
    FILE* fp_output = fopen(output_file, "a");

    int (*array_ptr_on_functions[])(void* str1, void* str2) = {compare_two_str_from_begin, compare_two_str_from_end, compare_ptr_on_str_up};
    size_t size_of_array_ptr_on_functions = (sizeof(array_ptr_on_functions) / sizeof(array_ptr_on_functions)[0]);
    for (size_t i = 0; i < size_of_array_ptr_on_functions; i++) {
        print_strs_in_file(fp_output, array_of_strs_info, str_number, *array_ptr_on_functions[i]);
    }

    fclose(fp_input);
    fclose(fp_output);

    free(buffer);
    free(array_of_strs_info);

    return 0;
}

char* read_from_file(FILE* name_file, size_t* str_number, off_t size_of_file_in) {
    
    assert(name_file != NULL);
    assert(str_number != NULL);

    char* buffer = (char*)calloc((size_t)size_of_file_in + 1, sizeof(char)); //+1 to add /0 in the end

    fread(buffer, sizeof(char), (size_t)size_of_file_in, name_file);
    buffer[size_of_file_in] = '\0';

    for (off_t i = 0; i < size_of_file_in + 1; i++) {
        if (buffer[i] == '\n') {
            (*str_number)++;
        }
    }
    (*str_number)++; //last str
    
    return buffer;  
}

int print_strs_in_file(FILE* name_file, str_info_t* array_of_strs_info, size_t str_number, 
                       int (*compare_function)(void* str1, void* str2)) {

    assert(name_file != NULL);
    assert(array_of_strs_info != NULL);

    quick_sort(array_of_strs_info, array_of_strs_info + str_number - 1, sizeof(str_info_t), compare_function);

    if (str_number == 0) {
        perror("No str!");
        exit(EXIT_REASON_NO_STR_IN_FILE);
    }

    print_sorting_mode(name_file, compare_function);

    for(size_t i = 0; i < str_number; i++) {
        if (array_of_strs_info[i].len_str - 1 != 0) { //to skip empty strs
            fprintf(name_file, "%s\n", array_of_strs_info[i].ptr_on_str);
        }
    }
    fprintf(name_file, "TOTAL STR AMOUNT - %zu\n", str_number);
    fprintf(name_file, "\n\nEND OF FILE\n------------------------------------------------------\n\n");;

    return 0;
}

// int print_strs_in_console(str_info_t* array_of_strs_info, size_t str_number, int (*compare_function)(void* str1, void* str2)) {

//     assert(array_of_strs_info != NULL);

//     quick_sort(array_of_strs_info, array_of_strs_info + str_number - 1, sizeof(str_info_t), compare_function);

//     if (str_number == 0) {
//         perror("No str!");
//         exit(EXIT_REASON_NO_STR_IN_FILE);
//     }

//     print_sorting_mode(stdout, compare_function);
//     for (size_t i = 0; i < str_number; i++) {
//         if (array_of_strs_info[i].len_str - 1 != 0) {
//             printf(PINK "STR_NUMBER = %zu LENGTH = %zu\n" RESET, i + 1, array_of_strs_info[i].len_str - 1); // -1 not to count \r
//             printf("%s\n", array_of_strs_info[i].ptr_on_str);   
//         }
//     }
//     printf(PINK "TOTAL STR AMOUNT - %zu \n" RESET, str_number);

//     return 0;
// }

int compare_two_str_from_begin(void* first_argument, void* second_argument) {

    char* str1 = ((str_info_t*)first_argument) -> ptr_on_str;
    char* str2 = ((str_info_t*)second_argument) -> ptr_on_str; 
    
    assert(str1 != NULL);
    assert(str2 != NULL);

    size_t index_str1 = 0;
    size_t index_str2 = 0;
        
    int flag_end_string = 0;

    int symbol1_lower = 0;
    int symbol2_lower = 0;

    while (!flag_end_string) {

        skip_not_letters_up(str1, &index_str1, &flag_end_string);
        skip_not_letters_up(str2, &index_str2, &flag_end_string);

        symbol1_lower = tolower(str1[index_str1]);
        symbol2_lower = tolower(str2[index_str2]);

        if (!flag_end_string) {
            if (symbol1_lower != symbol2_lower) {
                return symbol1_lower - symbol2_lower;
            }
            index_str1++;
            index_str2++; 
        }
    }

    return (str1[index_str1] - str2[index_str2]);
}

void skip_not_letters_up(char* str, size_t* index_str, int* flag_end_string) {
    
    assert(str != NULL);
    assert(index_str != NULL);
    assert(flag_end_string != NULL);

    while (isalpha(str[*index_str]) == 0) {
        if (str[*index_str] != '\0') { 
            (*index_str)++;
        }
        else {
            *flag_end_string = 1;
            break;
        }
    }
}

int compare_two_str_from_end(void* first_argument, void* second_argument) {

    char* str1 = ((str_info_t*)first_argument)->ptr_on_str;
    char* str2 = ((str_info_t*)second_argument)->ptr_on_str;

    assert(str1 != NULL);
    assert(str2 != NULL);

    size_t max_index_str1 = ((str_info_t*)first_argument)->len_str;
    size_t max_index_str2 = ((str_info_t*)second_argument)->len_str;
        
    int flag1_end_string = 1;
    int flag2_end_string = 1;

    int symbol1_lower = 0;
    int symbol2_lower = 0;

    while (flag1_end_string == 1 && flag2_end_string == 1) {

        skip_not_letters_down(str1, &max_index_str1, &flag1_end_string);
        skip_not_letters_down(str2, &max_index_str2, &flag2_end_string);

        if (flag1_end_string == 1 && flag2_end_string == 1) {

            symbol1_lower = tolower(str1[max_index_str1]);
            symbol2_lower = tolower(str2[max_index_str2]);

            if (symbol1_lower != symbol2_lower) {
                return symbol1_lower - symbol2_lower;
            }

            if (max_index_str1 > 0) {
                max_index_str1--;
            } 
            else {
                flag1_end_string = 0;
            }

            if (max_index_str2 > 0) {
                max_index_str2--;
            }
            else {
                flag2_end_string = 0;  
            }

            assert(max_index_str1 < 10000);  //SAVED!
            assert(max_index_str2 < 10000); //SAVED!
        }
    }

    if (flag1_end_string == 1) {
        skip_not_letters_down(str1, &max_index_str1, &flag1_end_string);
    }
    if (flag2_end_string == 1) {
        skip_not_letters_down(str2, &max_index_str2, &flag2_end_string);
    }

    if (flag1_end_string == flag2_end_string) { // 0|0 
        return 0;
    }
    else if (flag1_end_string == 0) {
        return -1; 
    }
    
    return 1;
}

int skip_not_letters_down(char* str, size_t* index_str, int* flag_end_string) {

    assert(str != NULL);
    assert(index_str != NULL);
    assert(flag_end_string != NULL);

    while (isalpha(str[*index_str]) == 0 &&  *index_str > 0) { 
            (*index_str)--;
    }

    if (*index_str == 0) {
        if (isalpha(str[*index_str]) == 0) {
            *flag_end_string = 0;
            return 0;
        }
    }
    return str[*index_str];   
}

int compare_ptr_on_str_up(void* first_adress, void* second_adress) {
    
    assert(first_adress != NULL);
    assert(second_adress != NULL);

    char *first_ptr = *(char **)first_adress;
    char *second_ptr = *(char **)second_adress;

    return (int)((char*)first_ptr - (char*)second_ptr);
}

void quick_sort(void* left, void* right, size_t size,  int (*comp)(void* first_arg, void* second_arg)) {
    
    if ((char*)left >= (char*)right) {
        return;
    }

    size_t size_of_array = ((char*)right - (char*)left) / size + 1;
    size_t pivot_index = index_of_pivot(left, right, size);
    assert(pivot_index >= 0 && pivot_index < size_of_array); 
    
    char* pivot = (char*)left + pivot_index * size;
    assert((char *) left <= pivot && pivot <= (char *) right);

    swap_numbers((char*)left, pivot, size);
    void* last = left;

    for (size_t i = 1; i < size_of_array; i++) { // check
        if (comp(((char*)left + i * size), left) < 0) {//data[i] < data[left]) {
            last = (char*)last + size;
            swap_numbers((char*)left + i * size, (char*)last, size);
        }
    }

    swap_numbers((char*)left, (char*)last, size);

    quick_sort((char*)last + size, right, size, comp);
    quick_sort(left, (char*)last - size, size,  comp);

}

void swap_numbers(char* value1, char* value2, size_t size) {

    for(size_t i = 0; i < size; i++) {
        char temp = *(value1 + i);
        *(value1 + i) = *(value2 + i);
        *(value2 + i) = temp;
    }
}

size_t index_of_pivot(void* left, void* right, size_t size) {
    return (((char*)right - (char*)left) / size  + 1) / 2; 
}

off_t size_of_file(const char* name_file) {

    struct stat stat_file_in;

    if (stat(name_file, &stat_file_in) == -1) {
        perror("stat failed");  // Обрабатываем ошибку
        exit(EXIT_REASON_EMPTY_FILE);
    }
    return stat_file_in.st_size;
}

 str_info_t* create_array_of_structs(size_t str_number, char* buffer, off_t size_of_file_in) {

    assert(buffer != NULL);
    
    str_info_t* array_of_structs = (str_info_t*)calloc(str_number, sizeof(str_info_t));

    size_t curr_i_array_of_structs = 0;
    
    array_of_structs[curr_i_array_of_structs].ptr_on_str = buffer;

    for (off_t i = 0; i < size_of_file_in + 1; i++) {
        if (buffer[i] == '\n') {
            buffer[i] = '\0';
            curr_i_array_of_structs++;
            array_of_structs[curr_i_array_of_structs].ptr_on_str = buffer + i + 1; //Next adress after \0
            array_of_structs[curr_i_array_of_structs - 1].len_str = (size_t)(buffer + i - array_of_structs[curr_i_array_of_structs - 1].ptr_on_str); //change previous len knowing current index of current str
        }
    } 
    array_of_structs[curr_i_array_of_structs].len_str = strlen(array_of_structs[curr_i_array_of_structs].ptr_on_str);

    return array_of_structs;
}

void open_file_and_clean(const char* name_file) {

    assert(name_file != NULL);

    FILE* open_file = fopen(name_file, "w");
    fclose(open_file);
}

void print_sorting_mode(FILE* name_file, int (*compare_function)(void* str1, void* str2)) {
    if (compare_function == compare_two_str_from_begin) {
        fprintf(name_file, "SORTING THE TEXT IN ASCENDING ORDER\n\n");
    }
    else if (compare_function == compare_two_str_from_end) {
        fprintf(name_file, "SORTING THE TEXT IN DESCENDING ORDER\n\n");
    }
    else if (compare_function == compare_ptr_on_str_up) {
        fprintf(name_file, "THE USUAL TEXT\n\n");
    }
    else {
        perror("Unknown comporator");  // Обрабатываем ошибку
        exit(EXIT_REASON_UNKNOWN_COMPORATOR);
    }
}