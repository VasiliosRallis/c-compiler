2017/2018 Compilers Coursework
==============================

Meta-comment
============

_This spec will evolve, as some components will be refined in
lectures in weeks 4, 5, and 6, so you will find some parts that
are still unspecified. This is not a valid reason to avoid
starting, as you have plenty of information to get started._

_There may well be ambiguities or under-specifications in this spec, which
I encourage you to look for (though there is nothing intentionally
in there). To encourage early reading of the spec, there is up to
1% marks bounty for issues that are:
- meaningful (i.e. they are a real issue that matters),
- timely (they are highlighted before most people encounter them), and
- useful (there is a proposed solution).
The reporter generally has precedence in the choice of resolution,
but others can comment on the issue as well before it is resolved.
_

Overview
========

There is a single assessed code deliverable, which is due on Tue 27th March at 22:00.
This deliverable contains three components:

- [*Test suite*](c_test_suite.md) (10%): Each submission must contain a set of test programs which 
  are able to determine whether a given compiler implements certain C language
  features correctly.

- [*C-translator*](c_translator.md) (20%) : Your compiler will be able to translate a sub-set of C
  into equivalent Python. This allows each submission to demonstrate lexing and
  parsing functionality, even if the code generation is weak.

- [*C-compiler*](c_compiler.md) (30%) : This is a compiler from C to MIPS assembly.
  
In all cases, the source language is pre-processed C90. The target environment
is Ubuntu 16.04, so the lab Ubuntu distribution, or equivalently an
Ubuntu 16.04 VM as configured in the attached Vagrantfile. If there is any conflict,
then the VM has precedence.

Associated with all deliverables is [a time-tracking/project management component](management.md) (10%).
This will be updated as you go along, but the final version is due on Fri 30th March at 22:00, then
will be assessed orally at the start of Summer term.


Repositories
============

Each group gets a bare private repository. It is up to you
if you want to clone the master specification, or to start from
scratch. Both approaches just require the git operations already used
in labs and possibly looking at the documentation for the commands
you have already used. Remember that you can always do `git ${CMD} --help`
for any CMD such as `pull`, `push`, `commit`, `remote`.

Starting from scratch
---------------------

1 - Create a local git repository using `git init` in a directory of your choice.

2 - Commit files to your local repository (as you would normally commit)

3 - Add the URL of your private repository as the remote `origin` (similar
    to manually adding the `spec` remote in the lab).

4 - You should now be able to push and pull commits to origin, which is
    your private repo.
    
Cloning the spec
----------------

1 - Clone the master specification from the public URL. This will produce
    a directory called `langproc-2017-cw` by default.
    
2 - Rename it to something more meaningful, e.g. `langproc-2017-cw-${LOGIN}`.

3 - Use `git remote` to rename `origin` (which points at the public spec) to
    `spec`.
    
4 - Use `git remote` to add a new remote called `origin` pointing at
    your private repository.

5 - You should then be able to push and pull to `origin`, and pull from `spec`,
    as in the lab.

Submission
==========

Submission will be via github (code) + blackboard (commit hash),
as in the lab.

All submissions will be tested functionally, and won't be modified
if they don't correctly compile, or if they produce output in the
wrong place or format. To avoid too much jeopardy, there is
the opportunity to submit hashes at three levels:

- Primary: the main submission, with no scaling. Can be submitted up
  to the deadline (Tue 27th Mar).

- Secondary: the main backup, which is scaled by 0.9x. Can be submitted
  up until a day before the deadline (Mon 26th Mar).

- Tertiary: the final fallback, which is scaled by 0.8x. Can be
  submitted up until two days before the deadline (Sun 25th Mar).
  
Each level can be updated as much as you like up until each respective
cut-off. You could just rotate things through as each new hash gets
into the primary, or you could choose to update the secondary and
tertiary at lower frequencies according to confidence (reccommended).

At evaluation time, each of the three levels will be assessed,
and the final mark will be `max(mark(Primary), mark(Secondary)*0.9, mark(Tertiary)*0.8)`

The mark for each level is the combination of all three components
(Parser+Compiler+Test). If a group submits different hashes for the
same level, then the mark for that level will be the lower of the two.

The reason for having multiple submission levels is:

- So that people aren't overly penalised for mistakes due to not testing
  in the target environment.
  
- To manage the tendency for last minute bugs or changes to leak
  into the final verson and stop it compiling.


