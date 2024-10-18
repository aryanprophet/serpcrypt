# serpcrypt

## Description

## Installation

### Prerequisites:
sudo apt-get install build-essential libssl-dev

### Compiling:
gcc -o serpcrypt serpcrypt.c -lssl -lcrypto

## Usage

### Encryption:

Pass your values as following:
./serpcrypt -e "$YourText" $YourKey $YourIV 

Example:

Input:
./serpcrypt -e "Hello, Serpent Cipher!" abcd1234abcd1234abcd1234abcd1234abcd1234abcd1234abcd1234abcd1234 1234567890abcdef1234567890abcdef

Output:
Ciphertext (hex): 6fbd1c2f156aba92bff5d6ed63cfbcc9

### Decryption:

Pass your values as following:
./serpcrypt -d "$YourCipherText" $YourKey $YourIV 

Example:

Input:
./serpcrypt -d "6fbd1c2f156aba92bff5d6ed63cfbcc9" abcd1234abcd1234abcd1234abcd1234abcd1234abcd1234abcd1234abcd1234 1234567890abcdef1234567890abcdef

Output:
Decrypted text: Hello, Serpent Cipher!
