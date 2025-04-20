# Secreto

**Secreto** is a minimal, single-file steganography tool written in pure C that hides and extracts **text files** inside **PNG images**.

It works on **Linux** and **Windows**, with no dependencies beyond `libpng`.

---

## Features

- Hide `.txt` files inside `.png` images
- Extract hidden `.txt` files from `.png`
- Pure C — portable and lightweight
- Compatible with Linux, and Windows

---

## Usage

### Compile

Make sure you have `libpng` installed.

```bash
gcc -o Secreto Secreto.c -lpng

Hide a text file

./Secreto hide input.png secret.txt output.png

This hides the contents of secret.txt inside input.png and writes it to output.png.

Extract the text file

./Secreto extract output.png recovered.txt

This extracts the hidden text from output.png and saves it to recovered.txt.


---

Example

./Secreto hide hello.png test.txt secret.png
./Secreto extract secret.png out.txt

```
---

Limitations

Only works with PNG images

Only hides and extracts text files

No encryption — just basic embedding

---

