/*
 * regex_demo — a tiny portable smoke test for the JMC portable layer.
 *
 * It exists to prove that the cross-platform pieces of this Windows project
 * actually build and run on a non-Windows host: it links the vendored PCRE
 * regex engine (jmc::pcre) and, when available, the optional system
 * dependencies (zlib, wolfSSL). Building and running this is the local
 * "does it work?" check on macOS/Linux.
 */
#include <stdio.h>
#include <string.h>

#include <pcre.h>

#ifdef HAVE_ZLIB
#include <zlib.h>
#endif

#ifdef HAVE_WOLFSSL
#include <wolfssl/version.h>
#endif

int main(void)
{
    const char  *error = NULL;
    int          erroffset = 0;
    const char  *pattern = "(\\w+)@(\\w+)\\.(\\w+)";
    const char  *subject = "contact alice@example.com please";
    int          ovector[30];

    printf("JMC portable layer smoke test\n");
    printf("  PCRE version : %s\n", pcre_version());
#ifdef HAVE_ZLIB
    printf("  zlib version : %s\n", zlibVersion());
#else
    printf("  zlib         : not linked\n");
#endif
#ifdef HAVE_WOLFSSL
    printf("  wolfSSL      : %s\n", LIBWOLFSSL_VERSION_STRING);
#else
    printf("  wolfSSL      : not linked\n");
#endif

    pcre *re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
    if (re == NULL) {
        fprintf(stderr, "PCRE compile failed at offset %d: %s\n", erroffset, error);
        return 1;
    }

    int rc = pcre_exec(re, NULL, subject, (int)strlen(subject), 0, 0, ovector, 30);
    if (rc < 0) {
        printf("  regex match  : (no match)\n");
    } else {
        printf("  regex match  : '%.*s'\n",
               ovector[1] - ovector[0], subject + ovector[0]);
    }
    pcre_free(re);

    printf("OK\n");
    return 0;
}
