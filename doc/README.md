documentation
=============


static docs
-----------

general documentation files reside in the `doc/` directory, where this readme file lies.
you can find ideas, milestones, planning and workflow descriptions etc in here.

a good entry point may be the file [doc/implementation/project_structure](implementation/project_structure).
it roughly explains where to find what code in the project.

if you made any relevant discoveries, create a file and write them down.


code docs
---------

 - C++ methods and classes are documented in their header files
 - Python methods and classes are documented via docstrings

We're lazy, so the above statements might be be out-right fabrications.


dynamic docs
------------

dynamic documentation can be auto-generated from source documentation using **doxygen**.

after calling `./configure`, you can invoke

	`make doc`

to create doxygen html and LaTeX files.


after creation, view them in a browser by

	`$BROWSER bin/doc/html/index.html`


or, if you want to create LaTeX documents:

	`make -C bin/doc/latex/ pdf`
	`$PDFVIEWER bin/doc/latex/refman.pdf`
