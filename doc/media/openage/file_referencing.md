# File Referencing

There are two ways to specify a path: *relative* and *absolute*.

## Relative Path References

Relative paths are relative to the location of the file they are defined in.
They can only refer to file resources that are in the same modpack as
the referencing file.

```
"idle.png"         # idle.png is in the same folder as the referencing file
"./idle.png"       # same as above, but more explicit
"media/attack.png" # attack.png is in the subfolder 'media', relative to the referencing file
```

## Absolute Path References

Absolute paths start from the (virtual) modpack root (the path where all
modpacks are installed). They must begin with `/` followed by either
* A modpack identifier **or**
* a shortened modpack alias

enclosed by `{}`. For information on modpack identifiers and aliases see
the [modpack](modpacks.md#alias-and-identifier) docs.

```
"/{aoe2_base@openage}/idle.png" # absolute path with modpack identifier
"/{aoe2_base}/idle.png"         # absolute path with modpack alias
```

Absolute paths are the only way to reference file resources from other
modpacks.
