
import re

if __name__ == "__main__":

    # Fill input from "AIAPP-ISP (V6.96A)" from tab "Header File" choosing the relevant "Series".

    input = """

"""


    regexSfrString = r"^([ ]*)__sfr[ ]*__at\((0x[0-9A-F]+)\)[ ]*([\w\d]+);$"
    regexSbitString = r"^([ ]*)__sbit[ ]*__at\((0x[0-9A-F]+)\)[ ]*([\w\d]+);$"
    regexSfrxString = r"^([ ]*)__xdata volatile unsigned ((char)|(short))[ ]*__at\((0x[0-9A-F]+)\)[ ]*([\w\d]+);$"

    regexSfrPattern = re.compile(regexSfrString)
    regexSbitPattern = re.compile(regexSbitString)
    regexSfrxPattern = re.compile(regexSfrxString)

    output = ""
    lastRegisterAddress = None
    for line in input.split('\n'):

        matchSfr = regexSfrPattern.match(line)
        if matchSfr != None:
            label = matchSfr.group(3)
            address = matchSfr.group(2)
            convertedLine = f"{matchSfr.group(1)}SFR({label}, {address});\n"
            output += convertedLine
            lastRegisterAddress = int(address, 0)
            continue

        matchSbit = regexSbitPattern.match(line)
        if matchSbit != None:
            label = matchSbit.group(3)
            address = matchSbit.group(2)

            addressOffset = int(address, 0) - lastRegisterAddress

            convertedLine = f"{matchSbit.group(1)}SBIT({label}, 0x{lastRegisterAddress:X}, {addressOffset});\n"
            output += convertedLine
            continue

        matchSfrx = regexSfrxPattern.match(line)
        if matchSfrx != None:
            label = matchSfrx.group(6)
            address = matchSfrx.group(5)
            convertedLine = f"{matchSfrx.group(1)}SFR({label}, {address});\n"
            output += convertedLine
            lastRegisterAddress = int(address, 0)
            continue



        # Simply copy the line
        output += line
        # Add previously removed [see split()] newline again.
        output += '\n'

    print(output)

    exit(0)