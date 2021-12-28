
# Logalizer

Helper to Visualize logs

## What you get

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
    + [category](#category) 
    + [patterns](#patterns)
    + [print](#print)
    + [variables](#variables)
    + [count](#count)
  + [disable_category](#disable_category)
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


### Translation Configuration

#### `translations` 

This is the brain of the configuration. This is used to generate the translation file. This is an array of translation elements. 

This is an example of a couple of translation configurations.

Configurations for the lines "The temperature is 38 degrees" and "The pressure is 69 pa"

```json
  "translations": [
    {
      "category": "Temperature_Sensing",
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
      "category": "Pressure_Sensing",
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

##### `category` 

This gives a name to a translation that can later be disabled by `disable_category`.

```json
"category": "Temperature_Sensing"
```

##### `patterns` 

This a list of tokens used to match a line for translation.

```json
"patterns": ["temperature", "degree"]
```
Here the words temperature and degree should be present in this line for it to be considered a match. 

##### `print` 

This is a string that gets written to the translation file if a match is found. This can have special tokens like `{$1}`, `{$2}`, `{$3}`, ... and `{$count}`. 
These tokens that gets replaced by configuring `variables` and `count`.

```json
"print": "TemperatureSensor has reported a change",
```
If a match is found, the text "TemperatureSensor has reported a change", is written to the translation file. 

##### `variables` 

This configuration is used to capture a variying value from the matched line. The captured value will replace the special tokens, `{$1}`, `{$2}`, `{$3}`,... To capture a variable we have to configure `starts_with` & `ends_with`. 
- `starts_with`: surrounding string with which the variable starts
- `ends_with`: surrounding string with which the variable ends

Let us try to capture the temperature in line "The temperature is `38` degrees"
We try to use the text around 38, as `startswith` and `endswith`.  "The temperature `is `38` degrees`"

```json
"variables": [
  {
    "endswith": " degrees",
    "startswith": "is "
  }
]
```
For line "The temperature is 38 degrees" with the above config, `38` is captured in `{$1}`

 - Automatic variable capture

```json
"print": "Temperature" 
```

The above config prints "Temperature(32)", variables are caputred in order and placed between parentheses

 - Manual variable capture 

```json
"print": "The temperature is ${1} degrees"
```

The above config prints "The temperature is 32 degrees"


##### `count` 

This is used to count the number of matching lines. This count will later be replaced in `print` with token `{$count}`. A count can be either `global` or `scoped`. 

- `global` counts all the matches in a file

```json
"count": "global"
```

- `scoped` counts all the continuous matches. The count is reset once a new match other than this translation is found. 

```json
"count": "scoped"
  ```

This is used to count the number of errors by searching lines with [FATAL] & [ERROR] tokens
```json
  "translations": [
    {
      "category": "Error_Count",
      "patterns": ["[FATAL]", "[ERROR]"],
      "count": "global",
      "print": "note left: {$count} errors found !!!",
      "variables": []
    }
  ]
```

----

#### `disable_category` 

This is used to disable a translation or a group of translations. This is like commenting out a group of translations.

```json
"disable_category": ["Temperature_Sensing", "Pressure_Sensing"]
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

If set to `true`, each print is written in a new line. If set to `false` you have to write '\n' when needed in your `print`s.

```json
"auto_new_line": true
```

If you set this to `false`, use \n manually in print.

```json
"print": "Temperature: {$1}\n"
```

#### `wrap_text_pre` 

This is a list of lines to place before the translation.

```json
"wrap_text_pre": [
  "@startuml",
  "skinparam dpi 300"
]
```

#### `wrap_text_post` 

This is a list of lines to place after the translation.

```json
"wrap_text_pre": [
  "@enduml"
]
```


### Output Configuration

#### `translation_file`

This is the path to create the generated translation file

```json
"translation_file": "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_sequence.txt"
```

This creates a sub directory in the name of the input file and places the translation file in it

#### `backup_file` 

This is the path to create a backup of the input file

```json
"backup_file": "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}.original"
```

This creates a sub directory in the name of the input file and places the backup file in it

### Stripping Configuration

#### `delete_lines` 

You can also use tools like sed to remove certain lines, instead of using this configuration. 

This helps to delete a line in the input file. This is a list of tokens. All the lines with matching tokens are deleted.

```json
"delete_lines": [
  "[debug]",
  "[info]",
]
```

This configuration supports regex. Remember that regex matching is slower.

#### `replace_words` 

You can also use tools like sed to replace certain text, instead of using this configuration. 

This helps to replace a token in each line to a new token. This is a list of key value pairs. The key is replaced by the value in each line of the input file. 

```json
"replace_words": {
  "find this string": "replace with this string",
  "find more string": "replace more string"
}
```

### External Tools Configuration

#### `execute` 

You can also execute commands in a script, instead of using this configuration. 

This is a list of commands that gets executed after the translation file is written.

- Special variables
-- `${fileDirname}` - The directory in which the input file is present
-- `${fileBasenameNoExtension}` - The input file name without extension
-- `${fileBasename}` - The input file name

```json
"execute": [
  "java -DPLANTUML_LIMIT_SIZE=32768 -jar plantuml.jar \"${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_sequence.txt\"",
  "rm \"${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_sequence.txt\""
]
```
This example configuration runs 

1. plantuml to generate a sequence diagram out of the translation file
2. remove the translation file
