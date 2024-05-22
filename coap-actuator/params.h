// external structure for the parameters of the actuator
#ifndef PARAMS_H
#define PARAMS_H


// Definizione della struttura
struct actuator_params_t {
    int model[4]; // one-hot encoding of the model 1 0 0 0
    int count[5]; // erroricont
    int array3[4]; // number of days past since the last substitution for each component
};

// Dichiarazione della variabile esterna
extern struct actuator_params_t actuator_params;

#endif // MY_ARRAYS_H

