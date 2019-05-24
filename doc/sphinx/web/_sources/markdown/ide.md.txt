# Developing openage in Integrated Development Environments (IDEs)

## Qt Creator IDE

Qt Creator can be used to launch build, edit, run and debug C++ and QML code.

Install Qt Creator from the repository of your distribution or with an installer from the Qt web-site.

To launch QtCreator in English on Linux:

```
$ LANGUAGE=C qtcreator
```

### Opening and configuring the project

Open the root `CMakeLists.txt` with the QtCreator,
select the Qt installation that you want to target and click **Configure Project**.

Watch the **General Messages** log window for CMake errors.


### Enabling parallel build

Go to **Projects** -> **openage** -> **Build & Run** -> **Build** -> **Build Steps** -> **Build** -> **Details**, and add `-j4` to the **Tool arguments**.


### Run & Debug

To set the correct executable, first go to **Projects** -> **openage** -> **Build & Run** -> **Run** page.
In the **Run configuration** combo-box select **Custom Executable** and set **Executable** to `<path-to-openage-src>/run`.
Set **Working directory** to `<path-to-openage-src>`.
The `<path-to-openage-src>` is the directory where the sources of the openage are (the directory of the root `CMakeLists.txt` file).

The `<path-to-openage-src>/run` binary has no executable bit set when first built, set it manually with `chmod`.
