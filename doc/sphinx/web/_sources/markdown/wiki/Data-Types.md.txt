# Data Types

Click on a link to view further examples and explanations about a data type.

## Primitive Types

| Type                                  | Notation  | Explanation
|---------------------------------------|-----------|--------------------------
| [Integer](Integer-(data-type))        | `int`     | Positive, negative or infinite integer numbers
| [Float](Float-(data-type))            | `float`   | Positive, negative or infinite floating point numbers
| [Text](Texts-(data-type))             | `text`    | Strings and characters
| [File](Files-(data-type))             | `file`    | File paths
| [Boolean](Boolean-(data-type))        | `bool`    | Boolean values (true or false)

## Complex Types

| Type                                  | Notation                   | Explanation
|---------------------------------------|----------------------------|-------------
| [nyan::Object](Objects)               | depends on object type     | Stores a reference to a non-abstract nyan object
| [Set](Set-(data-type))                | `set(type)`                | Stores a set of values with a specific type 
| [Ordered Set](Orderedset-(data-type)) | `orderedset(type)`         | Stores a set of values with a specific type and stores the order they were inserted
| [Dict](Dict-(data-type))\*            | `dict(keyType, valueType)` | Stores key-value pairs with a specific type of key and a specific type of values

\* Dicts are currently not implemented.