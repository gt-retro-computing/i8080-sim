# CLI

## Arguments

Arguments are parsed in order as instructions to the emulator. Order of instructions is therefore very important.

### -p num

Set the program counter (`pc`) to a value.

### -d str

Load a HEX string into memory. The HEX string encodes an address, length, checksum and data.

### -f file

Load HEX strings from a file. Any line not starting with `:` is ignored and any line starting with
`:` that is not a valid HEX string is considered fatal.

### -s int

Step n instructions or until halt. This arg will reset the halt status before attempting to execute any instructions.

### -R float

Execute instructions for n seconds or until halt. This arg will reset the halt status before attempting to execute any instructions.

### -P

Print current register values.

### -A int

Assert a certain value in the `a` register.

### -O str

Set the `out` instruction mode of the emulator.

Valid Options:
 - `none` makes `out` essentially a no-op.
 - `print` makes `out` print the value sent (the register `a`).
 - `halt` identical to `print` but also sets the halt flag.
 - `printbin` print the sent value in a binary representation.
 - `printbininv` print the sent value in a binary representation but inverted. This useful when running programs developed for the IMSAI 8080 where the front panel inverts the output value.


## Testing

Using `hlt`, execution can be stopped to assert values. Since `-s` (and `-R`) clear the halt flag,
this can be used to stop the program as determined by the code to assert values and then continue execution.

There is no "wait for halt" option intentionally. This enforces that execution completes in a reasonable
amount of time or that `-R` is used with a high timeout explicitly.

```sh
$ cat foo.s
.org 0
    mvi a, 5
    hlt
    mvi a, 8
    hlt
$ ./compile.sh foo.s > foo.hex
$ ./i8080cli -f foo.hex -s 1000 -P -A 5 -s 1000 -P -A 8
```

Broken down, these arguments:
1. Load file foo.hex
2. Execute up to 1000 instructions (we expect it to halt)
3. Print registers
4. Assert `a == 5`
5. Clear halt flag and execute up to 1000 more instructions
6. Print registers
7. Assert `a == 8`



