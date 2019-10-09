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

A good entry point may be the file [doc/project_structure](/doc/project_structure.md).
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

Due to potential documentation laziness,
the above statements might be out-right fabrications.


### Doc Generation

Dynamic documentation can be auto-generated from source documentation using **doxygen**.

after calling `./configure`, you can invoke

```
make doc
```

to create doxygen html and LaTeX files.


After creation, view them in a browser by

```
$BROWSER bin/doc/html/index.html
```

or, if you want to create LaTeX documents, run

```
make -C bin/doc/latex/ pdf
$PDFVIEWER bin/doc/latex/refman.pdf
```

### Webdoc Generation

#### Install Dependencies

You will need nearly all the basic dependencies as if you would build openage itself. Namely *cmake, make, python3, nyan, etc.* Make sure you have these.

Furthermore you'll need the following:

```
sudo apt update
sudo apt install doxygen graphwiz

python -m pip install --upgrade pip

pip install -U sphinx doxypypy recommonmark autodoc breathe graphviz sphinx-markdown-tables sphinx-rtd-theme sphinxcontrib-moderncmakedomain
```
#### Building Documentation

Dynamic documentation for webhosting-purposes can be auto-generated from source documentation using **doxygen** in the first stage with **sphinx** and **breathe** in the second stage.

after calling `./configure --build-webdoc`, you can invoke the complete building process with

```
make webdoc
```

Doxygen is taking care of the documentation of the C++ parts. Although Doxypypy will take care of the Python parts and the rearrangement of the Markdown doc parts, we use Sphinx in combination with Breathe to generate a clean documentation suitable to our needs and easily hostable in the web. After that `make`-command you will find the `openage-docs` folder one directory level over your cloned repository.

  __HINT:__ If you need to regenerate just the second stage with Sphinx you can call `make webdoc-sphinx`.
