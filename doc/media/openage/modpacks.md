# Modpacks

Input assets for openage are organized in packages that we call **modpacks**.
Everything you need to know about them is described in this document.


## What does a modpack contain?

Modpacks contain anything that is considered an input for the engine, e.g.
gamedata, graphics, sounds, scripts, translations or a combination of all
of those. There are no limits on their scope. A modpack can contain
anything from minor enhancements to standalone games.

Apart from game-related data, modpacks store configuration data for loading
them into the engine. Most of this data is located in the
**[modpack definition file](###modpack-definition-file)**. Modpacks can define
their interaction with other modpacks by specifying **dependencies** and
**conflicts**.

The organization of files inside the modpack can be decided by its creators.
openage does not enforce a specific file structure, although a "sensible"
structure is of course recommended. The only file that is bound to some requirements
is the modpack definition file (see its dedicated section for details).


### Modpack Definition File

The modpack definition file acts as the header file for the modpack. It is
the only mandatory file in the modpack. The definition file must be placed
in the modpack's root folder under the filename `modpack.nfo`.

The main purpose of the file is to store configuration parameters such as:

* Internal name, alias and version number
* Paths to game assets that should be loaded
* Descriptions and author information

Furthermore, the file can specify the interaction with other modpacks:

* Dependencies on other modpacks
* Conflicts with other modpacks
* Replacment of other modpacks
* Specification of a preferred load order (in relation to other mods)

A full list of configuration parameters can be found [here](modpack_definition_file.md).


## Distribution

Modpacks can be distributed as standalone packages or via a package
repository. Each of these methods has their own advantages.


### Standalone Distribution

Any modpack can be distributed as a standalone package by publishing
a ZIP or tar.gz archive of the modpack on a hosting platform or a
website. The archive can be imported by the openage launcher.

It is the easiest way to publish a modpack for openage. However, updates
and dependencies have to be done manually. Therefore, it is recommended
to use a package repository to offer better convenience features for users.

A note of advice: Allow everyone to download older versions of a modpack.
Other modpacks might depend on a specific release version of your content.


### Package Repositories

A package repository (repo for short) is a dedicated hosting platform for
modpacks. They are inspired by the package management systems of Linux
distributions, Python, Rust and others. Package repositories have the following
advantages:

* Can be checked regularly for updates
* Automatically download and install dependencies if they are in the same repo
* Can store the history of modpacks (previous versions)
* They can be searched by using keywords or content filters
* Can sign modpacks to indicate trust (i.e. check for exploits or malware)

Everyone can set up a package repository. Users can add an unlimted number of
repositories in the openage launcher. Some repositories are added by default
when openage is installed.

Repositories are the preferred method of distributing mods.


### Licensing

Once a mod is out in the open, it's good to clarify how and what other creators are
allowed to do with it. This can be done by adding a **license** to the
modpack. Using a license is completely optional, but it makes sense to
add one in some cases. For example, a license can specify how to give credit
or whether files from a modpack are allowed to be integrated into other modpacks.

The license can be chosen for every modpack individually by the creator.
While the engine is licensed as GPLv3+, modpack licenses do not need to
be compatible with it. Repos may decide which licenses they allow and
reject. Additionally, they may offer a default license.

When you include (not reference) assets from modpacks of other creators,
always check their license first. Copying files may be subject to terms set
in the license. It is usually safer (and less frowned upon) to have the modpack
referenced as a dependency than to copy files over.


## Addressing Modpacks

This section describes standards for addressing and referencing modpacks
as well as administrative requirements for that purpose.


### Naming Conventions

Modpack and repository names can only contain these characters:

* Letters from the latin alphabet (`a-z`, `A-Z`)
* Numbers (`0-9`)
* Separators (`-`, `_`, `.`)

It is recommend to use at least 4 characters, but this is not mandatory.
Repositories might enforce stricter rules on modpack names such as
word filters or prefixes.

Some repository names are reserved and cannot be used:

* `openage`: Reserved for engine development by us and modpacks created by the converter
* `local`: Internal repository name for modpacks that were not installed from a repository


### Alias and Identifier

There are two ways to reference a modpack: By using the modpack **identifier**
or by using an **alias**. It is recommended to use aliases wherever possible
and only use the identifier when it is absolutely necessary.

The modpack identifier is a unique string that is composed of the modpack
name and the repository name seperated by `@`.

```
modname@reponame
```

Mods that are not installed from a repository are referred to by using `local`
as the repo name.

```
modname@local
```

----

An alias is a string that is used as a quick reference for the modpack
internally. By default, the alias is the name of the modpack, but it can
also be assigned in the modpack definition file. Aliases must be unique
at load time.

```
modname
```

Aliases can be utilized for replacing other modpacks. For example, an enhanced
graphics pack for a mod can replace a normal graphics pack by assigning itself
the same alias. References using the alias will then point to the enhancement
graphics pack.


### Version Pinning

Sometimes a very specific version of a modpack can be required due to
compatibility issues. This can be done by apending the requested version
number to the modpack identifier separated by `::`.

```
modname@reponame::version
```
