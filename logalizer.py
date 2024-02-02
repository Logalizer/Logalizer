#!/usr/bin/env python

"""
Converts logs to plantuml diagram

usage: logalizer [-h] -c CONFIG -f FILE [-p PLANTUML]

optional arguments:
  -h, --help            show this help message and exit
  -c CONFIG, --config CONFIG
                        Input json configuration
  -f FILE, --file FILE  Log file to convert
  -p PLANTUML, --plantuml PLANTUML
                        Path to plantuml to generate diagram
"""
__author__ = "Logesh Gopalakrishnan"
__license__ = "MIT"


import json
import sys
import os
import subprocess
import argparse
import pprint


class Translator():
    """Translator takes a config and tranlates log file to plantuml diagram"""

    def __init__(self, plantuml, configFile, groupConfig):
        self.conf: dict = {}
        self.plantuml: str = plantuml
        try:
            with open(configFile, "r", encoding="utf8", errors='ignore') as f:
                self.conf = json.load(f)
        except FileNotFoundError as ioerror:
            print(
                f"{str(ioerror)}\n")
            sys.exit(2)
        except Exception as error:
            print(
                f"Exception while parsing json in file: {configFile}\n  {str(error)}\n")
            sys.exit(3)

        self.removeDisabledTranslation(groupConfig)

    def removeDisabledTranslation(self, groupConfig):
        """Remove disabled translations"""
        translations = self.conf["translations"]
        # If translation is disabled by command line it takes precedence, as configuration is considered default
        if not groupConfig['disables'] and not groupConfig['enables']:
            if "disable_group" in self.conf:
                self.conf["translations"] = [
                    tr for tr in translations
                    if ("group" in tr
                        and tr["group"] not in self.conf["disable_group"])
                    or ("enable" in tr
                        and tr["enable"] == True)
                ]

        # only one of disables or enables is considred, of which enables is given priority
        if groupConfig['disables']:
            self.conf["translations"] = [
                tr for tr in translations
                if "group" in tr
                and tr["group"] not in groupConfig['disables']
            ]

        elif groupConfig['enables']:
            self.conf["translations"] = [
                tr for tr in translations
                if "group" in tr
                and tr["group"] in groupConfig['enables']
            ]

    def checkPairs(self, translations):
        """Check if a matching pair is found after generating translations"""
        """
        Configure like this,
        "pairs": [
            {
                "source": "client -> server : request",
                "pairswith": "server -> client : response",
                "before": "client -> server : request",
                "error": "server -> clent : ERROR: NO RESPONSE"
            }
            `source` should have a corresponding `pairswith`
            This `pairswith` should be seen before `before` entry
            When a `pairswith` match is not found then `error` is added
        """

        insertions = []
        if "pairs" not in self.conf:
            return
        for pair in self.conf["pairs"]:
            source = pairswith = before = None
            if "before" not in pair:
                pair["before"] = pair["source"]
            for i, line in enumerate(translations):
                # case 5: Source is already found and source is found again
                if source is not None and pair["source"] in line:
                    # print( f"s->s: {i=} {line=} {source=}")
                    insertions.append((i, pair["error"]))
                    source = i
                    continue

                # case 1: Souce is found
                elif pair["source"] in line:
                    source = i
                    # print( f"s: {i=} {line=} {source=}")
                    continue

                # case 2: Souce is found, matching pair is found
                elif source is not None and pair["pairswith"] in line:
                    source = pairswith = None
                    continue

                # case 3: Souce is found, before is found before a matching pair is found
                elif source is not None and pair["before"] in line:
                    # print( f"s->b{i=} {line=} {source=}")
                    insertions.append((i, pair["error"]))
                    source = before = None

            # case 4: Source is found and pariswith is not found till EOF
            if source and not pairswith and not before:
                translations.append(pair["error"])

        # Insert all the errors
        for item in reversed(insertions):
            translations.insert(item[0], item[1])

    def generateDiagram(self, translationFile):
        """Generate a plantuml diagram with an input file"""

        # Use diagram size proportional to sequence file size
        lines = open(translationFile).read().count("\n")
        pumlSize = 32768 if lines < 100 else 65536
        command = f'java -DPLANTUML_LIMIT_SIZE={pumlSize} -jar "{self.plantuml}" -tpng "{translationFile}"'
        res = subprocess.Popen(command, stdout=subprocess.PIPE)
        res.wait()
        if res.returncode != 0:
            print(
                f"Diagram not generated due to errors in {translationFile}\n")
        else:
            print(f"Diagram generated: {translationFile}.png\n")

    def blacklisted(self, line):
        """If a line is blacklisted, do not consider for translation"""

        if "blacklist" in self.conf:
            if any(entry in line for entry in self.conf["blacklist"]):
                return True
        return False

    def isDuplicate(self, duplicateStrategy, translations, translation):
        """Based on the configured duplicate strategy, checks if a translation is duplicate or not"""

        if duplicateStrategy == "remove_all":
            if translation in translations:
                return True

        elif duplicateStrategy == "remove_continuous":
            if translation == translations[-1]:
                return True
        return False

    def replaceText(self, line):
        """Search and Replace contents in print string"""

        if not "replace_words" in self.conf:
            return line

        # Search and replace text in print
        newLine = line
        for search, replace in self.conf["replace_words"].items():
            newLine = newLine.replace(search, replace)
        return newLine

    def getVariableValues(self, variablesConfig, line):
        """Extract variable values from input line"""

        varValues = []
        for var in variablesConfig:
            try:
                start = line.index(var["startswith"]) + len(var["startswith"])
                end = start + line[start:].index(var["endswith"])
                varValue = line[start:end]
                varValues.append(varValue)
            except:
                continue
        return varValues

    def updateVariableValues(self, varValues, printStr):
        """Update variables with values in print string"""

        if "${1}" in printStr:
            # Print contains formatted variables
            for i, val in enumerate(varValues):
                search = "${" + str(i+1) + "}"
                printStr = printStr.replace(search, val)
        else:
            # Pack variable values in braces
            params = "(" + ", ".join(varValues) + ")"
            printStr += params
        return printStr

    def getMatchingTranslator(self, line):
        """Get a matching translation for a line based on configured patterns"""

        for translation in self.conf["translations"]:
            # Check if all patterns are found in line
            if all(pattern in line for pattern in translation["patterns"]):
                return translation
        return None

    def getPrint(self, line, translation):
        """Get the print string for the given line as per translation"""

        printStr = translation["print"]

        # When variables are configured fetch values and update the printStr
        varValues = self.getVariableValues(translation["variables"], line)
        if not varValues:
            return printStr

        printStr = self.updateVariableValues(varValues, printStr)
        return printStr

    def translateLine(self, line):
        """Translate an input line as per configured translation"""

        tr = self.getMatchingTranslator(line)
        if tr == None:
            return None, None

        line = self.replaceText(line)
        printStr = self.getPrint(line, tr)
        duplicateStrategy = tr["duplicates"] if "duplicates" in tr else None
        return printStr, duplicateStrategy

    def getTranslations(self, inputFile):
        """Get translated lines for an input log file"""

        translations = []
        try:
            with open(inputFile, "r", encoding="utf8", errors='ignore') as file:
                for line in file:
                    # if a line is blacklisted, ignore the line
                    if self.blacklisted(line):
                        continue
                    translation, duplicateStrategy = self.translateLine(line)
                    # If a translation is not avialble, do nothing
                    if translation is None:
                        continue
                    # Based on duplicate strategy, decide to add translation or not
                    if not self.isDuplicate(duplicateStrategy, translations, translation):
                        translations.append(translation)
        except FileNotFoundError:
            print(f"FileNotFound: No such file or directory: {inputFile}\n")
            sys.exit(2)
        return translations

    def writeTranslations(self, translations, translationFile):
        """Write translations to a file"""

        with open(translationFile, "w") as output:
            if "wrap_text_pre" in self.conf:
                output.write('\n'.join(self.conf["wrap_text_pre"]))
            output.write('\n' + '\n'.join(translations))
            if "wrap_text_post" in self.conf:
                output.write('\n' + '\n'.join(self.conf["wrap_text_post"]))
            print(f"Translation file written: {translationFile}")

    def translateFile(self, inputFile):
        """Translate input file to generate a diagram"""
        translations = self.getTranslations(inputFile)
        self.checkPairs(translations)
        translationFile = f"{inputFile}.txt"
        self.writeTranslations(translations, translationFile)
        self.generateDiagram(translationFile)


def getAppPath():
    """Get the path of the script / executable"""

    # determine if application is a script file or frozen exe
    if getattr(sys, 'frozen', False):
        appPath = os.path.dirname(sys.executable)
    elif __file__:
        appPath = os.path.dirname(__file__)
    return appPath


def getCmdArgs():
    """Returns command line arguments"""

    argParser = argparse.ArgumentParser(
        prog='logalizer',
        description='Converts logs to plantuml diagram',
        epilog='')
    argParser.add_argument(
        "-c", "--config", help="Input json configuration", required=True)
    argParser.add_argument(
        "-f", "--file", help="Log file to convert", required=True)
    argParser.add_argument(
        "-p", "--plantuml", help="Path to plantuml to generate diagram")
    argParser.add_argument(
        "-e", "--enable", help="Comma seperated list of groups to enable")
    argParser.add_argument(
        "-d", "--disable", help="Comma seperated list of groups to disable")

    args = argParser.parse_args()
    return args


def getPlantUMLPath(plantumlInputPath):
    # default plantuml =./plantuml/plantuml.jar
    if plantumlInputPath:
        plantuml = plantumlInputPath
    else:
        plantuml = os.path.join(getAppPath(), "plantuml", "plantuml.jar")
    return plantuml


def main():
    args = getCmdArgs()
    plantuml = getPlantUMLPath(args.plantuml)
    config = args.config
    log = args.file
    group = {}
    enable = getattr(args, 'enable', None)
    disable = getattr(args, 'disable', None)
    group['enables'] = [x.strip()
                        for x in args.enable.split(',')] if enable else []
    group['disables'] = [x.strip()
                         for x in args.disable.split(',')] if disable else []

    tr = Translator(plantuml, config, group)
    tr.translateFile(log)


if __name__ == "__main__":
    main()
