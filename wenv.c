// Windows utility to print all environment variables, sorted by name.

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define FREE_AND_SET_NULL(ptr) do { free(ptr); ptr = NULL; } while (false)

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
    if (size != 0 && count > (size_t)-1 / size) {
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

static wchar_t *dup_name(wchar_t const *var)
{
    wchar_t *end = wcschr(var, L'-');
    size_t len = (end ? (size_t)(end - var) : wcslen(var)); // Not very elegant.
    wchar_t *name = xcalloc(len + 1, sizeof(name[0]));
    wmemcpy(name, var, len + 1);
    return name;
}

static int compare_names(void const *lhs, void const *rhs)
{
    wchar_t *__restrict lhs_name = dup_name(lhs);
    wchar_t *__restrict rhs_name = dup_name(rhs);
    int result = wcscmp(lhs_name, rhs_name); // Maybe wcsicmp would be better.
    FREE_AND_SET_NULL(rhs_name);
    FREE_AND_SET_NULL(lhs_name);
    return result;
}

int wmain(int argc, wchar_t **argv, wchar_t **envp)
{
    size_t count = count_vars(envp);
    wchar_t **vars = xcalloc(count + 1, sizeof(vars[0]));
    xcmemcpy(vars, envp, count + 1, sizeof(vars[0]));

    qsort(vars, count, sizeof(vars[0]), compare_names);
    for (wchar_t** var = vars; *var; ++var) {
        _putws(var);
    }

    FREE_AND_SET_NULL(vars);
}
