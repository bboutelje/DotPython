# DotPython

# Python .NET Bridge

A C++/CLI library that provides seamless access to Python from .NET applications.

## Overview

This library wraps the CPython API, allowing .NET developers to embed Python, execute Python code, and interact with Python objects directly from C# or any other .NET language.

## Features

- Execute Python code from .NET
- Access Python objects and call Python functions
- Pass data between .NET and Python
- Support for Python modules and imports
- Native performance through C++/CLI

## Requirements

- .NET Framework 4.8 or later (or .NET 6+)
- Python 3.x installed
- Windows (C++/CLI requirement)

## Installation


1. Clone the repository:
```bash
git clone https://github.com/yourusername/python-dotnet-bridge.git
cd python-dotnet-bridge
```

2. Open the solution in Visual Studio 2019 or later

3. Build the solution (Release configuration recommended)

4. Reference the compiled DLL in your .NET project

## Configuration

### Python Path

By default, the library searches for Python in the system PATH. To specify a custom Python installation:

```csharp
Python.Initialize("C:\\Python311");
```

### Debug vs Release

This library is compiled against Python's release binaries for optimal performance. When debugging your .NET code, Python calls will still use the release Python runtime.

## Building from Source

### Prerequisites

- Visual Studio 2019 or later
- Python 3.x development libraries
- C++/CLI workload installed

### Build Steps

1. Ensure Python include files and libraries are accessible
2. Update project properties if needed:
   - C/C++ → General → Additional Include Directories: `C:\Python311\include`
   - Linker → General → Additional Library Directories: `C:\Python311\libs`
3. Build solution

## Project Structure

```
.
├── include/          # Public header files
│   └── PythonWrapper.h
├── src/              # Implementation files
│   └── PythonWrapper.cpp
├── lib/              # Python libraries (if bundled)
└── README.md
```

## Known Limitations

- Windows only (C++/CLI restriction)
- Requires Python to be installed on the target machine
- Single Python interpreter per process

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Built with the CPython C API
- Inspired by the need for lightweight Python embedding in .NET



**Star this repository if you find it useful!** ⭐
