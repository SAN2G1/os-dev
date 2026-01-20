#ifndef DEBUG_H
#define DEBUG_H



#define LOG_LEVEL_COUNT 3 

typedef enum {
    DEBUG, 
    INFO,
    ERROR
} log_level;

extern const char* log_level_str[LOG_LEVEL_COUNT] ;

void serial_set_log_level(log_level lvl); 
void serial_log(log_level level, const char* tag ,const char* text); 

#endif 