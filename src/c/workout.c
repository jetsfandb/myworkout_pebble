#include <malloc.h>
#include <string.h>
#include "workout.h"

void workout_init(Workout** workout, char* title) {
    Workout* w = (Workout *) malloc(sizeof(Workout));
    strncpy(w->title, title, sizeof(w->title) -1);
    w->exercises_capacity = EXERCISE_LIST_BLOCK;
    w->exercises_size = w->current_exercise = w->viewing_exercise = w->current_set = 0;
    w->state = WS_EXERSICE_SELECT;
    w->exercises = (Exercise *) malloc(w->exercises_capacity * sizeof(Exercise));
    for(Size e = 0; e < w->exercises_capacity; e++) {
        w->exercises[e].setting_size = 0;
        w->exercises[e].actual_set_size = 0;
        w->exercises[e].scheduled_set_size = 0;
        w->exercises[e].setting_size_capacity = SET_LIST_BLOCK;
        w->exercises[e].settings = (Setting*) malloc(sizeof(Setting) * w->exercises[e].setting_size_capacity);
        w->exercises[e].scheduled_set_capacity = SET_LIST_BLOCK;
        w->exercises[e].scheduled_sets = (Set*) malloc(sizeof(Set) * w->exercises[e].scheduled_set_capacity);
        w->exercises[e].actual_set_capacity = SET_LIST_BLOCK;
        w->exercises[e].actual_sets = (Set*) malloc(sizeof(Set) * w->exercises[e].actual_set_capacity);
    }    
    *workout = w;
}

void workout_load(Workout* workout) {
    //TODO: Get workout data from persistent storage
    Exercise* exercise = 0;
    strcpy(workout->title, "My workout");
    strcpy(workout->started, "10/11/2025 10:30:13");

    exercise = add_workout_exercise(workout, "Deltoid Butterfly");
    for(Size i = 0; i < 3; i++) {
        add_scheduled_set(exercise, 8, 11);
    }

    exercise = add_workout_exercise(workout, "Chest Press");
    add_exercise_setting(exercise, "Seat", 3);
    for(Size i = 0; i < 3; i++) {
        add_scheduled_set(exercise, 8, 7);
    }
    exercise = add_workout_exercise(workout, "Chest Raise");
    add_exercise_setting(exercise, "Seat", 5);
    for(Size i = 0; i < 3; i++) {
        add_scheduled_set(exercise, 8, 8);
    }
    exercise = add_workout_exercise(workout, "Bicep Curl");
    for(Size i = 0; i < 3; i++) {
        add_scheduled_set(exercise, 8, 8);
    }

    exercise = add_workout_exercise(workout, "Tricep downward curl");
    for(Size i = 0; i < 3; i++) {
        add_scheduled_set(exercise, 8, 9);
    }
}

void workout_free(Workout** workout) {
    Workout* w = *workout;
    for(Size ei = 0; ei < w->exercises_size; ei++) {
        if(w->exercises[ei].setting_size) {
            free(w->exercises[ei].settings);
        }
        if(w->exercises[ei].scheduled_set_size) {
            free(w->exercises[ei].scheduled_sets);
        }
        if(w->exercises[ei].actual_set_size) {
            free(w->exercises[ei].actual_sets);
        }
    }
    if(w->exercises_size) {
        free(w->exercises);
    }
    free(*workout);
    *workout = 0;
}

Exercise* add_workout_exercise(Workout* workout, char* title) {
    if(workout->exercises_size >= workout->exercises_capacity -1) {
        workout->exercises_capacity += EXERCISE_LIST_BLOCK;
        Exercise* temp = (Exercise *) realloc(workout->exercises, sizeof(Exercise) * workout->exercises_capacity);
        workout->exercises = temp;
    }
    strcpy(workout->exercises[workout->exercises_size].title, title);
    return &workout->exercises[workout->exercises_size++];
}

Setting* add_exercise_setting(Exercise* exercise, char* title, SettingValue value) {
    if(exercise->setting_size >= exercise->setting_size_capacity) {
        exercise->setting_size_capacity += SET_LIST_BLOCK;
        Setting* temp = (Setting*) realloc(exercise->settings, sizeof(Setting) * exercise->setting_size_capacity);
        exercise->settings = temp;
    }
    strcpy(exercise->settings[exercise->setting_size].title, title);
    exercise->settings[exercise->setting_size].value = value;
    return &exercise->settings[exercise->setting_size++];
}

Set* add_scheduled_set(Exercise* exercise, Size count, Weight weight) {
    if(exercise->scheduled_set_size >= exercise->scheduled_set_capacity) {
        exercise->scheduled_set_capacity += SET_LIST_BLOCK;
        Set* temp = (Set*) realloc(exercise->scheduled_sets, sizeof(Set) * exercise->scheduled_set_capacity);
        exercise->scheduled_sets = temp;
    }
    exercise->scheduled_sets[exercise->scheduled_set_size].count = count;
    exercise->scheduled_sets[exercise->scheduled_set_size].weight = weight;
    return &exercise->scheduled_sets[exercise->scheduled_set_size++];
}

Set* add_actual_set(Exercise* exercise, Size count, Weight weight) {
    if(exercise->actual_set_size >= exercise->actual_set_capacity) {
        exercise->actual_set_capacity += SET_LIST_BLOCK;
        Set* temp = (Set*) realloc(exercise->actual_sets, sizeof(Set) * exercise->actual_set_capacity);
        exercise->actual_sets = temp;
    }    
    exercise->actual_sets[exercise->actual_set_size].count = count;
    exercise->actual_sets[exercise->actual_set_size].weight = weight;
    return &exercise->actual_sets[exercise->actual_set_size++];
}
