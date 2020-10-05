# VT Exercises WS 2020/2021

This repo will contain the sources for the VT Exercises of WS 2020/2021.

You will have access to two repositories:
1) The public repository "upstream", where we provide all task descriptions and frameworks
2) Your personal repository, which you should use for developing your solutions and submitting them.

At the time when your personal repository is created, the content of both should be exactly this file.


## Using the Upstream Repo

To get the source files for each assignment, you first need to add upstream as additional remote repository:

```sh
git remote add -f upstream git@extgit.iaik.tugraz.at:scos/scos.teaching/scos.teaching.vt/vt2020/vt-sources.git
```

After you added upstream as a remote, you can pull changes from upstream with the command:

```sh
git fetch upstream
git checkout master
git merge upstream/master
git push
```

Whenever we change something in the upstream repository (Adding new assignments, bugfixes, etc.),
we will notify you via the newsgroup.

## Further information will be provided with the first assignment.

We wish you good luck and have fun with the excercises!
