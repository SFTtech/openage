Documentation
=============

Most readers love documentation, it's creators hate it.

Unfortunately, nobody can look inside your head and have the same ingenious
thoughts as you do. To ensure your thoughs also reach the afterworld, please
document your stuff reasonably!

The documentation is split up in two parts: *static* and *dynamic* docs,


Static Docs
-----------

Dedicated documentation files reside in the `doc/` directory, where this
readme file lies in as well. You can find ideas, milestones, planning and
workflow descriptions etc in here.

A good entry point may be the file
[doc/implementation/project_structure](implementation/project_structure.md)
as it roughly explains where to find what code in the project.

If **you** made any relevant discoveries or gained insights that help others
understand any part of the project:

* Create a file and write down your thoughts
* Ideally, use markdown syntax, alternatively plain text
* Submit your contribution (pull request) so we can include it in the official
  repository
* This ensures newcomers can start developing easily


Dynamic Docs
------------

Dynamic documentation files are generated from comments in the source code,
using **doxygen**. The dynamic docs tend to only describe stuff you could also
understand by reading the code itself, so don't expect too much, read the
static docs for conceptual insights instead.

Doxygen will generate nice shiny graphs of code correlations, this might help
getting an overview of the code components and their interaction.

After you configured the project, you can invoke

	make doc

to create doxygen html and LaTeX files.


After creation, view them in a browser by

	$(browser) bin/doc/html/index.html


or, if you want to create LaTeX documents, run

	make -C bin/doc/latex/ pdf
	$(pdfviewer) bin/doc/latex/refman.pdf


The quality of the dynamic documentation is dependent on the people writing
code, so please don't forget to document your contributions.
