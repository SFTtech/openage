tabs vs spaces
==============

or: how to begin a codestyle flamewar.


disclaimer: the tab-space discussion will never have a final solution.
            this is mainly JJ's attempt of uniting the world.


introduction
------------

most [sft]tech guys prefer tabs over spaces, as they use less memory,
and grant more freedom and flexibility. and we love freedom.

tabs:

* PRO: you can configure their width
* PRO: they use only one byte memory
* CON: some idiots think they type tabs,
       but insert spaces and the whitespace consistency faks up
* CON: alignment of things fuck up because of flexible width (ascii art...)

spaces:

* PRO: always have a fixed size -> can be used for alignment
* PRO: any idiot can press the space bar for making space
* CON: always have a fixed size -> not configurable for indentations

=> we love flexibility and would like tabs for indentation.
   so, we are using tabs for indenting the C++ code.
   but: alignments by tabs are impossible to be consistent.
   => spaces are needed for ascii arts and alignments


solution: use BOTH tabs and spaces.
--------

yes, more idiots will fail writing it the correct way,
we have to ignore them or fix their code.

example:

```
--->  == tab
.     == space

class LØL {
--->int gschicht;

--->void add_gschicht(struct my_struct_with_long_name *ie,
--->..................int addition) {
--->--->this->gschicht += ie->member + addition;
--->}
};
```

yeah, i'm also ok when you write all the parameters in one line,
i think this proposal's readability is better though.

this example should show you that with this technique:

* you keep the adjustable tab-width
* you keep the consistent alignments
* you may now travel to the nearest flower meadow
  and start picking your favorite plants

```
=> use tabs for indentation
=> use spaces for alignment
```

python
======

problem description
-------------------

the previous section proposes a perfect indentation method.
this all seems nice and rainbow-vomiting good.
but, the python developers decided to use 4 spaces instead of one tab
for their indentation, see PEP8.

for a long time, JJ was rebellious against using spaces for indentation
in python and used tabs, but he now gave up.

too many people complained about breaking rules, and the worst issue:
"i won't reconfigure my editor for you"

=> this led to searching for a solution that satisfies everybody:

* blabla, standard, guidelines, use spaces, PEP8 !!1111
* wraaah, i wanna use tabs, PEP8 can suck my %s

and, you may have guessed it, we found one.

solution
--------

and who will save us? GIT!

why git? git manages the files in your/our project folder
and sends stuff to remote repos.

=> let git replace tabs n spaces they way you like it.


that way:

* you may still use tabs transparently on your local machine for python code
* but the repo will contain spaces only

=> your files will have tabs, but git stores them as spaces.

wheeeee \o/


solution setup
--------------

in your local repo, create/edit .git/info/attributes:

    *.py  filter=tabspace

then configure the repository:

    git config filter.tabspace.smudge 'unexpand --tabs=4 --first-only'
    git config filter.tabspace.clean    'expand --tabs=4 --initial'

`expand` and `unexpand` are part of the GNU coreutils,
you can adapt the invocation at your needs (see `man 1 expand`).

you can use `git config --global ...` to store these filters in your
`~/.gitconfig` instead of saving them into this repo.

update all files in the local repo to have tabs instead of spaces:

    git checkout HEAD -- **

when you commit and push your code, the tabs will be saved as spaces.


conclusion
==========

the hours spent on the tabs-vs-spaces flamewars will never end.
please people, maybe these proposals will shut you up.
instead of flaming the shit out of each, you could have contributed epic code to openage.

so stop flaming, and code stuff. with love.

i hate you all. seriously.
