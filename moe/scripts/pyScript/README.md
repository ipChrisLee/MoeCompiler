# Intro

This folder is for python scripts.

Run `./init.sh` every time before coding to update dependency.

And add dependencies to `MoePyEnvConfig.yml` every time you include some python libraries.

View [conda Tutorial](https://docs.conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html#updating-an-environment) to get information about conda environment management.

# Coding Notice

You can code in pycharm/clion, but NOTICE that `MoeCompiler/moe` is always the 
working directory.

This is not a part of compiler, but a part of the project. The duty of these
python scripts is to check compiler, detect the errors and faults in compiler
and evaluate compiler performance.