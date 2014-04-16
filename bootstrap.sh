#!/usr/bin/bash
main="$1"
copy="$2"
jump="$3"
addr="$4"
PATH=".:$PATH"
tbuffer=$(awk -F '$' '/tbuffer/{printf("0x%s",$2)}' "$addr")
jumpbuf=$(awk -F '$' '/jumpbuf/{printf("0x%s",$2)}' "$addr")
printf "; Load our main resident routine into a free slot in bank 90-accessible RAM\n"
loadbytes $tbuffer 1 "$main"
mainsize=$(wc -c "$main" | cut -f 1)
printf "; Load a short routine for the main routine to where we really want it\n"
loadbytes $((tbuffer+mainsize)) 1 "$copy"
copysize=$(wc -c "$copy" | cut -f 1)
printf ";And execute it\n"
printf "jsr $%04x\nrts\nbrk\nbrk\nbrk\nbrk\n\n" $((tbuffer+mainsize))
printf ";Set up the jump to our final code\n"
loadbytes $jumpbuf 1 "$jump"
