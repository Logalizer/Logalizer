
# Logalizer

Helper to Visualize logs

## What you can do

+ Visualize logs
+ Understand logs better
+ Uncover problems

## How it works

Logalizer takes a log file and a config file as input. Logalizer outputs a translated file. 

<pre>
 +--------+                                      
 | Config |------------------------.
 +--------+                        |
                                   |
                                   V
 +-------------+            +-------------+            +---------------+
 |  Input Log  |----------->|  Logalizer  |----------->|  Translation  |
 +-------------+            +-------------+            +---------------+
</pre>

### Visualizing

The translated file can be used to generate UML diagrams.

<pre>
  +---------------+          +---------------+          +---------------+
  |  Translation  |--------->|   Plant UML   |--------->|  UML diagram  |
  +---------------+          +---------------+          +---------------+
</pre>

[PlantUML](https://plantuml.com/) and [Mermaid](https://mermaid-js.github.io/mermaid/#/) like tools can convert text into diagrams. 


### Usage

```
Usage:
  logalizer -c <config> -f <log>
  logalizer -f <log>
  logalizer -h | --help
  logalizer --config-help
  logalizer --version

Options:
  -h --help        Show this screen
  --config-help    Show sample configuration
  --version        Show version
  -c <config>      Translation configuration file. Default is ./config.json
  -f <log>         Log file to be interpreted

Example:
  logalizer -c config.json -f trace.log
  logalizer -f trace.log
```

## Configuration

+ [Translation Configuration](#Translation-Configuration)
  + [translations](#translations)
    + [group](#group) 
    + [patterns](#patterns)
    + [print](#print)
    + [variables](#variables)
    + [duplicates](#duplicates)
  + [disable_group](#disable_group)
  + [blacklist](#blacklist)
  + [auto_new_line](#auto_new_line)
  + [wrap_text_pre](#wrap_text_pre)
  + [wrap_text_post](#wrap_text_post)
+ [Output Configuration](#Output-Configuration)
  + [translation_file](#translation_file)
  + [backup_file](#backup_file)
+ [Stripping Configuration (Optional)](#Stripping-Configuration)
  + [delete_lines](#delete_lines)
  + [replace_words](#replace_words)
+ [External Tools Configuration (Optional)](#External-Tools-Configuration)
  + [execute](#execute)
+ [Special Variables for Path](#Special-Variables-for-Path).


### Translation Configuration

#### `translations` 

This is the brain of the configuration. This is used to generate the translation file. This is an array of translation elements. This is a mandatory configuration without which nothing can be translated.

This is an example of a couple of translation configurations.

Configurations for the lines "The temperature is 38 degrees" and "The pressure is 69 pa".

```json
  "translations": [
    {
      "group": "Temperature_Sensing",
      "patterns": ["temperature", "degree"],
      "print": "TemperatureSensor -> Controller: Temperature: ",
      "variables": [
        {
        "endswith": "is ",
        "startswith": " degrees"
        }
      ]
    },
    {
      "group": "Pressure_Sensing",
      "patterns": ["Pressure"],
      "print": "PressureSensor -> Controller: Pressure: ",
      "variables": [
        {
        "endswith": " Pa",
        "startswith": "is "
        }
      ]
    }
  ]
```

##### `group` 

This gives a name to a translation that can later be disabled by `disable_group`. This is optional.

```json
"group": "Temperature_Sensing"
```
##### `enable` 

This to disable a translation. This is optional. By default this is set to true so you don't have to set it to true every time.

```json
"enable": false
```

##### `patterns` 

This a list of tokens used to match a line for translation. This is mandatory.

```json
"patterns": ["temperature", "degree"]
```
Here the words temperature and degree should be present in this line for it to be considered a match. 

##### `print` 

This is a string that gets written to the translation file if a match is found. This can have special placeholders like `${1}`, `${2}`, `${3}`, ... and `${count}`. 

These tokens that gets replaced by configuring `variables` and `count`.

This is mandatory.

```json
"print": "TemperatureSensor has reported a change",
```
If a match is found, the text "TemperatureSensor has reported a change", is written to the translation file. 

##### `variables` 

You can configure variables if you want to capture dynamically changing value from the matched line. The captured value will replace the special placeholders, `${1}`, `${2}`, `${3}` and so on that are used in `print`

To capture a variable we have to configure `starts_with` & `ends_with`. 
- `starts_with`: surrounding string with which the variable starts
- `ends_with`: surrounding string with which the variable ends

Let us try to capture the temperature in line "The temperature is `38` degrees".
We try to use the text around 38, as `startswith` and `endswith`.  "The temperature `is `38` degrees`"

```json
"print": "The temperature is ${1} degrees",
"variables": [
  {
    "startswith": "is "
    "endswith": " degrees",
  }
]
```
For line "The temperature is 38 degrees" with the above config, `38` is captured in `${1}`

 - Automatic variable capture

 ```log
 First Name: James; Last Name: Bond;
 ```

```json
"print": "setName" 
"variables": [
  {
    "startswith": "First Name: "
    "endswith": ";",
  },
  {
    "startswith": "Last Name: "
    "endswith": ";",
  }
]
```

The above config prints "setName(James, Bond)". 

Variables are captured in order and placed between parentheses separated by comma. 


 - Manual variable capture 

```json
"print": "The temperature is ${1} degrees"
```

The above config prints "The temperature is 32 degrees"


##### `duplicates` 

This is used to manage duplicate entries.


- `allowed` allows duplicates in the translation file. This is the default behaviour if not specified.

```json
"duplicates": "allowed"
```

- `remove_all` removes any duplicates in the translation and keeps only the first entry. 

```json
"duplicates": "remove_all"
```

- `remove_continuous` removes duplicates entries that occurs continuously in the translation. Only continuous entries are removed.

```json
"duplicates": "remove_continuous"
```

- `count_all` is same as `remove_all`. It also counts the duplicates and updates `${count}` in the first entry.

This is used to count the number of errors by searching lines with [FATAL] & [ERROR] tokens
```json
  "translations": [
    {
      "group": "Error_Count",
      "patterns": ["[FATAL]", "[ERROR]"],
      "duplicates": "count_all"
      "print": "note left: ${count} errors found !!!",
      "variables": []
    }
  ]
```

- `count_continuous` is same as `remove_continuous`. It also counts continuously occurring duplicates and updates `${count}` in the corresponding entry. 

This is used to count the number of errors by searching lines with [FATAL] & [ERROR] tokens
```json
  "translations": [
    {
      "group": "Error_Count",
      "patterns": ["Retrying..."],
      "duplicates": "count_continuous"
      "print": "note left: Retried ${count} times !!!",
      "variables": []
    }
  ]
```

----

#### `disable_group` 

This is used to disable a translation or a group of translations. This is like commenting out a group of translations.

```json
"disable_group": ["Temperature_Sensing", "Pressure_Sensing"]
``` 

#### `blacklist` 

This is used to blacklist the lines from matching. If a line is blacklisted, then it is not checked for match.

```json
"blacklist": [
  "dont consider this line for translation",
  "this line too"
]
```

#### `auto_new_line` 

By default it is set to `true`. If set to `true`, each print is written in a new line. 


```json
"auto_new_line": true
```

If set to `false` a new line is not inserted automatically. You can add '\n' if you want in your `print`s.

```json
"print": "Temperature: ${1}\n"
```

#### `wrap_text_pre` 

This defines a list of lines that will be added to the top of the translation file.

```json
"wrap_text_pre": [
  "@startuml",
  "skinparam dpi 300"
]
```

#### `wrap_text_post` 

This defines a list of lines that will be added at the end of the translation file.

```json
"wrap_text_pre": [
  "@enduml"
]
```


### Output Configuration

#### `translation_file`

This defines the path where the translation file should be created. You can use [Special Variables for Path](#Special-Variables-for-Path).

```json
"translation_file": "${fileDirname}/${fileBasenameNoExtension}/${fileBasenameNoExtension}_sequence.txt"
```

This example configuration creates a sub directory in the name of the input file and places the translation file in it.

If the input file is "/tmp/test.log", the translation file will be placed at "/tmp/test/test_sequence.txt"

#### `backup_file` 

You can use this if you want to take a backup of the original file. This is an optional configuration. You can use [Special Variables for Path](#Special-Variables-for-Path).

```json
"backup_file": "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}.original"
```

This example configuration creates a sub directory in the name of the input file and places the backup file in it.

If the input file is "/tmp/test.log", the backup file will be placed at "/tmp/test/test.log.original"

### Stripping Configuration

#### `delete_lines` 

You can also use tools like sed to remove certain lines, instead of using this configuration. 

This modifies the input file. This is used to delete lines in the input file. This is a list of tokens. All the lines that matches the tokens are deleted.

```json
"delete_lines": [
  "[debug]",
  "[info]",
]
```

This configuration supports regex. Remember that regex matching is slower.

#### `replace_words` 

You can also use tools like sed to replace certain text, instead of using this configuration. 

This modifies the input file. This is like find and replace. This is a list of key value pairs. The key is replaced by the value in each line of the input file. 

```json
"replace_words": {
  "find this string": "replace with this string",
  "find another string": "replace another string"
}
```

### External Tools Configuration

#### `execute` 

You can also execute commands in a script, instead of using this configuration. 

This is a list of commands that gets executed after the translation file is written.

You can use [Special Variables for Path](#Special-Variables-for-Path).

```json
"execute": [
  "java -DPLANTUML_LIMIT_SIZE=32768 -jar plantuml.jar \"${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_sequence.txt\"",
  "rm \"${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_sequence.txt\""
]
```

This example configuration runs 

1. plantuml to generate a sequence diagram out of the translation file
2. remove the translation file


### Special Variables for Path

- `${fileDirname}` - The path to the directory in which the input file is present
- `${fileBasename}` - The name of the input file
- `${fileBasenameNoExtension}` - The name of the input file without extension

If the input file is "/tmp/logs/test.log",

- `${fileDirname}` - "/tmp/logs"
- `${fileBasename}` - "test.log"
- `${fileBasenameNoExtension}` - "test" 
