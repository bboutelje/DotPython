using System.Diagnostics;

[assembly: Parallelize(Scope = ExecutionScope.MethodLevel, Workers = 1)]
