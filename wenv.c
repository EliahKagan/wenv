// Windows utility to print all environment variables, sorted by name.

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define FREE_AND_SET_NULL(ptr) do { free(ptr); ptr = NULL; } while (false)

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

static size_t count_vars(wchar_t **envp)
{
    size_t count = 0;
    for (; *envp; ++envp) {
        ++count;
    }
    return count;
}

static inline size_t name_length(wchar_t *const var)
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

int wmain(int argc, wchar_t **argv, wchar_t **envp)
{
    size_t count = count_vars(envp);
    wchar_t **vars = xcalloc(count + 1, sizeof(vars[0]));
    xcmemcpy(vars, envp, count + 1, sizeof(vars[0]));

    qsort(vars, count, sizeof(vars[0]), compare_names_for_qsort);
    for (wchar_t **varp = vars; *varp; ++varp) {
        _putws(*varp);
    }

    FREE_AND_SET_NULL(vars);
}
