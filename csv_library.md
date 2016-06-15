# csv library

The CSV file is a file format that separates the values with the commas. This is mainly used in the test programs, validation programs, logging etc to be able to view and analyse the data over an Excel sheet. The Excel supports the CSV file format to view and write.

We are going to start our topic about the reading / writing of a CSV file in this section. The CSV file handling has many practical uses and some are already stated above.

A CSV file looks like the following:

    variable1,variable2,variable3,variable4
    
each element in the csv file is a column and is separated by a comma. Thus our parser here should depend on the ```,``` to parse each variable and extract the data or the variable corresponding to the row and the column. The CSV file would contain many of such lines or columns in a file. Each line inturn is called row.


### Reading From a CSV File



Parsing of a line that is in CSV format can be done using a small API as follows.

    int _csv_get_row_col(char *input, char *key, int off)
    {
        int i = 0, j = 0;
        
        for (i = off; (input[i] != '\0') && (input[i] != ','); i ++) {
            key[j] = input[i];
            j++;
        }
        
        // encountered either ',' or '\0'
        key[j] = '\0';
        
        return j;
    }
    
The API gets an element from the given row and column into the ```key``` argument.

Now, the function can be called in a loop as:

    void csv_read_record(char *record, int reclen)
    {
        int off = 0;
        
        do {
            char key[100];
            
            off += _csv_get_row_col(record, key, off);
        } while (off < reclen);
    }
    
The above function is called after reading a line from the file.

The key then contains the variable. The variable is then converted to integer, or double or unsigned integer etc.



### Writing into CSV File

Writing into the CSV file is nothing but adding the following

At every variable we write as a column to the CSV file,

1. Add new line at the end if the variable is at the end of the record. A record is a complete row of the csv.
2. Add a ' , ' immediately after writing the variable into the csv file if the variable is not the end of the record.

But, the write function must perform printing into the file with a specific pattern or format specifiers.

To accomplish this we define variable types and a void pointer containing the value's address.

The variable type defines us the type of the variable it is so that we can use a correct format specifier to print the data into the file.

Lastly we need to know if the variable is at the end of record. To do this we pass an argument to the function that tell us if it is at the end of function.

The below function would perform the above logic.

    // variable types and definitions
    typedef enum {
        CSV_VAR_TYPE_INT,
        CSV_VAR_TYPE_UINT,
        CSV_VAR_TYPE_DOUBLE,
        CSV_VAR_TYPE_STRING,
    } csv_vartype_t;
    
    // write one column into the file
    // 1. add new line if end_of_record is set
    // 2. add a ',' if end_of_record is not set
    int csv_write_record(FILE *csv_fp, void *var, csv_vartype_t vartype, int end_of_record)
    {
        int *int_var;
        uint32_t *uint_var;
        double *double_var;
        char *str_var;
        
        switch (vartype) {
            case CSV_VAR_TYPE_INT: {
                int_var = var;
                if (end_of_record) {
                    fprintf(csv_fp, "%d\n", *int_var);
                } else {
                    fprintf(csv_fp, "%d,", *int_var);
                }
            } break;
            case CSV_VAR_TYPE_UINT: {
                uint_var = var;
                if (end_of_record) {
                    fprintf(csv_fp, "%u\n", *uint_var);
                } else {
                    fprintf(csv_fp, "%u,", *uint_var);
                }
            } break;
            case CSV_VAR_TYPE_DOUBLE: {
                double_var = var;
                if (end_of_record) {
                    fprintf(csv_fp, "%f\n", *double_var);
                } else {
                    fprintf(csv_fp, "%f,", *double_var);
                }
            } break;
            case CSV_VAR_TYPE_STRING: {
                char *str_var = var;
                if (end_of_record) {
                    fprintf(csv_fp, "%s\n", str_var);
                } else {
                    fprintf(csv_fp, "%s,", str_var);
                }
            } break;
            default: {
                fprintf(stderr, "cannot know the format specifier for this type %d\n", vartype);
                return -1;
            }
        }
        
        return 0;
    }

The above function is called in a loop for each row similar to the `csv_read_record` API.