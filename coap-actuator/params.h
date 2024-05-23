// external structure for the parameters of the actuator
#ifndef PARAMS_H
#define PARAMS_H

// Structure definition
struct actuator_params_t {
    int model[4]; // one-hot encoding of the model 1 0 0 0
    int count[5]; // errorcount
    int array3[4]; // number of days past since the last substitution for each component
};

// External declaration of the structure
extern struct actuator_params_t actuator_params;

#endif // MY_ARRAYS_H

