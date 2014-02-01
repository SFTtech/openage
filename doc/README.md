documentation
=============


static docs
-----------

general documentation files reside in the `doc/` directory, where this readme file lies.
you can find ideas, milestones, planning and workflow descriptions etc in here.

a good entry point may be the file
[doc/implementation/project_structure](implementation/project_structure)
as it roughly explains where to find what code in the project.

if you made any relevant discoveries, create a file and write them down.


dynamic docs
------------

dynamic documentation files are generated from comments in the source code, using **doxygen**.
the dynamic docs tend to only describe stuff you could also understand by reading the code itself,
so don't expect too much, use the static docs instead.

after you configured the project, you can invoke

	make doc

to create doxygen html and LaTeX files.


after creation, view them in a browser by

	$(browser) bin/doc/html/index.html


or, if you want to create LaTeX documents:

	make -C bin/doc/latex/ pdf
	$(pdfviewer) bin/doc/latex/refman.pdf
