#!/usr/bin/env python3
# coding: utf-8
import subprocess as sp
import argparse as ap
import random, sys, os, re, shutil, string

COMPILER = "g++"
CXX_OPTS = []  # ["-g", "-Wall", "-Werror"]
REPORT_FUNS = ["_Z12report_storePvmS_", "_Z11report_loadPvmS_"]
DIR_PATH = os.path.dirname(os.path.realpath(__file__))
TMP_NUM = random.randint(0, 2 ** 24 - 1)
SCRATCH = ["rax", "rdi", "rsi", "rdx", "rcx", "r8", "r9", "r10", "r11"]
SCRATCH_ALIAS = [["rax", "eax", "ax", "ah", "al"],
                 ["rdi", "edi", "di", "dil"],
                 ["rsi", "esi", "si", "sil"],
                 ["rdx", "edx", "dx", "dh", "dl"],
                 ["rcx", "ecx", "cx", "ch", "cl"],
                 ["r8", "r8d", "r8w", "r8b"],
                 ["r9", "r9d", "r9w", "r9b"],
                 ["r10", "r10d", "r10w", "r10b"],
                 ["r11", "r11d", "r11w", "r11b"]]
TYPE_DESCS = {"XMMWORD": 16, "QWORD": 8, "DWORD": 4, "WORD": 2, "BYTE": 1}
LABEL_CTR = 0
HARMLESS_INSTRS = ["lea", "call"]
STD_RE = re.compile("_Z[STVINK]?[STVINK]?[STVINK]?St")
GNU_RE = re.compile("_ZZ?[NL]?[0-9]*__")
GLB_RE = re.compile("_GLOBAL_")

DO_CLEANUP = False  # True
OBJECTS = []
ASMS = []

FUN_REPLACE = {"_ZNSt5mutex4lockEv": "_Z10mutex_lockPSt5mutex",
               "_ZNSt5mutex6unlockEv": "_Z12mutex_unlockPSt5mutex",
               "_ZL26__gthrw_pthread_mutex_lockP15pthread_mutex_t@PLT": "_Z11pmutex_lockP15pthread_mutex_t",
               "_ZL28__gthrw_pthread_mutex_unlockP15pthread_mutex_t@PLT": "_Z13pmutex_unlockP15pthread_mutex_t"}


# generate a new unique label
def get_label():
    global LABEL_CTR
    LABEL_CTR += 1
    return ".Lcemila%d" % LABEL_CTR


# read a file into a string
def read_file(filename):
    with open(filename, "r") as f:
        return f.read()


# write a string to a file
def write_file(filename, text):
    with open(filename, "w+") as f:
        f.write(text)


# get temp filename for a given file path
def get_tmp_path(filename):
    global TMP_NUM
    return "/tmp/%s-%06x" % (filename.split("/")[-1], TMP_NUM)


# run other programs, and get the resulting stdout and stderr
def run_cmd(cmd, getout=False):
    print("Running: %s" % " ".join(cmd))
    ret = sp.run(cmd, stdout=sp.PIPE, stderr=sp.PIPE, encoding="utf-8")
    if ret.returncode != 0:
        print("Returned code %d" % (ret.returncode))
        print("Output is:\n%s\n%s" % (ret.stdout, ret.stderr))
        return 1
    if getout: return 0, ret.stderr
    return 0


# test if a line of assembly is a symbol declaration
def is_symbol(text):
    if len(text) == 0: return False
    return text[0] not in ("\t", ".") and text[-1] == ":"

TOKEN_SYMBOLS = string.ascii_letters + string.digits + "_"
def contains_token(text, token):
    pos = 0
    while True:
        pos = text.find(token, pos)
        if pos == -1: return False
        if (pos != 0 and text[pos - 1] in TOKEN_SYMBOLS or
            pos + len(token) < len(text) and text[pos + len(token)] in TOKEN_SYMBOLS):
            pos += 1
            continue
        return True

# get all scratch regs that are active in a function symbol
def get_active_regs(asm_text, start):
    active = set()
    for asm_line in asm_text[start + 1:]:
        if is_symbol(asm_line): break
        for reg_aliases in SCRATCH_ALIAS:
            if any(map(lambda x: contains_token(asm_line, x), reg_aliases)):
                active.add(reg_aliases[0])
    ret = sorted(active)
    return ret


def get_ptr_size(addr):
    for t in TYPE_DESCS:
        if t in addr:
            return TYPE_DESCS[t]
    assert (False)


# test if a symbol is a predefined std or gcc function
def is_predefined_fun(symbol):
    return (STD_RE.search(symbol) != None or
            GNU_RE.search(symbol) != None or
            GLB_RE.search(symbol) != None)

# patch memory accesses
def instrument_mem(asm_text):
    new_text = []
    active_regs = []
    std_fun = False
    for li, asm_line in enumerate(asm_text):
        # ignore blank lines
        if len(asm_line) == 0: continue
        # check in which function we are
        if is_symbol(asm_line):
            std_fun = is_predefined_fun(asm_line[:-1])
            if not std_fun:
                active_regs = get_active_regs(asm_text, li)
        new_text.append(asm_line)
        # ignore standard library functions
        if std_fun: continue
        # get addressing instructions
        line = asm_line.strip().split()
        if len(line) == 0: continue
        if line[0] in HARMLESS_INSTRS or all(map(lambda x: "[" not in x, line)):
            continue
        # get operands
        line = asm_line.strip().split("\t")[1].split(", ")
        if len(line) != 2:
            print("!!!!!!!!!!!!!!!!!!", asm_line.encode("ascii"))
        is_load = ("[" not in line[0]) & 1
        addr = line[is_load]
        size = get_ptr_size(addr)
        label_ip = get_label()

        prologue = (["\tpush\t%s" % x for x in active_regs] +
                    ["\tpush\trbx", "mov\trbx, rsp", "\tand\trsp, -16"])
        epilogue = (["\tmov\trsp, rbx", "\tpop\trbx"] +
                    ["\tpop\t%s" % x for x in active_regs[::-1]])
        new_text = (new_text[:-1] +
                    prologue +
                    ["\tlea\trdi, " + addr,
                     "\tmov\trsi, %d" % size,
                     "\tlea\trdx, [rip+ OFFSET %s]" % (label_ip),
                     "\tcall\t" + REPORT_FUNS[is_load]] +
                    epilogue +
                    ["%s:" % label_ip,
                     new_text[-1]])
    return new_text


# replace all calls to mutex object
def instrument_mux(asm_text):
    new_text = []
    std_fun = False
    for asm_line in asm_text:
        # if is_symbol(asm_line):
        #     std_fun = is_predefined_fun(asm_line[:-1])
        if std_fun or len(asm_line) < 2 or asm_line[0] != "\t" or asm_line[1] == ".":
            new_text.append(asm_line)
            continue
        for fr in FUN_REPLACE:
            if contains_token(asm_line, fr):
                print(asm_line)
                asm_line = asm_line.replace(fr, FUN_REPLACE[fr])
                print(asm_line)
        new_text.append(asm_line)
    return new_text


# compile and instrument c++ code
def instrument_cxx(cxx_filename):
    global OBJECTS
    global ASMS
    global CXX_OPTS
    tmp_name = get_tmp_path(cxx_filename)
    asm_filename = tmp_name + ".s"
    cmd = [COMPILER, "-masm=intel", "-S"] + CXX_OPTS + ["-o", asm_filename, cxx_filename]
    if run_cmd(cmd): return 1
    ASMS.append(asm_filename)
    asm_text = read_file(asm_filename).split("\n")
    asm_text = instrument_mem(asm_text)
    asm_text = instrument_mux(asm_text)

    write_file(asm_filename, "\n".join(asm_text))
    if OPTIONS.noasm: return 0
    obj_filename = tmp_name + ".o"
    cmd = ["as", "-o", obj_filename, asm_filename]
    if run_cmd(cmd): return 1
    OBJECTS.append(obj_filename)
    return 0


# clean program termination
def exit(code):
    global OBJECTS
    if DO_CLEANUP:
        for o in OBJECTS:
            os.remove(o)
        for a in ASMS:
            os.remove(a)
    sys.exit(code)


# parse and consume cemila specific compile options
def parse_options():
    global OPTIONS
    global CXX_OPTS
    p = ap.ArgumentParser(usage="%s [options] file...")
    p.add_argument('-o', type=str, default="a.out", dest='outfile', metavar='',
                   help='Output file')
    p.add_argument('-c', action='store_true', default=False, dest='nolink',
                   help='Compile but do not link')
    p.add_argument('-v', action='store_true', default=False, dest='info',
                   help='Show compiler information')
    p.add_argument('-S', action='store_true', default=False, dest='noasm',
                   help='Compile only, but do not assemble or link')
    OPTIONS, remainder = p.parse_known_args()
    CXX_OPTS = [a for a in remainder if a.startswith("-")]
    sys.argv = [sys.argv[0]] + [a for a in remainder if not a.startswith("-")]

    if OPTIONS.info:
        res, out = run_cmd(["g++", "-v"], True)
        sys.stderr.write(out)
        exit(0)
    if len(sys.argv) < 2:
        p.print_help()
        print("For more options, see 'g++ --help'")
        exit(0)
    if (OPTIONS.nolink and OPTIONS.noasm):
        print("Cannot specify both -c and -S")
        exit(2)
    if len(sys.argv) > 2 and (OPTIONS.noasm or OPTIONS.nolink):
        print("Cannot specify both -o and either -c or -S")
        exit(3)


# test if object file
def test_elf(filename):
    with open(filename, "rb") as f:
        r = f.read(8)
        if r[:4] == b"\x7fELF": return 1
        if r[:7] == b"!<arch>": return 1
        return 0


if __name__ == "__main__":
    parse_options()
    for f_name in sys.argv[1:]:
        try:
            if test_elf(f_name):
                OBJECTS.append(f_name)
                continue
        except FileNotFoundError as err:
            print(err)
            exit(-10)
        ret = instrument_cxx(f_name)
        if ret: exit(-1)
    if OPTIONS.noasm:
        shutil.copy(ASMS[0], OPTIONS.outfile)
        exit(0)
    if OPTIONS.nolink:
        shutil.copy(OBJECTS[0], OPTIONS.outfile)
        exit(0)
    cemila_cxx = DIR_PATH + "/cemila.cpp"
    cemila_obj = get_tmp_path(cemila_cxx) + ".o"
    cmd = [COMPILER, "-c", "-fPIC", "-O3", "-Wall", "-Werror", "-o", cemila_obj, cemila_cxx]
    if run_cmd(cmd): exit(-2)
    OBJECTS.append(cemila_obj)

    cmd = [COMPILER, "-o", OPTIONS.outfile] + CXX_OPTS + OBJECTS
    if run_cmd(cmd): exit(-3)
    exit(0)
