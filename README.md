# Paint Program

## Usage

### Running the Executable

To use the program, simply run the provided executable. Download the executable from the source code and launch it.

### Compiling from Source

If you prefer to compile the source code yourself, follow these steps:

1. Ensure you have the GCC compiler installed locally.
   
      To make sure you have the GCC Compiler downloaded, you can use the following command in you're bash terminal
     ```bash
     gcc -v
     ```
3. Open a terminal and navigate to the source code directory.
4. Run the following command:

   ```bash
   gcc -o paint.exe paint.c -mwindows -lgdi32
   ```
**Note:** This compilation method is suitable for users with the GCC compiler installed locally.

If you don't have GCC installed, consider downloading it from the [MSYS2 website](https://www.msys2.org/).



After successful compilation, you can execute the program using:

```bash
./paint.exe
```

## Program Information

- **Author:** William Beaudin
- **Version:** 2024-02-03/2.2

## About the Program

Paint Program is an open-source application that allows users to draw on a canvas with various colors, brush sizes, and grid options.

### Libraries Used

- Windows API
- Common Controls Library (CommCtrl)

## How It Works

The program utilizes the Windows API and Common Controls Library to create a graphical interface for drawing. The canvas supports features such as color selection, grid toggling, and brush size adjustments.

## Features

### Color Selection

The Paint Program offers a variety of colors for your drawing needs. Choose from the following color options:

- **Black**
- **Red**
- **Green**
- **Blue**
- **As Well as a RGB Selector**
   
#### Eraser functionality

#### Grid toggle option

#### Adjustable brush size

#### Canvas reset option

## Scalability

Paint Program is designed to be scalable, allowing for potential enhancements and modifications. It is open-source, and contributions from the community are welcome.

## Contribution

Feel free to contribute to the development of Paint Program. Fork the repository, make your changes, and submit a pull request.

---

**Note:** This README provides basic information. For detailed instructions and technical details, refer to the source code and relevant documentation.

