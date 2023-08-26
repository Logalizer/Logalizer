import json
import sys, getopt
import pprint
import os

global config

class Translator():

    def __init__(self, plantuml, configFile):
        self.conf: dict = {}
        self.plantuml: str = plantuml

        with open(configFile, "r") as f:
            self.conf = json.load(f)
        

    def getMatchingTranslator(self, line):
        for translation in self.conf["translations"]:
            if translation["group"] in self.conf["disable_group"]: return None

            for pattern in translation["patterns"]:
                if pattern not in line:
                    break
            else:
                return translation
        return None


    def getPrint(self, line, translation):

        # Extract variable values from input line
        varValues = []
        for var in translation["variables"]:
            start = line.index(var["startswith"]) + len(var["startswith"])
            end = start + line[start:].index(var["endswith"])
            varValue = line[start:end]
            varValues.append(varValue)

        printStr = translation["print"]

        # Search and replace text in print
        for search, replace in self.conf["replace_words"].items():
            printStr = printStr.replace(search, replace)

        if not varValues: return printStr

        if "${1}" in printStr:
            # Print contans formatted variables
            for i, val in enumerate(varValues):
                search = "${" + str(i+1) + "}"
                printStr = printStr.replace(search, val)

        else: 
            # Pack variable values in braces
            params = "(" + ", ".join(varValues) + ")"
            printStr += params

        return printStr

    def translateLine(self, line):
        tr = self.getMatchingTranslator(line)
        if tr == None: return str()
        duplicateStrategy = tr["duplicates"] if "duplicates" in tr else None
        return duplicateStrategy, self.getPrint(line, tr)

    def generateDiagram(self, seqFile):
        # Use diagram size proportional to sequence file size
        lines = open(seqFile).read().count("\n")
        pumlSize = 32768 if lines < 100 else 65536
        command = "java -DPLANTUML_LIMIT_SIZE={} -jar {} -tpng {}".format(pumlSize, self.plantuml, seqFile)
        print(os.popen(command).read())

 
# "pairs": [
#   {
#     "source": "client -> server : request",
#     "pairswith": "server -> client : response",
#     "before": "client -> server : request",
#     "error": "server -> clent : NO RESPONSE"
#   }
    def checkPairs(self, translations):
        insertions = []
        for pair in self.conf["pairs"]:
            source = pairswith = before = None
            if "before" not in pair: pair["before"] = pair["source"]
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

    def translateFile(self, inputFile):
        seqFile = inputFile + ".seq"

        translations = []
        with open(inputFile, "r") as file:
            for line in file:
                duplicateStrategy, translation = self.translateLine(line)
                if duplicateStrategy == "remove_all":
                    if translation not in translations: translations.append(translation)
                elif duplicateStrategy == "remove_continuous":
                    if translation != translations[-1]: translations.append(translation)
                else:
                    translations.append(translation)

        self.checkPairs(translations)

        with open(seqFile, "w") as output:
            output.write('\n'.join(self.conf["wrap_text_pre"]))
            output.write('\n' + '\n'.join(translations))
            output.write('\n' + '\n'.join(self.conf["wrap_text_post"]))

        self.generateDiagram(seqFile)

def help(er=0):
    print("""Usage: Logalizer.py [OPTION...]

  -p, --plantuml=<plantuml jar>      Path to plantuml jar, defaults to ./plantuml.jar
  -c, --config=<config file>         Input json configuration, defaults to ./config.json
  -l, --log=<log file>               Input log file

Example: """)
    print (sys.argv[0].split("/")[-1], '--plantuml ./plantuml.jar --config ./config.json --log ./log.json')
    print (sys.argv[0].split("/")[-1], '-c ./config.json -l ./log.json')
    sys.exit(er)


def parseCmdArgs():
    plantuml = "plantuml.jar"
    configFile = "config.json"
    logFile = ""
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hp:c:l:",["plantuml=", "config=", "log="])
    except getopt.GetoptError:
        help(2)
    for opt, arg in opts:
        if opt == '-h':
            help()
        elif opt in ("-c", "--config"):
            configFile = arg
        elif opt in ("-l", "--log"):
            logFile = arg
        elif opt in ("-p", "--plantuml"):
            plantuml = arg
    return plantuml, configFile, logFile

def main():
    plantuml, configFile, logFile = parseCmdArgs()
    tr = Translator(plantuml, configFile)
    tr.translateFile(logFile)

if __name__ == "__main__":
     main()
    
