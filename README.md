# redis

	redis - Multi-CPU disassembler with reverse-engineering capabilities

	(c) 2001-2002 Georg Acher, georg@acher.org 
	http://www.acher.org/

	(c) 2001-2015 Rainer Buchty, rainer@buchty.net
	http://www.buchty.net/

This is a small tool for disassembling 68k, 8085 and 6809 binaries. From the
68k family it currently supports the 68000 only.

It features an extension for reading a separate label file, supplying labels 
for addresses, constants, or complex data fields. Whenever the address is
encountered in a jump or a absolute/PC-relative addressing mode, it is
replaced by the defined label. The same applies for immediate data moves.

It is possible to specify the start address of the vector table (on 68k usually 0). 
The read values are automatically labeled with an appropriate name.

The following options exist so far:

-b <hexnumber> : Assume a binary base address (default: 0)
-L <labelfile> : Use the specified file for guessing labels (format below)
-l <hexnumber> : Length of code to disassemble (default: entire file)
-s <hexnumber> : Start disassembly at given offset (relative to base address)
-v <hexnumber> : Assume vector table at given address
-c <cpunumber> : numeric CPU number
		 valid are 6809, 8085, 68000/10/20/30/40/60

Note: The 8085-output is currently in Z80-Opcodes.

Example Usage:
redis -b c00000  -L test.labels -v 0 test.bin

Label file format
-----------------

This disassembler comes with a somewhat powerful label processor. You can not
only declare labels for addresses, but also additional comments (even w/o
printing out a label name) and data structures. A label file can consist of
the following commands:

	Comments in the label file.
		; comment

	Label declaration
		l address name

	Additional descriptions for labels which will be included in the
	disassembly
		c address description

	Data segment declaration
		b start_address end_address #_of_bytes
		w start_address end_address #_of_words
		d start_address end_address #_of_double_words(longs)
	
	String segment declaration
		s start_address end_address
	
	Text (of fixed size) segment declaration
		t start_address end_address #_of_chars

	Data field segment declaration
		p start_address end_address #_of_fields f#,f#,f#,...,f#

		where f denotes the data type (b/w/d/s/t) and # the number
		of data type items

End_address being 0 is a shortcut for start_address+ # * sizeof(data_type),
or - more human readable - a single string (s) or set of bytes.

Let's assume, there's a byte table at address $c00000 which is 64 bytes long
where each table row consists of 8 bytes. You want to give it the name
"tab_64" and give an additional explanation. This would require the
following entries:

	b $c00000 $c0003f 64
	l $c00000 tab_64
	c $c00000 8-by-8 char matrix

Now for a more complicated example -- which nobody would ever implement that
way, but anyway ... Assume a table of print vectors (32 bit) with additional
parameters, let's say x and y position (byte) plus a string of arbitrary
size. This table is again located between $c00000 and $c0003f. So the entry
would read like that:

	p $c00000 $c0003f d,b,b,s
or
	p $c00000 $c0003f d,b2,s

The latter example would force the two bytes to be written into one DC.B
line.

	my_label_name equ $hexnumber		for program labels
or
	my_label_name seg $start,$end,X#	for data labels

	where X can be either b(yte), w(ord), l(ong), t(ext) or s(tring)
	and # denotes the number of items per line (ignored for string)

	zero $end on string is short for $start+sizeof(string)

A ";" signals a comment, the rest of the line is ignored.

INSTALL:

Simply type "make". It should compile fine on Linux ...

CAUTION: 

This program is (as usual) a quick hack and is not extensively tested to
produce correct disassembly. Use at your own risk.

