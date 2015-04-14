Documentation
=============

Most readers love documentation, its creators hate it.

Unfortunately, nobody can look inside your head and have the same ingenious
thoughts as you do. To ensure your thoughts also reach the afterworld, please
document your stuff reasonably!

The documentation is split up in two parts: *static* and *dynamic* docs,


Static Docs
-----------

Dedicated documentation files reside in the `doc/` directory, where this
readme file lies in as well. You can find ideas, milestones, planning and
workflow descriptions etc in here.

a good entry point may be the file [doc/implementation/project_structure](implementation/project_structure.md).
it roughly explains where to find what code in the project.

If **you** made any relevant discoveries or gained insights that help others
understand any part of the project:

* Create a file and write down your thoughts
* Ideally, use markdown syntax, alternatively plain text
* Submit your contribution (pull request) so we can include it in the official
  repository
* This ensures newcomers can start developing easily


Dynamic Docs
------------

This type of documentation is written inside code files:

 - C++ methods and classes are documented in their header files
 - Python methods and classes are documented via docstrings

We're lazy, so the above statements might be out-right fabrications.


### Doc Generation

Dynamic documentation can be auto-generated from source documentation using **doxygen**.

after calling `./configure`, you can invoke

	make doc

to create doxygen html and LaTeX files.


After creation, view them in a browser by

	$BROWSER bin/doc/html/index.html

or, if you want to create LaTeX documents, run

	make -C bin/doc/latex/ pdf
	$PDFVIEWER bin/doc/latex/refman.pdf
