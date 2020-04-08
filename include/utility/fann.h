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
    static const unsigned int NUM_LAYERS_CONFIG           = 17;
    static constexpr float    LEARNING_RATE_CONFIG        = 0.700000;
    static constexpr float    CONNECTION_RATE_CONFIG      = 1.000000;
    static const unsigned int NETWORK_TYPE_CONFIG         = 1;
    static constexpr float    LEARNING_MOMENTUM_CONFIG    = 0.000000;
    static const unsigned int TRAINING_ALGORITHM_CONFIG   = 0; // 0 = INCREMENTAL - for online training support
    static const unsigned int TRAIN_ERROR_FUNCTION_CONFIG = 0;
    static const unsigned int TRAIN_STOP_FUNCTION_CONFIG  = 1;
    static constexpr double   BIT_FAIL_LIMIT_CONFIG       = 8.99999976158142089844e-01;

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
        const unsigned int layers_sizes[] = { 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3 };

        const float neurons_config[][3] = {
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {0,0,0.00000000000000000000e+00},
            {10,6,1.00000000000000000000e+00},
            {11,6,1.00000000000000000000e+00},
            {12,6,7.50000000000000000000e-01},
            {13,6,2.50000000000000000000e-01},
            {14,6,5.00000000000000000000e-01},
            {15,6,2.50000000000000000000e-01},
            {16,6,5.00000000000000000000e-01},
            {17,6,5.00000000000000000000e-01},
            {18,6,7.50000000000000000000e-01},
            {19,6,7.50000000000000000000e-01},
            {20,6,1.00000000000000000000e+00},
            {21,6,5.00000000000000000000e-01},
            {22,6,7.50000000000000000000e-01},
            {23,6,1.00000000000000000000e+00},
            {24,6,7.50000000000000000000e-01},
            {25,0,5.00000000000000000000e-01},
            {25,0,5.00000000000000000000e-01},
            {25,0,5.00000000000000000000e-01}
        };

        const float weights_config[][3] = {
            {0,    1.26327168941497802734e+00},
            {1,    6.54878914356231689453e-01},
            {2,    1.83350110054016113281e+00},
            {3,    3.59417700767517089844e+00},
            {4,    -2.01685667037963867188e+00},
            {5,    -4.99663874506950378418e-02},
            {6,    5.38370656967163085938e+00},
            {7,    3.74242067337036132812e-01},
            {8,    5.64252758026123046875e+00},
            {9,    1.62813320755958557129e-01},
            {0,    1.67902970314025878906e+00},
            {1,    2.60840058326721191406e+00},
            {2,    3.94318413734436035156e+00},
            {3,    2.67653197050094604492e-01},
            {4,    -3.97427630424499511719e+00},
            {5,    -2.66997814178466796875e-01},
            {6,    8.61923885345458984375e+00},
            {7,    -1.04117739200592041016e+00},
            {8,    3.03706812858581542969e+00},
            {9,    9.94453966617584228516e-01},
            {10,    -1.68369019031524658203e+00},
            {0,    2.39638161659240722656e+00},
            {1,    1.32551920413970947266e+00},
            {2,    -7.71384984254837036133e-02},
            {3,    6.44551739096641540527e-02},
            {4,    3.46077829599380493164e-01},
            {5,    5.71304976940155029297e-01},
            {6,    4.96660518646240234375e+00},
            {7,    -1.14939260482788085938e+00},
            {8,    8.26657390594482421875e+00},
            {9,    2.65614080429077148438e+00},
            {10,    6.16646111011505126953e-01},
            {11,    -9.13169670104980468750e+00},
            {0,    -8.34261536598205566406e-01},
            {1,    2.24617743492126464844e+00},
            {2,    6.79860496520996093750e+00},
            {3,    3.02236318588256835938e+00},
            {4,    -6.27488040924072265625e+00},
            {5,    2.80665063858032226562e+00},
            {6,    1.47890548706054687500e+01},
            {7,    2.96838116645812988281e+00},
            {8,    5.79018878936767578125e+00},
            {9,    -2.24370217323303222656e+00},
            {10,    -7.89156436920166015625e+00},
            {11,    5.83343172073364257812e+00},
            {12,    1.17179031372070312500e+01},
            {0,    7.72286057472229003906e-01},
            {1,    1.25700628757476806641e+00},
            {2,    -6.24369204044342041016e-01},
            {3,    -3.69885206222534179688e+00},
            {4,    1.61221593618392944336e-01},
            {5,    -1.87580263614654541016e+00},
            {6,    2.85330939292907714844e+00},
            {7,    3.37929248809814453125e-01},
            {8,    1.06835925579071044922e+00},
            {9,    4.00542736053466796875e+00},
            {10,    9.43595504760742187500e+00},
            {11,    -3.09908008575439453125e+00},
            {12,    -7.66023933887481689453e-01},
            {13,    -5.64976394176483154297e-01},
            {0,    -8.94336128234863281250e+00},
            {1,    -9.47185516357421875000e+00},
            {2,    1.30909976959228515625e+01},
            {3,    1.10350978374481201172e+00},
            {4,    -2.35554397106170654297e-01},
            {5,    7.11863040924072265625e+00},
            {6,    -2.41921291351318359375e+01},
            {7,    -9.03717899322509765625e+00},
            {8,    2.82997751235961914062e+00},
            {9,    2.33158326148986816406e+00},
            {10,    -3.52091956138610839844e+00},
            {11,    6.49055957794189453125e+00},
            {12,    1.48341436386108398438e+01},
            {13,    -6.45331001281738281250e+00},
            {14,    -3.73881649971008300781e+00},
            {0,    3.81438672542572021484e-01},
            {1,    2.17617630958557128906e+00},
            {2,    4.44873523712158203125e+00},
            {3,    1.28084695339202880859e+00},
            {4,    -3.98957610130310058594e+00},
            {5,    1.88784688711166381836e-01},
            {6,    1.23313608169555664062e+01},
            {7,    1.60289430618286132812e+00},
            {8,    5.03243637084960937500e+00},
            {9,    3.06867927312850952148e-01},
            {10,    -7.74678111076354980469e-01},
            {11,    1.00011634826660156250e+00},
            {12,    7.61370706558227539062e+00},
            {13,    -6.19338893890380859375e+00},
            {14,    -2.84726500511169433594e+00},
            {15,    -5.40651798248291015625e-01},
            {0,    -7.25784635543823242188e+00},
            {1,    7.05714881420135498047e-01},
            {2,    3.00475931167602539062e+00},
            {3,    3.99530053138732910156e-01},
            {4,    -1.40127027034759521484e+00},
            {5,    1.01527376174926757812e+01},
            {6,    1.14767904281616210938e+01},
            {7,    -8.41459095478057861328e-01},
            {8,    7.18521881103515625000e+00},
            {9,    -3.98597359657287597656e-01},
            {10,    -1.00271606445312500000e+00},
            {11,    3.01425099372863769531e+00},
            {12,    2.04555964469909667969e+00},
            {13,    -8.81642758846282958984e-01},
            {14,    1.04011793136596679688e+01},
            {15,    9.40970134735107421875e+00},
            {16,    -1.89687758684158325195e-01},
            {0,    -8.74473154544830322266e-01},
            {1,    -1.46229833364486694336e-01},
            {2,    4.40053701400756835938e+00},
            {3,    1.04389750957489013672e+00},
            {4,    -7.68534064292907714844e-01},
            {5,    8.42867255210876464844e-01},
            {6,    2.45778560638427734375e+00},
            {7,    -4.71545124053955078125e+00},
            {8,    -1.12139523029327392578e+00},
            {9,    2.97574949264526367188e+00},
            {10,    9.82139289379119873047e-01},
            {11,    5.49111795425415039062e+00},
            {12,    3.54051876068115234375e+00},
            {13,    -7.75195837020874023438e-01},
            {14,    2.03691077232360839844e+00},
            {15,    1.31558811664581298828e+00},
            {16,    -1.10676527023315429688e+00},
            {17,    -1.10886895656585693359e+00},
            {0,    4.60254848003387451172e-01},
            {1,    7.42774903774261474609e-01},
            {2,    4.67058610916137695312e+00},
            {3,    4.96621102094650268555e-01},
            {4,    5.88040781021118164062e+00},
            {5,    -2.82156419754028320312e+00},
            {6,    2.97375679016113281250e+00},
            {7,    3.46547675132751464844e+00},
            {8,    -5.56206524372100830078e-01},
            {9,    2.40502548217773437500e+00},
            {10,    7.18765079975128173828e-01},
            {11,    -9.98660027980804443359e-01},
            {12,    3.94433188438415527344e+00},
            {13,    5.03076076507568359375e+00},
            {14,    -7.88409185409545898438e+00},
            {15,    1.43840742111206054688e+00},
            {16,    4.05248117446899414062e+00},
            {17,    -2.22425460815429687500e-01},
            {18,    -4.36920315027236938477e-01},
            {0,    1.93748974800109863281e+00},
            {1,    -1.41296005249023437500e+00},
            {2,    -2.16529178619384765625e+00},
            {3,    2.68419962376356124878e-02},
            {4,    3.27911943197250366211e-01},
            {5,    2.53095412254333496094e+00},
            {6,    1.14249157905578613281e+00},
            {7,    5.53513622283935546875e+00},
            {8,    -1.16948747634887695312e+00},
            {9,    2.27556896209716796875e+00},
            {10,    -1.62978172302246093750e+00},
            {11,    2.07252526283264160156e+00},
            {12,    1.64875769615173339844e+00},
            {13,    9.38027918338775634766e-01},
            {14,    -2.39964991807937622070e-01},
            {15,    1.72623127698898315430e-01},
            {16,    -3.21745443344116210938e+00},
            {17,    3.75787425041198730469e+00},
            {18,    -1.35160660743713378906e+00},
            {19,    3.06177854537963867188e-01},
            {0,    -8.84332060813903808594e-02},
            {1,    7.77808487415313720703e-01},
            {2,    2.10005664825439453125e+00},
            {3,    1.30352330207824707031e+00},
            {4,    -2.38934278488159179688e+00},
            {5,    6.72004318237304687500e+00},
            {6,    1.66739101409912109375e+01},
            {7,    5.87293319404125213623e-02},
            {8,    -1.19204473495483398438e+00},
            {9,    9.35263812541961669922e-01},
            {10,    1.74126684665679931641e-01},
            {11,    4.28893995285034179688e+00},
            {12,    5.67036294937133789062e+00},
            {13,    -4.63194340467453002930e-01},
            {14,    3.08466464281082153320e-01},
            {15,    1.54952847957611083984e+00},
            {16,    7.64892959594726562500e+00},
            {17,    2.85534799098968505859e-01},
            {18,    1.01049985885620117188e+01},
            {19,    -2.26045918464660644531e+00},
            {20,    2.33272504806518554688e+00},
            {0,    9.88289892673492431641e-01},
            {1,    1.21093702316284179688e+00},
            {2,    3.76571631431579589844e+00},
            {3,    3.98208796977996826172e-01},
            {4,    2.41109585762023925781e+00},
            {5,    1.29377210140228271484e+00},
            {6,    4.19175958633422851562e+00},
            {7,    7.39924788475036621094e-01},
            {8,    -7.47044038772583007812e+00},
            {9,    2.13470959663391113281e+00},
            {10,    1.08384513854980468750e+00},
            {11,    -6.07554864883422851562e+00},
            {12,    4.20344638824462890625e+00},
            {13,    2.90761613845825195312e+00},
            {14,    -2.40946650505065917969e+00},
            {15,    -1.50617682933807373047e+00},
            {16,    -8.93021941184997558594e-01},
            {17,    2.30060935020446777344e+00},
            {18,    1.07113194465637207031e+00},
            {19,    -4.57783794403076171875e+00},
            {20,    3.50838088989257812500e+00},
            {21,    1.47683036327362060547e+00},
            {0,    -1.54493415355682373047e+00},
            {1,    1.01479649543762207031e+00},
            {2,    3.36310029029846191406e+00},
            {3,    2.82338529825210571289e-01},
            {4,    2.92293876409530639648e-01},
            {5,    -1.10788393020629882812e+00},
            {6,    6.40018653869628906250e+00},
            {7,    5.02650320529937744141e-01},
            {8,    -2.30320119857788085938e+00},
            {9,    1.93744695186614990234e+00},
            {10,    -4.73739475011825561523e-01},
            {11,    3.35143983364105224609e-01},
            {12,    -1.47177231311798095703e+00},
            {13,    5.74229434132575988770e-02},
            {14,    2.36898398399353027344e+00},
            {15,    -3.82639169692993164062e-02},
            {16,    -3.18981736898422241211e-01},
            {17,    2.70328330993652343750e+00},
            {18,    -1.51707112789154052734e-01},
            {19,    1.77977412939071655273e-01},
            {20,    -1.39676415920257568359e+00},
            {21,    -3.23586225509643554688e-01},
            {22,    2.45679950714111328125e+00},
            {0,    -1.62947177886962890625e+00},
            {1,    2.27357244491577148438e+00},
            {2,    1.54644227027893066406e+00},
            {3,    -2.48185172677040100098e-01},
            {4,    -3.12840080261230468750e+00},
            {5,    7.56934881210327148438e-01},
            {6,    -2.20591282844543457031e+00},
            {7,    1.34594750404357910156e+00},
            {8,    -2.84071588516235351562e+00},
            {9,    3.57151675224304199219e+00},
            {10,    1.59835267066955566406e+00},
            {11,    7.38144755363464355469e-01},
            {12,    3.14941072463989257812e+00},
            {13,    -1.40159523487091064453e+00},
            {14,    -3.04135382175445556641e-01},
            {15,    8.11584234237670898438e-01},
            {16,    4.26151990890502929688e+00},
            {17,    -2.02612066268920898438e+00},
            {18,    5.73584270477294921875e+00},
            {19,    -1.92749273777008056641e+00},
            {20,    8.02527442574501037598e-02},
            {21,    -2.87347745895385742188e+00},
            {22,    7.87142693996429443359e-01},
            {23,    7.85444140434265136719e-01},
            {0,    -1.92575013637542724609e+00},
            {1,    4.67005968093872070312e-01},
            {2,    1.29160320758819580078e+00},
            {3,    3.58866548538208007812e+00},
            {4,    -5.50615370273590087891e-01},
            {5,    1.48397314548492431641e+00},
            {6,    -3.16143870353698730469e+00},
            {7,    -1.03818511962890625000e+00},
            {8,    1.41545021533966064453e+00},
            {9,    1.67395913600921630859e+00},
            {10,    -1.16458714008331298828e+00},
            {11,    -6.18224823847413063049e-03},
            {12,    2.11950850486755371094e+00},
            {13,    -1.05901622772216796875e+00},
            {14,    -4.24276024103164672852e-01},
            {15,    -5.61868309974670410156e-01},
            {16,    2.84679234027862548828e-01},
            {17,    -5.52672147750854492188e-01},
            {18,    6.41588211059570312500e-01},
            {19,    -3.88300448656082153320e-01},
            {20,    3.90905737876892089844e-01},
            {21,    -2.62573987245559692383e-01},
            {22,    -1.50160670280456542969e-01},
            {23,    1.24500140547752380371e-01},
            {24,    -1.44523710012435913086e-01},
            {0,    3.80137920379638671875e-01},
            {1,    -6.54302677139639854431e-03},
            {2,    -1.12860190868377685547e+00},
            {3,    1.31042706966400146484e+00},
            {4,    1.06280601024627685547e+00},
            {5,    -8.38441729545593261719e-01},
            {6,    -2.41798090934753417969e+00},
            {7,    -8.87601315975189208984e-01},
            {8,    -1.18400108814239501953e+00},
            {9,    2.49760127067565917969e+00},
            {10,    3.75351047515869140625e+00},
            {11,    -1.38134694099426269531e+00},
            {12,    -2.29378104209899902344e+00},
            {13,    1.61364936828613281250e+00},
            {14,    -4.24875110387802124023e-01},
            {15,    3.06875914335250854492e-01},
            {16,    5.02546072006225585938e-01},
            {17,    -1.37169361114501953125e-01},
            {18,    -4.01942878961563110352e-01},
            {19,    -5.45676685869693756104e-02},
            {20,    3.50522063672542572021e-02},
            {21,    1.28568604588508605957e-01},
            {22,    -2.12472453713417053223e-01},
            {23,    1.29429683089256286621e-01},
            {24,    2.66720112413167953491e-02},
            {0,    1.55470824241638183594e+00},
            {1,    -3.24909061193466186523e-01},
            {2,    3.56730759143829345703e-01},
            {3,    2.52986907958984375000e+00},
            {4,    -1.02092349529266357422e+00},
            {5,    -2.64414072036743164062e-01},
            {6,    5.78840255737304687500e+00},
            {7,    1.85201394557952880859e+00},
            {8,    -2.25960269570350646973e-01},
            {9,    1.64854288101196289062e+00},
            {10,    -2.58544230461120605469e+00},
            {11,    1.43412530422210693359e+00},
            {12,    2.78845548629760742188e-01},
            {13,    -6.33705437183380126953e-01},
            {14,    8.45022201538085937500e-01},
            {15,    3.02834630012512207031e-01},
            {16,    -8.68600726127624511719e-01},
            {17,    6.97082698345184326172e-01},
            {18,    -2.51709192991256713867e-01},
            {19,    4.40206646919250488281e-01},
            {20,    -4.37593817710876464844e-01},
            {21,    1.35906562209129333496e-01},
            {22,    3.36984902620315551758e-01},
            {23,    -2.52627998590469360352e-01},
            {24,    1.09454974532127380371e-01}
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

        /* SCALE NOT INCLUDED IN OUR FANN */

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
        const fann_type fann_max[] = {367119., 1330863., 5608278., 2000006., 17562780., 5072266., 100., 100., 7.};
        const fann_type fann_min[] = {0., 0., 2032., 0., 0., 603., 0., 0., 1.};

        for (unsigned int i = 0; i < input_size; i++)
        {
            if(input[i] > fann_max[i])
                input[i] = fann_max[i];
            else if (input[i] < fann_min[i])
                input[i] = fann_min[i];
            else
                input[i] = 2*((input[i] - fann_min[i])/(fann_max[i] - fann_min[i])) - 1;
        }

        return input;
    }

    static void fann_set_train_in_out(struct fann* ann, fann_type * input, fann_type * output) {
        unsigned int i, num_input, num_output;
        num_input = ann->num_input;
        input = scale_input(input, num_input);
        /* first set the input */
        struct fann_neuron *first_neuron = ann->first_layer->first_neuron;
        for(i = 0; i != num_input; i++) {
            first_neuron[i].value = input[i];
        }

        /* set the output */
        num_output = ann->num_output;
        struct fann_neuron *neurons = (ann->last_layer - 1)->first_neuron;
        for(i = 0; i != num_output; i++)
        {
            ann->output[i] = output[i];
            neurons[i].value = output[i];
        }
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
