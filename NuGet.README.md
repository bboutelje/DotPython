# DotPython

A .NET library that provides seamless integration between .NET Framework and Python through C++/CLI interop with the CPython API.

## Overview

DotPython enables .NET Framework applications to execute Python code, call Python functions, and interact with Python objects directly from C#, VB.NET, or any other .NET language. Built using C++/CLI for optimal performance and reliability.

## Requirements

- .NET Framework 4.7.2 or higher
- Python 3.x installed on the system
- Windows operating system


## Key Features

- **Direct CPython API Access**: Built on top of the official CPython C API for maximum compatibility
- **Type Conversion**: Automatic conversion between .NET and Python types
- **Object Interop**: Create and manipulate Python objects from .NET
- **Module Import**: Import and use Python modules and packages
- **Exception Handling**: Python exceptions are translated to .NET exceptions
- **Performance**: C++/CLI implementation provides near-native performance

## Usage Examples

### Importing Python Modules

```csharp
// Import a Python module
var math = PythonEngine.ImportModule("math");
var piValue = math.pi;
```

### Working with Python Objects

```csharp
// Create Python list
var pyList = PythonEngine.CreateList();
pyList.Append(1);
pyList.Append(2);
pyList.Append(3);

// Create Python dictionary
var pyDict = PythonEngine.CreateDict();
pyDict.SetItem("key", "value");
```

### Calling Python Functions

```csharp
// Define a Python function
PythonEngine.Execute(@"
def multiply(a, b):
    return a * b
");

// Call the function
var result = PythonEngine.CallFunction("multiply", 5, 3);
```

## Python Environment Configuration

DotPython will attempt to locate your Python installation automatically. If you need to specify a custom Python installation, set the Python path before initialization:

```csharp
PythonEngine.SetPythonHome(@"C:\Python39");
PythonEngine.Initialize();
```




## Best Practices

1. **Initialize Once**: Call `PythonEngine.Initialize()` once at application startup
2. **Cleanup**: Always call `PythonEngine.Shutdown()` before application exit
3. **Thread Safety**: The Python GIL (Global Interpreter Lock) applies; use appropriate threading patterns
4. **Resource Management**: Dispose of Python objects when no longer needed to prevent memory leaks

## Troubleshooting

### Python Not Found
Ensure Python is installed and accessible in your system PATH, or explicitly set PYTHONHOME to the Python home directory.

### DLL Loading Issues
Verify that the Python DLLs (python3x.dll) are accessible from your application's directory or system PATH.

### .NET Framework Version Mismatch
DotPython requires .NET Framework 4.7.2 or higher, or .NET 5+. Ensure your project targets a compatible framework version.

## Performance Considerations

- Minimize cross-boundary calls between .NET and Python for optimal performance
- Batch operations when possible
- Consider keeping frequently-used Python objects in memory rather than recreating them

## Resources

- GitHub https://github.com/bboutelje/DotPython
