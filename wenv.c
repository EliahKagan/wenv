// Windows utility to print all environment variables.
//
// Pass -s or --sort for a case-sensitive sort. Otherwise the variables are
// shown in the order they appear in the environment block, which on Windows
// should be a case-insensitive sort.

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static inline size_t minimum(size_t x, size_t y)
{
    return y < x ? y : x;
}

static void *xcalloc(size_t count, size_t size)
{
    void *ptr = calloc(count, size);
    if (!ptr && count != 0 && size != 0) {
        abort();
    }
    return ptr;
}

static void *xcmemcpy(void *__restrict dest, const void *__restrict src,
                      size_t count, size_t size)
{
    if (size != 0 && count > SIZE_MAX / size) {
        abort(); // This is to avoid arithmetic overflow.
    }
    return memcpy(dest, src, count * size);
}

struct config {
    bool help;
    bool sort;
    wchar_t const* unrecognized;
};

static inline bool is_match(wchar_t const *arg,
                            wchar_t const *short_opt,
                            wchar_t const *long_opt)
{
    return wcscmp(arg, short_opt) == 0
        || wcscmp(arg, long_opt) == 0;
}

static void parse_config(int argc, wchar_t **argv, struct config *cfgp)
{
    cfgp->help = false;
    cfgp->sort = false;
    cfgp->unrecognized = NULL;

    for (int i = 1; i < argc; ++i) {
        if (is_match(argv[i], L"-h", L"--help")) {
            cfgp->help = true;
        } else if (is_match(argv[i], L"-s", L"--sort")) {
            cfgp->sort = true;
        } else if (!cfgp->unrecognized) {
            cfgp->unrecognized = argv[i];
        }
    }
}

static size_t count_vars(wchar_t **envp)
{
    size_t count = 0;
    for (; *envp; ++envp) {
        ++count;
    }
    return count;
}

static inline size_t name_length(wchar_t const *var)
{
    wchar_t const* end = wcschr(var, L'=');
    return end ? (size_t)(end - var) : wcslen(var);
}

static int compare_names(wchar_t const *lhs, wchar_t const *rhs)
{
    size_t lhs_len = name_length(lhs);
    size_t rhs_len = name_length(rhs);

    int prefix_result = wcsncmp(lhs, rhs, minimum(lhs_len, rhs_len));

    if (prefix_result != 0) {
        return prefix_result;
    }
    if (lhs_len < rhs_len) {
        return -1;
    }
    if (lhs_len > rhs_len) {
        return 1;
    }
    return 0;
}

static int compare_names_for_qsort(void const *lhspv, void const *rhspv)
{
    return compare_names(*(wchar_t const **)lhspv, *(wchar_t const **)rhspv);
}

static void print_vars(wchar_t **vars)
{
    for (wchar_t** varp = vars; *varp; ++varp) {
        _putws(*varp);
    }
}

static void print_vars_sorted(wchar_t** envp)
{
    size_t count = count_vars(envp);
    wchar_t **vars = xcalloc(count + 1, sizeof(vars[0]));
    xcmemcpy(vars, envp, count + 1, sizeof(vars[0]));

    qsort(vars, count, sizeof(vars[0]), compare_names_for_qsort);
    print_vars(vars);

    free(vars);
}

int wmain(int argc, wchar_t **argv, wchar_t **envp)
{
    struct config cfg = { 0 };
    parse_config(argc, argv, &cfg);

    if (cfg.help) {
        fwprintf(stderr, L"Usage:  %s [--sort|-s]\n", argv[0]);
        fwprintf(stderr, L"\n  Shows environment variables, one per line.\n");
        fwprintf(stderr, L"\n  Pass --sort or -s to sort case-sensitively.\n");
        return cfg.unrecognized ? EXIT_FAILURE : EXIT_SUCCESS;
    }
    if (cfg.unrecognized) {
        fwprintf(stderr, L"%s: error: unrecognized option: %s\n", argv[0], cfg.unrecognized);
        return EXIT_FAILURE;
    }

    if (cfg.sort) {
        print_vars_sorted(envp);
    } else {
        print_vars(envp);
    }

    return EXIT_SUCCESS;
}
