#ifndef TSH_SHARED_H
#define TSH_SHARED_H

//#define DEBUG

// DEBUG_MSG will print only if DEBUG is defined
#ifdef DEBUG
#define DEBUG_MSG(str)                                                         \
    do {                                                                       \
        std::cout << str << std::endl;                                         \
    } while (false)
#else
#define DEBUG_MSG(str)                                                         \
    do {                                                                       \
    } while (false)
#endif

#endif // TSH_SHARED_H
