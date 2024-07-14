# Project 4 - Filesystem

- This project prints out information about the filesystem.
- The amount of `#`s in the output scales with the logarithmic scale of data stored in said file.
  - Ex: `#` is a file less than 100 bytes; `##` is a file 100 <= bytes < 1000; etc up to 7 `#`s.
- The amount of `.`s in the output scales with the logarithmic scale of the age of file.
  - Ex: `.` is a file less than one minute old; `..` is a file less than 1 hour old; etc up to 7 `.`s

## How to run

`./fs [-i=m] [-d=n] [-a] [-h] [-s] [-t] [file(s)]`

- a use last access time to determine file age rather than last modification time.
- d=n use a maximum recursion depth of n when showing sub-directory contents. Value range is
0–8. Default is 2.
- h generate the output in HTML using font size and shading to represent file size and age.
  - If this option is being used, send the output to an html file: `./fs -h > out.html`
- i=m indent m spaces for each subdirectory. Value range is 1–8. Default is 4.
- s sort directory entries alphabetically rather than in the order they are read.
- t show the file type based on the output of file command. Should be done with colors (including
a key) when using HTML.
