#define MAX_TITLE 128
#define MAX_EXERCISES 128
#define MAX_SETS 128
#define MAX_EXERCISE_SETTINGS 128
#define DATETIME_LENGTH 20
#define MAX_EXERCISE_LIST_DISPLAY_COUNT 6
#define EXERCISE_LIST_BLOCK 5
#define SET_LIST_BLOCK 3

typedef unsigned short Weight;
typedef unsigned long  Size;
typedef unsigned long  Duration;
typedef unsigned short SettingValue;

#define DEFAULT_REST_DURATION 30;

enum WorkoutStates {
    WS_SELECT,
    WS_EXERSICE_SELECT,
    WS_SET_REST,
    WS_SET_NEXT,
    WS_COMPLETED
};
typedef enum WorkoutStates WorkoutState;

struct Setting {
    char title[MAX_TITLE];
    SettingValue value;
};
typedef struct Setting Setting;

struct Set {
    Size   count;
    Weight weight;
};
typedef struct Set Set;

struct Exercise {
    char     title[MAX_TITLE];
    Size     setting_size_capacity;
    Size     setting_size;
    Size     scheduled_set_capacity;
    Size     scheduled_set_size;
    Size     actual_set_capacity;
    Size     actual_set_size;
    Duration scheduled_duration;
    Setting* settings;
    Set*     scheduled_sets;
    Set*     actual_sets;
};
typedef struct Exercise Exercise;

struct Workout {
    char         title[MAX_TITLE];
    char         started[DATETIME_LENGTH];
    char         ended[DATETIME_LENGTH];
    Exercise     *exercises;
    WorkoutState state;
    Size exercises_capacity;
    Size exercises_size;
    Size current_exercise;
    Size viewing_exercise;
    Size current_set;
};
typedef struct Workout Workout;

void workout_init(Workout** workout, char* title);
void workout_load(Workout* workout);
void workout_free(Workout** workout);

Exercise* add_workout_exercise(Workout* workout, char* title);
Setting* add_exercise_setting(Exercise* exercise, char* title, SettingValue value);
Set* add_scheduled_set(Exercise* exercise, Size count, Weight weight);
Set* add_actual_set(Exercise* exercise, Size count, Weight weight);