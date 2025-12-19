# DotPython

A .NET library that provides an optimized bridge between .NET and Python through the C++/CLI interop and the CPython API.

## Requirements

- .NET Framework 4.7.2 or later, or .NET 5 or later
- Works for Python 3.13, and should work on Python 3.8 and later in theory
- Windows x64 operating system only


## Usage Example

```csharp
using (var pyContext = new PyContext())
{
	var np = pyContext.Import("numpy");

	var arr = np.array(new[] {0, 1, 2}, dtype: np.int32);
	var mean = arr.mean();
}
```

## Python Environment Configuration

DotPython will attempt to locate your Python installation in the system's PATH enviroment variable. If you need to specify a custom Python installation, set the Python DLL in the initialization:

```csharp
Python.Initialize(@"C:\Python312\Python312.dll");
```

## Resources

- GitHub https://github.com/bboutelje/DotPython
