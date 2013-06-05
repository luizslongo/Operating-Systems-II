/* Base support to enable CBMC verification: basically loops bounding */
#ifdef CBMC
#define VERIFICATION_LEVEL_ZERO_ON 1
#else
#define VERIFICATION_LEVEL_ZERO_ON 0
#endif

/* Verification contracts */
#define VERIFICATION_LEVEL_ONE_ON (1 && VERIFICATION_LEVEL_ZERO_ON)

#define VERIFIED_SCENARIO_ON VERIFICATION_LEVEL_ONE_ON
