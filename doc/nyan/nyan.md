nyan
====

nyan - yet another notation

https://github.com/SFTtech/nyan


Idea
----

nyan is the data storage format for openage.

We developed it because of the lack of any format suitable for storing
the enormous complexity of the Age of Empires data in a sane and readable way.

Yes, the data could be written in `yaml`, `json`, `xml`, `csv` or a `docx`
and contain the exact same information.
But you could also use a hexeditor to write the bytes yourself,
or even [use butterflies](https://xkcd.com/378/) to change the bits on disk.

So we try to maximize the experience for both developers and modders.
Users give a shit about how things work internally,
and as we're doing this project for fun and not profit and quick results,
we developed a data language to suit our needs.

nyan allows us to have a typesafe hierarchical data storage,
so all content data can be verified to be compatible at load time.

It is easy to read, write and modify, and most importantly,
it can actually store unit upgrades, unit abilities, research,
civ bonuses and age upgrades.


Technical specification
-----------------------

[nyan specification](https://github.com/SFTtech/nyan/blob/master/doc/nyan.md)


openage specifics
-----------------

nyan is a general purpose data language,
the following describes the openage specific usage of it.

* [data conversion](conversion.md)
* [openage engine nyan interface](openage-lib.md)
* [examples](examples.md)
