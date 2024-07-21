Simple Terminal based encrypted list application.
This application uses SHA265 to hash the password and AES to encrypt the list with the user password.

Requires OpenSSL
<br>
sudo apt-get install libssl-dev openssl
<br>
To compile:
gcc .\to_do_ListE.c -o .\to_do_ListE -lssl -lcrypto
