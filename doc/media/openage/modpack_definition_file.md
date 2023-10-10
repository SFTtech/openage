# Modpack Definition File

**Format Version:** 2

The modpack definition file is the header file for [modpacks](modpacks.md).

TOML is used as the configuration language.

This document contains information about the parameters and sections that
can be interpreted by the engine.

## File Header

It is recommended to start the file with the following comment.

```
# openage modpack definition file
```

This is not mandatory, but will help others who don't know the format
find this specification document.

The following parameters have to be specified.

| Parameter      | Data Type | Optional | Description                                    |
| -------------- | --------- | -------- | ---------------------------------------------- |
| `file_version` | String    | No       | Version of the modpack definition file format. |


## [info] Section

`[info]` contains general information about the modpack.

| Parameter          | Data Type     | Optional | Description                                                             |
| ------------------ | ------------- | -------- | ----------------------------------------------------------------------- |
| `packagename`      | String        | No       | Name of the modpack.                                                    |
| `version`\*        | String        | No       | The modpack's internal version number. Must use [semver] format.        |
| `versionstr`\*     | String        | Yes      | Human-readable version string.                                          |
| `repo`             | String        | Yes      | Name of the repo where the package is hosted.                           |
| `alias`            | String        | Yes      | Alias of the modpack. Aliases can be used for replacing other modpacks. |
| `title`            | String        | Yes      | Title used in UI.                                                       |
| `description`      | String        | Yes      | Path to a file with a short description (max 500 chars).                |
| `long_description` | String        | Yes      | Path to a file with a detailed description.                             |
| `url`              | String        | Yes      | Link to the modpack's website.                                          |
| `license`          | Array[String] | Yes      | License(s) of the modpack.                                              |

[semver]: https://semver.org/

\* `version` is used by the engine to determine the most recent version of a modpack. Therefore, it should be bumped when something in the modpack changes (e.g. whenever a new version gets published). `versionstr` is what is displayed to the user and can contain any string, so it can be used to represent any sensible version format.

## [assets] Section

`[assets]` contains paths to assets in the modpack.

| Parameter | Data Type     | Optional | Description                                                                                            |
| --------- | ------------- | -------- | ------------------------------------------------------------------------------------------------------ |
| `include` | Array[String] | No       | List of paths to assets that should be mounted on load time. Paths are allowed to contain wildcards.   |
| `exclude` | Array[String] | Yes      | List of paths to assets that should be excluded from mounting. Paths are allowed to contain wildcards. |


## [dependency] Section

`[dependency]` contains a list of other modpacks that the modpack depends on.

| Parameter  | Data Type     | Optional | Description                             |
| ---------- | ------------- | -------- | --------------------------------------- |
| `modpacks` | Array[String] | Yes      | List of modpack aliases or identifiers. |


## [conflict] Section

`[conflict]` contains a list of other modpacks that the modpack conflicts with.

| Parameter  | Data Type     | Optional | Description                             |
| ---------- | ------------- | -------- | --------------------------------------- |
| `modpacks` | Array[String] | Yes      | List of modpack aliases or identifiers. |


## [authors] Section

`[authors]` contains information about the creators and authors of the modpack.
Every author must have their own subtable `[authors.{authorname}]`. The
subtable can set the following parameters.

| Parameter  | Data Type     | Optional | Description                                                           |
| ---------- | ------------- | -------- | --------------------------------------------------------------------- |
| `name`     | String        | No       | Nickname of the author. Must be unique for the modpack.               |
| `fullname` | String        | Yes      | Full name of the author.                                              |
| `since`    | String        | Yes      | Version number of the release where the author started to contribute. |
| `until`    | String        | Yes      | Version number of the release where the author stopped to contribute. |
| `roles`    | Array[String] | Yes      | List of roles of the author during the creation of the modpack.       |
| `contact`  | Table         | Yes      | Contact information (see below).                                      |

The contact table can use the following parameters.

| Parameter  | Data Type | Optional | Description        |
| ---------- | --------- | -------- | ------------------ |
| `discord`  | String    | Yes      | Discord username.  |
| `email`    | String    | Yes      | Email address.     |
| `github`   | String    | Yes      | GitHub username.   |
| `gitlab`   | String    | Yes      | Gitlab username.   |
| `irc`      | String    | Yes      | IRC username.      |
| `mastodon` | String    | Yes      | Mastodon username. |
| `matrix`   | String    | Yes      | Matrix username.   |
| `reddit`   | String    | Yes      | Reddit username.   |
| `twitter`  | String    | Yes      | Twitter username.  |
| `youtube`  | String    | Yes      | YouTube username.  |


## [authorgroups] Section

`[authorgroups]` contains information about teams or groups of authors.
It can be used in addition to `[authors]` to signify that the modpack is
a team effort.


| Parameter     | Data Type     | Optional | Description                                                                                                                                           |
| ------------- | ------------- | -------- | ----------------------------------------------------------------------------------------------------------------------------------------------------- |
| `name`        | String        | No       | Group or team name.                                                                                                                                   |
| `authors`     | Array[String] | No       | List of author identifiers. These must match up with subtable keys in the `[authors]` section, e.g. `"xxbunny123"` references `[authors.xxbunny123]`. |
| `description` | String        | Yes      | Path to a file with a description of the team.                                                                                                        |
