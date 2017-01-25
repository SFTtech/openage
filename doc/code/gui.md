# QtQuick-based Graphical User Interface

## Using Qt Creator Integrated Development Environment (IDE)

See [Developing openage in IDE](ide.md).

## Writing presentation-level code

It's being developed in QML language.

Sources are in the `libopenage/gui/qml` directory.
Start the game and edit the code.
Changes apply immediately after any QML file is saved.

### Image providers

The by-filename URLs have the following form: `image://by-filename/<filename>.<subid>`.
They are resolved relative to the asset directory.

Example:

	image://by-filename/gaben.png.2

The by-id URLs are like: `image://by-graphic-id/<texture-id>.<subid>` or `image://by-terrain-id/<texture-id>.<subid>`.

Example:

	image://by-graphic-id/7231.5

## Exposing components to the GUI layer

Components are adapted by writing QObject counterparts for them.
Look for the examples in the `libopenage/gui` directory.

There is a property-based approach and a model-based extension to it.

### Property-based binding

Let's suppose we have a class `ResourceAmount` in `libopenage/economy`, and we want to be able to use it in the GUI.

In order to do that:

1. A class `ResourceAmountLink` must be created in the `libopenage/gui`.
It must derive from `GuiItemQObject` and `GuiItem<ResourceAmountLink>`.
It must be registered in the QML type system using a usual Qt approach:
```cpp
qmlRegisterType<ResourceAmountLink>("yay.sfttech.openage", 1, 0, "ResourceAmount");
```

2. Specializations `struct Wrap<ResourceAmount>` and `struct Unwrap<ResourceAmountLink>` must be defined:
```cpp
namespace qtsdl {
template<>
struct Wrap<ResourceAmount> {
	using Type = ResourceAmountLink;
};

template<>
struct Unwrap<ResourceAmountLink> {
	using Type = ResourceAmount;
};
} // namespace qtsdl
```

3. Also ResourceAmount needs a public member to be added:
```cpp
public:
	qtsdl::GuiItemLink *gui_link
```

4. Declare and implement needed properties and signals in the `ResourceAmountLink` using Qt property syntax.

### Model-based binding

There is a class `GeneratorParameters` in `libopenage/` directory.
It has a big list of parameters of different types like `generation_seed`, `player_radius`, `player_names`, etc.
So, we're not going to write a Qt property for each one:

1. `GeneratorParameters` must derive from the `qtsdl::GuiPropertyMap`.

2. `GeneratorParameters` should set its initial values like so:
```cpp
this->setv("generation_seed", 4321);
this->setv("player_radius", 10);
this->set_csv("player_names", std::vector<std::string>{"name1", "name2"});
```

3. A class `GeneratorParametersLink` must be created in the `libopenage/gui`.
It must derive from `QObject` and `GuiItemListModel<GeneratorParametersLink>`.
It must be registered in the QML type system using a usual Qt approach:
```cpp
qmlRegisterType<GeneratorParametersLink>("yay.sfttech.openage", 1, 0, "GeneratorParameters");
```

4. Specializations `struct Wrap<GeneratorParameters>` and `struct Unwrap<GeneratorParametersLink>` must be defined:
```cpp
namespace qtsdl {
template<>
struct Wrap<GeneratorParameters> {
	using Type = GeneratorParametersLink;
};

template<>
struct Unwrap<GeneratorParametersLink> {
	using Type = GeneratorParameters;
};
} // namespace qtsdl
```

That results into a `ListModel`-like QML type with `display` and `edit` roles.
Basically, a database table with two columns: `display` and `edit`.

Qt properties can be added to the model-based class just like to the property-based.

## Passing data

### Calling member functions of the game class from the its `Link` counterpart

Since the GUI and the game logic may be in different threads, additional care is needed.

It's done by the `GuiItem::i()` functon (`GuiItem` is a base of `Link` classes).

For example, forwarding of a `clear()` member function call from `GameMainLink` to `GameMain`:
```cpp
void GameMainLink::clear() {
	static auto f = [] (GameMain *_this) {
		_this->clear();
	};
	this->i(f, this);
}
```

Returning a value synchronously from such call isn't possible.
See section about signals for that functionality.

### Declaring properties

The Link classes act like caches.
So, the properties that are needed to be available in QML should be declared as members.
The Q_PROPERTY should be used (see Qt docs).

### Using properties

Setters of the properties that may receive constant values must be implemented using the `GuiItem::s()` or `GuiItem::sf()` functions.

### Passing data to the GUI

Create a class with needed signals, `EditorModeSignals` for example:
```cpp
class EditorModeSignals : public QObject {
	Q_OBJECT

public:
signals:
	void toggle();
};
```

Create a member of this type in the game class `EditorMode`.
Then connect its signals in the corresponding `EditorModeLink` class by overriding its `on_core_adopted()`:
```cpp
void EditorModeLink::on_core_adopted() {
	QObject::connect(&unwrap(this)->gui_signals, &EditorModeSignals::toggle, this, &EditorModeLink::toggle);
}
```

### Including headers

The files that are outside of the `libopenage/gui/` are allowed to include only the headers from the `libopenage/gui/guisys/public` and `libopenage/gui/integration/public`.

## Directory structure

The subsystem resides in the `libopenage/gui`.

* random files in the directory - bindings for the game components
* `guisys/` - non-openage-specific part
    * `guisys/public/` - pimpl wrappers
    * `guisys/private/` - implementation
    * `guisys/link/` - binding-related classes
* `integration/` - openage-specific part, notably the image providers
* `qml/` - QML code

## Development

The C++ code must be written in a such way that none of the errors in the QML code could provoke crash or trigger a C++ assert (C++ asserts are verifying C++ code, not QML).
There are several exceptional cases when the initialization of the QML environment has no choice but to fail (calls `qFatal`).

Files from `guisys/` are not allowed to include any game headers that are outside of `guisys/`.

Headers from `guisys/public/` and `integration/public/` are not allowed to include any Qt headers directly or through other headers.

### Commit prefixes explanation

* **gui:** - QML code
* **guilink:** - binding of the game logic or other subsystems (like image providers) to the GUI
* **guiinit:** - for the code in the game that creates and uses the GUI renderer
* **guisys:** - for the code in the `guisys/` directory
