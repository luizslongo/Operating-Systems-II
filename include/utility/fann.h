#include <utility/string.h>
#include <system.h>
#include <utility/ostream.h>
#include <utility/math.h>

#ifndef __FANN_EPOS_h
#define __FANN_EPOS_h

__BEGIN_SYS

class FANN_EPOS {
public: //type defs

    typedef float fann_type;
    typedef char FILE;

public:
    static const unsigned int NUM_LAYERS_CONFIG             = 4;
    static constexpr float LEARNING_RATE_CONFIG             = 0.700000;
    static constexpr float CONNECTION_RATE_CONFIG           = 1.000000;
    static const unsigned int NETWORK_TYPE_CONFIG           = 0;
    static constexpr float LEARNING_MOMENTUM_CONFIG         = 0.000000;
    static const unsigned int TRAINING_ALGORITHM_CONFIG     = 1;
    static const unsigned int TRAIN_ERROR_FUNCTION_CONFIG   = 1;
    static const unsigned int TRAIN_STOP_FUNCTION_CONFIG    = 0;
    static constexpr double BIT_FAIL_LIMIT_CONFIG           = 3.49999994039535522461e-01;

private: // Internal Macros
    #define fann_mult(x,y) (x*y)
    #define fann_abs(value) (((value) > 0) ? (value) : -(value))
    #define fann_clip(x, lo, hi) (((x) < (lo)) ? (lo) : (((x) > (hi)) ? (hi) : (x)))

    #define fann_linear_func(v1, r1, v2, r2, sum) (((((r2)-(r1)) * ((sum)-(v1)))/((v2)-(v1))) + (r1))
    #define fann_stepwise(v1, v2, v3, v4, v5, v6, r1, r2, r3, r4, r5, r6, min, max, sum) (sum < v5 ? (sum < v3 ? (sum < v2 ? (sum < v1 ? min : fann_linear_func(v1, r1, v2, r2, sum)) : fann_linear_func(v2, r2, v3, r3, sum)) : (sum < v4 ? fann_linear_func(v3, r3, v4, r4, sum) : fann_linear_func(v4, r4, v5, r5, sum))) : (sum < v6 ? fann_linear_func(v5, r5, v6, r6, sum) : max))


    /* FANN_LINEAR */
    /* #define fann_linear(steepness, sum) fann_mult(steepness, sum) */
    #define fann_linear_derive(steepness, value) (steepness)

    /* FANN_SIGMOID */
    /* #define fann_sigmoid(steepness, sum) (1.0f/(1.0f + exp(-2.0f * steepness * sum))) */
    // #define fann_sigmoid_real(sum) (1.0f/(1.0f + FANN_EXP(-2.0f * sum)))
    #define fann_sigmoid_derive(steepness, value) (2.0f * steepness * value * (1.0f - value))

    /* FANN_SIGMOID_SYMMETRIC */
    /* #define fann_sigmoid_symmetric(steepness, sum) (2.0f/(1.0f + exp(-2.0f * steepness * sum)) - 1.0f) */
    // #define fann_sigmoid_symmetric_real(sum) (2.0f/(1.0f + FANN_EXP(-2.0f * sum)) - 1.0f)
    #define fann_sigmoid_symmetric_derive(steepness, value) steepness * (1.0f - (value*value))

    /* FANN_GAUSSIAN */
    /* #define fann_gaussian(steepness, sum) (exp(-sum * steepness * sum * steepness)) */
    // #define fann_gaussian_real(sum) (FANN_EXP(-sum * sum))
    #define fann_gaussian_derive(steepness, value, sum) (-2.0f * sum * value * steepness * steepness)

    /* FANN_GAUSSIAN_SYMMETRIC */
    /* #define fann_gaussian_symmetric(steepness, sum) ((exp(-sum * steepness * sum * steepness)*2.0)-1.0) */
    // #define fann_gaussian_symmetric_real(sum) ((FANN_EXP(-sum * sum)*2.0f)-1.0f)
    #define fann_gaussian_symmetric_derive(steepness, value, sum) (-2.0f * sum * (value+1.0f) * steepness * steepness)

    /* FANN_ELLIOT */
    /* #define fann_elliot(steepness, sum) (((sum * steepness) / 2.0f) / (1.0f + fann_abs(sum * steepness)) + 0.5f) */
    #define fann_elliot_real(sum) (((sum) / 2.0f) / (1.0f + fann_abs(sum)) + 0.5f)
    #define fann_elliot_derive(steepness, value, sum) (steepness * 1.0f / (2.0f * (1.0f + fann_abs(sum)) * (1.0f + fann_abs(sum))))

    /* FANN_ELLIOT_SYMMETRIC */
    /* #define fann_elliot_symmetric(steepness, sum) ((sum * steepness) / (1.0f + fann_abs(sum * steepness)))*/
    #define fann_elliot_symmetric_real(sum) ((sum) / (1.0f + fann_abs(sum)))
    #define fann_elliot_symmetric_derive(steepness, value, sum) (steepness * 1.0f / ((1.0f + fann_abs(sum)) * (1.0f + fann_abs(sum))))

    #define fann_activation_switch(activation_function, value, result) \
    switch(activation_function) \
    { \
        case FANN_LINEAR: \
            result = (fann_type)value; \
            break; \
        case FANN_LINEAR_PIECE: \
            result = (fann_type)((value < 0) ? 0 : (value > 1) ? 1 : value); \
            break; \
        case FANN_LINEAR_PIECE_SYMMETRIC: \
            result = (fann_type)((value < -1) ? -1 : (value > 1) ? 1 : value); \
            break; \
        case FANN_SIGMOID_SYMMETRIC_STEPWISE: \
            result = (fann_type)fann_stepwise(((fann_type)-2.64665293693542480469e+00), ((fann_type)-1.47221934795379638672e+00), ((fann_type)-5.49306154251098632812e-01), ((fann_type)5.49306154251098632812e-01), ((fann_type)1.47221934795379638672e+00), ((fann_type)2.64665293693542480469e+00), ((fann_type)-9.90000009536743164062e-01), ((fann_type)-8.99999976158142089844e-01), ((fann_type)-5.00000000000000000000e-01), ((fann_type)5.00000000000000000000e-01), ((fann_type)8.99999976158142089844e-01), ((fann_type)9.90000009536743164062e-01), -1, 1, value); \
            break; \
        case FANN_SIGMOID_STEPWISE: \
            result = (fann_type)fann_stepwise(((fann_type)-2.64665246009826660156e+00), ((fann_type)-1.47221946716308593750e+00), ((fann_type)-5.49306154251098632812e-01), ((fann_type)5.49306154251098632812e-01), ((fann_type)1.47221934795379638672e+00), ((fann_type)2.64665293693542480469e+00), ((fann_type) 4.99999988824129104614e-03), ((fann_type) 5.00000007450580596924e-02), ((fann_type) 2.50000000000000000000e-01), ((fann_type)7.50000000000000000000e-01), ((fann_type)9.49999988079071044922e-01), ((fann_type)9.95000004768371582031e-01), 0, 1, value); \
            break; \
        case FANN_THRESHOLD: \
            result = (fann_type)((value < 0) ? 0 : 1); \
            break; \
        case FANN_THRESHOLD_SYMMETRIC: \
            result = (fann_type)((value < 0) ? -1 : 1); \
            break; \
        case FANN_ELLIOT: \
            result = (fann_type)fann_elliot_real(value); \
            break; \
        case FANN_ELLIOT_SYMMETRIC: \
            result = (fann_type)fann_elliot_symmetric_real(value); \
            break; \
        default: \
            result = 0; \
            break; \
    }

public: // Structs and Enums

    enum fann_errno_enum
    {
        FANN_E_NO_ERROR = 0,
        FANN_E_CANT_OPEN_CONFIG_R,
        FANN_E_CANT_OPEN_CONFIG_W,
        FANN_E_WRONG_CONFIG_VERSION,
        FANN_E_CANT_READ_CONFIG,
        FANN_E_CANT_READ_NEURON,
        FANN_E_CANT_READ_CONNECTIONS,
        FANN_E_WRONG_NUM_CONNECTIONS,
        FANN_E_CANT_OPEN_TD_W,
        FANN_E_CANT_OPEN_TD_R,
        FANN_E_CANT_READ_TD,
        FANN_E_CANT_ALLOCATE_MEM,
        FANN_E_CANT_TRAIN_ACTIVATION,
        FANN_E_CANT_USE_ACTIVATION,
        FANN_E_TRAIN_DATA_MISMATCH,
        FANN_E_CANT_USE_TRAIN_ALG,
        FANN_E_TRAIN_DATA_SUBSET,
        FANN_E_INDEX_OUT_OF_BOUND,
        FANN_E_SCALE_NOT_PRESENT,
        FANN_E_INPUT_NO_MATCH,
        FANN_E_OUTPUT_NO_MATCH,
        FANN_E_WRONG_PARAMETERS_FOR_CREATE
    };

    enum fann_nettype_enum
    {
        FANN_NETTYPE_LAYER = 0, /* Each layer only has connections to the next layer */
        FANN_NETTYPE_SHORTCUT /* Each layer has connections to all following layers */
    };

    enum fann_errorfunc_enum
    {
        FANN_ERRORFUNC_LINEAR = 0,
        FANN_ERRORFUNC_TANH
    };

    enum fann_activationfunc_enum
    {
        FANN_LINEAR = 0,
        FANN_THRESHOLD,
        FANN_THRESHOLD_SYMMETRIC,
        FANN_SIGMOID,
        FANN_SIGMOID_STEPWISE,
        FANN_SIGMOID_SYMMETRIC,
        FANN_SIGMOID_SYMMETRIC_STEPWISE,
        FANN_GAUSSIAN,
        FANN_GAUSSIAN_SYMMETRIC,
        /* Stepwise linear approximation to gaussian.
        * Faster than gaussian but a bit less precise.
        * NOT implemented yet.
        */
        FANN_GAUSSIAN_STEPWISE,
        FANN_ELLIOT,
        FANN_ELLIOT_SYMMETRIC,
        FANN_LINEAR_PIECE,
        FANN_LINEAR_PIECE_SYMMETRIC,
        FANN_SIN_SYMMETRIC,
        FANN_COS_SYMMETRIC,
        FANN_SIN,
        FANN_COS
    };


    enum fann_stopfunc_enum
    {
        FANN_STOPFUNC_MSE = 0,
        FANN_STOPFUNC_BIT
    };

    enum fann_train_enum
    {
        FANN_TRAIN_INCREMENTAL = 0,
        FANN_TRAIN_BATCH,
        FANN_TRAIN_RPROP,
        FANN_TRAIN_QUICKPROP,
        FANN_TRAIN_SARPROP
    };

    struct fann_neuron
    {
        /* Index to the first and last connection
        * (actually the last is a past end index)
        */
        unsigned int first_con;
        unsigned int last_con;
        /* The sum of the inputs multiplied with the weights */
        fann_type sum;
        /* The value of the activation function applied to the sum */
        fann_type value;
        /* The steepness of the activation function */
        fann_type activation_steepness;
        /* Used to choose which activation function to use */
        enum fann_activationfunc_enum activation_function;
    };

    struct fann_connection
    {
        /* Unique number used to identify source neuron */
        unsigned int from_neuron;
        /* Unique number used to identify destination neuron */
        unsigned int to_neuron;
        /* The numerical value of the weight */
        fann_type weight;
    };

    /* A single layer in the neural network.
    */
    struct fann_layer
    {
        /* A pointer to the first neuron in the layer
        * When allocated, all the neurons in all the layers are actually
        * in one long array, this is because we want to easily clear all
        * the neurons at once.
        */
        struct fann_neuron *first_neuron;

        /* A pointer to the neuron past the last neuron in the layer */
        /* the number of neurons is last_neuron - first_neuron */
        struct fann_neuron *last_neuron;
    };

    struct fann
    {
        /* The type of error that last occured. */
        enum fann_errno_enum errno_f;

        /* Where to log error messages. */
        FILE *error_log;

        /* A string representation of the last error. */
        char *errstr;

        /* the learning rate of the network */
        float learning_rate;

        /* The learning momentum used for backpropagation algorithm. */
        float learning_momentum;

        /* the connection rate of the network
        * between 0 and 1, 1 meaning fully connected
        */
        float connection_rate;

        /* is 1 if shortcut connections are used in the ann otherwise 0
        * Shortcut connections are connections that skip layers.
        * A fully connected ann with shortcut connections are a ann where
        * neurons have connections to all neurons in all later layers.
        */
        enum fann_nettype_enum network_type;

        /* pointer to the first layer (input layer) in an array af all the layers,
        * including the input and outputlayers
        */
        struct fann_layer *first_layer;

        /* pointer to the layer past the last layer in an array af all the layers,
        * including the input and outputlayers
        */
        struct fann_layer *last_layer;

        /* Total number of neurons.
        * very useful, because the actual neurons are allocated in one long array
        */
        unsigned int total_neurons;

        /* Number of input neurons (not calculating bias) */
        unsigned int num_input;

        /* Number of output neurons (not calculating bias) */
        unsigned int num_output;

        /* The weight array */
        fann_type *weights;

        /* The connection array */
        struct fann_neuron **connections;

        /* Used to contain the errors used during training
        * Is allocated during first training session,
        * which means that if we do not train, it is never allocated.
        */
        fann_type *train_errors;

        /* Training algorithm used when calling fann_train_on_..
        */
        enum fann_train_enum training_algorithm;

        /* Total number of connections.
        * very useful, because the actual connections
        * are allocated in one long array
        */
        unsigned int total_connections;

        /* used to store outputs in */
        fann_type *output;

        /* the number of data used to calculate the mean square error.
        */
        unsigned int num_MSE;

        /* the total error value.
        * the real mean square error is MSE_value/num_MSE
        */
        float MSE_value;

        /* The number of outputs which would fail (only valid for classification problems)
        */
        unsigned int num_bit_fail;

        /* The maximum difference between the actual output and the expected output
        * which is accepted when counting the bit fails.
        * This difference is multiplied by two when dealing with symmetric activation functions,
        * so that symmetric and not symmetric activation functions can use the same limit.
        */
        fann_type bit_fail_limit;

        /* The error function used during training. (default FANN_ERRORFUNC_TANH)
        */
        enum fann_errorfunc_enum train_error_function;

        /* The stop function used during training. (default FANN_STOPFUNC_MSE)
        */
        enum fann_stopfunc_enum train_stop_function;

        /* The callback function used during training. (default NULL)
        */
        unsigned int callback;

        /* A pointer to user defined data. (default NULL)
        */
        void *user_data;

        /* Variables for use with Cascade Correlation */

        /* The error must change by at least this
        * fraction of its old value to count as a
        * significant change.
        */
        float cascade_output_change_fraction;

        /* No change in this number of epochs will cause
        * stagnation.
        */
        unsigned int cascade_output_stagnation_epochs;

        /* The error must change by at least this
        * fraction of its old value to count as a
        * significant change.
        */
        float cascade_candidate_change_fraction;

        /* No change in this number of epochs will cause
        * stagnation.
        */
        unsigned int cascade_candidate_stagnation_epochs;

        /* The current best candidate, which will be installed.
        */
        unsigned int cascade_best_candidate;

        /* The upper limit for a candidate score
        */
        fann_type cascade_candidate_limit;

        /* Scale of copied candidate output weights
        */
        fann_type cascade_weight_multiplier;

        /* Maximum epochs to train the output neurons during cascade training
        */
        unsigned int cascade_max_out_epochs;

        /* Maximum epochs to train the candidate neurons during cascade training
        */
        unsigned int cascade_max_cand_epochs;

        /* Minimum epochs to train the output neurons during cascade training
        */
        unsigned int cascade_min_out_epochs;

        /* Minimum epochs to train the candidate neurons during cascade training
        */
        unsigned int cascade_min_cand_epochs;

        /* An array consisting of the activation functions used when doing
        * cascade training.
        */
        enum fann_activationfunc_enum *cascade_activation_functions;

        /* The number of elements in the cascade_activation_functions array.
        */
        unsigned int cascade_activation_functions_count;

        /* An array consisting of the steepnesses used during cascade training.
        */
        fann_type *cascade_activation_steepnesses;

        /* The number of elements in the cascade_activation_steepnesses array.
        */
        unsigned int cascade_activation_steepnesses_count;

        /* The number of candidates of each type that will be present.
        * The actual number of candidates is then
        * cascade_activation_functions_count *
        * cascade_activation_steepnesses_count *
        * cascade_num_candidate_groups
        */
        unsigned int cascade_num_candidate_groups;

        /* An array consisting of the score of the individual candidates,
        * which is used to decide which candidate is the best
        */
        fann_type *cascade_candidate_scores;

        /* The number of allocated neurons during cascade correlation algorithms.
        * This number might be higher than the actual number of neurons to avoid
        * allocating new space too often.
        */
        unsigned int total_neurons_allocated;

        /* The number of allocated connections during cascade correlation algorithms.
        * This number might be higher than the actual number of neurons to avoid
        * allocating new space too often.
        */
        unsigned int total_connections_allocated;

        /* Variables for use with Quickprop training */

        /* Decay is used to make the weights not go so high */
        float quickprop_decay;

        /* Mu is a factor used to increase and decrease the stepsize */
        float quickprop_mu;

        /* Variables for use with with RPROP training */

        /* Tells how much the stepsize should increase during learning */
        float rprop_increase_factor;

        /* Tells how much the stepsize should decrease during learning */
        float rprop_decrease_factor;

        /* The minimum stepsize */
        float rprop_delta_min;

        /* The maximum stepsize */
        float rprop_delta_max;

        /* The initial stepsize */
        float rprop_delta_zero;

        /* Defines how much the weights are constrained to smaller values at the beginning */
        float sarprop_weight_decay_shift;

        /* Decides if the stepsize is too big with regard to the error */
        float sarprop_step_error_threshold_factor;

        /* Defines how much the stepsize is influenced by the error */
        float sarprop_step_error_shift;

        /* Defines how much the epoch influences weight decay and noise */
        float sarprop_temperature;

        /* Current training epoch */
        unsigned int sarprop_epoch;

        /* Used to contain the slope errors used during batch training
        * Is allocated during first training session,
        * which means that if we do not train, it is never allocated.
        */
        fann_type *train_slopes;

        /* The previous step taken by the quickprop/rprop procedures.
        * Not allocated if not used.
        */
        fann_type *prev_steps;

        /* The slope values used by the quickprop/rprop procedures.
        * Not allocated if not used.
        */
        fann_type *prev_train_slopes;

        /* The last delta applied to a connection weight.
        * This is used for the momentum term in the backpropagation algorithm.
        * Not allocated if not used.
        */
        fann_type *prev_weights_deltas;

        /* Arithmetic mean used to remove steady component in input data.  */
        float *scale_mean_in;

        /* Standart deviation used to normalize input data (mostly to [-1;1]). */
        float *scale_deviation_in;

        /* User-defined new minimum for input data.
        * Resulting data values may be less than user-defined minimum.
        */
        float *scale_new_min_in;

        /* Used to scale data to user-defined new maximum for input data.
        * Resulting data values may be greater than user-defined maximum.
        */
        float *scale_factor_in;

        /* Arithmetic mean used to remove steady component in output data.  */
        float *scale_mean_out;

        /* Standart deviation used to normalize output data (mostly to [-1;1]). */
        float *scale_deviation_out;

        /* User-defined new minimum for output data.
        * Resulting data values may be less than user-defined minimum.
        */
        float *scale_new_min_out;

        /* Used to scale data to user-defined new maximum for output data.
        * Resulting data values may be greater than user-defined maximum.
        */
        float *scale_factor_out;
    };

private: //Fann Allocate Methods

    static struct fann* fann_allocate_structure(unsigned int num_layers) {
        struct fann* ann = (struct fann *) malloc(sizeof(struct fann));

        ann->errno_f = FANN_E_NO_ERROR;
        ann->error_log = 0;
        ann->errstr = 0;
        ann->learning_rate = 0.7f;
        ann->learning_momentum = 0.0;
        ann->total_neurons = 0;
        ann->total_connections = 0;
        ann->num_input = 0;
        ann->num_output = 0;
        ann->train_errors = 0;
        ann->train_slopes = 0;
        ann->prev_steps = 0;
        ann->prev_train_slopes = 0;
        ann->prev_weights_deltas = 0;
        ann->training_algorithm = FANN_TRAIN_INCREMENTAL;
        ann->num_MSE = 0;
        ann->MSE_value = 0;
        ann->num_bit_fail = 0;
        ann->bit_fail_limit = (fann_type)0.35;
        ann->network_type = FANN_NETTYPE_LAYER;
        ann->train_error_function = FANN_ERRORFUNC_TANH;
        ann->train_stop_function = FANN_STOPFUNC_MSE;
        ann->callback = 0;
        ann->user_data = 0; /* User is responsible for deallocation */
        ann->weights = 0;
        ann->connections = 0;
        ann->output = 0;
        ann->scale_mean_in = 0;
        ann->scale_deviation_in = 0;
        ann->scale_new_min_in = 0;
        ann->scale_factor_in = 0;
        ann->scale_mean_out = 0;
        ann->scale_deviation_out = 0;
        ann->scale_new_min_out = 0;
        ann->scale_factor_out = 0;

        /* variables used for cascade correlation (reasonable defaults) */
        ann->cascade_output_change_fraction = 0.01f;
        ann->cascade_candidate_change_fraction = 0.01f;
        ann->cascade_output_stagnation_epochs = 12;
        ann->cascade_candidate_stagnation_epochs = 12;
        ann->cascade_num_candidate_groups = 2;
        ann->cascade_weight_multiplier = (fann_type)0.4;
        ann->cascade_candidate_limit = (fann_type)1000.0;
        ann->cascade_max_out_epochs = 150;
        ann->cascade_max_cand_epochs = 150;
        ann->cascade_min_out_epochs = 50;
        ann->cascade_min_cand_epochs = 50;
        ann->cascade_candidate_scores = 0;
        ann->cascade_activation_functions_count = 10;
        ann->cascade_activation_functions =
            (enum fann_activationfunc_enum *)malloc(ann->cascade_activation_functions_count * sizeof(enum fann_activationfunc_enum));

        ann->cascade_activation_functions[0] = FANN_SIGMOID;
        ann->cascade_activation_functions[1] = FANN_SIGMOID_SYMMETRIC;
        ann->cascade_activation_functions[2] = FANN_GAUSSIAN;
        ann->cascade_activation_functions[3] = FANN_GAUSSIAN_SYMMETRIC;
        ann->cascade_activation_functions[4] = FANN_ELLIOT;
        ann->cascade_activation_functions[5] = FANN_ELLIOT_SYMMETRIC;
        ann->cascade_activation_functions[6] = FANN_SIN_SYMMETRIC;
        ann->cascade_activation_functions[7] = FANN_COS_SYMMETRIC;
        ann->cascade_activation_functions[8] = FANN_SIN;
        ann->cascade_activation_functions[9] = FANN_COS;

        ann->cascade_activation_steepnesses_count = 4;
        ann->cascade_activation_steepnesses =
            (fann_type *)malloc(ann->cascade_activation_steepnesses_count * sizeof(fann_type));

        ann->cascade_activation_steepnesses[0] = (fann_type)0.25;
        ann->cascade_activation_steepnesses[1] = (fann_type)0.5;
        ann->cascade_activation_steepnesses[2] = (fann_type)0.75;
        ann->cascade_activation_steepnesses[3] = (fann_type)1.0;

        /* Variables for use with with Quickprop training (reasonable defaults) */
        ann->quickprop_decay = -0.0001f;
        ann->quickprop_mu = 1.75;

        /* Variables for use with with RPROP training (reasonable defaults) */
        ann->rprop_increase_factor = 1.2f;
        ann->rprop_decrease_factor = 0.5;
        ann->rprop_delta_min = 0.0;
        ann->rprop_delta_max = 50.0;
        ann->rprop_delta_zero = 0.1f;

        /* Variables for use with SARPROP training (reasonable defaults) */
        ann->sarprop_weight_decay_shift = -6.644f;
        ann->sarprop_step_error_threshold_factor = 0.1f;
        ann->sarprop_step_error_shift = 1.385f;
        ann->sarprop_temperature = 0.015f;
        ann->sarprop_epoch = 0;

        ann->first_layer = (struct fann_layer *) malloc(num_layers * sizeof(struct fann_layer));
        memset(ann->first_layer, 0, num_layers * sizeof(struct fann_layer));

        ann->last_layer = ann->first_layer + num_layers;

        return ann;
    }

    static void fann_allocate_neurons(struct fann* ann) {
        struct fann_layer *layer_it;
        struct fann_neuron *neurons;
        unsigned int num_neurons_so_far = 0;
        unsigned int num_neurons = 0;

        /* all the neurons is allocated in one long array (calloc clears mem) */
        neurons = (struct fann_neuron *) malloc(ann->total_neurons * sizeof(struct fann_neuron));
        memset(neurons, 0, ann->total_neurons * sizeof(struct fann_neuron));
        ann->total_neurons_allocated = ann->total_neurons;

        for(layer_it = ann->first_layer; layer_it != ann->last_layer; layer_it++)
        {
            num_neurons = (unsigned int)(layer_it->last_neuron - layer_it->first_neuron);
            layer_it->first_neuron = neurons + num_neurons_so_far;
            layer_it->last_neuron = layer_it->first_neuron + num_neurons;
            num_neurons_so_far += num_neurons;
        }

        ann->output = (fann_type *) malloc(num_neurons * sizeof(fann_type));
        memset(ann->output, 0, num_neurons * sizeof(fann_type));
    }

    static void fann_allocate_connections(struct fann* ann) {
        ann->weights = (fann_type *) malloc(ann->total_connections * sizeof(fann_type));
        memset(ann->weights, 0, ann->total_connections * sizeof(fann_type));

        ann->total_connections_allocated = ann->total_connections;

        /* TODO make special cases for all places where the connections
        * is used, so that it is not needed for fully connected networks.
        */
        ann->connections =
            (struct fann_neuron **) malloc(ann->total_connections_allocated * sizeof(struct fann_neuron *));
        memset(ann->connections, 0, ann->total_connections_allocated * sizeof(struct fann_neuron *));
    }

public: // Create and Run and Online Train
    static struct fann* fann_create_from_config() {
        const unsigned int layers_sizes[] = { 6, 11, 11, 2 };

        const float neurons_config[][3] = {
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {6,6,5.00000000000000000000e-01},
            {6,6,5.00000000000000000000e-01},
            {6,6,5.00000000000000000000e-01},
            {6,6,5.00000000000000000000e-01},
            {6,6,5.00000000000000000000e-01},
            {6,6,5.00000000000000000000e-01},
            {6,6,5.00000000000000000000e-01},
            {6,6,5.00000000000000000000e-01},
            {6,6,5.00000000000000000000e-01},
            {6,6,5.00000000000000000000e-01},
            {0,6,0.00000000000000000000e+00},
            {11,6,5.00000000000000000000e-01},
            {11,6,5.00000000000000000000e-01},
            {11,6,5.00000000000000000000e-01},
            {11,6,5.00000000000000000000e-01},
            {11,6,5.00000000000000000000e-01},
            {11,6,5.00000000000000000000e-01},
            {11,6,5.00000000000000000000e-01},
            {11,6,5.00000000000000000000e-01},
            {11,6,5.00000000000000000000e-01},
            {11,6,5.00000000000000000000e-01},
            {0,6,0.00000000000000000000e+00},
            {11,6,5.00000000000000000000e-01},
            {0,6,0.00000000000000000000e+00}
        };

        const float weights_config[][2] = {
            {0,    3.09680968523025512695e-01},
            {1,    1.36943292617797851562e+00},
            {2,    2.40078233182430267334e-02},
            {3,    1.30099189281463623047e+00},
            {4,    2.24526926875114440918e-01},
            {5,    -1.45571541786193847656e+00},
            {0,    -9.14338305592536926270e-02},
            {1,    1.13749384880065917969e+00},
            {2,    2.61127322912216186523e-01},
            {3,    1.08947193622589111328e+00},
            {4,    1.87473833560943603516e-01},
            {5,    -6.85815393924713134766e-01},
            {0,    -7.23378419876098632812e-01},
            {1,    -8.82135629653930664062e-01},
            {2,    -2.81613171100616455078e-01},
            {3,    -8.12091410160064697266e-01},
            {4,    -4.30699884891510009766e-01},
            {5,    -1.73325806856155395508e-01},
            {0,    -4.13331612944602966309e-02},
            {1,    -2.49410080909729003906e+00},
            {2,    1.15964245796203613281e+00},
            {3,    -2.53630590438842773438e+00},
            {4,    8.53239238262176513672e-01},
            {5,    2.34094381332397460938e+00},
            {0,    1.38149404525756835938e+01},
            {1,    -5.89045095443725585938e+00},
            {2,    -4.04378318786621093750e+00},
            {3,    7.69798851013183593750e+00},
            {4,    -4.44660472869873046875e+00},
            {5,    -1.64719736576080322266e+00},
            {0,    -1.35576162338256835938e+01},
            {1,    3.38479542732238769531e+00},
            {2,    6.75282001495361328125e+00},
            {3,    8.99264430999755859375e+00},
            {4,    -1.78085553646087646484e+00},
            {5,    3.81935015320777893066e-02},
            {0,    4.01545667648315429688e+00},
            {1,    4.31895732879638671875e+00},
            {2,    3.35285854339599609375e+00},
            {3,    3.77804446220397949219e+00},
            {4,    3.41998696327209472656e+00},
            {5,    -1.01801224052906036377e-01},
            {0,    3.94545286893844604492e-01},
            {1,    1.23445522785186767578e+00},
            {2,    -1.81620530784130096436e-02},
            {3,    1.02267289161682128906e+00},
            {4,    2.66624718904495239258e-01},
            {5,    -1.04787898063659667969e+00},
            {0,    3.50656676292419433594e+00},
            {1,    1.09026126861572265625e+01},
            {2,    -4.09362363815307617188e+00},
            {3,    -1.12067918777465820312e+01},
            {4,    6.77044200897216796875e+00},
            {5,    -7.66877651214599609375e-01},
            {0,    7.22575604915618896484e-01},
            {1,    2.84609222412109375000e+00},
            {2,    -4.66178059577941894531e-01},
            {3,    2.12039518356323242188e+00},
            {4,    -6.54034674167633056641e-01},
            {5,    -2.94266343116760253906e+00},
            {6,    -1.25085487961769104004e-01},
            {7,    -1.90712094306945800781e-01},
            {8,    6.66596516966819763184e-02},
            {9,    5.87494038045406341553e-02},
            {10,    2.15760970115661621094e+00},
            {11,    2.58044290542602539062e+00},
            {12,    -1.66358554363250732422e+00},
            {13,    -1.98062568902969360352e-01},
            {14,    2.11847090721130371094e+00},
            {15,    6.01335704326629638672e-01},
            {16,    -1.95153951644897460938e+00},
            {6,    -2.12066268920898437500e+00},
            {7,    -2.08357572555541992188e+00},
            {8,    7.88651943206787109375e-01},
            {9,    2.48507571220397949219e+00},
            {10,    4.29081964492797851562e+00},
            {11,    1.82645428180694580078e+00},
            {12,    -4.29726362228393554688e+00},
            {13,    -1.80908703804016113281e+00},
            {14,    3.93598294258117675781e+00},
            {15,    -2.62545847892761230469e+00},
            {16,    -2.12915539741516113281e+00},
            {6,    1.63631647825241088867e-01},
            {7,    8.56795191764831542969e-01},
            {8,    -1.89010465145111083984e+00},
            {9,    -5.31341135501861572266e-01},
            {10,    1.13341403007507324219e+00},
            {11,    9.97139573097229003906e-01},
            {12,    5.34765577316284179688e+00},
            {13,    8.89325216412544250488e-02},
            {14,    1.54052364826202392578e+00},
            {15,    2.55883669853210449219e+00},
            {16,    1.71287655830383300781e+00},
            {6,    6.70721471309661865234e-01},
            {7,    4.68636095523834228516e-01},
            {8,    -4.24792110919952392578e-01},
            {9,    -6.90667390823364257812e-01},
            {10,    -8.30958783626556396484e-01},
            {11,    -1.46759557723999023438e+00},
            {12,    1.64240527153015136719e+00},
            {13,    6.94619834423065185547e-01},
            {14,    -1.23083758354187011719e+00},
            {15,    7.87076711654663085938e-01},
            {16,    4.56933438777923583984e-01},
            {6,    -1.47295820713043212891e+00},
            {7,    -1.22379386425018310547e+00},
            {8,    9.32301998138427734375e-01},
            {9,    1.83296835422515869141e+00},
            {10,    1.45931673049926757812e+00},
            {11,    2.69175076484680175781e+00},
            {12,    -2.35520863533020019531e+00},
            {13,    -1.57573807239532470703e+00},
            {14,    1.63835096359252929688e+00},
            {15,    -2.06218743324279785156e+00},
            {16,    -8.45166623592376708984e-01},
            {6,    6.61001741886138916016e-01},
            {7,    6.40592634677886962891e-01},
            {8,    -4.35815989971160888672e-01},
            {9,    -7.90386974811553955078e-01},
            {10,    -1.08035123348236083984e+00},
            {11,    -1.42090034484863281250e+00},
            {12,    1.58968722820281982422e+00},
            {13,    7.77397453784942626953e-01},
            {14,    -1.18865549564361572266e+00},
            {15,    5.94571530818939208984e-01},
            {16,    5.87567508220672607422e-01},
            {6,    -1.24666595458984375000e+00},
            {7,    -1.21526205539703369141e+00},
            {8,    5.71879208087921142578e-01},
            {9,    1.29922950267791748047e+00},
            {10,    3.41035747528076171875e+00},
            {11,    4.22462987899780273438e+00},
            {12,    -4.30971097946166992188e+00},
            {13,    -1.34522223472595214844e+00},
            {14,    3.17850470542907714844e+00},
            {15,    -1.48378574848175048828e+00},
            {16,    -2.81541419029235839844e+00},
            {6,    -1.35751381516456604004e-01},
            {7,    -1.48361623287200927734e-01},
            {8,    -7.22442120313644409180e-02},
            {9,    1.69564694166183471680e-01},
            {10,    2.56372153759002685547e-01},
            {11,    2.12261423468589782715e-01},
            {12,    -1.36499449610710144043e-01},
            {13,    -1.44774973392486572266e-01},
            {14,    1.67338192462921142578e-01},
            {15,    -7.55150541663169860840e-02},
            {16,    4.16977815330028533936e-02},
            {6,    1.80569350719451904297e-01},
            {7,    4.53611552715301513672e-01},
            {8,    -2.57919460535049438477e-01},
            {9,    -2.43067070841789245605e-01},
            {10,    -3.14859342575073242188e+00},
            {11,    -3.35652518272399902344e+00},
            {12,    2.51511597633361816406e+00},
            {13,    2.32600033283233642578e-01},
            {14,    -3.13464069366455078125e+00},
            {15,    -6.37867927551269531250e-01},
            {16,    2.77568936347961425781e+00},
            {6,    -2.96987026929855346680e-01},
            {7,    -2.88952916860580444336e-01},
            {8,    9.77505296468734741211e-02},
            {9,    3.72071057558059692383e-01},
            {10,    4.76566344499588012695e-01},
            {11,    6.19698464870452880859e-01},
            {12,    -5.69914638996124267578e-01},
            {13,    -2.58428573608398437500e-01},
            {14,    3.17086368799209594727e-01},
            {15,    -4.28359955549240112305e-01},
            {16,    1.81265145540237426758e-01},
            {17,    4.32347726821899414062e+00},
            {18,    8.27291679382324218750e+00},
            {19,    -8.47635173797607421875e+00},
            {20,    -2.91275382041931152344e+00},
            {21,    5.31442642211914062500e+00},
            {22,    -2.97885370254516601562e+00},
            {23,    7.83747100830078125000e+00},
            {24,    4.30884480476379394531e-01},
            {25,    -6.13246965408325195312e+00},
            {26,    1.13406574726104736328e+00},
            {27,    2.48811197280883789062e+00}
        };

        unsigned int num_layers, layer_size, input_neuron, i, num_connections;
	    unsigned int tmpVal;

        //unsigned int scale_included;

        struct fann_neuron *first_neuron, *neuron_it, *last_neuron, **connected_neurons;
        fann_type *weights;
        struct fann_layer *layer_it;

        num_layers = NUM_LAYERS_CONFIG;
        struct fann* ann = fann_allocate_structure(num_layers);
        ann->learning_rate = LEARNING_RATE_CONFIG;
        ann->connection_rate = CONNECTION_RATE_CONFIG;
        ann->network_type = (enum fann_nettype_enum) NETWORK_TYPE_CONFIG;
        ann->learning_momentum = LEARNING_MOMENTUM_CONFIG;
        ann->training_algorithm = (enum fann_train_enum)TRAINING_ALGORITHM_CONFIG;
        ann->train_error_function = (enum fann_errorfunc_enum)TRAIN_ERROR_FUNCTION_CONFIG;
        ann->train_stop_function = (enum fann_stopfunc_enum)TRAIN_STOP_FUNCTION_CONFIG;

        ann->bit_fail_limit = (fann_type) BIT_FAIL_LIMIT_CONFIG;
        unsigned int iter = 0;

        for(layer_it = ann->first_layer; layer_it != ann->last_layer; layer_it++)
        {
            layer_size = layers_sizes[iter];
            iter++;
            /* we do not allocate room here, but we make sure that
            * last_neuron - first_neuron is the number of neurons */
            layer_it->first_neuron = 0;
            layer_it->last_neuron = layer_it->first_neuron + layer_size;
            ann->total_neurons += layer_size;
        }

        ann->num_input = (unsigned int)(ann->first_layer->last_neuron - ann->first_layer->first_neuron - 1);
        ann->num_output = (unsigned int)((ann->last_layer - 1)->last_neuron - (ann->last_layer - 1)->first_neuron);

        if(ann->network_type == FANN_NETTYPE_LAYER)
        {
            /* one too many (bias) in the output layer */
            ann->num_output--;
        }

        /* SCALE NOT INCLUDED ON OUR FANN */

        fann_allocate_neurons(ann);

        last_neuron = (ann->last_layer - 1)->last_neuron;
        iter = 0;

        for(neuron_it = ann->first_layer->first_neuron; neuron_it != last_neuron; neuron_it++)
        {
            num_connections = neurons_config[iter][0];
            tmpVal = neurons_config[iter][1];
            neuron_it->activation_steepness = neurons_config[iter][2];
            iter++;
            neuron_it->activation_function = (enum fann_activationfunc_enum)tmpVal;
            neuron_it->first_con = ann->total_connections;
            ann->total_connections += num_connections;
            neuron_it->last_con = ann->total_connections;
        }

        fann_allocate_connections(ann);

        connected_neurons = ann->connections;
        weights = ann->weights;
        first_neuron = ann->first_layer->first_neuron;
        iter = 0;

        for(i = 0; i < ann->total_connections; i++)
        {
            input_neuron = weights_config[iter][0];
            weights[i] = weights_config[iter][1];
            iter++;
            connected_neurons[i] = first_neuron + input_neuron;
        }

        return ann;
    }

    static fann_type* scale_input(fann_type* input, unsigned int input_size) {
        fann_type fann_max_0[] = {25000.,10000.,10000.,150.,6000.};
        fann_type fann_max_1[] = {8100000.,840000.,840000.,7000.,2590000.};
        fann_type fann_max_2[] = {10520000.,1240000.,1240000.,10000.,3400000.};
        for (unsigned int  i = 0; i < input_size; i++)
        {
            if (input[i] < fann_max_0[i]){
                input[i] = 0;
            } else if (input[i] < fann_max_1[i]) {
                input[i] = 0.25;
            } else if (input[i] < fann_max_2[i]) {
                input[i] = 0.5;
            } else {
                input[i] = 0.75;
            }
        }

        return input;
    }

    static fann_type * fann_run(struct fann* ann, fann_type * input) {
        struct fann_neuron *neuron_it, *last_neuron, *neurons, **neuron_pointers;
        input = scale_input(input, ann->num_input);
        unsigned int i, num_connections, num_input, num_output;
        fann_type neuron_sum, *output;
        fann_type *weights;
        struct fann_layer *layer_it, *last_layer;
        unsigned int activation_function;
        fann_type steepness;

        /* store some variabels local for fast access */
        struct fann_neuron *first_neuron = ann->first_layer->first_neuron;

        fann_type max_sum = 0;

        /* first set the input */
        num_input = ann->num_input;
        for(i = 0; i != num_input; i++) {
            first_neuron[i].value = input[i];
        }

        /* Set the bias neuron in the input layer */
        (ann->first_layer->last_neuron - 1)->value = 1;

        last_layer = ann->last_layer;
        for(layer_it = ann->first_layer + 1; layer_it != last_layer; layer_it++) {
            last_neuron = layer_it->last_neuron;
            for(neuron_it = layer_it->first_neuron; neuron_it != last_neuron; neuron_it++) {
                if(neuron_it->first_con == neuron_it->last_con)
                {
                    /* bias neurons */
                    neuron_it->value = 1;
                    continue;
                }

                activation_function = neuron_it->activation_function;
                steepness = neuron_it->activation_steepness;

                neuron_sum = 0;
                num_connections = neuron_it->last_con - neuron_it->first_con;
                weights = ann->weights + neuron_it->first_con;

                if(ann->connection_rate >= 1) {
                    if(ann->network_type == FANN_NETTYPE_SHORTCUT) {
                        neurons = ann->first_layer->first_neuron;
                    } else {
                        neurons = (layer_it - 1)->first_neuron;
                    }

                    /* unrolled loop start */
                    i = num_connections & 3;	/* same as modulo 4 */
                    switch (i)
                    {
                        case 3:
                            neuron_sum += fann_mult(weights[2], neurons[2].value);
                        case 2:
                            neuron_sum += fann_mult(weights[1], neurons[1].value);
                        case 1:
                            neuron_sum += fann_mult(weights[0], neurons[0].value);
                        case 0:
                            break;
                    }

                    for(; i != num_connections; i += 4)
                    {
                        neuron_sum +=
                            fann_mult(weights[i], neurons[i].value) +
                            fann_mult(weights[i + 1], neurons[i + 1].value) +
                            fann_mult(weights[i + 2], neurons[i + 2].value) +
                            fann_mult(weights[i + 3], neurons[i + 3].value);
                    }
                    /* unrolled loop end */

                    /*
                    * for(i = 0;i != num_connections; i++){
                    * printf("%f += %f*%f, ", neuron_sum, weights[i], neurons[i].value);
                    * neuron_sum += fann_mult(weights[i], neurons[i].value);
                    * }
                    */
                } else {
                    neuron_pointers = ann->connections + neuron_it->first_con;

                    i = num_connections & 3;	/* same as modulo 4 */
                    switch (i)
                    {
                        case 3:
                            neuron_sum += fann_mult(weights[2], neuron_pointers[2]->value);
                        case 2:
                            neuron_sum += fann_mult(weights[1], neuron_pointers[1]->value);
                        case 1:
                            neuron_sum += fann_mult(weights[0], neuron_pointers[0]->value);
                        case 0:
                            break;
                    }

                    for(; i != num_connections; i += 4)
                    {
                        neuron_sum +=
                            fann_mult(weights[i], neuron_pointers[i]->value) +
                            fann_mult(weights[i + 1], neuron_pointers[i + 1]->value) +
                            fann_mult(weights[i + 2], neuron_pointers[i + 2]->value) +
                            fann_mult(weights[i + 3], neuron_pointers[i + 3]->value);
                    }
                }

                neuron_sum = fann_mult(steepness, neuron_sum);

                max_sum = 150/steepness;
                if(neuron_sum > max_sum)
                    neuron_sum = max_sum;
                else if(neuron_sum < -max_sum)
                    neuron_sum = -max_sum;

                neuron_it->sum = neuron_sum;

                fann_activation_switch(activation_function, neuron_sum, neuron_it->value);
            }
        }
        /* set the output */
        output = ann->output;
        num_output = ann->num_output;
        neurons = (ann->last_layer - 1)->first_neuron;
        for(i = 0; i != num_output; i++)
        {
            output[i] = neurons[i].value;
        }
        return ann->output;
    }

    /* Trains the network with the backpropagation algorithm.
     * Online learning is based on incremental learn with only one entry
     * EXECUTION FLOW:
     * 1. At each hyperperiod we execute fun_run() and execute the migraiton/action
     * 2. In the next hyperperiod we compute MSE with the measured reward
     * 3. We backpropagate the error, and update weights
     * 4. With the new configuration, go back to step 1.
     *
     * We do not reset MSE as the online learn is here considered a single train set.
     */
    void fann_learn_last_run(struct fann *ann, fann_type *desired_output)
    {
        fann_compute_MSE(ann, desired_output);

        fann_backpropagate_MSE(ann);

        fann_update_weights(ann);

    }

private: // Internal Online Train Methods

    /* INTERNAL FUNCTION
        compute the error at the network output
        (usually, after forward propagation of a certain input vector, fann_run)
        the error is a sum of squares for all the output units
        also increments a counter because MSE is an average of such errors

        After this train_errors in the output layer will be set to:
        neuron_value_derived * (desired_output - neuron_value)
    */
    void fann_compute_MSE(struct fann *ann, fann_type * desired_output)
    {
        fann_type neuron_value, neuron_diff, *error_it = 0, *error_begin = 0;
        struct fann_neuron *last_layer_begin = (ann->last_layer - 1)->first_neuron;
        const struct fann_neuron *last_layer_end = last_layer_begin + ann->num_output;
        const struct fann_neuron *first_neuron = ann->first_layer->first_neuron;

        /* if no room allocated for the error variabels, allocate it now */
        if(ann->train_errors == 0)
        {
            ann->train_errors = (fann_type *) malloc(ann->total_neurons * sizeof(fann_type));
            memset(ann->train_errors, 0, (ann->total_neurons) * sizeof(fann_type));
        }
        else
        {
            /* clear the error variabels */
            memset(ann->train_errors, 0, (ann->total_neurons) * sizeof(fann_type));
        }
        error_begin = ann->train_errors;

        /* calculate the error and place it in the output layer */
        error_it = error_begin + (last_layer_begin - first_neuron);

        for(; last_layer_begin != last_layer_end; last_layer_begin++)
        {
            neuron_value = last_layer_begin->value;
            neuron_diff = *desired_output - neuron_value;

            neuron_diff = fann_update_MSE(ann, last_layer_begin, neuron_diff);

            if(ann->train_error_function)
            {                       /* TODO make switch when more functions */
                if(neuron_diff < -.9999999)
                    neuron_diff = -17.0;
                else if(neuron_diff > .9999999)
                    neuron_diff = 17.0;
                else
                    neuron_diff = (fann_type) Math::logf((1.0 + neuron_diff) / (1.0 - neuron_diff));
            }

            *error_it = fann_activation_derived(last_layer_begin->activation_function,
                                                last_layer_begin->activation_steepness, neuron_value,
                                                last_layer_begin->sum) * neuron_diff;

            desired_output++;
            error_it++;

            ann->num_MSE++;
        }
    }

    /* INTERNAL FUNCTION
       Helper function to update the MSE value and return a diff which takes symmetric functions into account
    */
    fann_type fann_update_MSE(struct fann *ann, struct fann_neuron* neuron, fann_type neuron_diff)
    {
        float neuron_diff2;
        
        switch (neuron->activation_function)
        {
            case FANN_LINEAR_PIECE_SYMMETRIC:
            case FANN_THRESHOLD_SYMMETRIC:
            case FANN_SIGMOID_SYMMETRIC:
            case FANN_SIGMOID_SYMMETRIC_STEPWISE:
            case FANN_ELLIOT_SYMMETRIC:
            case FANN_GAUSSIAN_SYMMETRIC:
            case FANN_SIN_SYMMETRIC:
            case FANN_COS_SYMMETRIC:
                neuron_diff /= (fann_type)2.0;
                break;
            case FANN_THRESHOLD:
            case FANN_LINEAR:
            case FANN_SIGMOID:
            case FANN_SIGMOID_STEPWISE:
            case FANN_GAUSSIAN:
            case FANN_GAUSSIAN_STEPWISE:
            case FANN_ELLIOT:
            case FANN_LINEAR_PIECE:
            case FANN_SIN:
            case FANN_COS:
                break;
        }

        neuron_diff2 = (float) (neuron_diff * neuron_diff);
    
        ann->MSE_value += neuron_diff2;

        /*printf("neuron_diff %f = (%f - %f)[/2], neuron_diff2=%f, sum=%f, MSE_value=%f, num_MSE=%d\n", neuron_diff, *desired_output, neuron_value, neuron_diff2, last_layer_begin->sum, ann->MSE_value, ann->num_MSE); */
        if(fann_abs(neuron_diff) >= ann->bit_fail_limit)
        {
            ann->num_bit_fail++;
        }
        
        return neuron_diff;
    }

    /* INTERNAL FUNCTION
      Calculates the derived of a value, given an activation function
       and a steepness
    */
    fann_type fann_activation_derived(unsigned int activation_function,
                                      fann_type steepness, fann_type value, fann_type sum)
    {
        switch (activation_function)
        {
            case FANN_LINEAR:
            case FANN_LINEAR_PIECE:
            case FANN_LINEAR_PIECE_SYMMETRIC:
                return (fann_type) fann_linear_derive(steepness, value);
            case FANN_SIGMOID:
            case FANN_SIGMOID_STEPWISE:
                value = fann_clip(value, 0.01f, 0.99f);
                return (fann_type) fann_sigmoid_derive(steepness, value);
            case FANN_SIGMOID_SYMMETRIC:
            case FANN_SIGMOID_SYMMETRIC_STEPWISE:
                value = fann_clip(value, -0.98f, 0.98f);
                return (fann_type) fann_sigmoid_symmetric_derive(steepness, value);
            case FANN_GAUSSIAN:
                /* value = fann_clip(value, 0.01f, 0.99f); */
                return (fann_type) fann_gaussian_derive(steepness, value, sum);
            case FANN_GAUSSIAN_SYMMETRIC:
                /* value = fann_clip(value, -0.98f, 0.98f); */
                return (fann_type) fann_gaussian_symmetric_derive(steepness, value, sum);
            case FANN_ELLIOT:
                value = fann_clip(value, 0.01f, 0.99f);
                return (fann_type) fann_elliot_derive(steepness, value, sum);
            case FANN_ELLIOT_SYMMETRIC:
                value = fann_clip(value, -0.98f, 0.98f);
                return (fann_type) fann_elliot_symmetric_derive(steepness, value, sum);
            case FANN_SIN_SYMMETRIC:
                return 0; //(fann_type) fann_sin_symmetric_derive(steepness, sum);
            case FANN_COS_SYMMETRIC:
                return 0; //(fann_type) fann_cos_symmetric_derive(steepness, sum);
            case FANN_SIN:
                return 0; //(fann_type) fann_sin_derive(steepness, sum);
            case FANN_COS:
                return 0; //(fann_type) fann_cos_derive(steepness, sum);
            case FANN_THRESHOLD:
                return 0;
                //fann_error(NULL, FANN_E_CANT_TRAIN_ACTIVATION);
        }
        return 0;
    }

    /* INTERNAL FUNCTION
       Propagate the error backwards from the output layer.

       After this the train_errors in the hidden layers will be:
       neuron_value_derived * sum(outgoing_weights * connected_neuron)
    */
    void fann_backpropagate_MSE(struct fann *ann)
    {
        fann_type tmp_error;
        unsigned int i;
        struct fann_layer *layer_it;
        struct fann_neuron *neuron_it, *last_neuron;
        struct fann_neuron **connections;

        fann_type *error_begin = ann->train_errors;
        fann_type *error_prev_layer;
        fann_type *weights;
        const struct fann_neuron *first_neuron = ann->first_layer->first_neuron;
        const struct fann_layer *second_layer = ann->first_layer + 1;
        struct fann_layer *last_layer = ann->last_layer;

        /* go through all the layers, from last to first.
         * And propagate the error backwards */
        for(layer_it = last_layer - 1; layer_it > second_layer; --layer_it)
        {
            last_neuron = layer_it->last_neuron;

            /* for each connection in this layer, propagate the error backwards */
            if(ann->connection_rate >= 1)
            {
                if(ann->network_type == FANN_NETTYPE_LAYER)
                {
                    error_prev_layer = error_begin + ((layer_it - 1)->first_neuron - first_neuron);
                }
                else
                {
                    error_prev_layer = error_begin;
                }

                for(neuron_it = layer_it->first_neuron; neuron_it != last_neuron; neuron_it++)
                {

                    tmp_error = error_begin[neuron_it - first_neuron];
                    weights = ann->weights + neuron_it->first_con;
                    for(i = neuron_it->last_con - neuron_it->first_con; i--;)
                    {
                        /*printf("i = %d\n", i);
                         * printf("error_prev_layer[%d] = %f\n", i, error_prev_layer[i]);
                         * printf("weights[%d] = %f\n", i, weights[i]); */
                        error_prev_layer[i] += tmp_error * weights[i];
                    }
                }
            }
            else
            {
                for(neuron_it = layer_it->first_neuron; neuron_it != last_neuron; neuron_it++)
                {

                    tmp_error = error_begin[neuron_it - first_neuron];
                    weights = ann->weights + neuron_it->first_con;
                    connections = ann->connections + neuron_it->first_con;
                    for(i = neuron_it->last_con - neuron_it->first_con; i--;)
                    {
                        error_begin[connections[i] - first_neuron] += tmp_error * weights[i];
                    }
                }
            }

            /* then calculate the actual errors in the previous layer */
            error_prev_layer = error_begin + ((layer_it - 1)->first_neuron - first_neuron);
            last_neuron = (layer_it - 1)->last_neuron;

            for(neuron_it = (layer_it - 1)->first_neuron; neuron_it != last_neuron; neuron_it++)
            {
                *error_prev_layer *= fann_activation_derived(neuron_it->activation_function, 
                    neuron_it->activation_steepness, neuron_it->value, neuron_it->sum);
                error_prev_layer++;
            }
            
        }
    }

    /* INTERNAL FUNCTION
       Update weights for incremental training
    */
    void fann_update_weights(struct fann *ann)
    {
        struct fann_neuron *neuron_it, *last_neuron, *prev_neurons;
        fann_type tmp_error, delta_w, *weights;
        struct fann_layer *layer_it;
        unsigned int i;
        unsigned int num_connections;

        /* store some variabels local for fast access */
        const float learning_rate = ann->learning_rate;
        const float learning_momentum = ann->learning_momentum;        
        struct fann_neuron *first_neuron = ann->first_layer->first_neuron;
        struct fann_layer *first_layer = ann->first_layer;
        const struct fann_layer *last_layer = ann->last_layer;
        fann_type *error_begin = ann->train_errors;
        fann_type *deltas_begin, *weights_deltas;

        /* if no room allocated for the deltas, allocate it now */
        if(ann->prev_weights_deltas == 0)
        {
            ann->prev_weights_deltas =
                (fann_type *) malloc(ann->total_connections_allocated * sizeof(fann_type));
            memset(ann->prev_weights_deltas, 0, ann->total_connections_allocated * sizeof(fann_type));
            if(ann->prev_weights_deltas == 0)
            {
                return;
            }       
        }

        deltas_begin = ann->prev_weights_deltas;
        prev_neurons = first_neuron;
        for(layer_it = (first_layer + 1); layer_it != last_layer; layer_it++)
        {
            last_neuron = layer_it->last_neuron;
            if(ann->connection_rate >= 1)
            {
                if(ann->network_type == FANN_NETTYPE_LAYER)
                {
                    prev_neurons = (layer_it - 1)->first_neuron;
                }
                for(neuron_it = layer_it->first_neuron; neuron_it != last_neuron; neuron_it++)
                {
                    tmp_error = error_begin[neuron_it - first_neuron] * learning_rate;
                    num_connections = neuron_it->last_con - neuron_it->first_con;
                    weights = ann->weights + neuron_it->first_con;
                    weights_deltas = deltas_begin + neuron_it->first_con;
                    for(i = 0; i != num_connections; i++)
                    {
                        delta_w = tmp_error * prev_neurons[i].value + learning_momentum * weights_deltas[i];
                        weights[i] += delta_w ;
                        weights_deltas[i] = delta_w;
                    }
                }
            }
            else
            {
                for(neuron_it = layer_it->first_neuron; neuron_it != last_neuron; neuron_it++)
                {
                    tmp_error = error_begin[neuron_it - first_neuron] * learning_rate;
                    num_connections = neuron_it->last_con - neuron_it->first_con;
                    weights = ann->weights + neuron_it->first_con;
                    weights_deltas = deltas_begin + neuron_it->first_con;
                    for(i = 0; i != num_connections; i++)
                    {
                        delta_w = tmp_error * prev_neurons[i].value + learning_momentum * weights_deltas[i];
                        weights[i] += delta_w;
                        weights_deltas[i] = delta_w;
                    }
                }
            }
        }
    }

};

__END_SYS

#endif
