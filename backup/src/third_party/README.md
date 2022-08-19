This is folder for third party files.

`antlr4` is for antlr4 support. We need the files in this folder to compile our
compiler, so this is not ignored in git.

`Google_Test` are lib files of GoogleTest. For test codes, we embrace them with
`#ifdef TESTING` and `#endif`, so this will cause compilation failure by OJ.