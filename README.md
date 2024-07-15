# wenv - Windows utility to print all environment variables

Pass `-s` or `--sort` for a case-sensitive sort. Otherwise the variables are
shown in the order they appear in the environment block, which on Windows
should be a case-insensitive sort.

The case-sensitive sort is sometimes useful for comparison to the output of
utilities that sort environment variables from Unix-like systems that way
because they expect them to be wholly unsorted.

## License

[0BSD](LICENSE)
