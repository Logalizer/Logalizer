# Logalizer

Filter and translate a log to a meaningful format

## Features

+ Translate the input log to an understandable format
   + Generate sequence diagram
   + Generate state diagram
   + Generate report
+ Strip the input log

## How it works
<pre>
                           +--------+                                      
                           | Config |             +-----------------------+
                           +--------+             |    +-------------+    |
                               |                  |    | Backup file |    |
                               |                  |    +-------------+    |
                               V                  |                       |
 +----------+            +-----------+            |   +---------------+   |
 |   File   |----------->| Logalizer |----------->|   | Stripped file |   |
 +----------+            +-----------+            |   +---------------+   |
                                                  |                       |
                                                  |   +---------------+   |
                                                  |   |  Translation  |   |
                                                  |   +---------------+   |
                                                  +-----------------------+ 
</pre>
+ Logalizer takes as input 
  + a log file 
  + a config file
+ Logalizer outputs
  + a backup of the input file
  + a stripped version of the input file
  + a translated file based on the input config
  
### Combining with other tools

The translated file can be used to,
  + Generate UML diagram
  + Generate report with HTML

<pre>
  +---------------+            +---------------+           +--------------------+
  |  Translation  |----------->|   Plant UML   |---------->|  Sequence diagram  |
  +---------------+            +---------------+           +--------------------+
</pre>

## Configuration

+ [Stripping Configuration](#Stripping-Configuration)
  + [delete_lines](#delete_lines)
  + [replace_words](#replace_words)
+ [Output Configuration](#Output-Configuration)
  + [translation_file](#translation_file)
  + [backup_file](#backup_file)
  + [translations](#translations)
+ [Translation Configuration](#Translation-Configuration)
  + [translation](#translation)
    + [category](#category) 
    + [patterns](#patterns)
    + [print](#print)
    + [count](#count)
    + [variables](#variables)
  + [disable_category](#disable_category)
  + [blacklist](#blacklist)
  + [auto_new_line](#auto_new_line)
  + [wrap_text_pre](#wrap_text_pre)
  + [wrap_text_post](#wrap_text_post)
+ [External Tools Configuration](#External-Tools-Configuration)
  + [execute](#execute)

### Stripping Configuration

#### `delete_lines` 

This helps to delete a line in the input file. This is a list of tokens. All the lines with matching tokens are deleted.

```json
"delete_lines": [
  "[debug]",
  "[info]",
]
```
- This configuration supports regex. But using regex is discouraged as this drastically slows down the process of translation.

#### `replace_words` 

This helps to replace a token in each line to a new token. This is a list of key value pairs. The key is replaced by the value in each line of the input file. 

```json
"replace_words": {
  "find this string": "replace with this string",
  "find more string": "replace more string"
}
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

#### `translations` 

This is the brain of the configuration. This gives a hint on how to generate the translation file. This is an array of translation elements. Refer the [Translation Configuration](#Translation-Configuration) section.  


### Translation Configuration

#### translation 

This is not a tag, but a group of tags. 

This is an example of a single translation configured under `translations`

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

##### `print` 

This is a strig that gets written to the translation file if a match is found. This can have special tokens that gets replaced later. Special tokens are `{$count}`, `{$1}`, `{$2}`, `{$3}`, ...

```json
"print": "Temperature: ${1}"
```
The above config prints "Temperature: 32"

```json
"print": "Temperature: " 
```
The above config prints "Temperature: (32)", variables are caputred in order and placed between parentheses

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

##### `variables` 

This is used to capture a variying value from the matched line and to print the captured value later by placing `{$1}`, `{$2}`, `{$3}`. To capture a variable we have to configure `starts_with` & `ends_with`. 
- `starts_with`: surrounding string with which the variable starts
- `ends_with`: surrounding string with which the variable ends

```json
"variables": [
  {
    "endswith": "is ",
    "startswith": " degrees"
  }
]
```
For line "The temperature is 38 degrees" with the above config, `38` is captured in `{$1}`

----

#### `disable_category` 

This is used to disable a translation or a group of translations. This is like commenting out a group of translations.
```json
"disable_category": ["Temperature_Sensing", "Pressure_Sensing"]
``` 

#### `blacklist` 

This is used to blacklist the lines from matching. If a line is blacklisted, then it is not checked for match
```json
"blacklist": [
  "dont consider this line for translation",
  "this line too"
]
```

#### `auto_new_line` 

If set to `true`, each print is written in a new line. If set to `false` you have to write '\n' when needed in your `print`s
```json
"auto_new_line": true
```

If you set this to `false`, use \n manually in print
```json
"print": "Temperature: {$1}\n"
```

#### `wrap_text_pre` 

This is a list of lines to place before the translation
```json
"wrap_text_pre": [
  "@startuml",
  "skinparam dpi 300"
]
```

#### `wrap_text_post` 

This is a list of lines to place after the translation
```json
"wrap_text_pre": [
  "@enduml"
]
```

### External Tools Configuration

#### `execute` 

This is a list of commands that gets executed after the translation file is written
```json
"execute": [
  "java -DPLANTUML_LIMIT_SIZE=32768 -jar plantuml.jar \"${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_sequence.txt\"",
  "rm \"${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_sequence.txt\""
]
```
This runs the plantuml to generate a sequence diagram out of the translation file