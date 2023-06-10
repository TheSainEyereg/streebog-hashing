# Streebog Hashing Algorithm

C++ implementation of the Streebog hashing algorithm, as specified in the Russian national standard GOST R 34.11-2012.

[**README на русском тут**](README_ru.md)

## Disclaimer
This project was made as part of a practical work for my university and is not ready for widespread use. The repository is posted on GitHub for demonstration purposes. 

## Credits

- [D35 Crypto Library](https://github.com/D35YNC/D35CryptoLib/blob/master/lib/hash/streebog.cpp)
- [Rust Crypto Hashes](https://github.com/RustCrypto/hashes/tree/master/streebog)

## Literature

- [Russian National Standard GOST R 34.11-2012](https://ru.wikisource.org/wiki/%D0%93%D0%9E%D0%A1%D0%A2_%D0%A0_34.11%E2%80%942012)
- [Wikipedia page with algorithm description](https://ru.wikipedia.org/wiki/%D0%93%D0%9E%D0%A1%D0%A2_34.10-2018)
- [Implementation article on xakep.ru](https://xakep.ru/2016/07/20/hash-gost-34-11-2012/ )

## Usage

To build the project, you need to have CMake installed on your machine. Run the following commands to build the project:

```
mkdir build
cd build
cmake ..
make
```

Once the project is built, you can use it to compute the hash of a file or a directory. To compute the hash of a file or a directory, run the following commands:

```
./streebog path/to/file
./streebog path/to/directory
```

Hash will be printed to the standard output in hexadecimal format.

## License

This code is distributed under the MIT License, see the `LICENSE` file for more information.