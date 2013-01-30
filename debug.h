#ifdef DEBUG
/* Aids printf-style debugging.
 *
 * If you rely on printf() extensively for your debugging, DPRINTF()
 * might be slightly better and can be quickly put to use by simply
 * "calling" DPRINTF() instead of printf().
 *
 * The objectives of DPRINTF() are two-fold:
 *
 * 1) Identify with filename, function name, line number the location
 * of the print statement. This could help if you have many printf()
 * statements.
 *
 * 2) Can be quickly enabled/disabled simply by re-compiling,
 * _without_ touching the code (e.g., commenting/uncommenting). This
 * could save time and help minimize the risk of unintentional code
 * modifications.
 *
 * Usage:
 *
 * Copy-paste all the lines between (and including) the lines "#ifdef
 * DEBUG" and "#endif" to the top (or after the "#include"s) of every
 * .c file where you want to use DPRINTF().
 *
 * To use it in your code, just call it as you do printf().
 *
 * To enable DPRINTF(), use these two make commands:
 *
 * make clean
 * FLAGS+="-DDEBUG" make
 *
 * If you just use "make" without the FLAGS+="-DDEBUG", then DPRINTF()
 * will not print anything (to be exact, it's as if all the DPRINTF()
 * calls were commented out).
 *
 *
 *
 * !! NOTE !!
 *
 * This is only for your printf-style debugging. The auto-grader will
 * NOT enable DEBUG or DPRINTF(). Your MP logic must NOT depend on
 * DEBUG or DPRINTF().
 *
 * This is not thread-safe.
 *
*/
#define DPRINTF(fmt, ...) \
       fprintf(stderr, "%s(), %s:%d: " fmt, \
           __func__, __FILE__, __LINE__, ##__VA_ARGS__)
#else
/* no-op */
#define DPRINTF(...)
#endif
